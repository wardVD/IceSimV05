#
# $Id: CMakeLists.txt 141149 2016-01-21 23:46:41Z nega $
#

if(SLA_FOUND OR PAL_FOUND)

  i3_project(astro
	PYTHON_DIR python
 	DOCS_DIR resources/docs)

  if (PAL_FOUND)
    add_definitions(-DUSE_PAL)
    set(_pal_tool pal)
    colormsg(GREEN   "  + Using PAL")		
  else (PAL_FOUND)
    set(_pal_tool sla)
    colormsg(GREEN   "  + Using SLALIB")			
  endif(PAL_FOUND)

  i3_add_library(astro private/astro/*.cxx
    USE_TOOLS boost ${_pal_tool}  
    USE_PROJECTS icetray dataclasses tableio
    )

  i3_test_scripts(resources/test/*.py)
  set_tests_properties(
    astro::test_inverse.py
    astro::test_radec.py
    PROPERTIES LABELS RUN_LONG
    )

  i3_add_pybindings(astro
      private/pybindings/I3Astro.cxx
      USE_TOOLS python 
      USE_PROJECTS astro  tableio
      )

else(SLA_FOUND OR PAL_FOUND)
  colormsg(YELLOW "  - neither PAL nor slalib library not found, skipping build of ${PROJECT_NAME}") 
  colormsg(YELLOW "    Ask your admin to install PAL or SLALIB.
       Instructions for PAL can be found at
       http://software.icecube.wisc.edu/offline_trunk/projects/cmake/tools/pal.html                 
       slalib-c package is available from IceCube ports.")
endif(SLA_FOUND OR PAL_FOUND)
