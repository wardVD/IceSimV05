#
# $Id: CMakeLists.txt 137043 2015-08-31 07:58:03Z olivas $
#

i3_project(icepick
  PYTHON_DIR python
  DOCS_DIR resources/docs)


i3_add_library(icepick 
  private/icepick/I3AndServices.cxx             
  private/icepick/I3ZenithFilter.cxx
  private/icepick/ModuleExport.cxx
  private/icepick/I3FrameObjectFilter.cxx       
  private/icepick/I3NHitChannelFilter.cxx       
  private/icepick/I3NLaunchFilter.cxx           
  private/icepick/I3OrServices.cxx              
  private/icepick/I3PickRawNHitEventFilter.cxx  
  private/icepick/I3RecoPulseFilter.cxx         
  private/icepick/I3NChFilter.cxx
  private/icepick/I3RandomFilter.cxx
  private/icepick/I3DOMLaunchTriggerTypeFilter.cxx
  private/icepick/I3InverseServices.cxx

  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses phys-services interfaces
  )

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS icepick interfaces
  )

i3_test_scripts(resources/test/*.py)
