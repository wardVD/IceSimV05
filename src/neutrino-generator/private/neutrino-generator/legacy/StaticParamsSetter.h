#ifndef NuGStaticParamsSetter_H 
#define NuGStaticParamsSetter_H 
/**
 *   Copyright  (C) 2005
 *   The IceCube Collaboration
 *   $Id: $
 *
 *   @file StaticParamsSetter.h
 *   @version $Revision: $
 *   @date    $Date:     $ 
 *   @author Kotoyo Hoshina <kotoyo.hoshina@icecube.wisc.edu>
 *
 *   @brief StaticParamsSetter header file
 *   provides setting functions for static params in 
 *   public/neutrino-generator/utils/StaticParams.h.
 *   All functions must be static.
 */

#include <string>
#include <icetray/I3Logging.h>

namespace nugen {

class StaticParamsSetter {

 private:
 // do not instansiate the class

 StaticParamsSetter() {}
 StaticParamsSetter(const StaticParamsSetter&) {}

 static std::string fgPath_;

 public:


  SET_LOGGER(""); 
};

}
#endif //


