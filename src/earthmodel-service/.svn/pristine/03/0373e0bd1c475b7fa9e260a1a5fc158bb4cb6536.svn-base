#!/usr/bin/env python
#
# pybinding load test
#

from I3Tray import *
from icecube import icetray, dataclasses, earthmodel_service

#
# for logging
#
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)
#icetray.set_log_level_for_unit("EarthModelService",icetray.I3LogLevel.LOG_TRACE)

# 
# generate earthmodel service
#

print("*** flat density integration test ***")

earthmodel = ["PREM_mmc", "FLATCORE_5.568"]
earth1 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel)

r = 3480000; 
orig = dataclasses.I3Position(0, 0, 0); 
to = dataclasses.I3Position(0, 0, r);

pass1 = earth1.integrate_density_in_cgs(orig, to, earthmodel_service.path) ;
pass2 = earth1.integrate_density_in_cgs(orig, to, earthmodel_service.radius) ; 
         
print("pass1 = %g pass2 = %g" % (pass1, pass2));
assert(pass1 == pass2)
                                                                          
mass = earth1.integrate_density_in_cgs(orig, to, earthmodel_service.sphere) ;                                                                
         
# manual calculation of mass
r_cm = r*1e2;
density = 5.56837052290423795;
vol_mass = 4. / 3. * dataclasses.I3Constants.pi * r_cm * r_cm * r_cm * density;        
         
print("mass = %g vol_mass = %g ratio = %g" % (mass, vol_mass, mass/vol_mass));
assert(mass/vol_mass < 1.0001 and mass / vol_mass > 0.9999 )
            

print("*** PREM integration test ***")

earthmodel = ["PREM_mmc"]
earth2 = earthmodel_service.EarthModelService("EarthModelService","",
                              earthmodel);

pass1 = earth2.integrate_density_in_cgs(orig, to, earthmodel_service.path) ;
pass2 = earth2.integrate_density_in_cgs(orig, to, earthmodel_service.radius) ; 
         
print("pass1 = %g pass2 = %g" % (pass1, pass2));
assert(pass1 == pass2)
                                                                          
mass = earth2.integrate_density_in_cgs(orig, to, earthmodel_service.sphere) ;                                                                
         
# manual calculation of mass

# inner core : density = 13.0885 -2.17742748697875934e-13 * r^2 (r < 1221500)
# f = 4*pi*r2 * density = 4*pi*(13.0885 * r^2  -2.17742748697875934e-13 * r^4)
# F = 4*pi*(1/3 * 13.0885 * r^3 - 1/5 * 2.17742748697875934e-13 * r^5)

import math

r1 = 1221500.
r12 = r1 * r1
inner_core_mass = 4 * math.pi * r12 * r1 * (1./3. * 13.0885 - 0.2 * 2.17742748697875934e-13 * r12)
print("inner core mass = %g" % (inner_core_mass));
 
# outer core : density = 12.5815 -1.98367603202009108e-07 * r -8.97421093229181259e-14 * r^2 -2.13773109929070169e-20 r^3 ( 1221500 < r < 3480000)
# F = 4*pi*(1/3 * 12.5815 * r^3 - 1/4 * 1.98367603202009108e-07 * r^4 - 1/5 * 8.97421093229181259e-14 * r^5 - 1/6 * -2.13773109929070169e-20 * r^6

outer_core_mass1 = 4 * math.pi * r12 * r1 * (1./3. * 12.5815 - 0.25 * 1.98367603202009108e-07 * r1 - 0.2 * 8.97421093229181259e-14 * r12 - 1./6. * 2.13773109929070169e-20 * r1 * r12)

print("outer core mass1 = %g" % (outer_core_mass1));

r2 = 3480000.
r22 = r2 * r2
outer_core_mass2 = 4 * math.pi * r22 * r2 * (1./3. * 12.5815 - 0.25 * 1.98367603202009108e-07 * r2 - 0.2 * 8.97421093229181259e-14 * r22 - 1./6. * 2.13773109929070169e-20 * r2 * r22)

print("outer core mass2 = %g" % (outer_core_mass2));
total_mass = (inner_core_mass + (outer_core_mass2 - outer_core_mass1)) * 1e6 
print("mass = %g total_mass = %g, ratio = %g" % (mass, total_mass, mass / total_mass));

assert(mass/total_mass < 1.0001 and mass / total_mass > 0.9999 )
            

r = 6371324; 
to = dataclasses.I3Position(0, 0, r);
whole_mass = earth2.integrate_density_in_cgs(orig, to, earthmodel_service.sphere) ;                                                                

print("whole mass (PREM) = %g, official value of total mass of the Earth is 5.972e+27g" % (whole_mass));



