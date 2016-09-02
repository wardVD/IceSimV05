/**
 * Copyright (C) 2006
 * The IceCube collaboration
 * ID: $Id: I3TopRecoPlaneFitParams.cxx 141473 2016-02-04 02:13:37Z hdembinski $
 *
 * @file I3TopRecoPlaneFitParams.cxx
 * @version $Rev: 18397 $
 * @date $Date: 2016-02-03 20:13:37 -0600 (Wed, 03 Feb 2016) $
 * @author $Author: klepser $
 */

#include <icetray/serialization.h>
#include <icetray/I3Units.h>
#include <recclasses/I3TopRecoPlaneFitParams.h>

template <class Archive>
void I3TopRecoPlaneFitParams::serialize(Archive& ar, unsigned version)
{
  ar & make_nvp("I3FrameObject", base_object<I3FrameObject>(*this));
  ar & make_nvp("ET",  ET );
  ar & make_nvp("DeltaT", DeltaT );
  ar & make_nvp("T0", T0 );
  ar & make_nvp("X0", X0 );
  ar & make_nvp("Y0",  Y0 );
  ar & make_nvp("Chi2", Chi2 );
  ar & make_nvp("NPulses", NPulses );

  if (version < 1) DeltaT *= I3Units::s;  // in the old version this was stored in seconds
}

std::ostream& operator<<(std::ostream& os, const I3TopRecoPlaneFitParams& p) {
  os << "[ I3TopRecoPlaneFitParams::"
     << "\n  ET     : " << p.ET
     << "\n  DeltaT : " << p.DeltaT
     << "\n  T0     : " << p.T0
     << "\n  X0     : " << p.X0
     << "\n  Y0     : " << p.Y0
     << "\n  Chi2   : " << p.Chi2
     << "\n  NPulses: " << p.NPulses << " ]";
  return os;
}

I3_SERIALIZABLE(I3TopRecoPlaneFitParams);
