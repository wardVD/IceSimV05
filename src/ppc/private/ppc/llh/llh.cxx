#include <set>
#include <map>
#include <vector>

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iostream>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_sf_gamma.h>

using namespace std;

#include "ppc.h"
#include "nnls.cxx"
#include "lssl.cxx"

double fdur=70;
bool fail=false;
bool fast=false;
bool mlpd=false;
bool flasher=false;
int srep=1, drep=1;

const int num=200, cnum=40;
const bool cutz=true;  // cut end bins
const double bin=25, cbin=2./cnum;
const double qmin=0.1;
const double qsat=500; // 2500
const double nsrt=500e-9;
vector<double> noise, noise_q;

int fsep=1;
double lbin=bin, nsq=nsrt*lbin*drep;
double rr=0, dR=10, dA=30, dI=0, LX=0;
const double cv=M_PI/180, sol=0.299792458;

void step(){
  cerr<<"rr="<<rr<<" dR="<<dR<<" dA="<<dA<<" dI="<<dI<<endl;
}

double square(double x){
  return x*x;
}

template <int n> class V{
  double x[n];

public:
  V(){
    for(int i=0; i<n; i++) x[i]=0;
  }

  double & operator[](int i){
    return x[i];
  }
};

double xrnd(){
  return drand48();
}

struct hit{
  float t, q;

  hit() : t(0), q(0){ }

  hit(float t, float q){
    this->t=t; this->q=q;
  }
};

struct hix : hit{
  float c;

  hix() : hit(), c(NAN){ }

  hix(float t, float q, float c) : hit(t, q){
    this->c=c;
  }
};

typedef pair<int, int> key;
key fla;

typedef struct{
  double r[3];
  key dom;

  vector<hit> draw, sraw;
  double dbin[num], sbin[num], dtot, stot;

  int n1, n2, ntop;
  double *dtop, *stop;

  void prep(double t0){
    for(int k=0; k<num; k++) dbin[k]=0; dtot=0;
    for(vector<hit>::const_iterator j=draw.begin(); j!=draw.end(); ++j){
      double t=floor((j->t-t0)/lbin), q=j->q;
      if(cutz) if(t<=0 || t>=num-2) q=0;
      if(t<0) t=0; else if(t>=num) t=num-1;
      dbin[int(t)]+=q; dtot+=q;
    }

    {
      int dmax=0;
      double vmax=0;

      for(int i=0; i<num; i++) if(dbin[i]>vmax) vmax=dbin[i], dmax=i;
      n1=max(0, dmax-(int) round(500/lbin));

      for(int i=num-1; i>dmax; i--) if(dbin[i]==vmax){ dmax=i; break; }
      n2=min(dmax+(int) round(1000/lbin), num);

      double d1=0, d2=0;
      for(int i=0; i<n1; i++) d1+=dbin[i];
      for(int i=n2; i<num; i++) d2+=dbin[i];
      dbin[n1]+=d1; dbin[n2-1]+=d2;

      dtop=&dbin[n1], stop=&sbin[n1], ntop=n2-n1;
    }

    optimize();
  }

  void fill(double dt){
    if(isfinite(dt)){
      for(int k=n1; k<n2; k++) sbin[k]=0; stot=0;
      for(vector<hit>::const_iterator j=sraw.begin(); j!=sraw.end(); ++j){
	double t=floor((j->t+dt)/lbin), q=j->q;
	if(cutz) if(t<=0 || t>=num-2) q=0;
	if(t<n1) t=n1; else if(t>=n2) t=n2-1;
	sbin[int(t)]+=q; stot+=q;
      }

      join();
    }
  }

  double llh(double dt, double nsim){
    fill(dt);
    double result=0;
    for(int k=0; k<fnum; k++) result+=llf(sim[k], dat[k], nsim, drep);
    return result;
  }

  double clear(){
    sraw.clear();
  }

  double G(double L, double D){
    const int prior=0;  // 0 or 8
    double R=D<=0?0:D*log(D/L);
    double Q=prior>0?-log(prior):gsl_sf_lnfact(L);
    return R+Q;
  }

  double opt[num];
  int lastchange[num];

  double dat[num], sim[num];
  int fnum;

  void optimize(){
    for(int n=0; n<ntop; n++){
      double sD=0, mxo;
      int mxj=-1;
      for(int j=n; j>=0; j--){
	sD+=dtop[j];
	int len=n-j+1;
	double aux=G(len, sD);
	if(j>0) aux+=opt[j-1];
	if(mxj==-1 || aux>mxo){ mxo=aux; mxj=j; }
      }
      opt[n]=mxo; lastchange[n]=mxj;
    }

    int f=ntop-1; fnum=0;
    for(int k=lastchange[f]; ; k=lastchange[k-1]){
      double d=0;
      for(; f>=k; f--) d+=dtop[f];
      dat[fnum++]=d; if(k==0) break;
    }
  }

  void join(){
    int f=ntop-1, fnum=0;
    for(int k=lastchange[f]; ; k=lastchange[k-1]){
      double s=0;
      for(; f>=k; f--) s+=stop[f];
      sim[fnum++]=s; if(k==0) break;
    }
  }

  void bins(){
    int f=ntop-1;
    double q=0;
    for(int k=lastchange[f]; ; k=lastchange[k-1]){
      double s=0;
      if(f==ntop-1) s+=num-n2;
      for(; f>=k; f--) s++;
      if(f==0) s+=n1;
      noise.push_back(s), q+=s;
      if(k==0) break;
    }
    noise_q.push_back(q);
  }

  static double llf(double s, double d, double nsim, double ndat, double sig = LSSL::SIG){
    if(s<0 || d<0){ cerr<<"Error!"<<endl; exit(1); }
    if(s==0 && d==0) return 0;

    double s2=sig*sig, tol=1.e-8;
    double sd=s+d, ntot=ndat+nsim;
    double ls=sd/ntot, ld=ls, llx;

    if(sig>0){
      double dls;
      do{
	double f=nsim*ls-s-log(ld/ls)/s2;
	double df=1+(1/(ndat*ld)+1/(nsim*ls))/s2;
	dls=-f/(nsim*df); ls+=dls; ld=(sd-nsim*ls)/ndat;
	if(ls<0 || ld<0){ cerr<<"Error!"<<endl; exit(1); }
      } while(fabs(dls)>tol*ls);

      llx=square(log(ld/ls))/(2*s2);
    }
    else llx=0;

    double lls=0; if(s>0) lls+=s*log((s/nsim)/ls);
    double lld=0; if(d>0) lld+=d*log((d/ndat)/ld);

    return llx+lls+lld;
  }

  bool in(){
    return dtot<qsat*drep && dtot>qmin*drep && (!flasher || dom.first!=fla.first || abs(dom.second-fla.second)>fsep);
  }
} dat;

set<key> bad;
bool isok(const key & om){
  return bad.find(om)==bad.end();
}

map<key, dat> all;
unsigned int nchn=1, ndof=1;

double COG[3]={0};
double qdat=0, qsim=0, nohit=0, tshift=0;

double qint(double q){
  // q=round(q); if(q<=0) q=1;
  return q;
}

float spe(){
  // return 1.;
  float q=-1.f, pe=0.2987f, s0=0.2916f, qt=0.5057f;
  while(!isfinite(q) || q<0.f || q>3.f) q=xppc::xrnd()<pe?-qt*logf(1.f-xppc::xrnd()):1.f+s0*xppc::grnd();
  return q;
}

struct xyz{
  double x, y, z;

  xyz(){ x=0, y=0, z=0; }

  xyz(double x, double y, double z){
    this->x=x, this->y=y, this->z=z;
  }

  void ini(){
    {
      FILE * f = fopen("bad", "r");
      if(f){
	int str, dom;
	while(fscanf(f, "%d %d", &str, &dom)==2) bad.insert(make_pair(str, dom));
	fclose(f);
      }
    }

    {
      const double dt=0;
      double t0=NAN, tx=NAN;

      FILE * f = fopen("dat", "r");
      if(f){
	int str, dom;
	double t, q;
	while(fscanf(f, "%d %d %lf %lf", &str, &dom, &t, &q)==4){
	  key om=make_pair(str, dom);
	  if(isok(om)){
	    if(std::isnan(t0) || t<t0) t0=t;
	    if(std::isnan(tx) || t>tx) tx=t;
	    q=qint(q*drep);
	    if(q>0) all[om].draw.push_back(hit(t, q));
	  }
	}
	fclose(f);
      }

      {
	double aux=(tx-t0+1)/num; lbin=aux>bin?aux:bin;
	cerr<<"T="<<(tx-t0)<<" "<<(num*bin)<<" lbin="<<lbin<<" ";
      }

      tshift=t0-dt; nchn=1, ndof=1;
      for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	dat & d = i->second; d.dom=i->first;
	d.prep(tshift); if(d.in()) nchn++, ndof+=d.fnum;
      }

      noise.push_back((xppc::hvs.size()-all.size())*num);
      noise_q.push_back(noise.front());
      for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	dat & d=i->second;
	if(d.in()) d.bins();
      }

      nsq=nsrt*lbin*drep;
      for(vector<double>::iterator i=noise.begin(); i!=noise.end(); ++i) *i*=nsq;
      for(vector<double>::iterator i=noise_q.begin(); i!=noise_q.end(); ++i) *i*=nsq;
    }

    {
      map<key, xyz> geo;

      for(vector<xppc::OM>::const_iterator i=xppc::i3oms.begin(); i!=xppc::i3oms.end(); ++i)
	geo.insert(make_pair(make_pair(i->str, i->dom), xyz(i->r[0], i->r[1], i->r[2])));

      x=0, y=0, z=0; qdat=0;
      for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	xyz g = geo[i->first];
	dat & d=i->second;
	double q=d.dtot;
	d.r[0]=g.x, d.r[1]=g.y, d.r[2]=g.z;
	x+=q*g.x; y+=q*g.y; z+=q*g.z; qdat+=q;
      }
      if(qdat>0) x/=qdat, y/=qdat, z/=qdat;
    }

    COG[0]=x, COG[1]=y, COG[2]=z;
    cerr<<"COG="<<x<<","<<y<<","<<z<<endl<<"Ndof="<<ndof<<" Qdat="<<qdat<<endl;
  }
};

struct dir{
  double th, ph, n[3];
  double p1[3], p2[3], u[3];

  dir() : th(0), ph(0){ n[0]=0; n[1]=0; n[2]=1; }

  void normalize(){
    double r=0;  for(int i=0; i<3; i++) r+=n[i]*n[i];
    r=1/sqrt(r); for(int i=0; i<3; i++) n[i]*=r;
  }

  double xvmf(double da){
    double ka=square(da*cv), xi;
    do{ xi=1+ka*log(xrnd()); } while (xi<-1);
    return acos(xi);
  }

  void setn(){
    double th=cv*this->th, ph=cv*this->ph;
    double st=sin(th), ct=cos(th);
    double sp=sin(ph), cp=cos(ph);
    n[0]=-st*cp, n[1]=-st*sp, n[2]=-ct;
  }

  void seta(){
    th=acos(-n[2])/cv; ph=atan2(-n[1], -n[0])/cv;
  }

  void setp(){
    {
      int i0=0;
      double n0=n[0];
      for(int i=1; i<3; i++) if(fabs(n[i])>fabs(n0)){ i0=i; n0=n[i]; }
      int i1=(i0+1)%3, i2=(i0+2)%3;
      double n1=n[i1], n2=n[i2], nq=n0*n0;
      double r1=1/sqrt(nq+n1*n1), r2=1/sqrt(nq+n2*n2);

      p1[i0]=-n1*r1; p1[i1]=n0*r1; p1[i2]=0;
      p2[i0]=-n2*r2; p2[i1]=0; p2[i2]=n0*r2;
    }
    {
      double q1[3], q2[3], r1=0, r2=0;
      for(int i=0; i<3; i++){
	double a1=p1[i]-p2[i]; q1[i]=a1; r1+=a1*a1;
	double a2=p1[i]+p2[i]; q2[i]=a2; r2+=a2*a2;
      }

      r1=1/sqrt(r1); r2=1/sqrt(r2);

      for(int i=0; i<3; i++){
	p1[i]=q1[i]*r1;
	p2[i]=q2[i]*r2;
      }
    }
  }

  void setu(){
    double zi=xrnd(); zi*=2*M_PI;
    double px=cos(zi), py=sin(zi);
    for(int i=0; i<3; i++) u[i]=px*p1[i]+py*p2[i];
  }

  void rotate(double z){
    if(flasher) return;
    double cs=cos(z), si=sin(z);
    {
      double r=0;
      for(int i=0; i<3; i++){
	double a=n[i]*cs+u[i]*si;
	n[i]=a; r+=a*a;
      }
      {
	r=1/sqrt(r);
	for(int i=0; i<3; i++) n[i]*=r;
      }
    }
  }
};

struct cascade : xyz, dir{
  double e, t, s, a;
  vector<double> unfE;

  cascade() : xyz(), dir(){
    e=0, t=0, s=1, a=1;
  }

  bool ini(){
    xyz::ini();
    bool flag=false;

    ifstream h("ini", ifstream::in);
    if(!h.fail()){
      string in;
      if(getline(h, in)){
	cascade c;
	int num=sscanf(in.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &c.x, &c.y, &c.z, &c.th, &c.ph, &c.e, &c.t, &c.s, &c.a);
	if(num>=5){
	  c.t-=tshift; c.setn(); * this = c; flag=true;
	  if(mlpd){
	    double dx=COG[0]-x, dy=COG[1]-y, dz=COG[2]-z;
	    double dot=dx*n[0]+dy*n[1]+dz*n[2];
	    x+=dot*n[0], y+=dot*n[1], z+=dot*n[2];
	    if(num>=7) t+=dot/sol; else t=-tshift;
	  }
	  cerr<<"Setting initial cascade parameters from file ini."<<endl;
	}
      }
      if(getline(h, in)){
	stringstream str(in);
	double aux;
	while(str>>aux) unfE.push_back(aux);
	if(!unfE.empty()) cerr<<"Setting unfolded pattern with "<<unfE.size()<<" entries from file ini."<<endl;
      }
      if(getline(h, in)){
	double r, dr, da, di, lx;
	int num=sscanf(in.c_str(), "%lf %lf %lf %lf %lf", &r, &dr, &da, &di, &lx);
	if(num>=4){
	  rr=r, dR=dr, dA=da, dI=di;
	  cerr<<"Setting search parameters from file ini."<<endl;
	}
	if(num>=5){
	  LX=lx;
	  cerr<<"Setting LX="<<LX<<endl;
	}
      }
      h.close();
    }

    step();
    return flag;
  }

  double gaus(){
    static bool flag=false;
    static double r1, r2;
    flag=!flag;
    if(flag){
      double ra=sqrt(-2*log(xrnd())), rb=2*M_PI*xrnd();
      r1=ra*cos(rb); r2=ra*sin(rb);
      return r1;
    }
    else{
      flag=false;
      return r2;
    }
  }

  void move(double dr=dR, double da=dA, double di=dI){
    x+=rr*n[0], y+=rr*n[1], z+=rr*n[2];

    dr/=M_SQRT3;
    x+=dr*gaus(), y+=dr*gaus(), z+=dr*gaus();

    setu(); rotate(xvmf(da));

    x-=rr*n[0], y-=rr*n[1], z-=rr*n[2];

    s*=exp(di*gaus()), a*=exp(di*gaus());
  }

  void unfold(double[], double[], vector< pair<key, hit> >[], int);
  void unfold_t(double[], double[], vector< pair<key, hit> >[], int);

  void runc(map< key, vector<hix> > &);
  void runf(map< key, vector<hix> > &);
  void ppc(vector< pair<key, hix> > *);
  void end();

  double llh(vector< pair<key, hix> > *, bool, bool);
  void printout(vector< pair<key, hix> > [], int);
  void refine(const multimap<double, cascade> &, int);
};

ostream& operator<<(ostream& o, cascade& c){
  c.seta();
  o<<c.x<<" "<<c.y<<" "<<c.z<<" "<<c.th<<" "<<c.ph<<" "<<c.e<<" "<<c.t+tshift<<" "<<c.s<<" "<<c.a;
}

double fllh(double dt, double nsim){
  double llh=dat::llf(nohit, 0, nsim, drep);
  for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
    dat & d=i->second;
    if(d.in()) llh+=d.llh(dt, nsim);
  }
  return llh/ndof;
}

double fllh(const gsl_vector *x, void *p){
  return fllh(gsl_vector_get(x, 0), srep*exp(gsl_vector_get(x, 1)));
}


void cascade::unfold(double Q[], double X[], vector< pair<key, hit> > sto[], int jnum){
  double * A = new double[nchn*jnum]();

  for(int m=0; m<jnum; m++){
    vector< pair<key, hit> > & s = sto[m];
    for(vector< pair<key, hit> >::iterator h=s.begin(); h!=s.end(); ++h){
      const hit& tq = h->second;
      double t=tq.t, q=tq.q;
      map<key, dat>::iterator i=all.find(h->first);
      if(i==all.end()) nohit+=q;
      else i->second.sraw.push_back(hit(t, q));
    }
    {
      int n=0;
      A[n+++m*nchn]=nohit;
      for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	dat & d=i->second;
	if(d.in()){
	  d.fill(0);
	  double q=0;
	  for(int k=0; k<d.fnum; k++) q+=d.sim[k];
	  A[n+++m*nchn]=q;
	}
      }
      end();
    }
  }

  {
    NNLS::nnls(A, Q, X, nchn, jnum);
    LSSL::wref(A, Q, X, noise_q.data(), nchn, jnum);
  }

  delete A;
}

void cascade::unfold_t(double D[], double X[], vector< pair<key, hit> > sto[], int jnum){
  bool same=false, done=false;
  double * A = new double[ndof*jnum]();
  double dt=0, del=4*lbin, low=lbin/25, best=NAN;
  double sdt=dt;

  while(true){
    for(int m=0; m<jnum; m++){
      vector< pair<key, hit> > & s = sto[m];
      for(vector< pair<key, hit> >::iterator h=s.begin(); h!=s.end(); ++h){
	const hit& tq = h->second;
	double t=tq.t, q=tq.q;
	map<key, dat>::iterator i=all.find(h->first);
	if(i==all.end()) nohit+=q;
	else i->second.sraw.push_back(hit(t, q));
      }
      {
	int n=0;
	A[n+++m*ndof]=nohit;
	for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	  dat & d=i->second;
	  if(d.in()){
	    d.fill(dt);
	    for(int k=0; k<d.fnum; k++) A[n+++m*ndof]=d.sim[k];
	  }
	}
	end();
      }
    }

    {
      double rnorm=NNLS::nsum(A, D, X, ndof, jnum);
      // rnorm=w.wllh(A, D, X, noise.data(), ndof, jnum);
      // cout<<dt<<" "<<rnorm<<" "<<del<<endl;
      if(done) break;

      if(std::isnan(best)) best=rnorm;
      else{
	if(fabs(del)<low){
	  if(rnorm<best){
	    best=rnorm;
	    break;
	  }
	  else{
	    del=sdt-dt;
	    done=true;
	  }
	}
	else{
	  if(rnorm<best){
	    best=rnorm; sdt=dt;
	    if(same) del*=2; else same=true;
	  }
	  else{
	    dt-=del; del/=-2;
	    if(same){ del/=2; same=false; }
	  }
	}
      }
      dt+=del;
    }
  }

  {
    NNLS::nnls(A, D, X, ndof, jnum);
    LSSL::wref(A, D, X, noise.data(), ndof, jnum, -1);
  }

  delete A;
  t+=dt;
}

void cascade::runc(map< key, vector<hix> > & sim){
  pair<int, unsigned long long> id=make_pair(0, 0);
  const double ini=1.e6, emax=1.e8;

  if(!mlpd){
    if(e<=0) e=ini;
    xppc::eini();
    xppc::sett(n[0], n[1], n[2], id, 0);
    xppc::addp(x, y, z, t, e, 0);
    xppc::eout();

    for(unsigned int i=0; i<xppc::hitz.size(); i++){
      xppc::ihit & h = xppc::hitz[i];
      key om=make_pair(h.omkey.str, h.omkey.dom);
      sim[om].push_back(hix(h.time, spe(), h.dir));
    }

    xppc::efin();
  }
  else{
    {
      double dx=COG[0]-x, dy=COG[1]-y, dz=COG[2]-z;
      double dot=dx*n[0]+dy*n[1]+dz*n[2];
      x+=dot*n[0], y+=dot*n[1], z+=dot*n[2], t+=dot/sol;
    }

    const double etot=ini*srep; // total simulated energy
    const double step=7.5;  // distance between simulated cascades
    const double dtrs=100;  // max. distance to the closest DOM at ends
    const double dpad=100;  // padding to the segment of track with hits

    double dmin=0, dmax=0;  // fit end points

    for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
      dat & d=i->second;

      double dx=d.r[0]-x, dy=d.r[1]-y, dz=d.r[2]-z;
      double rn=dx*n[0]+dy*n[1]+dz*n[2];
      dx-=rn*n[0], dy-=rn*n[1], dz-=rn*n[2];
      double dd=sqrt(dx*dx+dy*dy+dz*dz);

      if(dd<dtrs){
	if(rn<dmin) dmin=rn;
	else if(rn>dmax) dmax=rn;
      }
    }

    int jmin=(int) round((dmin-dpad)/step);
    int jmax=(int) round((dmax+dpad)/step);
    int jnum=jmax-jmin+1;  // cout<<jnum<<" "<<ndof<<endl;

    if(unfE.empty()){
      double D[ndof], Q[nchn], E[jnum], X[jnum], T[jnum];

      {
	int n=0, m=0;
	D[n++]=0; Q[m++]=0;
	for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	  dat & d=i->second;
	  if(d.in()){
	    double q=0;
	    for(int k=0; k<d.fnum; k++) D[n++]=d.dat[k], q+=d.dat[k];
	    Q[m++]=q;
	  }
	}

	double e0=etot/jnum;  // jmin=0, jmax=0, jnum=1; e0=e;
	e=0; for(int j=0; j<jnum; j++) E[j]=e0, T[j]=E[j];
      }

      vector< pair<key, hit> > sto[jnum];
      for(int l=0; l<2; l++){
	xppc::eini();
	for(int j=0; j<jnum; j++){
	  double rn=step*(j+jmin);
	  xppc::sett(n[0], n[1], n[2], id, j);
	  if(e>0) E[j]*=etot/e, T[j]+=E[j];
	  xppc::addp(x+rn*n[0], y+rn*n[1], z+rn*n[2], t+rn/sol, E[j], -1);
	}
	xppc::eout();

	for(unsigned int i=0; i<xppc::hitz.size(); i++){
	  xppc::ihit & h = xppc::hitz[i];
	  key om=make_pair(h.omkey.str, h.omkey.dom);
	  if(isok(om)) sto[h.track.frame].push_back(make_pair(om, hit(h.time, spe())));
	}
	xppc::efin();

	unfold(Q, X, sto, jnum);
	if(!fast) if(l>0) unfold_t(D, X, sto, jnum);

	e=0; for(int j=0; j<jnum; j++) E[j]=T[j]*X[j]/drep, e+=E[j];
	// cout<<"E:"; for(int j=0; j<jnum; j++) cout<<" "<<E[j]; cout<<endl;
	if(e>emax){
	  cerr<<"Warning: e="<<e<<" exceeded max energy of "<<emax<<"!"<<endl;
	  if(fail) exit(2);
	  for(int j=0; j<jnum; j++) E[j]*=emax/e; e=emax;
	}
      }
      for(int j=0; j<jnum; j++) unfE.push_back(e>0?E[j]/e:0);
      cout<<"E:"; for(vector<double>::iterator i=unfE.begin(); i!=unfE.end(); ++i) cout<<" "<<*i; cout<<endl;
    }

    {
      xppc::eini();
      xppc::sett(n[0], n[1], n[2], id, 0);
      for(int j=0; j<jnum; j++){
	double rn=step*(j+jmin);
	xppc::addp(x+rn*n[0], y+rn*n[1], z+rn*n[2], t+rn/sol, e*unfE[j], -1);
      }
      xppc::eout();

      for(unsigned int i=0; i<xppc::hitz.size(); i++){
	xppc::ihit & h = xppc::hitz[i];
	key om=make_pair(h.omkey.str, h.omkey.dom);
	sim[om].push_back(hix(h.time, spe(), h.dir));
      }

      xppc::efin();
    }
  }
}

void cascade::runf(map< key, vector<hix> > & sim){
  const double ini=5, emax=100, bunch=2.5e9, ovr=25, width=fdur;

  if(!mlpd){
    if(e<=0) e=ini;
    xppc::flone(e*bunch/ovr);

    for(unsigned int i=0; i<xppc::hitz.size(); i++){
      xppc::ihit & h = xppc::hitz[i];
      key om=make_pair(h.omkey.str, h.omkey.dom);
      sim[om].push_back(hix(t+h.time+width*xrnd(), spe(), h.dir));
    }

    xppc::efin();
  }
  else{
    pair<int, unsigned long long> id=make_pair(0, 0);

    const double etot=ini*srep;  // total number of photon bunches
    const double step=5.;   // distance between directions [degrees]
    const int jmin=0, jnum=2+360/step;  // number of angular bins

    if(unfE.empty()){
      double D[ndof], Q[nchn], E[jnum], X[jnum], T[jnum];

      {
	int n=0, m=0;
	D[n++]=0; Q[m++]=0;
	for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	  dat & d=i->second;
	  if(d.in()){
	    double q=0;
	    for(int k=0; k<d.fnum; k++) D[n++]=d.dat[k], q+=d.dat[k];
	    Q[m++]=q;
	  }
	}

	double e0=etot/jnum;  // jmin=0, jmax=0, jnum=1; e0=e;
	e=0; for(int j=0; j<jnum; j++) E[j]=e0, T[j]=E[j];
      }

      vector< pair<key, hit> > sto[jnum];
      for(int l=0; l<2; l++){
	xppc::eini();
	for(int j=0; j<jnum; j++){
	  float n[3], r[4]={x, y, z, t};
	  if(j<2) n[0]=0, n[1]=0, n[2]=2*j-1;
	  else{
	    sincosf(cv*step*(j+jmin), &n[1], &n[0]); n[2]=0;
	    xppc::flshift(r, n);
	  }

	  xppc::sett(n[0], n[1], n[2], id, j);
	  if(e>0) E[j]*=etot/e, T[j]+=E[j];
	  xppc::addp(r[0], r[1], r[2], r[3], E[j]*bunch, char(j<2?0:1));
	}
	xppc::eout();

	for(unsigned int i=0; i<xppc::hitz.size(); i++){
	  xppc::ihit & h = xppc::hitz[i];
	  key om=make_pair(h.omkey.str, h.omkey.dom);
	  if(isok(om)) sto[h.track.frame].push_back(make_pair(om, hit(h.time+width*xrnd(), spe())));
	}
	xppc::efin();

	unfold(Q, X, sto, jnum);
	if(!fast) if(l>0) unfold_t(D, X, sto, jnum);

	e=0; for(int j=0; j<jnum; j++) E[j]=T[j]*X[j]/drep, e+=E[j];
	// cout<<"E:"; for(int j=0; j<jnum; j++) cout<<" "<<E[j]; cout<<endl;
	if(e>emax){
	  cerr<<"Warning: e="<<e<<" exceeded max energy of "<<emax<<"!"<<endl;
	  if(fail) exit(2);
	  for(int j=0; j<jnum; j++) E[j]*=emax/e; e=emax;
	}
      }
      for(int j=0; j<jnum; j++) unfE.push_back(e>0?E[j]/e:0);
      cout<<"E:"; for(vector<double>::iterator i=unfE.begin(); i!=unfE.end(); ++i) cout<<" "<<*i; cout<<endl;
    }

    {
      xppc::eini();
      for(int j=0; j<jnum; j++){
	float n[3], r[4]={x, y, z, t};
	if(j<2) n[0]=0, n[1]=0, n[2]=2*j-1;
	else{
	  sincosf(cv*step*(j+jmin), &n[1], &n[0]); n[2]=0;
	  xppc::flshift(r, n);
	}

	xppc::sett(n[0], n[1], n[2], id, j);
	xppc::addp(r[0], r[1], r[2], r[3], e*unfE[j]*bunch, char(j<2?0:1));
      }
      xppc::eout();

      for(unsigned int i=0; i<xppc::hitz.size(); i++){
	xppc::ihit & h = xppc::hitz[i];
	key om=make_pair(h.omkey.str, h.omkey.dom);
	sim[om].push_back(hix(h.time+width*xrnd(), spe(), h.dir));
      }

      xppc::efin();
    }
  }
}

void cascade::ppc(vector< pair<key, hix> > * sav = NULL){
  qsim=0;

  for(int i=0; i<srep; i++){
    map< key, vector<hix> > sim; flasher?runf(sim):runc(sim);

    for(map<xppc::ikey, float>::const_iterator i=xppc::hvs.begin(); i!=xppc::hvs.end(); ++i){
      float hv=i->second;
      if(hv>0){
	float t=-1000;
	key om=make_pair(i->first.str, i->first.dom);
	while((t-=log(xrnd())/nsrt)<6000) sim[om].push_back(hix(t, spe(), NAN));
      }
    }

    const double dthr=0.25;
    for(map< key, vector<hix> >::iterator i=sim.begin(), j; i!=sim.end(); ){
      const vector<hix>& qs = i->second;
      double qi=0;
      for(vector<hix>::const_iterator k=qs.begin(); k!=qs.end(); ++k) qi+=k->q;
      j=i++; if(qi<dthr) sim.erase(j);
    }

    for(map< key, vector<hix> >::const_iterator i=sim.begin(); i!=sim.end(); ++i){
      const key& om = i->first;

      const int lc=2;
      if(lc>0){
	bool skip=true;
	if(skip){
	  map< key, vector<hix> >::const_iterator j(i); j++;
	  if(j!=sim.end() && j->first.first==om.first && j->first.second-om.second<=lc) skip=false;
	}
	if(skip){
	  map< key, vector<hix> >::const_reverse_iterator j(i);
	  if(j!=sim.rend() && j->first.first==om.first && om.second-j->first.second<=lc) skip=false;
	}

	if(skip) continue;
      }

      if(isok(om)){
	const vector<hix>& ts = i->second;
	map<key, dat>::iterator i=all.find(om);
	for(vector<hix>::const_iterator j=ts.begin(); j!=ts.end(); ++j){
	  const hix & h = *j;

	  if(i==all.end()) nohit+=h.q;
	  else i->second.sraw.push_back(h);

	  if(sav!=NULL)
	    if(isfinite(h.c)) sav[min(cnum-1, max(0, (int) floor((1+h.c)/cbin)))].push_back(make_pair(om, h));

	  qsim+=h.q;
	}
      }
    }
  }
}

void cascade::end(){
  nohit=0;
  for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i) i->second.clear();
}

double cascade::llh(vector< pair<key, hix> > * sav = NULL, bool first = true, bool second = true){
  double llh;

  if(first){
    ppc();

    if(qsim>0){
      bool verbose=false;

      const int num=2;
      double xi[num]={0, 1}, ei[num]={100, 1};

      gsl_vector * x = gsl_vector_alloc(num);
      gsl_vector * u = gsl_vector_alloc(num);

      for(int i=0; i<num; i++) gsl_vector_set(x, i, xi[i]), gsl_vector_set(u, i, ei[i]);

      gsl_multimin_function F;
      F.n = num;
      F.f = fllh;
      F.params = NULL;

      const gsl_multimin_fminimizer_type * T = gsl_multimin_fminimizer_nmsimplex;
      gsl_multimin_fminimizer * s = gsl_multimin_fminimizer_alloc(T, num);
      gsl_multimin_fminimizer_set(s, &F, x, u);

      int iter=0, max_iter=200;

      for(; iter<max_iter; iter++){
	if(GSL_SUCCESS!=gsl_multimin_fminimizer_iterate(s)){
	  if(verbose) printf("Failed to iterate\n");
	  break;
	}

	double size=gsl_multimin_fminimizer_size(s);
	if(verbose){
	  cout<<iter<<" "<<s->fval<<" "<<size;
	  for(int i=0; i<num; i++) cout<<" "<<gsl_vector_get(s->x, i); cout<<endl;
	}

	if(GSL_SUCCESS==gsl_multimin_test_size(size, 0.001)){
	  if(verbose) printf("Converged!\n");
	  break;
	}
      }

      double dt=gsl_vector_get(s->x, 0), nsim=exp(gsl_vector_get(s->x, 1));
      if(mlpd){
	const double nmax=100;
	if(nsim*nmax<1 || nsim>nmax){
	  cerr<<"Warning: fitted nsim="<<nsim<<"!"<<endl;
	  if(fail) exit(2);
	  nsim=nsim<1?1/nmax:nmax;
	}
      }
      t+=dt; e/=nsim; llh=fllh(dt, srep);
      if(verbose) cout<<"fval="<<s->fval<<" "<<llh<<" "<<fllh(0, srep)<<endl;

      gsl_multimin_fminimizer_free(s);
      gsl_vector_free(u);
      gsl_vector_free(x);
    }
    else llh=fllh(0, srep);

    end();
  }

  if(second){
    ppc(sav);
    llh=fllh(0, srep);
    end();
  }

  if(mlpd) unfE.clear();
  return llh;
}

void cascade::printout(vector< pair<key, hix> > sto[], int jnum){
  double * A = new double[ndof*jnum]();

  for(int m=0; m<jnum; m++){
    vector< pair<key, hix> > & s = sto[m];
    for(vector< pair<key, hix> >::iterator j=s.begin(); j!=s.end(); ++j){
      const hix& h = j->second;
      map<key, dat>::iterator i=all.find(j->first);
      if(i==all.end()) nohit+=h.q;
      else i->second.sraw.push_back(h);
    }
    {
      int n=0;
      A[n+++m*ndof]=nohit;
      for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
	dat & d=i->second;
	if(d.in()){
	  d.fill(0);
	  for(int k=0; k<d.fnum; k++) A[n+++m*ndof]=d.sim[k];
	}
      }
      end();
    }
  }

  int n=1;
  for(map<key, dat>::iterator i=all.begin(); i!=all.end(); ++i){
    dat & d=i->second;
    if(d.in()){
      for(int k=0; k<d.fnum; k++, n++){
	const key & om=i->first;
	cout<<om.first<<" "<<om.second<<" "<<noise[n]/nsq<<" "<<d.dat[k]/drep;
	for(int m=0; m<jnum; m++) cout<<" "<<A[n+m*ndof]/srep;
	cout<<endl;
      }
    }
  }

  delete A;
}

void cascade::refine(const multimap<double, cascade> & v, int top){
  vector< V<7> > vs;

  int num=0;
  for(multimap<double, cascade>::const_iterator j=v.begin(); j!=v.end() && num<top; ++j, num++){
    const cascade & cj = j->second;
    V<7> w;
    w[0]=n[0]*cj.p1[0]+n[1]*cj.p1[1]+n[2]*cj.p1[2];
    w[1]=n[0]*cj.p2[0]+n[1]*cj.p2[1]+n[2]*cj.p2[2];
    w[2]=cj.x-x, w[3]=cj.y-y, w[4]=cj.z-z;
    w[5]=1-(n[0]*cj.n[0]+n[1]*cj.n[1]+n[2]*cj.n[2]);
    w[6]=j->first;
    vs.push_back(w);
  }

  const bool verbose=false;
  const int dim=5;
  const int siz=(dim+1)*(dim+2)/2;

  double q[num][siz], f[num];
  double par[dim][dim], val[dim];

  for(int m=0; m<num; m++){
    int k=0;
    for(int i=0; i<dim; i++){
      double xi=vs[m][i];
      q[m][k++]=xi*xi;
      for(int j=i+1; j<dim; j++, k++) q[m][k]=2*xi*vs[m][j];
      q[m][k++]=xi;
    }
    q[m][k++]=1;
    q[m][dim]-=q[m][0]; q[m][0]=vs[m][5];
    f[m]=vs[m][6];
  }

  {
    gsl_matrix * A = gsl_matrix_alloc(num, siz);
    gsl_vector * B = gsl_vector_alloc(num);

    for(int i=0; i<num; i++){
      for(int j=0; j<siz; j++) gsl_matrix_set(A, i, j, q[i][j]);
      gsl_vector_set(B, i, f[i]);
    }

    gsl_multifit_linear_workspace * W = gsl_multifit_linear_alloc(num, siz);
    gsl_vector * X = gsl_vector_alloc(siz);
    gsl_matrix * C = gsl_matrix_alloc(siz, siz);
    double c;

    gsl_multifit_linear(A, B, X, C, &c, W);

    double p[siz];
    for(int i=0; i<siz; i++) p[i]=gsl_vector_get(X, i);

    for(int i=0, k=0; i<dim; i++){
      par[i][i]=p[k++];
      for(int j=i+1; j<dim; j++, k++) par[i][j]=p[k], par[j][i]=p[k];
      val[i]=p[k++];
    }
    double a=par[0][0]/2, b=par[1][1];
    par[0][0]=a-b; par[1][1]=a+b;

    if(verbose){
      for(int i=0; i<dim; i++){ cout<<val[i]; for(int j=0; j<dim; j++) cout<<"\t"<<par[i][j]; cout<<endl; }
      cout<<p[siz-1]<<endl;
    }

    gsl_vector_free(X);
    gsl_matrix_free(C);
    gsl_multifit_linear_free(W);

    gsl_vector_free(B);
    gsl_matrix_free(A);
  }

  {
    gsl_matrix * A = gsl_matrix_alloc(dim, dim);
    gsl_vector * B = gsl_vector_alloc(dim);

    for(int i=0; i<dim; i++){
      for(int j=0; j<dim; j++) gsl_matrix_set(A, i, j, 2*par[i][j]);
      gsl_vector_set(B, i, -val[i]);
    }

    gsl_vector * X = gsl_vector_alloc(dim);

    gsl_linalg_HH_solve(A, B, X);

    gsl_matrix_free(A);
    gsl_vector_free(B);

    double p[dim];
    for(int i=0; i<dim; i++) p[i]=gsl_vector_get(X, i);
    gsl_vector_free(X);

    if(verbose){ cout<<"X:"; for(int i=0; i<dim; i++) cout<<"\t"<<p[i]; cout<<endl; }

    {
      double nx=p[0], ny=p[1];
      double nz=sqrt(nx*nx+ny*ny), zx=sin(dA*cv);
      if(nz>zx) nx*=zx/nz, ny*=zx/nz, nz=zx;
      nz=sqrt(1-nz*nz);

      for(int i=0; i<3; i++) n[i]=nx*p1[i]+ny*p2[i]+nz*n[i];
    }

    {
      double dx=p[2], dy=p[3], dz=p[4];
      double dr=sqrt(dx*dx+dy*dy+dz*dz);
      if(dr>dR) dx*=dR/dr, dy*=dR/dr, dz*=dR/dr;

      x+=dx, y+=dy, z+=dz;
    }
  }
}

void loc_rs(cascade & ci){  // localized random search
  const int steps=40, tries=25, upto=steps/2;

  multimap<double, cascade> bag;
  for(int k=0; k<steps; k++){
    ci.setp();
    cascade cf;
    double best;

    multimap<double, cascade> v;
    for(int i=0; i<tries; i++){
      cascade ct=ci; ct.move();
      double temp=ct.llh();
      cout<<"|\t"<<temp<<" "<<ct<<endl;
      v.insert(make_pair(temp, ct));
      if(i==0 || temp<best) best=temp, cf=ct;
    }
    if(!mlpd && !flasher){
      cascade ct=ci;
      ct.refine(v, tries);
      double temp=ct.llh();
      cout<<"-\t"<<temp<<" "<<ct<<endl;
      if(temp<best) best=temp, cf=ct;
    }

    ci=cf; bag.insert(make_pair(best, ci));
    cout<<k<<"\t"<<best<<" "<<ci<<endl;
  }
  ci=bag.begin()->second;

  if(flasher){
    int num=0;
    double r2=0, r[3]={0};
    for(multimap<double, cascade>::const_iterator i=bag.begin(); num<upto && i!=bag.end(); ++i, num++){
      const cascade & c = i->second;
      r[0]+=c.x, r[1]+=c.y, r[2]+=c.z;
      r2+=c.x*c.x+c.y*c.y+c.z*c.z;
    }

    {
      double r2a=0;
      for(int i=0; i<3; i++){
	r[i]/=num;
	r2a+=r[i]*r[i];
      }

      dR=sqrt(r2/num-r2a);
    }
  }
  else if(!mlpd){
    int num=0;
    double rn=0, r2=0, r[3]={0}, n[3]={0};
    for(multimap<double, cascade>::const_iterator i=bag.begin(); num<upto && i!=bag.end(); ++i, num++){
      const cascade & c = i->second;
      for(int i=0; i<3; i++) n[i]+=c.n[i];
      r[0]+=c.x, r[1]+=c.y, r[2]+=c.z;
      r2+=c.x*c.x+c.y*c.y+c.z*c.z;
      rn+=c.x*c.n[0]+c.y*c.n[1]+c.z*c.n[2];
    }

    {
      double rna=0, r2a=0, n2a=0;
      for(int i=0; i<3; i++){
	r[i]/=num, n[i]/=num;
	rna+=r[i]*n[i], r2a+=r[i]*r[i], n2a+=n[i]*n[i];
      }
      rn/=num; rr=n2a<1?(rna-rn)/(1-n2a):0;

      dR=sqrt(r2/num-r2a+2*rr*(rn-rna)+rr*rr*(1-n2a));
      dA=n2a<1?1/(sqrt(sqrt(n2a)*(3-n2a)/(1-n2a))*cv):0;
    }
  }
  else{
    int num=0;
    double a[3][3]={0}, b[3]={0};

    for(multimap<double, cascade>::const_iterator i=bag.begin(); num<upto && i!=bag.end(); ++i, num++){
      const cascade & c = i->second;
      double dot=c.x*c.n[0]+c.y*c.n[1]+c.z*c.n[2];

      a[0][0]+=1-c.n[0]*c.n[0]; a[0][1]-=c.n[1]*c.n[0]; a[0][2]-=c.n[2]*c.n[0]; b[0]+=c.x-dot*c.n[0];
      a[1][0]-=c.n[0]*c.n[1]; a[1][1]+=1-c.n[1]*c.n[1]; a[1][2]-=c.n[2]*c.n[1]; b[1]+=c.y-dot*c.n[1];
      a[2][0]-=c.n[0]*c.n[2]; a[2][1]-=c.n[1]*c.n[2]; a[2][2]+=1-c.n[2]*c.n[2]; b[2]+=c.z-dot*c.n[2];
    }

    gsl_matrix * A = gsl_matrix_alloc(3, 3);
    gsl_vector * B = gsl_vector_alloc(3);

    for(int i=0; i<3; i++){
      for(int j=0; j<3; j++) gsl_matrix_set(A, i, j, a[i][j]);
      gsl_vector_set(B, i, b[i]);
    }

    gsl_vector * X = gsl_vector_alloc(3);

    gsl_linalg_HH_solve(A, B, X);

    gsl_matrix_free(A);
    gsl_vector_free(B);

    double r[3];
    for(int i=0; i<3; i++) r[i]=gsl_vector_get(X, i);
    gsl_vector_free(X);

    const bool verbose=false;
    if(verbose){ cout<<"X:"; for(int i=0; i<3; i++) cout<<"\t"<<r[i]; cout<<endl; }

    num=0;
    double r2a=0, n[3]={0};
    for(multimap<double, cascade>::const_iterator i=bag.begin(); num<upto && i!=bag.end(); ++i, num++){
      const cascade & c = i->second;
      for(int i=0; i<3; i++) n[i]+=c.n[i];

      double dx=r[0]-c.x, dy=r[1]-c.y, dz=r[2]-c.z;
      double dot=dx*c.n[0]+dy*c.n[1]+dz*c.n[2];
      dx-=dot*c.n[0], dy-=dot*c.n[1], dz-=dot*c.n[2];

      r2a+=dx*dx+dy*dy+dz*dz;
    }

    {
      double n2a=0;
      for(int i=0; i<3; i++){
	n[i]/=num; n2a+=n[i]*n[i];
      }

      dR=sqrt(r2a/num); rr=0;
      dA=n2a<1?1/(sqrt(sqrt(n2a)*(3-n2a)/(1-n2a))*cv):0;
      // for(int i=0; i<3; i++) COG[i]=r[i];
    }
  }

  step();
}

void spsa(cascade & ci){  // simultaneous perturbation stochastic approximation
  const int steps=1000, start=1;
  const double dL0=40;

  for(int k=0; k<steps; k++){
    double an=start/double(start+k), cn=pow(an, 1./4);
    double dr=cn*dR, da=cn*dA, di=cn*dI;

    ci.setp(); ci.setu();
    cascade c1=ci, c2=ci;

    double nz=2*xrnd()-1;
    double nr=sqrt(1-nz*nz), np=2*M_PI*xrnd();
    double nx=nr*cos(np), ny=nr*sin(np);

    c1.x-=dr*nx, c1.y-=dr*ny, c1.z-=dr*nz; c1.rotate(-cv*da);
    c2.x+=dr*nx, c2.y+=dr*ny, c2.z+=dr*nz; c2.rotate( cv*da);

    double as=2*M_PI*xrnd();
    double is=cos(as), ia=sin(as);
    c1.s/=exp(di*is), c1.a/=exp(di*ia);
    c2.s*=exp(di*is), c2.a*=exp(di*ia);

    double l1=c1.llh(), l2=c2.llh();
    double dL=l2-l1, llh=(l1+l2)/2;

    double fr=an*dL/(2*dL0*cn); if(fabs(fr)>cn) fr*=cn/fabs(fr);

    dr=fr*dR, da=fr*dA, di=fr*dI;
    ci.x-=dr*nx, ci.y-=dr*ny, ci.z-=dr*nz; ci.rotate(-cv*da);
    ci.s/=exp(di*is), ci.a/=exp(di*ia);

    llh=ci.llh();
    cout<<k<<"\t"<<llh<<" "<<ci<<endl;
  }
}

double pv(double y1, double y2, double y3){
  double x1=-1, x2=1, x3=0;
  double D=(x1-x2)*(x1-x3)*(x2-x3);
  double A=(x3*(y2-y1)+x2*(y1-y3)+x1*(y3-y2))/D;

  if(A<=0) return y1<y2?-1:y2<y1?1:0;
  else{
    double B=(x3*x3*(y1-y2)+x2*x2*(y3-y1)+x1*x1*(y2-y3))/D;
    //  C=(x2*x3*(x2-x3)*y1+x3*x1*(x3-x1)*y2+x1*x2*(x1-x2)*y3)/D;
    double x=-B/(2*A);
    return x<-1?-1:x>1?1:x;
  }
}

void spsawh(cascade & ci){  // simultaneous perturbation stochastic approximation with Hessian
  const int steps=1000, start=1;
  double llh=ci.llh();

  for(int k=0; k<steps; k++){
    double an=start/double(start+k), cn=pow(an, 0.25);

    ci.setp();
    cascade c1=ci, c2=ci;

    c1.move(cn*dR, cn*dA, cn*dI);
    c2.x=2*ci.x-c1.x, c2.y=2*ci.y-c1.y, c2.z=2*ci.z-c1.z;
    double dot=ci.n[0]*c1.n[0]+ci.n[1]*c1.n[1]+ci.n[2]*c1.n[2];
    for(int i=0; i<3; i++) c2.n[i]=2*dot*ci.n[i]-c1.n[i];
    c2.s=ci.s*ci.s/c1.s; c2.a=ci.a*ci.a/c1.a;

    double l1=c1.llh(), l2=c2.llh();

    double fr=pv(l1, l2, llh)*(an/cn);
    // cout<<l1<<" "<<llh<<" "<<l2<<" "<<fr<<endl<<c1<<endl<<ci<<endl<<c2<<endl;

    if(fr<0){
      ci.x=-fr*c1.x+(1+fr)*ci.x; ci.y=-fr*c1.y+(1+fr)*ci.y; ci.z=-fr*c1.z+(1+fr)*ci.z;
      for(int i=0; i<3; i++) ci.n[i]=-fr*c1.n[i]+(1+fr)*ci.n[i];
      ci.s=pow(c1.s, -fr)*pow(ci.s, 1+fr); ci.a=pow(c1.a, -fr)*pow(ci.a, 1+fr);
    }
    else if(fr>0){
      ci.x=fr*c2.x+(1-fr)*ci.x; ci.y=fr*c2.y+(1-fr)*ci.y; ci.z=fr*c2.z+(1-fr)*ci.z;
      for(int i=0; i<3; i++) ci.n[i]=fr*c2.n[i]+(1-fr)*ci.n[i];
      ci.s=pow(c2.s, fr)*pow(ci.s, 1-fr); ci.a=pow(c2.a, fr)*pow(ci.a, 1-fr);
    }
    ci.normalize();

    llh=ci.llh();
    cout<<k<<"\t"<<llh<<" "<<ci<<endl;
  }
}

void half(){
  dR*=M_SQRT1_2, dA*=M_SQRT1_2;
}

void mcmc(cascade & ci){  // Markov chain monte carlo
  const int steps=1000, upto=steps/2;
  for(int k=0, n=0; k<steps; k++){
    double best=ci.llh();
    ci.setp();

    cascade cf=ci; cf.move();
    double temp=cf.llh();

    if(temp<best) ci=cf, best=temp, n=0;
    else if(k<upto){
      n++;
      if(n>25) half(), step(), n=0;
    }

    cout<<k<<"\t"<<best<<" "<<ci<<endl;
  }
}

void errors(cascade & ci){  // uniform sample to estimate uncertainties
  const int steps=1000;
  ci.setp();

  for(int k=0, n=0; k<steps; k++){
    double best=ci.llh();
    cout<<k<<"\t"<<best<<" "<<ci<<endl;

    cascade cf=ci; cf.move();
    double temp=cf.llh();

    cout<<k<<"\t"<<temp<<" "<<cf<<endl;
  }
}

void err_mc(cascade & ci){  // uniform sample to estimate uncertainties
  const int steps=10000;
  ci.setp();

  for(int k=0, n=0; k<steps; k++){
    cascade cf=ci; cf.move();
    double aux=cf.llh();

    if(aux<LX){
      ci=cf; ci.setp();
      cout<<"+";
    }
    else{
      cout<<"-";
    }

    cout<<"\t"<<aux<<" "<<cf<<endl;
  }
}

void method(cascade & ci, int n){
  cout<<"Method: "<<n<<endl;
  if(n<0){
    double llh=0;
    vector< pair<key, hix> > sav[cnum];

    switch(n){
    case -1: llh=ci.llh(sav, true,  true); break;
    case -2: llh=ci.llh(sav, false, true); break;
    case -3: llh=ci.llh(sav, true, false); break;
    }

    cout<<"*\t"<<llh<<" "<<ci<<endl;
    ci.printout(sav, cnum);
  }
  else if(n<10){
    double llh=0;
    switch(n){
    case 0: llh=ci.llh(); break;
    case 1: llh=ci.llh(NULL, true,  true); break;
    case 2: llh=ci.llh(NULL, false, true); break;
    case 3: llh=ci.llh(NULL, true, false); break;
    }

    cout<<"?\t"<<llh<<" "<<ci<<endl;
  }
  else{
    switch(n){
    case 10: method(ci, 0); method(ci, 11); break;
    case 11: for(int i=0; i<10; i++) loc_rs(ci); break;
    case 12: spsa(ci);   break;
    case 13: spsawh(ci); break;
    case 14: mcmc(ci);   break;
    case 15: errors(ci); break;
    case 16: err_mc(ci); break;
    }
  }
}

main(int arg_c, char *arg_a[]){
  gsl_set_error_handler_off();
  xppc::DOM f;
  cascade ci;

  {
    char * bmp=getenv("FSEP");
    if(bmp!=NULL) fsep=atoi(bmp);
    cerr<<"FSEP="<<fsep<<endl;
  }
  {
    char * bmp=getenv("SREP");
    if(bmp!=NULL) srep=atoi(bmp);
    cerr<<"SREP="<<srep<<endl;
  }
  {
    char * bmp=getenv("DREP");
    if(bmp!=NULL) drep=atoi(bmp);
    cerr<<"DREP="<<drep<<endl;
  }
  {
    char * bmp=getenv("FAIL");
    if(bmp!=NULL && !(*bmp!=0 && atoi(bmp)==0)) fail=true;
    cerr<<"Fail on warnings: "<<(fail?"enabled":"not set")<<"."<<endl;
  }
  {
    char * bmp=getenv("FAST");
    if(bmp!=NULL && !(*bmp!=0 && atoi(bmp)==0)) fast=true;
    cerr<<"Running in a "<<(fast?"fast":"slow")<<" mode!"<<endl;
  }
  {
    char * bmp=getenv("MLPD");
    if(bmp!=NULL && !(*bmp!=0 && atoi(bmp)==0)) mlpd=true;
    cerr<<"Running in a "<<(mlpd?"millipede":"cascade")<<" mode!"<<endl;
  }
  {
    char * bmp=getenv("FLSH");
    if(bmp!=NULL){
      fla.first=strtol(bmp, &bmp, 10);
      fla.second=strtol(bmp+1, &bmp, 10);
      flasher=true;
      cerr<<"Flasher at "<<fla.first<<","<<fla.second<<"."<<endl;
    }
  }
  {
    char * bmp=getenv("FDUR");
    if(bmp!=NULL) fdur=atof(bmp);
    cerr<<"FDUR="<<fdur<<endl;
  }

  xppc::start();
  xppc::initialize(0.9); // flasher?1:0.9*0.85
  xppc::choose(-1);
  if(flasher){
    if(mlpd) f=xppc::flget(fla.first, fla.second);
    else xppc::flini(fla.first, fla.second);
  }
  else xppc::ini(0);
  cerr<<"Initialized!"<<endl;

  if(!ci.ini() && flasher){
    if(mlpd) ci.x=f.r[0], ci.y=f.r[1], ci.z=f.r[2];
    else ci.x=0, ci.y=0, ci.z=0;
  }

  if(arg_c>1){
    for(int i=1; i<arg_c; i++) method(ci, atoi(arg_a[i]));
  }
  else if(arg_c>0){
    cerr<<"Use as following:"<<endl;
    cerr<<"[FLSH=dom,str] [MLPD=0/1] [SREP=sim. events] [DREP=dat. events] "<<arg_a[0]<<" [method]"<<endl;
  }

  if(mlpd || !flasher) xppc::fin();
  xppc::stop();
}
