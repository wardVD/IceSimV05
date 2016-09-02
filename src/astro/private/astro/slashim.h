//
// shim for compiling astro with either PAL or SLALIB
// $Id: slashim.h 141940 2016-02-16 16:42:17Z ddouglas $
//

#ifndef ASTRO_PALSHIM_H_INCLUDED
#define ASTRO_PALSHIM_H_INCLUDED

#if USE_PAL
#include <star/pal.h>
#else
#include <slalib/slalib.h>
//#warning I3Astro has been compiled with SLALIB, support for this library will go away at some point in the future, please migrate to PAL instead, for instructions see http://software.icecube.wisc.edu/offline_trunk/projects/astro/index.html#dependancies
bool slalib_already_warned = false;
void sla_warning()
{
  if (!slalib_already_warned){
    //log_warn("I3Astro has been compiled with SLALIB, support for this library will go away at some point in the future, please migrate to PAL instead, for instructions see http://software.icecube.wisc.edu/offline_trunk/projects/astro/index.html#dependancies");
    slalib_already_warned = true;
  }
}
#define palAmp(...)    ({ sla_warning(); slaAmp(__VA_ARGS__);  })
#define palAop(...)    ({ sla_warning(); slaAop(__VA_ARGS__);  })
#define palDtt(...)    ({ sla_warning(); slaDtt(__VA_ARGS__);   })
#define palEqgal(...)  ({ sla_warning(); slaEqgal(__VA_ARGS__); })
#define palGaleq(...)  ({ sla_warning(); slaGaleq(__VA_ARGS__); })
#define palGalsup(...) ({ sla_warning(); slaGalsup(__VA_ARGS__); })
#define palSupgal(...) ({ sla_warning(); slaSupgal(__VA_ARGS__); })
#define palMap(...)   ({ sla_warning(); slaMap(__VA_ARGS__);  })
#define palOap(...)   ({ sla_warning(); slaOap(__VA_ARGS__);  })
#define palRdplan(...) ({ sla_warning(); slaRdplan(__VA_ARGS__);})
#endif

#endif // #ifndef COORDINATE_SERVICE_PALSHIM_H_INCLUDED
