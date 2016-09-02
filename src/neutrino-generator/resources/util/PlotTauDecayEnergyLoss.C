//eta vs theta
static const double TAU_CTE = 4.9021097E-5;   //c*\tau_\tau /m_\tau in m/GeV
static const double C_LIGHT = 2.9979246E8;//m/s
static const double M_TAU = 1.777;//GeV
static const double rock_density = 10.00*100;//g/cm*cm*m
static const  int   stepN = 10000;
static const int energyBinNumber = 80;
static const int sampleN = 100;

void PlotTauDecayEnergyLoss(){ 

  //multiply TAU_CTE by E_\tau gives observed mean range in the lab frame

  TF1 *f1 = new TF1("f1","1",0,1);

  Double_t maxEnergyLog=10.0, minEnergyLog=2.0;

  Double_t energyBinSize= (maxEnergyLog - minEnergyLog)/energyBinNumber;

  cout << energyBinNumber*(sampleN+1) << endl;

  Double_t energy[energyBinNumber*(sampleN+1)];
  Double_t range[energyBinNumber*(sampleN+1)];
  Double_t rangeWithLoss[energyBinNumber*(sampleN+1)];
  Double_t rangeLost[energyBinNumber*(sampleN+1)];
  Double_t energyLost[energyBinNumber*(sampleN+1)];
  Double_t lifetime[energyBinNumber*(sampleN+1)];
  Double_t beta[energyBinNumber*(sampleN+1)];

  TH1D* TauLife = new TH1D("TauLife","#tau Life Time [#mus]",100, 0, 0.00001);
  //TH1D* TauBeta = new TH1D("beta","#beta*10^{7}",100, 0, 100);

  int binN=0;
  for(int i=0; i<sampleN; ++i){
    
    for(int j=0; j<energyBinNumber; ++j)
      {
	
	binN=i*energyBinNumber+j;
	energy[binN]=minEnergyLog+j*energyBinSize;

	double randomN = f1->GetRandom();
	TauLife->Fill(GetDecayTime(randomN));
	double this_e = pow(10,energy[binN]);
	//TauBeta->Fill(GetBeta(this_e)*1e7);

	//cout << this_e <<":"<<GetBeta(this_e)*1e7<< endl;
	lifetime[binN]=GetDecayTime(randomN);
	range[binN]=GetDecayRange(energy[binN],GetDecayTime(randomN));
	
	rangeWithLoss[binN]=GetDecayRangeLoss(energy[binN],GetDecayTime(randomN));	
	energyLost[binN]=log10(GetDecayEnergyLoss(energy[binN],GetDecayTime(randomN)));

	rangeLost[binN]=range[binN]-rangeWithLoss[binN];
	//energyLost[i*sampleN+j]=GetDecayEnergyLoss(energy[i*sampleN+j],GetDecayTime(randomN))/pow(10,energy[i*sampleN+j]);

	
      }
  }  
  
  TGraph *gr = new TGraph(energyBinNumber*sampleN, energy, range);
  TGraph *grL = new TGraph(energyBinNumber*sampleN, energy, rangeWithLoss);
  TGraph *grLost = new TGraph(energyBinNumber*sampleN, energy, rangeLost);
  TGraph *grELost = new TGraph(energyBinNumber*sampleN, energy, energyLost);

  TGraph *grTime = new TGraph(energyBinNumber*sampleN, lifetime, rangeWithLoss);
  //TGraph *grTime = new TGraph(energyBinNumber*sampleN, lifetime, energyLost);

  gr->SetMarkerStyle(2);
  gr->GetXaxis()->SetTitle("log(Initial #tau Energy at Production [GeV])");
  gr->SetTitle("#tau Range without Energy-loss");
  gr->GetYaxis()->SetTitle("#tau range [m]");
  gr->GetYaxis()->SetTitleOffset(1.8);

  grL->SetMarkerStyle(7);
  grL->GetXaxis()->SetTitle("log(Initial #tau Energy at Production [GeV])");
  grL->SetTitle("#tau Range WITH Energy-loss (in rock)");
  grL->GetYaxis()->SetTitle("#tau range [m] with energy loss in rock");
  grL->GetYaxis()->SetTitleOffset(1.8);

  grELost->SetMarkerStyle(2);
  grELost->GetXaxis()->SetTitle("log(Initial #tau Energy at Production [GeV])");
  grELost->SetTitle("#tau energy after propagation with an energy-loss in rock");
  grELost->GetYaxis()->SetTitle("log(#tau energy [GeV]) with an energy loss in rock");
  grELost->GetYaxis()->SetTitleOffset(1.8);

  
    grLost->SetMarkerStyle(3);
    grLost->SetMarkerStyle(2);
    grLost->SetMarkerStyle(1);

    grLost->GetXaxis()->SetTitle("log(Initial #tau Energy at Production [GeV])");
    grLost->SetTitle("distance lost due to an energy-loss in rock");
    grLost->GetYaxis()->SetTitle("distance lost due to an energy loss in rock");
    grLost->GetYaxis()->SetTitleOffset(1.8);
  

  grTime->GetXaxis()->SetTitle("#tau lifetime [#mu s]");
  grTime->SetTitle("distance with energy loss");
  grTime->GetYaxis()->SetTitle("distance with energy loss");
  grTime->GetYaxis()->SetTitleOffset(1.8);

  TCanvas *c1 = new TCanvas("c1","test",200,10,1000,800);
  c1->Divide(3,2, 0.0001, 0.0001);
  ////////
  c1->cd(1);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTopMargin(0.12);
  gr->Draw("AP");
  TF1 *r1 = new TF1("r1","49*1e-6*pow(10,x)",4,10);
  r1->SetLineColor(kRed);
  r1->Draw("same");

  ////////
  c1->cd(2);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTopMargin(0.12);
  grL->Draw("AP");
  r1->Draw("same");

  ////////
  c1->cd(3);
  TauLife->GetXaxis()->SetTitle("MC #tau Life Time [#mus]");
  TauLife->Draw();



  c1->cd(4);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTopMargin(0.12);
  grELost->Draw("AP");



  c1->cd(5);
  gPad->SetLeftMargin(0.15);
  gPad->SetRightMargin(0.05);
  gPad->SetTopMargin(0.12);
  grLost->Draw("AP");
  //TauBeta->Draw();

  c1->cd(6);
  grTime->Draw("AP");

	c1->Print("tau_decay_step100-2.gif");
}
////////////////////////////////////////////////////////////////////////
double GetDecayTime(double rand){

  //[(c * tau_tau)/ m_tau] in m/GeV
  //lifetime returns tau lifetime *c/m_tau

  double lifetime = - TAU_CTE*log(rand);//=lifetime*(c/m_tau=3e8/1.77[m/(s*GeV)])[m/GeV]  
  return lifetime*1e6/C_LIGHT*M_TAU;//in micro-second
}
////////////////////////////////////////////////////////////////////////
double GetDecayRange(double energy, double lifetime){
  
  double lifetime = lifetime*1e-6;//second
  double range = pow(10,energy)*C_LIGHT/M_TAU * lifetime; //GeV*(m/s)/GeV*s=m  
  return range; //m
}
////////////////////////////////////////////////////////////////////////
double GetDecayRangeLoss(double eLog, double lifetime){
  //given lifetime for this particle
  double lifetime = lifetime*1e-6;//in second
  double initial_energy = pow(10,eLog);

  double delta_t = lifetime/stepN;
  double energy = initial_energy;
  double distance=0;

  for(int i=0; i<stepN; ++i){
    //cout << energy << endl;
    double delta_d = delta_t*C_LIGHT*energy/M_TAU;
    double delta_e = energy*delta_d*GetBeta(energy)*rock_density;
    distance += delta_d;
    if(energy<delta_e){energy=0;break;} 
    energy -= delta_e;
  }
  return distance; //m
}///////////////////////////////////////////////////////
double GetDecayEnergyLoss(double eLog, double lifetime){

  //given lifetime for this particle
  double lifetime = lifetime*1e-6;//in second
  double initial_energy = pow(10,eLog);

  double delta_t = lifetime/stepN;
  double energy = initial_energy;
  double distance=0;

  for(int i=0; i<stepN; ++i){
    
    double delta_d = delta_t*C_LIGHT*energy/M_TAU;
    double delta_e = energy*delta_d*GetBeta(energy)*rock_density;
    distance += delta_d;
    if(energy<delta_e){energy=0;break;} 
    energy -= delta_e;
  }
  //return energy after energy loss
  return energy; //m
}
////////////////////////////////////////////////////////////////////////
// Beta(E) = (1/E)*(dE/dX) in cm^2/g
// 8e-9                       is due to bremstrahlung
// 1.5e-7                     is due to pair-production
// 6.3*10^{-7}*(E/10^9)^(0.2) is due to photonuclear scattering
///////////////////////////////////////////////////////////////////////
double GetBeta(double energy) {//energy in GeV

  if ( energy > 1e4)  //old parametrization

    return (1.508 + 6.3*pow((energy/1e9),0.2))*1e-7;
  else
    return 2.138e-7;
  //      return (0.16 + 0.069*log10(E))*1e-6; // from hep-ph/0308042
}
///////////////////////////////////////////////////////////////////////
