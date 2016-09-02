/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file vem_yield.cxx
 * @version $Rev: $
 * @date $Date: $
 * @author Stefan Klepser, Patrick Berghaus
 */


#include <iostream>
#include <topsimulator/responses/vem_yield.h>

using namespace std;

namespace vem_yield {

// constant parts VEM = const.

const double lowE0 = std::pow(10.0, -1);
const double lowE1 = std::pow(10.0, -1.5);
const double lowE2 = std::pow(10.0, -2);

const double highE0 = std::pow(10.0, -0.4);
const double highE1 = std::pow(10.0, -0.45);
const double highE2 = std::pow(10.0, -1.6);

const double con0 = 0.00153448;
const double con1 = 0.00541449;
const double con2 = 0.00664103;


// shifted std::powerlaws VEM = shift + factor*std::pow(E,index)

const double lowEp0 = std::pow(10.0, -2);
const double lowEp1 = std::pow(10.0, -1.55);
const double lowEp2 = std::pow(10.0, -0.3);
const double lowEp3 = std::pow(10.0, -2);
const double lowEp4 = std::pow(10.0, -1.55);
const double lowEp5 = std::pow(10.0, -0.3);
const double lowEp6 = std::pow(10.0, -2.7);
const double lowEp7 = std::pow(10.0, -0.35);
const double lowEp8 = std::pow(10.0, -1.3);
const double lowEp9 = std::pow(10.0, -0.98);
const double lowEp10 = std::pow(10.0, -0.5);
const double lowEp11 = std::pow(10.0, -1.3);
const double lowEp12 = std::pow(10.0, -1.12);
const double lowEp13 = std::pow(10.0, -0.45);
const double lowEp14 = std::pow(10.0, -0.4);
const double lowEp15 = std::pow(10.0, -2);
const double lowEp16 = std::pow(10.0, -0.5);
const double lowEp17 = std::pow(10.0, -0.45);
const double lowEp18 = std::pow(10.0, -2);
const double lowEp19 = std::pow(10.0, -0.3);
const double lowEp20 = std::pow(10.0, -1.6);
const double lowEp21 = std::pow(10.0, -2);
const double lowEp22 = std::pow(10.0, -2);
const double lowEp23 = std::pow(10.0, -2);
const double lowEp24 = std::pow(10.0, -2);

const double highEp0 = std::pow(10.0, -1.55);
const double highEp1 = std::pow(10.0, -0.3);
const double highEp2 = std::pow(10.0, 2.5);
const double highEp3 = std::pow(10.0, -1.55);
const double highEp4 = std::pow(10.0, -0.3);
const double highEp5 = std::pow(10.0, 2.5);
const double highEp6 = std::pow(10.0, -0.35);
const double highEp7 = std::pow(10.0, 3);
const double highEp8 = std::pow(10.0, -0.98);
const double highEp9 = std::pow(10.0, -0.5);
const double highEp10 = std::pow(10.0, 3.5);
const double highEp11 = std::pow(10.0, -1.12);
const double highEp12 = std::pow(10.0, -0.45);
const double highEp13 = std::pow(10.0, 3.5);
const double highEp14 = std::pow(10.0, 3);
const double highEp15 = std::pow(10.0, -0.5);
const double highEp16 = std::pow(10.0, 3);
const double highEp17 = std::pow(10.0, 3);
const double highEp18 = std::pow(10.0, -0.3);
const double highEp19 = std::pow(10.0, 3);
const double highEp20 = std::pow(10.0, 3);
const double highEp21 = std::pow(10.0, 3);
const double highEp22 = std::pow(10.0, 3);
const double highEp23 = std::pow(10.0, 3);
const double highEp24 = std::pow(10.0, 3);

const double shift0 = -0.657087;
const double shift1 = -0.155872;
const double shift2 = -2.6855;
const double shift3 = -0.00170685;
const double shift4 = -0.247134;
const double shift5 = -1.4513;
const double shift6 = -0.00244643;
const double shift7 = -1.99509;
const double shift8 = -11.2218;
const double shift9 = -0.197563;
const double shift10 = -2659.12;
const double shift11 = 0.0610708;
const double shift12 = 0.0416769;
const double shift13 = -793.155;
const double shift14 = -0.61907;
const double shift15 = -0.203576;
const double shift16 = 1.07027;
const double shift17 = -0.24919;
const double shift18 = 0.00321739;
const double shift19 = 0.199565;
const double shift20 = -0.141411;
const double shift21 = -0.0344296;
const double shift22 = -0.0726239;
const double shift23 = 1.25941;
const double shift24 = 0.164325;

const double factor0 = 0.679547;
const double factor1 = 2.34833;
const double factor2 = 4.67655;
const double factor3 = 0.0935907;
const double factor4 = 2.25601;
const double factor5 = 3.35781;
//const double factor6 = 4.02625; // This is Stefans original value which leads to a big discontinuity at E=lowEp6
const double factor6 = 2.37340;   // This is the corrected value which is continous
const double factor7 = 3.45558;
const double factor8 = 11.3756;
const double factor9 = 2.60166;
const double factor10 = 2659.89;
const double factor11 = 0.41587;
const double factor12 = 2.40796;
const double factor13 = 793.948;
const double factor14 = 1.0147;
const double factor15 = 2.56841;
const double factor16 = 0.843406;
const double factor17 = 0.524519;
const double factor18 = 2.30643;
const double factor19 = 0.727576;
const double factor20 = 0.966665;
const double factor21 = 0.858686;
const double factor22 = 0.773135;
const double factor23 = 0.756258;
const double factor24 = 0.487564;

const double index0 = 0.00707092;
const double index1 = 0.750112;
const double index2 = 0.281949;
const double index3 = 0.826965;
const double index4 = 0.616594;
const double index5 = 0.347713;
const double index6 = 1.17829;
const double index7 = 0.212694;
const double index8 = 0.00272074;
const double index9 = 0.982432;
const double index10 = 9.96308e-06;
const double index11 = 0.573477;
const double index12 = 1.18025;
const double index13 = 3.31792e-05;
const double index14 = 0.539015;
const double index15 = 0.258919;
const double index16 = 0.491593;
const double index17 = 0.643671;
const double index18 = 1.66533;
const double index19 = 0.520686;
const double index20 = 0.480246;
const double index21 = 0.476828;
const double index22 = 0.479467;
const double index23 = 0.528013;
const double index24 = 0.588115;

//
// ------------- Here start Patrick's parametrisations including the snow --------------- 
//

double em_snow(double E, double snow) {
  if(E <= 0.0 ) return 0.0;
  if(snow < 0.01) snow = 0.01; // Avoid invalid arguments in logarithm
  
  E = log10(E);
  double result;
  if (E > -1.4+.2*log(snow)) {
    if (E+1.52704-.186249*log(snow)+.158255*pow(snow,1.08837) > 0) {
      result = pow(10,(2.09756+.153399*log(snow)-.385878*pow(snow,1.32332)+(1.90115-.267133*log(snow)+1.72116*pow(snow,.868692))*log10(E+1.52704-.186249*log(snow)+.158255*pow(snow,1.08837))));
    }
    else {
      result = 0.0;
    }
  }
  else {
    result = pow(10,2.09756+.153399*log(snow)-.385878*pow(snow,1.32332)+(1.90115-.267133*log(snow)+1.72116*pow(snow,.868692))*log10(1.52704-.186249*log(snow)+.158255*pow(snow,1.08837)-1.4+.2*log(snow))-(-1.4+.2*log(snow))*2.6+E*2.6);
  }

  if(!(result > 0.0)) result = 0.0;
  return VEM_SCALE_WITH_SNOW*result/VEM_NORM_SNOW;
}

double ga_snow(double E, double snow) {
  if (E <= 0.) return 0.;
  if(snow < 0.01) snow = 0.01; // Avoid invalid arguments in logarithm
  
  E = log10(E);
  double result;
  if (E > -1.27192+.225510*log(snow)+.0184467*pow(log(snow),2)) {
    result = exp(1.98420+.208687*log(snow)-.39232*pow(snow,2.04977))*pow((E+2.562-.144564*log(snow)+.01647*pow(snow,2.56584)),3.622+.7*pow(snow,1.44613));
  }
  else {
    if (E+3.56-.134468*pow(snow,1.333354) > 0) {
      result = exp(1.98420+.208687*log(snow)-.39232*pow(snow,2.04977))*pow((-1.27192+.225510*log(snow)+.0184467*pow(log(snow),2)+2.562-.144564*log(snow)+.01647*pow(snow,2.56584)),3.622+.7*pow(snow,1.44613))/pow((-1.27192+.225510*log(snow)+.0184467*pow(log(snow),2)+3.56-.134468*pow(snow,1.333354)),(4.4112+.521083*log(snow)+.103767*pow(log(snow),2))*(log(snow)<.5)+(log(snow)>=.5)*(4.32204 + 0.959234*log(snow)-1.1762*pow(log(snow),2)))*pow((E+3.56-.134468*pow(snow,1.333354)),(4.4112+.521083*log(snow)+.103767*pow(log(snow),2))*(log(snow)<.5)+(log(snow)>=.5)*(4.32204 + 0.959234*log(snow)-1.1762*pow(log(snow),2)));
    }
    else {
      result = 0.;
    }
  }

  if(!(result > 0.0)) result = 0.0;
  return VEM_SCALE_WITH_SNOW*result/VEM_NORM_SNOW;
}

double mm_snow(double E, double snow) {
  if (E <= 0.) return 0.;
  if(snow < 0.01) snow = 0.01; // Just to be consistent with the functions above 

  E = log10(E);
  double result = 148+8*pow(snow,.274854)+(-15.7-7.44*pow(snow,.959))*exp(E*(-2.18-.00718463*pow(snow,3.)));
  
  if(!(result > 0.0)) result = 0.0;
  return VEM_SCALE_WITH_SNOW*result/VEM_NORM_SNOW;
}


//
// ------------- Here start Stefan's original parametrisations --------------- 
//

double em(double E){
  if(E<lowEp0) return 0;
  else{
    if(E>=lowEp0 && E< highEp0) return VEM_SCALE_NO_SNOW*(shift0+factor0*pow(E,index0));
    else{
      if(E>=lowEp1 && E< highEp1) return VEM_SCALE_NO_SNOW*(shift1+factor1*pow(E,index1));
      else{
        if(E>=lowEp2) return VEM_SCALE_NO_SNOW*(shift2+factor2*pow(E,index2));
        else cout << "Error in em function!!!\n";
      }
    }
  }
  return 0;
}

double ep(double E){
  if(E<lowEp3) return 0;
  else{
    if(E>=lowEp3 && E< highEp3) return VEM_SCALE_NO_SNOW*(shift3+factor3*pow(E,index3));
    else{
      if(E>=lowEp4 && E< highEp4) return VEM_SCALE_NO_SNOW*(shift4+factor4*pow(E,index4));
      else{
        if(E>=lowEp5) return VEM_SCALE_NO_SNOW*(shift5+factor5*pow(E,index5));
        else cout << "Error in ep function!!!\n";
      }
    }
  }
  return 0;
}

double ga(double E){
  if(E<lowEp6) return 0;
  else{
    if(E>=lowEp6 && E< highEp6) return VEM_SCALE_NO_SNOW*(shift6+factor6*pow(E,index6));
    else{
      if(E>=lowEp7) return VEM_SCALE_NO_SNOW*(shift7+factor7*pow(E,index7));
      else cout << "Error in ga function!!!\n";
    }
  }
  return 0;
}

double mm(double E){
  if(E<lowEp8) return 0;
  else{
    if(E>=lowEp8 && E< highEp8) return VEM_SCALE_NO_SNOW*(shift8+factor8*pow(E,index8));
    else{
      if(E>=lowEp9 && E< highEp9) return VEM_SCALE_NO_SNOW*(shift9+factor9*pow(E,index9));
      else{
        if(E>=lowEp10) return VEM_SCALE_NO_SNOW*(shift10+factor10*pow(E,index10));
        else cout << "Error in mm function!!!\n";
      }
    }
  }
  return 0;
}

double mp(double E){
  if(E<lowEp11) return 0;
  else{
    if(E>=lowEp11 && E< highEp11) return VEM_SCALE_NO_SNOW*(shift11+factor11*pow(E,index11));
    else{
      if(E>=lowEp12 && E< highEp12) return VEM_SCALE_NO_SNOW*(shift12+factor12*pow(E,index12));
      else{
        if(E>=lowEp13) return VEM_SCALE_NO_SNOW*(shift13+factor13*pow(E,index13));
        else cout << "Error in mp function!!!\n";
      }
    }
  }
  return 0;
}

double p(double E){
  if(E<lowE0) return 0;
  else{
    if(E>=lowE0 && E< highE0) return VEM_SCALE_NO_SNOW*con0;
    else{
      if(E>=lowEp14) return VEM_SCALE_NO_SNOW*(shift14+factor14*pow(E,index14));
      else cout << "Error in p function!!!\n";
    }
  }
  return 0;
}

double ap(double E){
  if(E<lowEp15) return 0;
  else{
    if(E>=lowEp15 && E< highEp15) return VEM_SCALE_NO_SNOW*(shift15+factor15*pow(E,index15));
    else{
      if(E>=lowEp16) return VEM_SCALE_NO_SNOW*(shift16+factor16*pow(E,index16));
      else cout << "Error in ap function!!!\n";
    }
  }
  return 0;
}

double n(double E){
  if(E<lowE1) return 0;
  else{
    if(E>=lowE1 && E< highE1) return VEM_SCALE_NO_SNOW*con1;
    else{
      if(E>=lowEp17) return VEM_SCALE_NO_SNOW*(shift17+factor17*pow(E,index17));
      else cout << "Error in n function!!!\n";
    }
  }
  return 0;
}



double pm(double E){
  if(E<lowEp18) return 0;
  else{
    if(E>=lowEp18 && E< highEp18) return VEM_SCALE_NO_SNOW*(shift18+factor18*pow(E,index18));
    else{
      if(E>=lowEp19) return VEM_SCALE_NO_SNOW*(shift19+factor19*pow(E,index19));
      else cout << "Error in pm function!!!\n";
    }
  }
  return 0;
}

double pp(double E){
  if(E<lowE2) return 0;
  else{
    if(E>=lowE2 && E< highE2) return VEM_SCALE_NO_SNOW*con2;
    else{
      if(E>=lowEp20) return VEM_SCALE_NO_SNOW*(shift20+factor20*pow(E,index20));
      else cout << "Error in pp function!!!\n";
    }
  }
  return 0;
}

double km(double E){
  if(E<lowEp21) return 0;
  else{
    if(E>=lowEp21) return VEM_SCALE_NO_SNOW*(shift21+factor21*pow(E,index21));
    else cout << "Error in km function!!!\n";
  }
  return 0;
}

double kp(double E){
  if(E<lowEp22) return 0;
  else{
    if(E>=lowEp22) return VEM_SCALE_NO_SNOW*(shift22+factor22*pow(E,index22));
    else cout << "Error in kp function!!!\n";
  }
  return 0;
}

double an(double E){
  if(E<lowEp23) return 0;
  else{
    if(E>=lowEp23) return VEM_SCALE_NO_SNOW*(shift23+factor23*pow(E,index23));
    else cout << "Error in an function!!!\n";
  }
  return 0;
}

double k0(double E){
  if(E<lowEp24) return 0;
  else{
    if(E>=lowEp24) return VEM_SCALE_NO_SNOW*(shift24+factor24*pow(E,index24));
    else cout << "Error in k0 function!!!\n";
  }
  return 0;
}

} // end of namespace vem_yield
