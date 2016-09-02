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
// [example]
// $ root -l 'PlotIceCap.C(0, 20)'
//
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

class I3NuGStaticParamsSetter;

//______________________________________________________________
Int_t FillPoints(Int_t n, Float_t *pos, Double_t rmin, 
                 Double_t rmax, Double_t mintheta, Double_t maxtheta ) 
{

   Int_t npos = 0;

   for (Int_t i=0; i<n; ++i) { 
      Float_t r = gRandom->Uniform(rmin, rmax);
      Float_t theta = gRandom->Uniform(mintheta, maxtheta);
      Float_t phi   = gRandom->Uniform(0, 2*TMath::Pi());
      //cerr << "r, theta, phi = " << r << "," << theta << "," << phi << endl;
      Float_t costheta = cos(theta);
      Float_t sintheta = sin(theta);
      Float_t cosphi   = cos(phi);
      Float_t sinphi   = sin(phi);
      Float_t x = r*sintheta*cosphi;
      Float_t y = r*sintheta*sinphi;
      Float_t z = r*costheta;

      Double_t density = I3NuGInteractionInfo::GetEarthDensity(r, theta, phi);
      if (density > I3NuGStaticParams::gATMO_DENSITY) {
         pos[npos*3] = x;    
         pos[npos*3+1] = y;    
         pos[npos*3+2] = z;    
         //cerr << "x, y, z, difdensity = " << x << "," << y << "," << z << "," << density - I3NuGStaticParams::gATMO_DENSITY << endl;
         ++npos;
      }
   }
   return npos;
}

//______________________________________________________________
void PlotPolarCap(Double_t minthetadeg = 0, 
                  Double_t maxthetadeg = 10., 
                  string earthmodel = "PREM",
                  string crustmodel = "PREM_mmc", 
                  Double_t coredensity = -1.,
                  Double_t earthdensity = -1.,
                  string polarcap = "SimpleCap",
                  Double_t capangle = 30.0)
{ 

   // you must link library on current directory
   gSystem->Load("$I3_BUILD/lib/libneutrino-generator.so");
   I3NuGStaticParamsSetter::SetPath(string("../earthparams"));
   I3NuGStaticParamsSetter::SetEarthModel(earthmodel);
   I3NuGStaticParamsSetter::SetCrustModel(crustmodel);
   I3NuGStaticParams::gICECAP = polarcap;

   if (coredensity > 0) {
      I3NuGStaticParamsSetter::SetCoreDensity(coredensity);
   }
   if (earthdensity > 0) {
      I3NuGStaticParamsSetter::SetEarthDensity(earthdensity);
   }

   I3NuGStaticParamsSetter::SetIceCapAngle(capangle/180*TMath::Pi());


   // Show 3-D markers
   //
   gROOT->Reset();
   TCanvas *c1 = new TCanvas("c1","PolyMarker3D Window",200,10,700,500);

   // create a pad
   TPad *p1 = new TPad("p1","p1",0.05,0.02,0.95,0.82, kWhite);
   p1->Draw();
   p1->cd();

   const Int_t n = 10000;
   Double_t mintheta = minthetadeg /180 * TMath::Pi();
   Double_t maxtheta = maxthetadeg /180 * TMath::Pi();
   Double_t rmin     = I3NuGStaticParams::gMOHO_BOUNDARY;
   Double_t rmax     = I3NuGStaticParams::gEARTH_RADIUS;

   // creating a view
   TView * view = TView::CreateView(1);
   Double_t rangemax = rmax*TMath::Sin(maxtheta);
   Double_t zmin     = rmax*TMath::Cos(maxtheta);

   view->SetRange(-rangemax, -rangemax, zmin, rangemax, rangemax, rmax);

   const Int_t arrayn = n * 3;
   Float_t *pos1 = new Float_t[arrayn];
   Float_t *pos2 = new Float_t[arrayn];

   Int_t npos1 = FillPoints(n, pos1, rmin, rmax, mintheta, maxtheta);

   I3NuGStaticParams::gICECAP = "NoIce";
   Int_t npos2 = FillPoints(n, pos2, rmin, rmax, mintheta, maxtheta);
  
   // create a first PolyMarker3D
   TPolyMarker3D *pm3d1 = new TPolyMarker3D(npos1,pos1,8);
   pm3d1->SetMarkerColor(kCyan);
   pm3d1->SetMarkerSize(0.5);

   TPolyMarker3D *pm3d2 = new TPolyMarker3D(npos2,pos2,8);
   pm3d2->SetMarkerColor(kRed+3);
   pm3d2->SetMarkerSize(0.5);

   pm3d1->Draw();
   pm3d2->Draw("same");

   c1->cd();
   TPaveText *title = new TPaveText(0.1,0.85,0.9,0.97);
   title->SetFillColor(24);
   title->AddText("Examples of 3-D primitives");
   title->AddText("Bedrock:brown, Ice:cyan");
   TText *click=title->AddText("Click anywhere on the picture to rotate");
   click->SetTextColor(4);
   title->Draw();


}

 

