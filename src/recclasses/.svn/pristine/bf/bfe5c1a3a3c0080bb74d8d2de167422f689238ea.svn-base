/**
 * $Id$
 * Copyright (C) 2015
 * The IceCube collaboration
 *
 * @version $Rev$
 * @date $Date$
 * @author $Author: hdembinski $
 */

#ifndef RECCLASSES_LAPUTOPENUMS_H_
#define RECCLASSES_LAPUTOPENUMS_H_

// Note: we emulate enum classes from C++11 here using enums embedded in structs

/// @brief Contains all flags used by I3LaputopParams
///
/// The I3LaputopParams relies on the correct mapping of
/// these enums to integers, so there are some rules:
/// - Never change the order of the flags in these enums.
/// - Never delete a flag from these enums.
/// - Never assign numbers to the keys manually, except if
///   you know what you are doing (e.g. to create an alias).
/// - Add new flags for an enum at the end. In case of Parameter,
///   before ``size``.
namespace Laputop {

  /// Enum of fitted parameters
  struct Parameter {
    enum Enum {
      Log10_S125,           ///< log10 of shower size at 125 m radial distance to shower axis
      Beta,                 ///< slope of LDF
      Age,                  ///< shower age as in NKG formula
      Nmu,                  ///< Muon scale (exact meaning to be defined)
      Nx,                   ///< x-coordinate of direction vector
      Ny,                   ///< y-coordinate of direction vector
      Tc,                   ///< time at shower core
      Xc,                   ///< x-coordinate of shower core
      Yc,                   ///< y-coordinate of shower core
      Theta,                ///< theta angle of shower axis (don't store this, store Nx, Ny)
      Phi,                  ///< phi angle of shower axis (don't store this, store Nx, Ny)
      size                  ///< size automatically counts parameters
    };
  };

  /// Enum of LDF formulas
  struct LDF {
    enum Enum {
      None = -1,            ///< Indicator for I3LaputopParams to use stored value
      DLP                   ///< double-log-parabola LDF
    };
  };

  /// Enum of shower front delay formulas
  struct FrontDelay {
    enum Enum {
      None = -1,            ///< Indicator for I3LaputopParams to use stored value
      GaussParabola         ///< Parabolic umbrella with a Gaussian tip around the center
    };
  };

  /// Enum of energy calibrations
  struct Energy {
    enum Enum {
      None = -1,               ///< Indicator for I3LaputopParams to use stored value
      IC73SpectrumPaper,       ///< Calibration from IC73 spectrum paper, 2013
      IC73SpectrumPaperProton, ///< Calibration based on proton showers, Sibyll-2.1
      IC73SpectrumPaperIron,   ///< Calibration based on iron showers, Sibyll-2.1
      ICRC2015_H4a_E27         ///< Calibration from KR's ICRC2015 proceeding, using H4a fractions upon E-2.7, Sibyll-2.1
    };
  };

}

#endif
