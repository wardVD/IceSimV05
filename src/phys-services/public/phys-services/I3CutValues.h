/**
    copyright  (C) 2005
    the icecube collaboration
    $Id: I3CutValues.h 108715 2013-07-26 13:16:13Z boersma $
    @author dule
*/

#ifndef I3CUTVALUES_H
#define I3CUTVALUES_H

#include <vector>
#include <phys-services/I3CutValuesBase.h>
#include <dataclasses/I3Constants.h>
#include <dataclasses/I3Position.h>

template <typename Key, typename Value> struct I3Map;
class OMKey;
class I3Particle;
class I3RecoPulse;
class I3Geometry;

/**
 * @brief A class to store the basic hit information from the event
 */
class I3CutValues : public I3CutValuesBase
{
 public:
  int Nchan;
  int Nhit;
  int Nstring;
  int Ndir;
  double Ldir;
  double Sdir;
  double Sall;
  I3Position cog;

  I3CutValues() :
    Nchan(-1),
    Nhit(-1),
    Nstring(-1),
    Ndir(-1),
    Ldir(NAN),
    Sdir(NAN),
    Sall(NAN){}

  void Calculate(const I3Particle& track,
                 const I3Geometry& geometry,
                 const I3Map< OMKey, std::vector< I3RecoPulse> >& pulsemap,
                 const double& begTWindow = I3Constants::dt_window_l,
                 const double& endTWindow = I3Constants::dt_window_h);

  virtual ~I3CutValues();

 private:

  friend class boost::serialization::access;
  template <class Archive> void serialize(Archive & ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3CutValues);

#endif
