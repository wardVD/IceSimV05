void PlotTauDecayFinalStateFile(){

  //log energy is given by 1 + energybinN*0.1
  gSystem->Clear();
  string dir(getenv("I3_BUILD"));
  dir +="/neutrino-generator/resources/cross_section_data/cteq5/";

  string file1 = dir + "tau_decay_tauola.dat";
  char* buf1 = file1.c_str(); 
  ifstream mFile1;
  
  mFile1.open(buf1);    
  if (mFile1.is_open() == 0)cout << "file not found: "  << buf1 << endl;
  else cout << "file found: "  << buf1 << endl; 
  
   char tDum[64];
   int Nsample, Ndecay;

   mFile1 >> Nsample;//10000              
   mFile1 >> Ndecay; //6        
   mFile1 >> tDum;
   mFile1 >> tDum >>tDum;
   
   static const int nxbin = 11000;
   static const int nybin = 10;
   Double_t a1[nxbin][nybin];

   int nline=0;
   for (int i=0; i<Nsample; i++)
     for (int j = 0; j < Ndecay; j++)
       mFile1 >> a1[i][j];
   
   if (mFile1.fail()){
     log_fatal("Something has gone wrong with reading file: %s", infile.c_str());
     exit(EXIT_FAILURE);
   }
   mFile1.close();
   
   Double_t x0[nxbin];
   Double_t x1[nxbin];
   Double_t x2[nxbin];
   Double_t x3[nxbin];
   Double_t x4[nxbin];
   Double_t x5[nxbin];
   Double_t x6[nxbin];
   Double_t x7[nxbin];
   
   TH1D* nu_tau = new TH1D("nu_tau", "nu tau", 100,0,1);
   TH1D* other = new TH1D("other", "other", 100,0,1);
   TH2D* other2 = new TH2D("other2", "other2", 100,0,1, 100,0,1);
   int count =0;
   for(int j=0; j<Nsample; ++j){
     x0[j] = a1[j][0];
     x1[j] = a1[j][1];
     x2[j] = a1[j][2];
     x3[j] = a1[j][3];
     x4[j] = a1[j][4];
     x5[j] = a1[j][5];
     x6[j] = a1[j][0]+a1[j][1]+a1[j][2]+a1[j][3]+a1[j][4]+a1[j][5];
     x7[j] = a1[j][1]+a1[j][2]+a1[j][3]+a1[j][4]+a1[j][5];
     //cout << x1[j]<<":"<< x2[j]<<":"<< x3[j]<<":"<< x4[j]<<":"<< x5[j]<<":"<< x6[j] << endl;
     if(x0[j]>0.0001){++count; cout << x0[j]<<endl;}
     nu_tau->Fill(x0[j]);
     other->Fill(x7[j]);other2->Fill(x0[j],x7[j]);
   }
   cout << "count" << count << endl;
   TGraph *gr1 = new TGraph(1000, x0, x7);//nu_tau, all the other
   TGraph *grsum = new TGraph(1000, x0, x6);//nu_tau, total
   TCanvas *c1 = new TCanvas("c1","c1",800,800);

   c1->Divide(2,2);
      c1->cd(1);
      //gPad->SetLogy();
      //gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      //nu_tau, nu_mu, nu_e, hadron, muon, electron
      c1->cd(1);
      gr1->SetTitle("TAUOLA: RandomSample");
      gr1->GetXaxis()->SetTitle("nu tau");
      gr1->GetYaxis()->SetTitle("all the other sum");
      gr1->Draw("PA");     
      //other2->Draw();
      c1->cd(2);
      grsum->SetTitle("TAUOLA: RandomSample");
      grsum->GetXaxis()->SetTitle("nu tau");
      grsum->GetYaxis()->SetTitle("total sum");
      grsum->Draw("PA");   
      c1->cd(3);
      nu_tau->SetTitle("TAUOLA: RandomSample: nu_tau");
      nu_tau->GetXaxis()->SetTitle("nu_tau");
      nu_tau->Draw("");       
      c1->cd(4);
      other->SetTitle("TAUOLA: RandomSample: all the other sum");
      other->GetXaxis()->SetTitle("all the other sum");
      other->Draw("");       

}
