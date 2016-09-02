/**
 * $Id$
 *
 * Copyright (C) 2006
 * The IceCube collaboration
 *
 * @author $Author: Hans Dembinski$
 */

#ifndef _RECCLASSES_I3LAPUTOPPARAMS_H_
#define _RECCLASSES_I3LAPUTOPPARAMS_H_

class I3Particle;
class I3Position;
#include "ParameterStorage.h"
#include "LaputopEnums.h"
#include <icetray/I3FrameObject.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/Utility.h>
#include <ostream>
#include <limits>

static const unsigned i3laputopparams_version_ = 2;

/**
 * @brief Stores fit result of Laputop module (see project toprec).
 *
 * Stored is the best fit of the shower description (axis, size, age).
 * Also stored are parameters to judge the quality of the fit, like
 * chi2 values for LDF and time fit and the number of minimization steps.
 *
 * The following setters/getters are inherited from ParameterStorage,
 * see ParameterStorage.h for documentation:
 *
 * bool Has(Parameter::Enum type);
 * double GetValue(Parameter::Enum type);
 * double GetError(Parameter::Enum type);
 * double GetCovariance(Parameter::Enum type1, Parameter::Enum type2);
 * double GetCorrelation(Parameter::Enum type1, Parameter::Enum type2);
 *
 * double SetValue(Parameter::Enum type, double value);
 * double SetError(Parameter::Enum type, double error);
 * double SetCovariance(Parameter::Enum type1,
 *                      Parameter::Enum type2, double cov);
 */
class I3LaputopParams : public ParameterStorage<Laputop::Parameter::Enum,
                                                Laputop::Parameter::size>,
                        public I3FrameObject {
  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive& ar, unsigned version);
  typedef ParameterStorage<Laputop::Parameter::Enum,
                           Laputop::Parameter::size> Base;
  typedef Laputop::Parameter Par;

  double Jacobi(Laputop::Parameter::Enum a, Laputop::Parameter::Enum b) const;

public:
  /// LDF used in this fit
  Laputop::LDF::Enum typeLDF_;
  /// Shower front model used in this fit
  Laputop::FrontDelay::Enum typeFrontDelay_;
  /// Energy calibration recommended for this fit
  Laputop::Energy::Enum typeEnergy_;

  // other stuff
  double zPos_;                     ///< z-coordinate of shower core, has no error
  double logLikelihood_;            ///< LogLikelihood value of best-fit solution without silent stations
  double logLikelihood_Silent_;     ///< Same with silent stations included
  double chi2_LDF_;                 ///< Chi2 of LDF fit
  double chi2_Time_;                ///< Chi2 of Time fit
  int ndf_LDF_;                     ///< Degrees of freedom in LDF fit
  int ndf_Time_;                    ///< Degrees of freedom in Time fit
  int nMini_;                       ///< Number of minimization steps, good for diagnotics

  /** User data for special uses:
    * If you are extending the reconstruction fit, you can use this
    * to store data as an intermediate solution. Once your feature
    * becomes standard, a proper storage should be added to this class.
    */
  I3MapStringDouble userData_;

  /// Helper for Laputop to sync with I3Particle
  void FillFromI3Particle(const I3Particle&);

  /// Expected charge in VEM at given radius and polar angle in shower plane coords
  double ExpectedSignal(double r, double xi = 0,
                        Laputop::LDF::Enum type = Laputop::LDF::None) const;
  /// Expected error of charge in VEM at given radius and polar angle in shower plane coords
  double ExpectedSignalError(double r, double xi = 0,
                             Laputop::LDF::Enum type = Laputop::LDF::None) const;

  /// Expected time delay wrt the ideal plane shower front
  double ExpectedShowerFrontDelay(double r, double xi = 0,
                                  Laputop::FrontDelay::Enum type = Laputop::FrontDelay::None) const;

  /// Expected error of time delay wrt the ideal plane shower front
  double ExpectedShowerFrontDelayError(double r, double xi = 0,
                                       Laputop::FrontDelay::Enum type = Laputop::FrontDelay::None) const;

  /** Reconstructed energy for this shower.
    * Differs from true shower energy, because
    * of finite detector resolution.
    */
  double Energy(Laputop::Energy::Enum type = Laputop::Energy::None) const;

  /** Has is re-implemented here to indicate that the values of 
    * Theta and Phi are available if it can be computed 
    * from stored values on the fly
    */
  bool Has(Laputop::Parameter::Enum par) const;

  /** GetValue is re-implemented here to compute the values of 
    * Theta and Phi from stored values on the fly
    */
  double GetValue(Laputop::Parameter::Enum par) const;

  /** GetError is re-implemented here to compute the errors of 
    * Theta and Phi from stored values on the fly
    */
  double GetError(Laputop::Parameter::Enum par) const;

  /** GetCovariance is re-implemented here to compute the 
    * covariance of Theta and Phi from stored values on the fly
    */
  double GetCovariance(Laputop::Parameter::Enum par1,
                       Laputop::Parameter::Enum par2) const;

  /** Angular resolution is defined analog to 
    * http://arxiv.org/abs/0901.3138 as the angular radius around
    * the reconstructed direction which contains the true direction
    * with 68 % confidence.
    */
  double GetAngularResolution() const;

  // let C++ add copy constructor, assignment operator
  I3LaputopParams():
    typeLDF_(Laputop::LDF::None),
    typeFrontDelay_(Laputop::FrontDelay::None),
    typeEnergy_(Laputop::Energy::None),
    zPos_(std::numeric_limits<double>::quiet_NaN()),
    logLikelihood_(std::numeric_limits<double>::quiet_NaN()),
    logLikelihood_Silent_(std::numeric_limits<double>::quiet_NaN()),
    chi2_LDF_(std::numeric_limits<double>::quiet_NaN()),
    chi2_Time_(std::numeric_limits<double>::quiet_NaN()),
    ndf_LDF_(-1),
    ndf_Time_(-1),
    nMini_(-1)
  {}

  virtual ~I3LaputopParams() {}

  bool operator==(const I3LaputopParams&) const;
};

/// For pretty-printing of I3LaputopParams
std::ostream& operator<<(std::ostream&, const I3LaputopParams&);

BOOST_CLASS_VERSION(I3LaputopParams, i3laputopparams_version_);
I3_POINTER_TYPEDEFS(I3LaputopParams);

#include "icetray/I3Frame.h"
/** 
 * A special overload for the conversion of a 
 * I3TopLateralFitParams into a I3LaputopParams;
 * see private/recclasses/I3LaputopParamsTransform.cxx
 */
template <>
I3LaputopParamsConstPtr
I3Frame::Get(const std::string& name, bool quietly, void*, void*) const;

#endif
