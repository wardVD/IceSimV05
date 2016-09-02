/**
 *
 * @author Anna Pollmann <anna.pollmann@uni-wuppertal.de>
 */

#ifndef I3TIMECHARACTERISTICSVALUES_H_INCLUDED
#define I3TIMECHARACTERISTICSVALUES_H_INCLUDED

#include <icetray/I3FrameObject.h>
#include <dataclasses/Utility.h>
#include <ostream>
#include <cmath>

static const unsigned i3timecharacteristics_version_ = 0;

/**
 *  @brief Stores parameters of the time structure of an event
 *  (see project CommonVariables).
 *
 *  Output of I3TimeCharacteristicsCalculator module.
 */
class I3TimeCharacteristicsValues : public I3FrameObject
{
  public:
    I3TimeCharacteristicsValues()
      :  timelength_fwhm_(NAN), // double
        timelength_last_first_(-1), // int
        timelenght_maxgap_(-1), // int
        zpattern_(0)
    {}

    I3TimeCharacteristicsValues(const I3TimeCharacteristicsValues& rhs) // copy constructor
      : timelength_fwhm_(rhs.GetTimeLengthFwhm()), // double
        timelength_last_first_(rhs.GetTimeLengthLastFirst()), // int
        timelenght_maxgap_(rhs.GetTimeLenghtMaxgap()), // int
        zpattern_(rhs.GetZPattern())
    {}

    I3TimeCharacteristicsValues(
        double timelength_fwhm, // double
        int timelength_last_first, // int
        int timelenght_maxgap, // int
        int zpattern
      )
      :  timelength_fwhm_(timelength_fwhm), // double
        timelength_last_first_(timelength_last_first), // int
        timelenght_maxgap_(timelenght_maxgap), // int
        zpattern_(zpattern)
    {}

    inline double GetTimeLengthFwhm() const { return this->timelength_fwhm_; }
    inline int GetTimeLengthLastFirst() const { return this->timelength_last_first_; }
    inline int GetTimeLenghtMaxgap() const { return this->timelenght_maxgap_; }
    inline int GetZPattern() const { return this->zpattern_; }

    inline void  SetTimeLengthFwhm(double l){ this->timelength_fwhm_ = l; }
    inline void  SetTimeLengthLastFirst(int l){ this->timelength_last_first_ = l; }
    inline void  SetTimeLenghtMaxgap(int l){ this->timelenght_maxgap_ = l; }
    inline void  SetZPattern(int l){this->zpattern_=l;}

  protected: // member variables
    /// EventTimeLength
    double timelength_fwhm_; // double
    int timelength_last_first_; // int
    int timelenght_maxgap_; // int
    int zpattern_;

  private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, unsigned version);

    SET_LOGGER("I3TimeCharacteristicsValues");
};

std::ostream& operator<<(std::ostream&, const I3TimeCharacteristicsValues&);

I3_POINTER_TYPEDEFS(I3TimeCharacteristicsValues);
BOOST_CLASS_VERSION(I3TimeCharacteristicsValues, i3timecharacteristics_version_);

#endif // I3TIMECHARACTERISTICSVALUES_H_INCLUDED
