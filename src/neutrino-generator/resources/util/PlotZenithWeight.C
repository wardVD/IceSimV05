//*****************************************************************
//
// Utility tools to draw Zenith distribution 
//
// To use the script, you need to compile neutrino-generator project
// with -DUSE_CINT option, and execute "env.sh" at your I3_BUILD 
// directory.
// $ $I3_PORTS/bin/cmake -DCMAKE_BUILD_TYPE:STRING=Release -DUSE_CINT ../src/
// $ .$I3_BUILD/env.sh
//
// [example draw]
// $ root -l 'PlotZenithWeight.C(0.2, -1, 1)'
//
//*****************************************************************

void PlotZenithWeight(double alpha=0.2, double mincosth=-1, double maxcosth=1) {

   gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");

   double l = maxcosth - mincosth;
   if (l <= 0) {
      cerr << "mincosth is larger than maxcosth" << endl;
      return;
   }
   if (alpha < 0 || alpha > 1) {

      cerr << "alpha must be within 0 <= alpha <= 1.0" << endl;
      return;
   }

   TH1D *h1 = new TH1D("h1","h1", 100, -1., 1.);
   TH1D *h2 = new TH1D("h2","h2", 100, -1., 1.);

   I3NuGStaticParams::gLINEAR_ZENITH_WEIGHT_ALPHA = alpha;

   double coszen, weight, random;

   for (int i=0; i<200000; ++i) {
      random = gRandom->Uniform(0, 1.0);
      I3NuGInjector::LinearZenithWeight(random, mincosth, maxcosth, coszen, weight);

      h1->Fill(coszen);
      h2->Fill(coszen, weight);
   }

   TCanvas *c1 = new TCanvas("c1","c1",10, 10, 800, 300);
   c1->Divide(2, 1);
   c1->cd(1); h1->Draw();
   c1->cd(2); h2->Draw();

}
