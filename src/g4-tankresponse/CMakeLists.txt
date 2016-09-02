#
# $Id: CMakeLists.txt $
#
  
if (GEANT4_CONFIG_ERROR OR NOT DEFINED GEANT4_CONFIG_ERROR)

  message(STATUS "    Geant4 not in I3_PORTS - trying system installation")

  include(FindGeant4.cmake)
  include(FindCLHEP.cmake)

else (GEANT4_CONFIG_ERROR OR NOT DEFINED GEANT4_CONFIG_ERROR)
  set(GEANT4_FOUND TRUE)
  set(CLHEP_FOUND TRUE)
endif (GEANT4_CONFIG_ERROR OR NOT DEFINED GEANT4_CONFIG_ERROR)

if (GEANT4_FOUND AND CLHEP_FOUND AND GEANT4_VERSION LESS 1000)

  i3_project(g4-tankresponse
    PYTHON_DIR python
    DOCS_DIR resources/docs
    )

  i3_add_library(g4-tankresponse
    private/g4-tankresponse/*.cxx
    private/g4-tankresponse/*/*.cxx
    private/g4-tankresponse/triangle/*.c
    
    USE_TOOLS boost python clhep geant4 gsl
    
#KR    USE_PROJECTS icetray dataclasses pmt-simulator topsimulator phys-services
    USE_PROJECTS icetray dataclasses topsimulator phys-services simclasses
    )
  
  #
  # The following definitions are required by Triangle
  #
  add_definitions("-DANSI_DECLARATORS")
  add_definitions("-DTRILIBRARY")
  add_definitions("-DREDUCED")
  add_definitions("-DCDT_ONLY")
  
  include(FindG4VisDrivers.cmake)
  
  i3_test_scripts(resources/test/test_examples.py)
  set_tests_properties(g4-tankresponse::test_examples.py PROPERTIES LABELS RUN_LONG)

  i3_test_executable(test
    private/test/main.cxx
    private/test/triangle.cxx

    USE_PROJECTS g4-tankresponse sim-services
    )

else (GEANT4_FOUND AND CLHEP_FOUND AND GEANT4_VERSION LESS 1000)

  if (GEANT4_VERSION LESS 1000)
    message(STATUS "  - Geant4 and/or CLHEP not found - Skipping build of g4-tankresponse")
  else (GEANT4_VERSION LESS 1000)
    message(STATUS "  - Geant4.10 or newer detected, which is not supported - Skipping build of g4-tankresponse")
  endif (GEANT4_VERSION LESS 1000)

endif (GEANT4_FOUND AND CLHEP_FOUND AND GEANT4_VERSION LESS 1000)
