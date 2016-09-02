
void PlotFinalStateFiles(){

  //log energy is given by 1 + energybinN*0.1
  //gSystem->Clear(); // don't call it, makes segfault!

   // at first fills 3D array

   //number of energy bins
   //static const int nEbins = 120;
   static const int nEbins = 111;

   //number os states per energy bin
   //static const int nStatebins = 1100;
   static const int nStatebins = 1000;

   //number of return values
   //static const int nReturnbins = 10;
   static const int nReturnbins = 2;

   static Double_t a1[nEbins][nStatebins][nReturnbins];
   static Double_t a2[nEbins][nStatebins][nReturnbins];
   static Double_t a3[nEbins][nStatebins][nReturnbins];
   static Double_t a4[nEbins][nStatebins][nReturnbins];

   static Double_t x1_100GeV[nStatebins];
   static Double_t y1_100GeV[nStatebins];
   static Double_t x1[nStatebins];
   static Double_t y1[nStatebins];
   static Double_t xx1[nStatebins];
   static Double_t yy1[nStatebins];
   static Double_t xxx1[nStatebins];
   static Double_t yyy1[nStatebins];
   static Double_t xxxx1[nStatebins];
   static Double_t yyyy1[nStatebins];

   static Double_t x2_100GeV[nStatebins];
   static Double_t y2_100GeV[nStatebins];
   static Double_t x2[nStatebins];
   static Double_t y2[nStatebins];
   static Double_t xx2[nStatebins];
   static Double_t yy2[nStatebins];
   static Double_t xxx2[nStatebins];
   static Double_t yyy2[nStatebins];
   static Double_t xxxx2[nStatebins];
   static Double_t yyyy2[nStatebins];

   static Double_t x3[nStatebins];
   static Double_t y3[nStatebins];
   static Double_t xx3[nStatebins];
   static Double_t yy3[nStatebins];
   static Double_t xxx3[nStatebins];
   static Double_t yyy3[nStatebins];
   static Double_t xxxx3[nStatebins];
   static Double_t yyyy3[nStatebins];

   static Double_t x4[nStatebins];
   static Double_t y4[nStatebins];
   static Double_t xx4[nStatebins];
   static Double_t yy4[nStatebins];
   static Double_t xxx4[nStatebins];
   static Double_t yyy4[nStatebins];
   static Double_t xxxx4[nStatebins];
   static Double_t yyyy4[nStatebins];

   static Double_t  x1theta_100GeV[nStatebins];
   static Double_t  x1theta[nStatebins];
   static Double_t  xx1theta[nStatebins];
   static Double_t  xxx1theta[nStatebins];
   static Double_t  xxxx1theta[nStatebins];
   static Double_t  x2theta[nStatebins];
   static Double_t  xx2theta[nStatebins];
   static Double_t  xxx2theta[nStatebins];
   static Double_t  xxxx2theta[nStatebins];
   static Double_t  x3theta[nStatebins];
   static Double_t  xx3theta[nStatebins];
   static Double_t  xxx3theta[nStatebins];
   static Double_t  xxxx3theta[nStatebins];
   static Double_t  x4theta[nStatebins];
   static Double_t  xx4theta[nStatebins];
   static Double_t  xxx4theta[nStatebins];
   static Double_t  xxxx4theta[nStatebins];

   static Double_t  y1out_100GeV[nStatebins];
   static Double_t  y1out[nStatebins];
   static Double_t  yy1out[nStatebins];
   static Double_t  yyy1out[nStatebins];
   static Double_t  yyyy1out[nStatebins];
   static Double_t  y2out[nStatebins];
   static Double_t  yy2out[nStatebins];
   static Double_t  yyy2out[nStatebins];
   static Double_t  yyyy2out[nStatebins];
   static Double_t  y3out[nStatebins];
   static Double_t  yy3out[nStatebins];
   static Double_t  yyy3out[nStatebins];
   static Double_t  yyyy3out[nStatebins];
   static Double_t  y4out[nStatebins];
   static Double_t  yy4out[nStatebins];
   static Double_t  yyy4out[nStatebins];
   static Double_t  yyyy4out[nStatebins];

  //string dir(getenv("I3_PORTS"));
  //dir +="/anis-1.0/crosssections/cteq5/";
  string dir = "./anis.v1.8.2/data/cteq5/";

  string file1 = dir + "final_cteq5_cc_nu.data";
  string file2 = dir + "final_cteq5_cc_nubar.data";
  string file3 = dir + "final_cteq5_nc_nu.data";
  string file4 = dir + "final_cteq5_nc_nubar.data";
  char* buf1 = file1.c_str(); 
  char* buf2 = file2.c_str(); 
  char* buf3 = file3.c_str(); 
  char* buf4 = file4.c_str(); 

      ifstream mFile1;
      ifstream mFile2;
      ifstream mFile3;
      ifstream mFile4;

      //energy range
      double emax=0, emin=0;
      //number of energy bin
      int nEnergies=0;
      //number of final states per energy bin
      int nStates=0;
      //number of return values
      int nReturns=0;

      char tDum[64];
      float tValues1;
      float tValues2;
      float tValues3;
      float tValues4;

      mFile1.open(buf1);    
      mFile2.open(buf2);    
      mFile3.open(buf3);    
      mFile4.open(buf4);    
      if (mFile1.is_open() == 0)cout << "file not found: "  << buf1 << endl;
      else cout << "file found: "  << buf1 << endl; 
      if (mFile2.is_open() == 0)cout << "file not found: "  << buf2 << endl;
      else cout << "file found: "  << buf2 << endl; 
      if (mFile3.is_open() == 0)cout << "file not found: "  << buf3 << endl;
      else cout << "file found: "  << buf3 << endl; 
      if (mFile4.is_open() == 0)cout << "file not found: "  << buf4 << endl;
      else cout << "file found: "  << buf4 << endl; 

  int energybin100GeV=10;//100GeV 10^2
  int energybin=20;//1TeV 10^3
  int energybin2=50;//1PeV 10^6
  int energybin3=80; //1EeV 10^9
  int energybin4=100; //100EeV10^11

      //the first line is ignored
      cout << "energy log is: "  << 1 + energybin*0.1 <<endl;

      mFile1 >> nStates; 
      cout << "number of final states per energy bin: " << nStates << endl;
      mFile1 >> nReturns; //always 2
      cout << "number of values to be returned: " << nReturns << endl;

      mFile1 >> nEnergies; 
      cout << "number of energy bin: "<<nEnergies << endl;

      mFile1 >> emin; //emin
      cout << "minimum energy range: "<< emin << endl;
      mFile1 >> emax; //emax
      cout << "maximum energy range: "<< emax << endl;
      Double_t binsize = (emax-emin)/(nEnergies-1) ;

      mFile2 >> tDum; 
      mFile2 >> tDum; 
      mFile2 >> tDum;  
      mFile2 >> tDum; 
      mFile2 >> tDum; 

      mFile3 >> tDum; 
      mFile3 >> tDum; 
      mFile3 >> tDum; 
      mFile3 >> tDum; 
      mFile3 >> tDum; 

      mFile4 >> tDum; 
      mFile4 >> tDum; 
      mFile4 >> tDum; 
      mFile4 >> tDum; 
      mFile4 >> tDum; 


      int nline=0;
      for(int i=0; i<nEnergies; ++i){
	for(int j=0; j<nStates; ++j){
	  for(int k=0; k<nReturns; ++k){
	  mFile1 >> tValues1;
	  a1[i][j][k]=(tValues1);
	  mFile2 >> tValues2;
	  a2[i][j][k]=(tValues2);
	  mFile3 >> tValues3;
	  a3[i][j][k]=(tValues3);
	  mFile4 >> tValues4;
	  a4[i][j][k]=(tValues4);
	}}}

      mFile1.close();    
      mFile2.close();    
      mFile3.close();    
      mFile4.close();    
 

      for(int j=0; j<nStates; ++j){

	x1_100GeV[j] = a1[energybin100GeV][j][0];
	y1_100GeV[j] = a1[energybin100GeV][j][1];

	x1[j] = a1[energybin][j][0];
	y1[j] = a1[energybin][j][1];
	//cout << "x value=" << x1[j] << endl; 
	//cout << "y value=" << y1[j] << endl; 

	xx1[j] = a1[energybin2][j][0];
	yy1[j] = a1[energybin2][j][1];
	xxx1[j] = a1[energybin3][j][0];
	yyy1[j] = a1[energybin3][j][1];
	xxxx1[j] = a1[energybin4][j][0];
	yyyy1[j] = a1[energybin4][j][1];

	x2_100GeV[j] = a2[energybin100GeV][j][0];
	y2_100GeV[j] = a2[energybin100GeV][j][1];

	x2[j] = a2[energybin][j][0];
	y2[j] = a2[energybin][j][1];
	xx2[j] = a2[energybin2][j][0];
	yy2[j] = a2[energybin2][j][1];
	xxx2[j] = a2[energybin3][j][0];
	yyy2[j] = a2[energybin3][j][1];
	xxxx2[j] = a2[energybin4][j][0];
	yyyy2[j] = a2[energybin4][j][1];
	//cout << "x2 value=" << x2[j] << endl; 
	//cout << "y2 value=" << y2[j] << endl; 

	x3[j] = a3[energybin][j][0];
	y3[j] = a3[energybin][j][1];
	xx3[j] = a3[energybin2][j][0];
	yy3[j] = a3[energybin2][j][1];
	xxx3[j] = a3[energybin3][j][0];
	yyy3[j] = a3[energybin3][j][1];
	xxxx3[j] = a3[energybin4][j][0];
	yyyy3[j] = a3[energybin4][j][1];
	//cout << "x3 value=" << x3[j] << endl; 
	//cout << "y3 value=" << y3[j] << endl; 

	x4[j] = a4[energybin][j][0];
	y4[j] = a4[energybin][j][1];
	xx4[j] = a4[energybin2][j][0];
	yy4[j] = a4[energybin2][j][1];
	xxx4[j] = a4[energybin3][j][0];
	yyy4[j] = a4[energybin3][j][1];
	xxxx4[j] = a4[energybin4][j][0];
	yyyy4[j] = a4[energybin4][j][1];
	//cout << "x4 value=" << x4[j] << endl; 
	//cout << "y4 value=" << y4[j] << endl; 
      }

      cerr << "flag1 " << endl;
      TGraph *gr1 = new TGraph(nStatebins, x1, y1);
      gr1->SetNameTitle("gr1","gr1");
      gr1->GetXaxis()->SetTitle("x CTEQ5 1TeV");
      gr1->GetYaxis()->SetTitle("y");

      TGraph *ggr1 = new TGraph(nStatebins, xx1, yy1);
      ggr1->SetNameTitle("ggr1","ggr1");
      ggr1->GetXaxis()->SetTitle("x CTEQ5 1PeV");
      ggr1->GetYaxis()->SetTitle("y");

      TGraph *gggr1 = new TGraph(nStatebins, xxx1, yyy1);
      gggr1->SetNameTitle("gggr1","gggr1");
      gggr1->GetXaxis()->SetTitle("x CTEQ5 1EeV");
      gggr1->GetYaxis()->SetTitle("y");

      TGraph *ggggr1 = new TGraph(nStatebins, xxxx1, yyyy1);
      ggggr1->SetNameTitle("ggggr1","ggggr1");
      ggggr1->GetXaxis()->SetTitle("x CTEQ5 100EeV");
      ggggr1->GetYaxis()->SetTitle("y");


      TGraph *gr2 = new TGraph(nStatebins, x2, y2);
      gr2->SetNameTitle("gr2","gr2");
      TGraph *ggr2 = new TGraph(nStatebins, xx2, yy2);
      ggr2->SetNameTitle("ggr2","ggr2");
      TGraph *gggr2 = new TGraph(nStatebins, xxx2, yyy2);
      gggr2->SetNameTitle("gggr2","gggr2");
      TGraph *ggggr2 = new TGraph(nStatebins, xxxx2, yyyy2);
      ggggr2->SetNameTitle("ggggr2","ggggr2");
      TGraph *gr3 = new TGraph(nStatebins, x3, y3);
      gr3->SetNameTitle("gr3","gr3");
      TGraph *ggr3 = new TGraph(nStatebins, xx3, yy3);
      ggr3->SetNameTitle("ggr3","ggr3");
      TGraph *gggr3 = new TGraph(nStatebins, xxx3, yyy3);
      gggr3->SetNameTitle("gggr3","gggr3");
      TGraph *ggggr3 = new TGraph(nStatebins, xxxx3, yyyy3);
      ggggr3->SetNameTitle("ggggr3","ggggr3");
      TGraph *gr4 = new TGraph(nStatebins, x4, y4);
      gr4->SetNameTitle("gr4","gr4");
      TGraph *ggr4 = new TGraph(nStatebins, xx4, yy4);
      ggr4->SetNameTitle("ggr4","ggr4");
      TGraph *gggr4 = new TGraph(nStatebins, xxx4, yyy4);
      gggr4->SetNameTitle("gggr4","gggr4");
      TGraph *ggggr4 = new TGraph(nStatebins, xxxx4, yyyy4);
      ggggr4->SetNameTitle("ggggr4","ggggr4");

//	int energybin=20;//1TeV 10^3
//	int energybin2=50;//1PeV 10^6
//	int energybin3=80; //1EeV 10^9
//	int energybin4=100; //100EeV10^11

      double E0=1e2; //[GeV]      
      double E1=1e3; //[GeV]      
      double E2=1e6;
      double E3=1e9;
      double E4=1e11;

      cerr << "flag2 " << endl;
      const double m_N = 0.939;//[GeV] 
      for(int j=0; j<nStates; ++j){

	y1out_100GeV[j] = (1-y1_100GeV[j])*E0;

	y1out[j] = (1-y1[j])*E1;
	yy1out[j] = (1-yy1[j])*E2;
	yyy1out[j] = (1-yyy1[j])*E3;
	yyyy1out[j] = (1-yyyy1[j])*E4;

	x1theta_100GeV[j] = x1_100GeV[j]*y1_100GeV[j]*m_N/y1out_100GeV[j];
	x1theta[j] = x1[j]*y1[j]*m_N/y1out[j];
	xx1theta[j] = xx1[j]*yy1[j]*m_N/yy1out[j];
	xxx1theta[j] = xxx1[j]*yyy1[j]*m_N/yyy1out[j];
	xxxx1theta[j] = xxxx1[j]*yyyy1[j]*m_N/yyyy1out[j];

	y2out[j] = (1-y2[j])*E1;
	yy2out[j] = (1-yy2[j])*E2;
	yyy2out[j] = (1-yyy2[j])*E3;
	yyyy2out[j] = (1-yyyy2[j])*E4;

	x2theta[j] = x2[j]*y2[j]*m_N/y2out[j];
	xx2theta[j] = xx2[j]*yy2[j]*m_N/yy2out[j];
	xxx2theta[j] =  xxx2[j]*yyy2[j]*m_N/yyy2out[j];
	xxxx2theta[j] =  xxxx2[j]*yyyy2[j]*m_N/yyyy2out[j];
      }
      
      cerr << "flag3 " << endl;
      TGraph *gr1theta = new TGraph(nStatebins, x1theta, y1out);
      gr1theta->SetNameTitle("gr1theta","gr1theta");
      gr1theta->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      gr1theta->GetYaxis()->SetTitle("Out-going #mu Energy [GeV]");


      TGraph *ggr1theta = new TGraph(nStatebins, xx1theta, yy1out);
      ggr1theta->SetNameTitle("ggr1theta","ggr1theta");
      TGraph *gggr1theta = new TGraph(nStatebins, xxx1theta, yyy1out);
      gggr1theta->SetNameTitle("gggr1theta","gggr1theta");
      TGraph *ggggr1theta = new TGraph(nStatebins, xxxx1theta, yyyy1out);
      ggggr1theta->SetNameTitle("ggggr1theta","ggggr1theta");
      TGraph *gr2theta = new TGraph(nStatebins, x2theta, y2out);
      gr2theta->SetNameTitle("gr2theta","gr2theta");
      TGraph *ggr2theta = new TGraph(nStatebins, xx2theta, yy2out);
      ggr2theta->SetNameTitle("ggr2theta","ggr2theta");
      TGraph *gggr2theta = new TGraph(nStatebins, xxx2theta, yyy2out);
      gggr2theta->SetNameTitle("gggr2theta","gggr2theta");
      TGraph *ggggr2theta = new TGraph(nStatebins, xxxx2theta, yyyy2out);
      ggggr2theta->SetNameTitle("ggggr2theta","ggggr2theta");

      frame = new TH2D("frame","frame",1000,1e-10,2, 1000,1e-10,2);
      frame->SetStats(0);      
      frame->GetXaxis()->SetTitle("x");
      frame->GetYaxis()->SetTitle("y");
      frame->GetXaxis()->SetTitleOffset(1.2);
      frame21 = new TH2D("frame21","frame21",2000, 0.0, 1e-1, 2000,1e0,1.1*E1);
      frame21->SetStats(0);      
      frame21->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      frame21->GetYaxis()->SetTitle("Out-going #mu Energy [GeV]");
      frame22 = new TH2D("frame22","frame22",2000, 0.0, 1e-5, 2000,E1,1.1*E2);
      frame22->SetStats(0);      
      frame22->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      frame22->GetYaxis()->SetTitle("Out-going #mu Energy [GeV]");
      frame23 = new TH2D("frame23","frame23",2000, 0.0, 1e-12, 2000,E2,1.1*E3);
      frame23->SetStats(0);      
      frame23->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      frame23->GetYaxis()->SetTitle("Out-going #mu Energy [GeV]");
      frame24 = new TH2D("frame24","frame24",2000, 0.0, 1e-15, 2000,E3,1.1*E4);
      frame24->SetStats(0);      
      frame24->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      frame24->GetYaxis()->SetTitle("Out-going #mu Energy [GeV]");
      frame21->GetXaxis()->SetTitleOffset(1.2);
      frame21->GetYaxis()->SetTitleOffset(1.5);
      frame22->GetXaxis()->SetTitleOffset(1.2);
      frame22->GetYaxis()->SetTitleOffset(1.5);
      frame23->GetXaxis()->SetTitleOffset(1.2);
      frame23->GetYaxis()->SetTitleOffset(1.5);
      frame24->GetXaxis()->SetTitleOffset(1.2);
      frame24->GetYaxis()->SetTitleOffset(1.5);

      gr1->SetMarkerColor(1);
      ggr1->SetMarkerColor(1);
      gggr1->SetMarkerColor(1);
      ggggr1->SetMarkerColor(1);
      gr2->SetMarkerColor(2);
      ggr2->SetMarkerColor(2);
      gggr2->SetMarkerColor(2);
      ggggr2->SetMarkerColor(2);
      gr3->SetMarkerColor(3);
      ggr3->SetMarkerColor(3);
      gggr3->SetMarkerColor(3);
      ggggr3->SetMarkerColor(3);
      gr4->SetMarkerColor(4);
      ggr4->SetMarkerColor(4);
      gggr4->SetMarkerColor(4);
      ggggr4->SetMarkerColor(4);


      gr1theta->SetMarkerColor(1);
      ggr1theta->SetMarkerColor(1);
      gggr1theta->SetMarkerColor(1);
      ggggr1theta->SetMarkerColor(1);
      gr2theta->SetMarkerColor(2);
      ggr2theta->SetMarkerColor(2);
      gggr2theta->SetMarkerColor(2);
      ggggr2theta->SetMarkerColor(2);


      cerr << "flag4 " << endl;
      TCanvas *c1 = new TCanvas("c1","test",200,10,600,600);
      c1->Divide(2,2);

      c1->cd(1);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      c1->cd(2);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      c1->cd(3);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      c1->cd(4);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      
      c1->cd(1);
      cerr << "flag4.1" << endl;

      frame->SetTitle("CTEQ5: 1 TeV");
      frame->DrawCopy();
      gr1->Draw("P");     
      gr2->Draw("Psame");
      gr3->Draw("Psame");     
      gr4->Draw("Psame");     


      c1->cd(2);
      frame->SetTitle("CTEQ5: 1 PeV");
      frame->DrawCopy();
      ggr1->Draw("P");     
      ggr2->Draw("Psame");
      ggr3->Draw("Psame");     
      ggr4->Draw("Psame");     

      c1->cd(3);
      frame->SetTitle("CTEQ5: 1 EeV");
      frame->DrawCopy();
      gggr1->Draw("P");     
      gggr2->Draw("Psame");
      gggr3->Draw("Psame");     
      gggr4->Draw("Psame");     

      c1->cd(4);
      frame->SetTitle("CTEQ5: 100EeV");
      frame->DrawCopy();
      ggggr1->Draw("P");     
      ggggr2->Draw("Psame");
      ggggr3->Draw("Psame");     
      ggggr4->Draw("Psame");   

      cerr << "flag5 " << endl;

      ////
      TCanvas *c1mu = new TCanvas("c1mu","test mu",200,10,600,600);
      c1mu->Divide(2,2);
      c1mu->cd(1);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.15);
      c1mu->cd(2);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.15);
      c1mu->cd(3);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.15);
      c1mu->cd(4);
      gPad->SetLogy();
      gPad->SetLogx();
      gPad->SetBottomMargin(0.12);
      gPad->SetLeftMargin(0.15);


      c1mu->cd(1);
      frame21->SetTitle("CTEQ5: 1 TeV");
      frame21->DrawCopy();
      //frame21->Draw();
      gr1theta->Draw("P");     
      gr2theta->Draw("Psame");
      c1mu->cd(2);
      frame22->SetTitle("CTEQ5: 1 PeV");
      frame22->DrawCopy();
      //frame22->Draw();
      ggr1theta->Draw("P");     
      ggr2theta->Draw("Psame");
      
      c1mu->cd(3);
      frame23->SetTitle("CTEQ5: 1 EeV");
      frame23->DrawCopy();
      //frame23->Draw();
      gggr1theta->Draw("P");     
      gggr2theta->Draw("Psame");
      
      c1mu->cd(4);
      frame24->SetTitle("CTEQ5: 100 EeV");
      frame24->DrawCopy();
      //frame24->Draw();
      ggggr1theta->Draw("P");     
      ggggr2theta->Draw("Psame");
      cerr << "flag6 " << endl;


      //
      // 1D histograms
      //

      // nu CC interaction 

      // plot y

      TCanvas *c1D = new TCanvas("c1D", "c1D", 10, 10, 600, 600);
      c1D->Divide(2, 2);

      TH1D *y1D0 = new TH1D("y1D0","y1D0",100,1e-10,1);
      y1D0->SetStats(0);      
      y1D0->GetXaxis()->SetTitle("y");
      y1D0->GetXaxis()->SetTitleOffset(1.2);
      y1D0->SetTitle("Nu CC CTEQ5: 100GeV");

      TH1D *y1D1 = new TH1D("y1D1","y1D1",100,1e-10,1);
      y1D1->SetStats(0);      
      y1D1->GetXaxis()->SetTitle("y");
      y1D1->GetXaxis()->SetTitleOffset(1.2);
      y1D1->SetTitle("Nu CC CTEQ5: 1TeV");

      TH1D *y1D2 = new TH1D("y1D2","y1D2",100,1e-10,1);
      y1D2->SetStats(0);      
      y1D2->GetXaxis()->SetTitle("y");
      y1D2->GetXaxis()->SetTitleOffset(1.2);
      y1D2->SetTitle("Nu CC CTEQ5: 1PeV");

      TH1D *y1D3 = new TH1D("y1D3","y1D3",100,1e-10,1);
      y1D3->SetStats(0);      
      y1D3->GetXaxis()->SetTitle("y");
      y1D3->GetXaxis()->SetTitleOffset(1.2);
      y1D3->SetTitle("Nu CC CTEQ5: 1EeV");

      TH1D *y1D4 = new TH1D("y1D4","y1D4",100,1e-10,1);
      y1D4->SetStats(0);      
      y1D4->GetXaxis()->SetTitle("y");
      y1D4->GetXaxis()->SetTitleOffset(1.2);
      y1D4->SetTitle("Nu CC CTEQ5: 100EeV");

      for (int i=0; i<nStatebins; ++i) {
         y1D0->Fill(y1_100GeV[i]);
         y1D1->Fill(y1[i]);
         y1D2->Fill(yy1[i]);
         y1D3->Fill(yyy1[i]);
         y1D4->Fill(yyyy1[i]);
      }

      c1D->cd(1); y1D0->Draw();
      c1D->cd(2); y1D1->Draw();
      c1D->cd(3); y1D2->Draw();
      c1D->cd(4); y1D3->Draw();
    
      // costheta 

      TCanvas *ctheta1D = new TCanvas("ctheta1D", "ctheta1D", 10, 10, 600, 600);
      ctheta1D->Divide(2, 2);

      TH1D *theta1D0 = new TH1D("theta1D0","theta1D0",100,-10,0);
      theta1D0->SetStats(0);      
      theta1D0->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      theta1D0->GetXaxis()->SetTitleOffset(1.2);
      theta1D0->SetTitle("Nu CC CTEQ5: 100GeV");

      TH1D *theta1D1 = new TH1D("theta1D1","theta1D1",100,-20,1);
      theta1D1->SetStats(0);      
      theta1D1->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      theta1D1->GetXaxis()->SetTitleOffset(1.2);
      theta1D1->SetTitle("Nu CC CTEQ5: 1TeV");

      TH1D *theta1D2 = new TH1D("theta1D2","theta1D2",100,-20,1);
      theta1D2->SetStats(0);      
      theta1D2->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      theta1D2->GetXaxis()->SetTitleOffset(1.2);
      theta1D2->SetTitle("Nu CC CTEQ5: 1PeV");

      TH1D *theta1D3 = new TH1D("theta1D3","theta1D3",100,-20,1);
      theta1D3->SetStats(0);      
      theta1D3->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      theta1D3->GetXaxis()->SetTitleOffset(1.2);
      theta1D3->SetTitle("CTEQ5: 1EeV");

      TH1D *theta1D4 = new TH1D("theta1D4","theta1D4",100,-20,1);
      theta1D4->SetStats(0);      
      theta1D4->GetXaxis()->SetTitle("Out-going #mu 1-Cos(#theta)");
      theta1D4->GetXaxis()->SetTitleOffset(1.2);
      theta1D4->SetTitle("CTEQ5: 100EeV");

      for (int i=0; i<nStatebins; ++i) {
         //cerr << log10(xxxx1theta[i]) << endl;
         if (x1theta_100GeV[i]>0) theta1D0->Fill(log10(x1theta_100GeV[i]));
         if (x1theta[i]>0) theta1D1->Fill(log10(x1theta[i]));
         if (xx1theta[i]>0) theta1D2->Fill(log10(xx1theta[i]));
         if (xxx1theta[i]>0) theta1D3->Fill(log10(xxx1theta[i]));
         if (xxxx1theta[i]>0) theta1D4->Fill(log10(xxxx1theta[i]));
      }

      ctheta1D->cd(1); theta1D0->Draw();
      ctheta1D->cd(2); theta1D1->Draw();
      ctheta1D->cd(3); theta1D2->Draw();
      ctheta1D->cd(4); theta1D3->Draw();

}
