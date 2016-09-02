#ifndef I3MPCONSTANTS_H_INCLUDED
#define I3MPCONSTANTS_H_INCLUDED

#include "dataclasses/I3Constants.h"

namespace I3MPConst{
  /**
   * The fine structure constant times 2 pi
   */
  const double ALPHA_2_PI(2.*I3Constants::pi / 137.);
  
  /**
   * The lower and upper integration limits of
   * the Cherenkov spectrum.  This needs to match
   * the simulation range used in photonics.
   */
  const double LAMBA_0(300.*I3Units::nanometer);
  const double LAMBA_1(600.*I3Units::nanometer);
  
  /**
   * Monopole Charge.  Units of e.
   * The charge is 2*pi*h_bar*c/e
   */
  const double G = 68.5;//units of e.
    
  /**
   * The charge number of the particle.
   * Note that phase index needed
   */
  const double Z_SQUARED = pow(G*I3Constants::n_ice_phase,2);

  /**
   * This is the constant factor in the Cherenkov radiation formula
   * including the integration over wavelength.  The product of
   * this times the speed dependent factor gives the number of photons
   * emitted per unit length.
   */
  const double MP_FACTOR = ALPHA_2_PI * Z_SQUARED * (1./LAMBA_0 - 1./LAMBA_1);
  
  /**
   * Photonics track type boundaries
   */
  const double BETA_TYPE9 = 1.0;
  const double BETA_TYPE10 = 0.9;
  const double BETA_TYPE11 = 0.8;
  const double BETA_TYPE12 = 0.76;
  const double BETA_TYPE13 = 0.02;
}

#endif
