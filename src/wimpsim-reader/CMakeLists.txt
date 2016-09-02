i3_project(wimpsim-reader
	DOCS_DIR resources/docs
	PYTHON_DIR python
	PYTHON_DEST icecube/wimpsim_reader
)

LIST(APPEND LIB_${PROJECT_NAME}_HEADERFILES
  public/wimpsim-reader/I3WimpSimReader.h
  public/wimpsim-reader/WimpSimTools.h
)

LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES
  private/wimpsim-reader/I3WimpSimReader.cxx
  private/wimpsim-reader/WimpSimTools.cxx
)

i3_add_library(wimpsim-reader
  ${LIB_${PROJECT_NAME}_HEADERFILES}
  ${LIB_${PROJECT_NAME}_SOURCEFILES}

  USE_TOOLS boost python 
  USE_PROJECTS icetray dataclasses phys-services simclasses
)

LIST(APPEND LIB_${PROJECT_NAME}_TEST_SOURCEFILES
  private/test/main.cxx
  private/test/WimpSimToolsTest.cxx
)

LIST(APPEND TEST_SCRIPTS
  resources/test/test_sun.py
  resources/test/test_earth.py
  resources/test/test_sun_event.py
  resources/test/test_earth_event.py
  resources/test/recursion_test.py
)

IF(BUILD_ASTRO)
  LIST(APPEND ${TEST_SCRIPTS}
    resources/test/test_sun_position.py
  )
ELSE(BUILD_ASTRO)
  MESSAGE(STATUS "astro not build, skipping 'test_sun_position'-test")
ENDIF(BUILD_ASTRO)

i3_test_scripts(
  ${TEST_SCRIPTS}
)

i3_test_executable(test
  ${LIB_${PROJECT_NAME}_TEST_SOURCEFILES}
  USE_TOOLS boost
  USE_PROJECTS wimpsim-reader dataclasses icetray

)


