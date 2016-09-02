/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file vem_yield.h
 * @version $Rev: $
 * @date $Date: $
 * @author Stefan Klepser, Patrick Berghaus
 */

#ifndef _TOPSIMULATOR_VEM_YIELD_H_
#define _TOPSIMULATOR_VEM_YIELD_H_

#include <cmath>

/** \brief Various constants and parametrizations to simulate VEM-calibrated signals.
 *
 * They are used by I3ParamTankResponse. For more detailed comments, please look
 * into the header file (I couldn't figure out how to group variables in doxygen).
 */
namespace vem_yield {

// Absolute calibration factors for VEM-parametrizations in order to get
// 1 VEM (after the signal processing) for 4 GeV vertical muons
const double VEM_SCALE_WITH_SNOW = 1.070; ///< For Patrick's parametrizations
const double VEM_SCALE_NO_SNOW   = 1.275; ///< For Stefan's parametrizations

// Patrick Berghaus's VEM-parametrisations including the snow
const double VEM_NORM_SNOW = 155.0; ///< Relative normalization constant, number of PEs per VEM
double em_snow(double E, double snow); ///< Response for e-, E in [GeV], snow in [m]
double ga_snow(double E, double snow); ///< Response for photons, E in [GeV], snow in [m]
double mm_snow(double E, double snow); ///< Response for mu-, E in [GeV], snow in [m]

// Stefan Klepser's original VEM-parametrisations
double em(double E); ///< Response for e-, E in [GeV]
double ep(double E); ///< Response for e+, E in [GeV]
double ga(double E); ///< Response for photons, E in [GeV]
double mm(double E); ///< Response for mu-, E in [GeV]
double mp(double E); ///< Response for mu+, E in [GeV]
double p(double E); ///< Response for protons, E in [GeV]
double ap(double E); ///< Response for anti-protons, E in [GeV]
double n(double E); ///< Response for neutrons, E in [GeV]
double an(double E); ///< Response for anti-neutrons, E in [GeV]
double pm(double E); ///< Response for pi-, E in [GeV]
double pp(double E); ///< Response for pi+, E in [GeV]
double km(double E); ///< Response for K-, E in [GeV]
double kp(double E); ///< Response for K+, E in [GeV]
double k0(double E); ///< Response for K0, E in [GeV]

// Masses of the particles in GeV
const double mass_ga = 0.0;
const double mass_e = 0.51099892e-3;
const double mass_mu = 0.105658369;
const double mass_p = 0.93827203;
const double mass_n = 0.93956536;
const double mass_pi = 0.13957018;
const double mass_ka = 0.493677;
const double mass_ka0 = 0.497648;

} // end of namespace vem_yield

#endif // VEM_YIELD_H_INCLUDED
