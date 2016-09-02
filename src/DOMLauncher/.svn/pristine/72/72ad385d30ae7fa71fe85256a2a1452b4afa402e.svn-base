i3_project(DOMLauncher
  PYTHON_DIR  python
  PYTHON_DEST icecube/DOMLauncher
  DOCS_DIR    resources/docs
)

LIST(APPEND LIB_${PROJECT_NAME}_HEADERFILES
  public/DOMLauncher/domlauncherutils.h
  public/DOMLauncher/DOMLauncher.h
  public/DOMLauncher/I3DOM.h
  public/DOMLauncher/DOM.h
  public/DOMLauncher/PMTResponseSimulator.h
)

LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES
  private/DOMLauncher/DOMLauncher.cxx
  private/DOMLauncher/I3DOM.cxx
  private/DOMLauncher/I3InIceDOM.cxx
  private/DOMLauncher/I3IceTopDOM.cxx
  private/DOMLauncher/PMTResponseSimulator.cxx
  private/DOMLauncher/InterpolatedSPETemplate.cxx
)

i3_add_library(DOMLauncher
        ${LIB_${PROJECT_NAME}_HEADERFILES}
        ${LIB_${PROJECT_NAME}_SOURCEFILES}

        USE_TOOLS boost gsl python 
        USE_PROJECTS  ${LIB_${PROJECT_NAME}_TOOLS} icetray dataclasses phys-services simclasses sim-services
)

i3_add_pybindings(DOMLauncher
    private/pybindings/module.cxx
    private/pybindings/InterpolatedSPETemplate.cxx
    private/pybindings/I3DOM.cxx
    private/pybindings/I3InIceDOM.cxx
    private/pybindings/I3IceTopDOM.cxx
    private/pybindings/domlauncherutils.cxx
    private/pybindings/PMTResponseSimulator.cxx

    USE_TOOLS boost python
    USE_PROJECTS icetray DOMLauncher dataclasses
)

LIST(APPEND LIB_${PROJECT_NAME}_TEST_SOURCEFILES
  private/test/main.cxx
  private/test/PMTResponseSimulatorTests.cxx
)

i3_test_scripts(
   resources/test/launchTimingTest.py
   resources/test/discriminatorTest.py
   resources/test/LC-logicTest.py
   resources/test/launchTimingTestMultiFrame.py
)

LIST(APPEND TEST_PROJECTS
    DOMLauncher
    dataio
    phys-services
    WaveCalibrator
)

## wavedeform won't build without suitesparse
if(SUITESPARSE_FOUND)
if(EXISTS ${CMAKE_SOURCE_DIR}/wavedeform)
    LIST(APPEND LIB_${PROJECT_NAME}_TEST_SOURCEFILES
        private/test/RoundTripChargeTest.cxx
        private/test/PulseTemplateTests.cxx
    )

    LIST(APPEND TEST_PROJECTS
        wavedeform
    )
    colormsg(GREEN "+-- wavedeform found, adding RoundTripChargeTest to tests.")
endif()
endif(SUITESPARSE_FOUND)

#DOMLauncher dataio phys-services WaveCalibrator wavedeform
i3_test_executable(test
  ${LIB_${PROJECT_NAME}_TEST_SOURCEFILES}
  USE_PROJECTS ${TEST_PROJECTS}
)
# add_definitions(-g -O0)

set_tests_properties(DOMLauncher::discriminatorTest.py
  DOMLauncher::launchTimingTest.py
  DOMLauncher::launchTimingTestMultiFrame.py
  DOMLauncher::test
  PROPERTIES LABELS RUN_LONG)
