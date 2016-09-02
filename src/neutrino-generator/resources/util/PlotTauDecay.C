//eta vs theta
static const double TAU_CTE=4.9021097E-5;   //c*\tau_\tau /m_\tau in m/GeV
static const double C_LIGHT=2.9979246E8;//m/s
static const double M_TAU=1.777;//GeV
static const double DENSITY=6;//g/cm3
void PlotTauDecay(){ 

  //multiply TAU_CTE by E_\tau gives observed mean range in the lab frame
  
  TF1 *f1 = new TF1("f1","1",0,1);
  
  Double_t maxEnergyLog=10.0, minEnergyLog=4.0;
  static const int energyBinNumber = 60;
  static const int sampleN = 60;
  Double_t energyBinSize= (maxEnergyLog - minEnergyLog)/energyBinNumber;
  Double_t energy[energyBinNumber*sampleN+1];
  Double_t range[energyBinNumber*sampleN+1];
  Double_t rangeloss[energyBinNumber*sampleN+1];

  TH1D* TauLife = new TH1D("TauLife","Tau Life Time [#mus]",100, 0, 0.00001);

  TH1D* TauDiff= new TH1D("TauDiff","Tau Diff",6000, -5000, 1000);

  for(int i=0; i<sampleN; ++i){
    
    for(int j=0; j<energyBinNumber; ++j)
      {
	double randomN = f1->GetRandom();
	energy[i*sampleN+j]=minEnergyLog+j*energyBinSize;
	range[i*sampleN+j]=GetDecayRange(energy[i*sampleN+j],randomN);
	TauLife->Fill(GetDecayTime(randomN));
	rangeloss[i*sampleN+j]=GetDecayRangeWithEnergyLoss(energy[i*sampleN+j], GetDecayRange(energy[i*sampleN+j],randomN));
	TauDiff->Fill(range[i*sampleN+j]-rangeloss[i*sampleN+j]);
      }
  }  
  
  TGraph *gr = new TGraph(energyBinNumber*sampleN, energy, range);
  TGraph *grloss = new TGraph(energyBinNumber*sampleN, energy, rangeloss);
  TCanvas *c1 = new TCanvas("c1","test",200,10,600,600);

  c1->Divide(2,2);
  c1->cd(1);
  gr->GetXaxis()->SetTitle("log(Initial Tau Energy at Production [GeV])");
  gr->SetTitle("Tau Range without Energy-loss");
  gr->GetYaxis()->SetTitle("log(#tau range [m]) used by neutrino-generator");
  gr->GetYaxis()->SetTitleOffset(1.8);
  gr->SetMarkerStyle(2);
  gr->Draw("AP");
  c1->cd(2);
  TauLife->GetXaxis()->SetTitle("#mus");
  TauLife->Draw();
  c1->cd(3);
  grloss->GetXaxis()->SetTitle("log(Initial Tau Energy at Production [GeV])");
  grloss->SetTitle("Tau Range with Energy-loss");
  grloss->GetYaxis()->SetTitle("log(#tau range [m]) used by neutrino-generator");
  grloss->SetMarkerStyle(2);
  grloss->GetYaxis()->SetTitleOffset(1.8);
  grloss->Draw("AP"); 
  c1->cd(4);
  TauDiff->Draw();
}
////////////////////////////////////////////////////////////////////////
double GetDecayTime(double rand){
  double range =0.0;// [(c * tau_tau)/ m_tau] in m/GeV
  //lifetime returns tau lifetime *c/m_tau
  double lifetime = - TAU_CTE*log(rand);//=lifetime*(c/m_tau=3e8/1.77[m/(s*GeV)])[m/GeV]
  //TF1 *f2 = new TF1("f2","10000*exp(x/4.9021097/100000)",0,10);
  //range = TAU_CTE*log(rand);
  
  return lifetime/C_LIGHT*M_TAU*1e6;//in micro-second
}
////////////////////////////////////////////////////////////////////////
double GetDecayRange(double energy, double rand){

  double lifetime = GetDecayTime(rand)*1e-6;//second
  double range = pow(10,energy)*lifetime*C_LIGHT/M_TAU; //m  
  return range; 
}
////////////////////////////////////////////////////////////////////////
// Beta(E) = (1/E)*(dE/dX) in cm^2/g
// 8e-9                       is due to bremstrahlung
// 1.5e-7                     is due to pair-production
// 6.3*10^{-7}*(E/10^9)^(0.2) is due to photonuclear scattering
///////////////////////////////////////////////////////////////////////
double GetBeta(double E) {

  if ( E > 1E4)  //old parametrization
    //return (1.508 + 6.3*pow((E/1e9),0.2))*1e-7;
    return (1.508 + 6.3*pow((E),0.2))*1e-7;
  else
    return 2.138e-7;
  //      return (0.16 + 0.069*log10(E))*1e-6; // from hep-ph/0308042
}
///////////////////////////////////////////////////////////////////////
double GetDecayRangeWithEnergyLoss(double E, double origrange) {
  //At first calculate a distance in which initial energy E become E*frac
  //where frac is a small const for example 0.98.
  //the distance that energy E become E*frac is 
  //assuming Delta E = E - E*frac sim beta*E*distance*density
  //next step Delta E' = E*frac(1-frac)=beta*E*frac*distance*density
  //distance = (1-frac)/(beta*density)
  //now the time spent by propagationg the distance is distance/C_LIGHT
  //if lifetime - distance/C_LIGHT >0 we will proceed one more step
  //else back to previous step and we have the (frac)^step*energy, lifetime and the range
  double density = 6.0;//g/cm3
  double Lifetime = M_TAU*origrange/C_LIGHT/pow(10,E);//s
  //double range = energy*lifetime*C_LIGHT/M_TAU; //m  
  //return range; 
  double Energy = pow(10,E);
  double efrac=0.99;//1/pow(10,0.2);//~0.62
  double beta = GetBeta(Energy);
  double lossrange = 0.01*(1-efrac)/(density*beta);//(g/cm2)*(cm3/g)=cm/100=m

  while(Lifetime>(lossrange/C_LIGHT)){
    double beta = GetBeta(Energy);
    lossrange += 0.01*(1-efrac)/(density*beta);//(g/cm2)*(cm3/g)
    Energy = Energy*efrac;
  }

  return lossrange;
}
///////////////////////////////////////////////////////////////////////
