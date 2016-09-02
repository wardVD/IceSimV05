#ifndef RECCLASSES_PARAMETERSTORAGE_H_
#define RECCLASSES_PARAMETERSTORAGE_H_

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/symmetric.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/serialization/nvp.hpp>
#include <limits>
#include <cmath>
#include <sstream>
#include "Utility.h"

/**
 * @brief SlimParameterStorage defines a policy to store
 * fit parameters.
 *
 * How to use: Implement an enum listing of the parameters and 
 * create a template instance. Template arguments are the enum 
 * and its size. SlimParameterStorage has no serialization.
 * It is expected to be used only as a base class for other
 * classes with more functionality.
 */
template<class ParEnum, unsigned ParEnumSize>
class SlimParameterStorage {
protected:
  // Map from enums to actual indices
  short idx_[ParEnumSize];
  // Don't touch this ever, use getters/setters
  boost::numeric::ublas::vector<double> par_;

private:
  /// Add parameter to index, resize containers, initialize to NaN
  void AddParameter(ParEnum type)
  {
    const short i = idx_[int(type)] = par_.size();
    par_.resize(i + 1, true);
    par_(i) = std::numeric_limits<double>::quiet_NaN();
  }

public:
  // Default constructor is automatically called by Derived class
  SlimParameterStorage():
    par_()
  {
    // make sure that index is properly initialized
    for (unsigned i = 0; i < ParEnumSize; ++i)
      idx_[i] = -1; // indicate absence
  }

  /// Check whether a particular parameter is stored
  inline
  bool Has(ParEnum type) const
  { return idx_[int(type)] >= 0; }

  /// Best-fit value of parameter
  double GetValue(ParEnum type) const
  {
    if (!Has(type))
      return std::numeric_limits<double>::quiet_NaN();
    const short i = idx_[int(type)];
    return par_(i); }

  /// Set value, enlarge storage if necessary
  void SetValue(ParEnum type, double value)
  {
    if (!Has(type))
      AddParameter(type);
    const short i = idx_[int(type)];
    par_(i) = value;
  }

  bool operator==(const SlimParameterStorage<ParEnum, ParEnumSize>& other) const
  {
    for (unsigned i = 0; i < ParEnumSize; ++i) {
      if (Has(ParEnum(i)) && !other.Has(ParEnum(i)))
        return false;
      if (!Has(ParEnum(i)) && other.Has(ParEnum(i)))
        return false;
      if (Has(ParEnum(i)) && other.Has(ParEnum(i)) &&
          !nan_aware_equality(GetValue(ParEnum(i)), other.GetValue(ParEnum(i))))
        return false;
    }
    return true;
  }
};


/**
 * @brief ParameterStorage defines a policy to store fit parameters
 * and their covariances.
 *
 * ParameterStorage refines SlimParameterStorage, which stores
 * only fit parameters. ParameterStorage also stores the full
 * covariance matrix.
 *
 * How to use: Implement an enum listing of the parameters and 
 * use this template and I3FrameObject as a base classes for your
 * class. Template arguments are the enum and its size.
 */
template<class ParEnum, unsigned ParEnumSize>
class ParameterStorage: public SlimParameterStorage<ParEnum, ParEnumSize> {
private:
  // Useful typedef to refer to base type
  typedef SlimParameterStorage<ParEnum, ParEnumSize> Base;

  // Don't touch this ever, use getters/setters
  boost::numeric::ublas::symmetric_matrix<double> cov_;

  friend class boost::serialization::access;
  template <class Archive>
  void serialize(Archive & ar, unsigned version) {
    using boost::serialization::make_nvp;
    ar & make_nvp("idx", Base::idx_);
    // ublas has serialize for vector, ok
    ar & make_nvp("par", Base::par_);
    // ublas, y u no have serialize for symmetric_matrix? (boost-1.38)
    unsigned cov_size = cov_.size1();
    ar & make_nvp("cov_size", cov_size);
    if (cov_.size1() != cov_size)
      cov_.resize(cov_size);

    for (unsigned i = 0; i < cov_size; ++i) {
      for (unsigned j = i; j < cov_size; ++j) {
        std::ostringstream field;
        field << "cov_" << i << "_" << j;
        ar & make_nvp(field.str().c_str(), cov_(i, j));
      }
    }
  }

  /// Add parameter to index, resize containers, initialize to NaN
  void AddParameter(ParEnum type)
  {
    const short i = Base::idx_[int(type)] = Base::par_.size();
    Base::par_.resize(i + 1, true);
    cov_.resize(i + 1, true);
    Base::par_(i) = std::numeric_limits<double>::quiet_NaN();
    for (short j = 0; j <= i; ++j)
      cov_(i, j) = std::numeric_limits<double>::quiet_NaN();
  }

public:
  // This typedef is useful to refer to the full type of this template in Derived
  typedef ParameterStorage<ParEnum, ParEnumSize> ParameterStorageType;

  // Default constructor is automatically called by Derived class
  ParameterStorage():
    Base(),
    cov_()
  {}

  /// Best-fit estimate of statistical error of parameter
  double GetError(ParEnum type) const
  {
    if (!Base::Has(type))
      return std::numeric_limits<double>::quiet_NaN();
    const short i = Base::idx_[int(type)];
    return sqrt(cov_(i, i)); }

  /// Best-fit estimate of covariance of two parameters
  double GetCovariance(ParEnum type1,
                       ParEnum type2) const
  {
    if (!Base::Has(type1) || !Base::Has(type2))
      return std::numeric_limits<double>::quiet_NaN();
    const short i = Base::idx_[int(type1)], j = Base::idx_[int(type2)];
    return cov_(i, j); }

  /// Best-fit estimate of correlation of two parameters
  double GetCorrelation(ParEnum type1,
                        ParEnum type2) const
  {
    if (!Base::Has(type1) || !Base::Has(type2))
      return std::numeric_limits<double>::quiet_NaN();
    const short i = Base::idx_[int(type1)], j = Base::idx_[int(type2)];
    return cov_(i, j) / std::sqrt(cov_(i, i) * cov_(j, j)); }

  /// Set value, enlarge storage if necessary
  void SetValue(ParEnum type, double value)
  {
    if (!Base::Has(type))
      AddParameter(type);
    const short i = Base::idx_[int(type)];
    Base::par_(i) = value;
  }

  /// Set error, enlarge storage if necessary
  void SetError(ParEnum type, double error)
  {
    if (!Base::Has(type))
      AddParameter(type);
    const short i = Base::idx_[int(type)];
    cov_(i, i) = error * error;
  }

  /// Set covariance, enlarge storage if necessary
  void SetCovariance(ParEnum type1,
                     ParEnum type2,
                     double cov)
  {
    if (!Base::Has(type1))
      AddParameter(type1);
    if (!Base::Has(type2))
      AddParameter(type2);
    const short i = Base::idx_[int(type1)], j = Base::idx_[int(type2)];
    cov_(i, j) = cov;
  }

  /// Set value and error, added for convenience
  void SetParameter(ParEnum type, double value, double error)
  { SetValue(type, value); SetError(type, error); }

  bool operator==(const ParameterStorageType& other) const
  {
    for (unsigned i = 0; i < ParEnumSize; ++i) {
      if (Base::Has(ParEnum(i)) && !other.Has(ParEnum(i)))
        return false;
      if (!Base::Has(ParEnum(i)) && other.Has(ParEnum(i)))
        return false;
      if (Base::Has(ParEnum(i)) && other.Has(ParEnum(i))) {
        if (!nan_aware_equality(Base::GetValue(ParEnum(i)),
                                other.GetValue(ParEnum(i))) ||
            !nan_aware_equality(GetError(ParEnum(i)),
                               other.GetError(ParEnum(i))))
          return false;
        for (unsigned j = 0; j < i; ++j) {
          if (!nan_aware_equality(GetCovariance(ParEnum(i), ParEnum(j)),
                                  other.GetCovariance(ParEnum(i), ParEnum(j))))
            return false;
        }
      }
    }
    return true;    
  }
};

#endif
