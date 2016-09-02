
i3_project(photonics-service
    PYTHON_DIR python
    DOCS_DIR resources/docs
)

# define the lists used to build the basic library
SET(LIB_${PROJECT_NAME}_SOURCEFILES
  private/photonics-service/I3PhotonicsHitMaker.cxx
  private/photonics-service/I3PhotonicsService.cxx
  private/photonics-service/I3PhotonicsServiceFactory.cxx
  private/photonics-service/I3PhotonicsServiceCommons.cxx
  private/photonics-service/I3DummyPhotonicsService.cxx
  private/photonics-service/I3PhotonicsTableService.cxx	
  private/photonics-service/I3PhotonicsReader.cxx
  private/photonics-service/I3PhotonicsL1Reader.cxx
  private/photonics-service/I3PhotonicsL2Reader.cxx     
)
SET(LIB_${PROJECT_NAME}_PROJECTS icetray dataclasses phys-services simclasses)
SET(LIB_${PROJECT_NAME}_TOOLS boost python)
SET(TESTS_${PROJECT_NAME}_SOURCEFILES
  private/test/IceTrayTest.cxx
  private/test/main.cxx
)

# add some things if photospline is installed
if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/photospline)
    LIST(APPEND LIB_${PROJECT_NAME}_PROJECTS photospline)
    LIST(APPEND LIB_${PROJECT_NAME}_TOOLS gsl)
    LIST(APPEND LIB_${PROJECT_NAME}_SOURCEFILES
	private/photonics-service/I3PhotoSplineService.cxx
	private/photonics-service/I3PhotoSplineServiceGradients.cxx
	private/photonics-service/I3PhotoSplineTable.cxx)
    ADD_DEFINITIONS(-DUSE_PHOTOSPLINE)
    colormsg(GREEN "+-- photospline support")
    LIST(APPEND TESTS_${PROJECT_NAME}_SOURCEFILES
         ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}/private/test/I3PhotoSplineTest.cxx)
endif(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/photospline)

# optional numpy stuff
if(NUMPY_FOUND)
	LIST(APPEND LIB_${PROJECT_NAME}_TOOLS numpy)
	ADD_DEFINITIONS(-DUSE_NUMPY)
	colormsg(GREEN "+-- numpy support")
endif(NUMPY_FOUND)

i3_add_library(${PROJECT_NAME} ${LIB_${PROJECT_NAME}_SOURCEFILES}
	USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
	USE_PROJECTS ${LIB_${PROJECT_NAME}_PROJECTS}
)

i3_test_executable(test 
	${TESTS_${PROJECT_NAME}_SOURCEFILES}
	USE_TOOLS gsl boost
	USE_PROJECTS icetray dataio photonics-service
	)
	
i3_test_scripts(resources/test/photonics_hitmaker_test.py
		resources/test/photospline_hitmaker_test.py
                resources/test/test_jacobian.py)

i3_add_pybindings(photonics_service
	private/pybindings/wrappers.cxx
	private/pybindings/I3DummyPhotonicsService.cxx
	private/pybindings/I3PhotonicsServiceCommons.cxx
	private/pybindings/I3PhotonicsTableService.cxx
	private/pybindings/I3PhotonicsService.cxx
	private/pybindings/I3PhotoSplineService.cxx
	private/pybindings/module.cxx
	USE_TOOLS boost python 
	USE_PROJECTS icetray photonics-service
	)

set_tests_properties(photonics-service::test_jacobian.py PROPERTIES LABELS RUN_LONG)
