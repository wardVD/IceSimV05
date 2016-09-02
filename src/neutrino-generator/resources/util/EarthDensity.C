//*****************************************************************
//
// Utility tools to draw earth density, column depth etc.
//
// To use the script, you need to compile neutrino-generator project
// with -DUSE_CINT option, and execute "env.sh" at your I3_BUILD 
// directory.
// $ $I3_PORTS/bin/cmake -DCMAKE_BUILD_TYPE:STRING=Release -DUSE_CINT ../src/
// $ .$I3_BUILD/env.sh
//
// [example1 draw EarthDensity]
// $ root -l 'EarthDensity.C("PREM", "PREM_mmc", 0, 6.4e6)'
//
// [example2 draw column depth]
// $ root -l
// .L EarthDensity.C
// DrawColumnDepthVsCosZen() 
//
//*****************************************************************

#include "TVector3.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TROOT.h"
#include <iostream>
#include <string>
#include <string>

const double detdepth = 1948;

class I3NuGStaticParamsSetter;

//______________________________________________________________
double GetTrackPathLength(double angrad, double earthrad = 6378000.)
{
//
// returns track path length from entrance point of track
// to detector center
//
// costheta  =  TMath::Cos(angrad)
// costheta' = TMath::Cos(pi - angrad) = -costheta
// b = -l * costheta' = l*costheta 
//
   double r = earthrad;  // radius of Earth
   double costheta = TMath::Cos(angrad);
   double l = r - detdepth;
   double c = (l-r)*(l+r);
   double b = l*costheta;

   double x1 = (-b + TMath::Sqrt(b*b - c));
   double x2 = (-b - TMath::Sqrt(b*b - c));
   double angindeg = angrad *180 / TMath::Pi();
   //printf("track path for %f deg at depth %f m is %f m and %f m \n", angindeg, detdepth, x1, x2);
   
   double x = 0;
   if (x1 > 0) x = x1;
   else if (x2 > 0) x = x2;
   
   return x;
}

//______________________________________________________________
void GetTrackEntrancePoint(double angrad, double earthrad,
                           TVector3 &in, TVector3 &origin)
{
//
// returns TVector3 entrance point of track
// in Earth Center coordinate
// detector origin is also outpoint of the path
//
   // get track path length from entrance point to detector center
   double r = earthrad;  
   double tracklen = GetTrackPathLength(angrad, r);
   //cerr << "tracklen = " << tracklen << endl;

   // vector from origin to in
   double costheta = TMath::Cos(angrad);
   double sintheta = TMath::Sqrt((1+costheta)*(1-costheta));
   TVector3 dir(sintheta, 0, costheta); 
   dir.Unit();

   // detector origin in Earth center coordinate
   origin.SetXYZ(0,0, r - detdepth);  

   // now trace entrance point of the track  
   in = origin + tracklen*dir;
   //printf("in (%f,%f,%f) out (%f,%f,%f)\n", in.X(), in.Y(), in.Z(), origin.X(), origin.Y(), origin.Z());
   if (in.Mag() > earthrad + 10) {
      cerr << "Entrance point is not on the Earth Surface! excess "
           << in.Mag() - earthrad << endl;
   } 
}

//______________________________________________________________
double ColumnDepth(TVector3 in, TVector3 out,
                   string earthmodel, string crustmodel, double earthdensity)
{ 
  gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");

  // you must link library on current directory
  string mypath("../earthparams");
  I3NuGStaticParamsSetter::SetPath(mypath);
  I3NuGStaticParamsSetter::SetEarthModel(earthmodel);
  I3NuGStaticParamsSetter::SetCrustModel(crustmodel);
  I3NuGStaticParamsSetter::SetEarthDensity(earthdensity);

  /**
   *Note that this function called in term of the center of earth coordinate
   */
  TVector3 dir     = out-in;
  double MaxLength = dir.Mag();
  dir *= 1./MaxLength;
  double theta = dir.Theta();
  double phi   = dir.Phi() > 0 ? dir.Phi() : 2 * TMath::Pi() + dir.Phi();
  //cerr << "in (" << in.X() << ", " << in.Z() << ")" << endl;
  //cerr << "MaxLen " << MaxLength << endl;
  //cerr << "dir (" << dir.X() << "," << dir.Z() << ")" << endl;

  int    nsteps    = 10000;
  double dx        = MaxLength/nsteps;
  double columndepth   = 0;
  TVector3 local(in);
  double localrin, localrout, densin, densout, avedep;
  localrin = localrout = local.Mag();

  for(int istep=0; istep<nsteps; ++istep) {
     // update localrin
     localrin = localrout;
     densin = I3NuGInteractionInfo::GetEarthDensity(localrin, theta, phi);
     local += dir * dx;
     // update localrout 
     localrout = local.Mag();
     densout = I3NuGInteractionInfo::GetEarthDensity(localrout, theta, phi);
     avedep = 0.5*(densin + densout);
     columndepth += avedep*dx;

     //cerr << "step = " << istep << " densin(r = " << localrin << ")  = " 
     // << densin << ", densout(r = " << localrout << ") = " << densout 
     // << " ave = " << avedep << " totcdep = " << columndepth << endl; 
  }

  //columndepth is now in c*(g/cm3)*m = c*1.0e6 (g/m3)*m= 1.0e6 g/m2
  return columndepth*1.0e6; // in g/m2
  //return columndepth*1.0e2;   // in g/cm2, (g/cm3 * m) -> (g/cm * 1e2* cm)
  //
}

//______________________________________________________________
void DrawColumnDepthVsZenith() 
{

  gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");

  const int n=100;
  double minzenith  =90;
  //double maxzenith  =180;
  double maxzenith  =120;
  double rad = TMath::Pi() / 180;
  double zenbin= (maxzenith - minzenith)/n;

  Double_t      zen[n];
  Double_t      cdepth[4][n];

  TVector3  in[3];
  TVector3  out[3];

  for(int i=0; i<n; ++i) { 
     zen[i]=i*zenbin + minzenith;
     double angrad = zen[i]*rad;
     GetTrackEntrancePoint(angrad, 6378000, in[0], out[0]); // crust legacy ice surface
     GetTrackEntrancePoint(angrad, 6374134, in[1], out[1]); // crust PREM_mmc ice surface
     GetTrackEntrancePoint(angrad, 6377810, in[2], out[2]); // crust PREM_legacy_mmcnewice ice surface

     cdepth[0][i]=ColumnDepth(in[0], out[0], "PREM", "PREM_legacy", 5.5); 
     cdepth[1][i]=ColumnDepth(in[1], out[1], "PREM", "PREM_mmc", 5.5); 
     cdepth[2][i]=ColumnDepth(in[2], out[2], "PREM", "PREM_legacy_mmcnewice", 5.5); 
     //cdepth[3][i]=ColumnDepth(in[1], out[1], "PREM_FLATCORE", "PREM_mmc", 5.5); 
     cdepth[3][i]=ColumnDepth(in[1], out[1], "HOMOGENEOUS", "PREM_mmc", 0.1); 
  }
  cerr << "PREM total column depth at max zenith " << maxzenith << " is " <<  cdepth[0][n-1] << " g/m2 " << endl;

  TGraph *gr0 = new TGraph(n,zen,cdepth[0]);
  gr0->SetMarkerStyle(6);
  gr0->SetLineWidth(2);
  gr0->SetMarkerColor(kRed);
  gr0->SetLineColor(kRed);
  gr0->GetXaxis()->SetTitle("Zenith[deg]");
  gr0->GetYaxis()->SetTitle("Total Column Depth [g/m^{2}]");

  TGraph *gr1 = new TGraph(n,zen,cdepth[1]);
  gr1->SetMarkerStyle(6);
  gr1->SetLineWidth(2);
  gr1->SetMarkerColor(kBlue);
  gr1->SetLineColor(kBlue);

  TGraph *gr2 = new TGraph(n,zen,cdepth[2]);
  gr2->SetMarkerStyle(6);
  gr2->SetLineWidth(2);
  gr2->SetMarkerColor(kGreen);
  gr2->SetLineColor(kGreen);

  TGraph *gr3 = new TGraph(n,zen,cdepth[3]);
  gr3->SetMarkerStyle(6);
  gr3->SetLineWidth(2);
  gr3->SetMarkerColor(4);
  gr3->SetLineColor(4);

  TCanvas *c1 = new TCanvas("c1","test",200,10,600,500);
  //gr->Draw("ACP");
  gr0->Draw("APL");
  gr1->Draw("PL");
  gr2->Draw("PL");
  gr3->Draw("PL");

  TLegend *leg = new TLegend(0.1, 0.6, 0.5, 0.9);
  leg->AddEntry(gr0, "PREM, PREM_legacy", "pl");
  leg->AddEntry(gr1, "PREM, PREM_mmc", "pl");
  leg->AddEntry(gr2, "PREM, PREM_legacy_mmcnewice", "pl");
  leg->AddEntry(gr3, "PREM_FLATCORE, PREM_mmc", "pl");
  leg->Draw();
}

//______________________________________________________________
void DrawColumnDepthVsCosZen() 
{

  gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");

  const int n=100;
  double minzenith  = -1.;
  double maxzenith  = 0.;
  //double maxzenith  =120;
  double zenbin= (maxzenith - minzenith)/n;

  Double_t      zen[n];
  Double_t      cdepth[4][n];
  Double_t      cdepthr[4][n];

  TVector3  in[3];
  TVector3  out[3];

  for(int i=0; i<n; ++i) { 
     zen[i]=i*zenbin + minzenith;
     double angrad = TMath::ACos(zen[i]);
     GetTrackEntrancePoint(angrad, 6378000, in[0], out[0]); // crust legacy
     GetTrackEntrancePoint(angrad, 6374134, in[1], out[1]); // crust PREM_mmc
     GetTrackEntrancePoint(angrad, 6377810, in[2], out[2]); // crust PREM_legacy_mmcnewice 

     cdepth[0][i]=ColumnDepth(in[0], out[0], "PREM", "PREM_legacy", 5.5); 
     cdepth[1][i]=ColumnDepth(in[1], out[1], "PREM", "PREM_mmc", 5.5); 
     cdepth[2][i]=ColumnDepth(in[2], out[2], "PREM", "PREM_legacy_mmcnewice", 5.5); 
     cdepth[3][i]=ColumnDepth(in[1], out[1], "PREM_FLATCORE", "PREM_mmc", 5.5); 

     cdepthr[0][i] = cdepth[0][i] / cdepth[0][i];
     cdepthr[1][i] = cdepth[1][i] / cdepth[0][i];
     cdepthr[2][i] = cdepth[2][i] / cdepth[0][i];
     cdepthr[3][i] = cdepth[3][i] / cdepth[0][i];
  }
  cerr << "PREM total column depth at coszen " << minzenith << " is " <<  cdepth[0][0] << " g/m2 " << endl;

  TGraph *gr0 = new TGraph(n,zen,cdepth[0]);
  gr0->SetMarkerStyle(6);
  gr0->SetLineWidth(2);
  gr0->SetMarkerColor(kRed);
  gr0->SetLineColor(kRed);
  gr0->GetXaxis()->SetTitle("CosZenith");
  gr0->GetYaxis()->SetTitle("Total Column Depth [g/m^{2}]");

  TGraph *gr1 = new TGraph(n,zen,cdepth[1]);
  gr1->SetMarkerStyle(6);
  gr1->SetLineWidth(2);
  gr1->SetMarkerColor(kBlue);
  gr1->SetLineColor(kBlue);

  TGraph *gr2 = new TGraph(n,zen,cdepth[2]);
  gr2->SetMarkerStyle(6);
  gr2->SetLineWidth(2);
  gr2->SetMarkerColor(kGreen);
  gr2->SetLineColor(kGreen);

  TGraph *gr3 = new TGraph(n,zen,cdepth[3]);
  gr3->SetMarkerStyle(6);
  gr3->SetLineWidth(2);
  gr3->SetMarkerColor(4);
  gr3->SetLineColor(4);

  TCanvas *c1 = new TCanvas("c1","test",200,10,600,500);
  //gr->Draw("ACP");
  gr0->Draw("APL");
  gr1->Draw("PL");
  gr2->Draw("PL");
  //gr3->Draw("PL");

  TLegend *leg = new TLegend(0.1, 0.6, 0.5, 0.9);
  leg->AddEntry(gr0, "PREM, PREM_legacy", "pl");
  leg->AddEntry(gr1, "PREM, PREM_mmc", "pl");
  leg->AddEntry(gr2, "PREM, PREM_legacy_mmcnewice", "pl");
  //leg->AddEntry(gr3, "PREM_FLATCORE, PREM_mmc", "pl");
  leg->Draw();

  TGraph *grr0 = new TGraph(n,zen,cdepthr[0]);
  grr0->SetMarkerStyle(6);
  grr0->SetLineWidth(2);
  grr0->SetMarkerColor(kRed);
  grr0->SetLineColor(kRed);
  grr0->GetXaxis()->SetTitle("CosZenith");
  grr0->GetYaxis()->SetTitle("Total Column Depth Ratio");

  TGraph *grr1 = new TGraph(n,zen,cdepthr[1]);
  grr1->SetMarkerStyle(6);
  grr1->SetLineWidth(2);
  grr1->SetMarkerColor(kBlue);
  grr1->SetLineColor(kBlue);

  TGraph *grr2 = new TGraph(n,zen,cdepthr[2]);
  grr2->SetMarkerStyle(6);
  grr2->SetLineWidth(2);
  grr2->SetMarkerColor(kGreen);
  grr2->SetLineColor(kGreen);

  TGraph *grr3 = new TGraph(n,zen,cdepthr[3]);
  grr3->SetMarkerStyle(6);
  grr3->SetLineWidth(2);
  grr3->SetMarkerColor(4);
  grr3->SetLineColor(4);

  TCanvas *c2 = new TCanvas("c2","test",200,10,600,500);
  //gr->Draw("ACP");
  grr0->Draw("APL");
  grr1->Draw("PL");
  grr2->Draw("PL");
  //grr3->Draw("PL");

  leg->Draw();

}



//______________________________________________________________
void EarthDensity(string earthmodel = "PREM", string crustmodel = "PREM_mmc", 
                  double minrad = 0, 
                  double earthrad = 6.4e6,
                  double coredensity = -1.,
                  double earthdensity = -1.)
{ 

  // you must link library on current directory
  gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");
  I3NuGStaticParamsSetter::SetPath(string("../earthparams"));
  I3NuGStaticParamsSetter::SetEarthModel(earthmodel);
  I3NuGStaticParamsSetter::SetCrustModel(crustmodel);

  if (coredensity > 0) {
     I3NuGStaticParamsSetter::SetCoreDensity(coredensity);
  }
  if (earthdensity > 0) {
     I3NuGStaticParamsSetter::SetEarthDensity(earthdensity);
  }


  const int n=10000;
  double radbin= (earthrad - minrad)/n;

  Double_t      rad[n];
  Double_t      den[n];
 
  for(int i=0; i<n; ++i) { 
     rad[i]=i*radbin + minrad;
     den[i]=I3NuGInteractionInfo::GetEarthDensity(rad[i], 0, 0);
  }
  
  TGraph *gr = new TGraph(n,rad,den);

  char title[2048];
  sprintf("Earth Density Model %s %s", earthmodel.c_str(), crustmodel.c_str());

  gr->SetTitle(title);
  gr->SetMarkerStyle(8);
  gr->SetLineWidth(2);
  gr->SetMarkerColor(1);
  gr->SetLineColor(1);
  gr->GetXaxis()->SetTitle("Radius from the Earth Center [m]");
  gr->GetYaxis()->SetTitle("Density [g/cm^{3}]");

  TCanvas *c1 = new TCanvas("c1","test",200,10,600,500);
  gr->SetMarkerStyle(1);
  //gr->Draw("ACP");
  gr->Draw("APL");

}

//______________________________________________________________
void EarthDensityAll(){ 

  const int n=10000;
  double radmax = 6.4e6;
  double radmin = 0;
  double radbin= (radmax - radmin)/n;

  // you must link library on current directory
  gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");
  I3NuGStaticParamsSetter::SetPath(string("../earthparams"));

  Double_t      rad[n];
  Double_t      coszen[n];
  Double_t      den[2][n];
 
  // PREM + PREM_mmc
  I3NuGStaticParamsSetter::SetEarthModel("PREM");
  I3NuGStaticParamsSetter::SetCrustModel("PREM_mmc");
  //I3NuGStaticParamsSetter::SetEarthDensity(earthdensity);
  for(int i=0; i<n; ++i) { 
     rad[i]=i*radbin + radmin;
     double sinzen = rad[i] / radmax;
     coszen[i] = - (TMath::Sqrt((1-sinzen)*(1+sinzen)));
     den[0][i]=I3NuGInteractionInfo::GetEarthDensity(rad[i], 0, 0);  // PREM
  }

  // flatcore + PREM_mmc
  I3NuGStaticParamsSetter::SetEarthModel("PREM_FLATCORE");
  I3NuGStaticParamsSetter::SetCrustModel("PREM_mmc");
  for(int i=0; i<n; ++i) { 
     rad[i]=i*radbin + radmin;
     den[1][i]=I3NuGInteractionInfo::GetEarthDensity(rad[i], 0, 0);  // PREM_FLATCORE
  }

  TCanvas *c1 = new TCanvas("c1","c1", 10, 10, 800, 400);
  c1->Divide(2, 1);
  c1->cd(1);

  TGraph *gr0 = new TGraph(n,rad,den[0]);
  gr0->SetName("PREM");
  gr0->SetTitle("PREM");
  //gr0->SetMarkerStyle(8);
  gr0->SetLineWidth(2);
  gr0->SetMarkerColor(kGreen+1);
  gr0->SetLineColor(kGreen+1);
  gr0->GetXaxis()->SetTitle("Radius from the Earth Center [m]");
  gr0->GetYaxis()->SetTitle("Density [g/cm^{3}]");

  TGraph *gr1 = new TGraph(n,rad,den[1]);
  gr1->SetName("FLATCORE");
  gr1->SetTitle("FLATCORE");
  //gr1->SetMarkerStyle(8);
  gr1->SetLineWidth(2);
  gr1->SetMarkerColor(kViolet+1);
  gr1->SetLineColor(kViolet+1);

  //gr->Draw("ACP");
  gr0->Draw("APL");
  gr1->Draw("PL");

  TLegend *leg = new TLegend(0.6, 0.7, 0.9, 0.9);
  leg->AddEntry(gr0, "PREM", "pl");
  leg->AddEntry(gr1, "FLATCORE", "pl");
  leg->Draw();

  c1->cd(2);

  TGraph *gr2 = new TGraph(n,coszen,den[0]);
  gr2->SetName("PREM2");
  gr2->SetTitle("PREM2");
  //gr2->SetMarkerStyle(8);
  gr2->SetLineWidth(2);
  gr2->SetMarkerColor(kGreen+1);
  gr2->SetLineColor(kGreen+1);
  gr2->GetXaxis()->SetTitle("cos(zenith)");
  gr2->GetYaxis()->SetTitle("Density [g/cm^{3}]");

  TGraph *gr3 = new TGraph(n,coszen,den[1]);
  gr3->SetName("FLATCORE2");
  gr3->SetTitle("FLATCORE2");
  //gr3->SetMarkerStyle(8);
  gr3->SetLineWidth(2);
  gr3->SetMarkerColor(kViolet+1);
  gr3->SetLineColor(kViolet+1);

  //gr->Draw("ACP");
  gr2->Draw("APL");
  gr3->Draw("PL");

  //TLegend *leg = new TLegend(0.1, 0.6, 0.5, 0.9);
  //leg->AddEntry(gr0, "PREM", "pl");
  //leg->AddEntry(gr1, "PREM_FLATCORE", "pl");
  //leg->Draw();

}
 

