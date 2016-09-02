void PlotCrosssectionFiles(){
  
  //string dir(getenv("I3_BUILD"));
  //dir +="/neutrino-generator/resources/cross_section_data/cteq5/";
  string dir = "../cross_section_data/cteq5";

  string file1 = dir+"cteq5_cc_nu.data";
  string file2 = dir+"cteq5_cc_nubar.data";
  string file3 = dir+"cteq5_nc_nu.data";
  string file4 = dir+"cteq5_nc_nubar.data";

  char* buf1 = file1.c_str(); 
  char* buf2 = file2.c_str(); 
  char* buf3 = file3.c_str(); 
  char* buf4 = file4.c_str(); 

      ifstream mFile1, mFile2, mFile3, mFile4; 
      double xmax=0, xmin=0;
      int nx=0;
      char tDum[64];
      float tValues1,tValues2,tValues3,tValues4;

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
 
      int nline=0;
      static const int nxbin = 10000;
     
      //the first line is ignored
      mFile1 >> nx; //nx
      cout << nx << endl;
      mFile1 >> tDum; //always 1
      cout << tDum << endl;
      mFile1 >> xmin; //xmin
      cout << xmin << endl;
      mFile1 >> xmax; //xmax
      cout << xmax << endl;

      Double_t x[nxbin];
      Double_t y1[nxbin];
      Double_t y2[nxbin];
      Double_t y3[nxbin];
      Double_t y4[nxbin];
      Double_t binsize = (xmax-xmin)/(nx-1) ;



      mFile2 >> tDum;
      mFile2 >> tDum;
      mFile2 >> tDum;
      mFile2 >> tDum;
      mFile3 >> tDum;
      mFile3 >> tDum;
      mFile3 >> tDum;
      mFile3 >> tDum;
      mFile4 >> tDum;
      mFile4 >> tDum;
      mFile4 >> tDum;
      mFile4 >> tDum;

      for(int i=1; i<nx; ++i)
	{
	  mFile1 >> tValues1;
	  mFile2 >> tValues2;
	  mFile3 >> tValues3;
	  mFile4 >> tValues4;
	  x[i]=i*binsize;
	  cout << x[i] << " : " << tValues1<<":"	 << tValues2<<":"  << tValues3<<":"	 << tValues4<<endl;
	  //y1[i]=log10(tValues1);
	  //y2[i]=log10(tValues2);
	  //y3[i]=log10(tValues3);
	  //y4[i]=log10(tValues4);

	  y1[i]=(tValues1);
	  y2[i]=(tValues2);
	  y3[i]=(tValues3);
	  y4[i]=(tValues4);
	  //cout << "x value=" << x[i] << endl; 
	  //cout << "y value=" << y[i] << endl; 
	  //if(!mFile.eof())break;
	}
      TGraph *gr1 = new TGraph(nx, x, y1);
      TGraph *gr2 = new TGraph(nx, x, y2);
      TGraph *gr3 = new TGraph(nx, x, y3);
      TGraph *gr4 = new TGraph(nx, x, y4);

      gr1->SetTitle("CTEQ5");
      gr1->SetMaximum(1e-4);
      gr1->SetMinimum(1e-12);
      TAxis *xaxis = gr1->GetXaxis();
      TAxis *yaxis = gr1->GetYaxis();
      xaxis->SetRangeUser(0,12);
      xaxis->SetTitle("log_{10}(Energy [GeV])");
      yaxis->SetTitle("#sigma [mb]");

      TCanvas *c1 = new TCanvas("c1","test",200,10,600,400);
      gPad->SetLogy();

      gr1->SetMarkerColor(1);
      gr2->SetMarkerColor(2);
      gr3->SetMarkerColor(3);
      gr4->SetMarkerColor(4);

      gr1->SetLineColor(1);
      gr2->SetLineColor(2);
      gr3->SetLineColor(3);
      gr4->SetLineColor(4);

      gr1->Draw("ACP");     
      gr2->Draw("CPsame");
      gr3->Draw("CPsame");     
      gr4->Draw("CPsame");     
}
