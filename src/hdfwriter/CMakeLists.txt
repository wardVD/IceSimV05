#
# $Id: CMakeLists.txt 119244 2014-04-25 03:57:59Z jvansanten $
#

i3_project(hdfwriter 
	PYTHON_DIR python
	DOCS_DIR resources/docs
)

IF(HDF5_FOUND)

  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})
  ADD_DEFINITIONS(-DH5_USE_16_API)

  i3_add_library(hdfwriter
    private/hdfwriter/*.cxx
    private/hdfwriter/*.c

    USE_TOOLS boost hdf5
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS icetray dataio dataclasses tableio)

  i3_add_pybindings(hdfwriter
    private/pybindings/I3HDFTableService.cxx
    private/pybindings/module.cxx
    USE_TOOLS boost python
    USE_PROJECTS icetray hdfwriter tableio
    )

  # add the path to the place where the hdf5 libs exist
  #TARGET_LINK_LIBRARIES(hdf-writer ${HDF5_LIBRARIES})
  i3_test_executable(test 
    private/test/*.cxx
    USE_PROJECTS dataclasses icetray phys-services hdfwriter tableio)

  # Link manipulation scripts into $I3_BUILD/bin for ease of use.
  i3_executable_script(merge resources/scripts/merge.py)
  i3_executable_script(mix resources/scripts/mix.py)

ELSE(HDF5_FOUND)

  colormsg(CYAN   "+-- hdf5 library not found, skipping build of ${PROJECT_NAME}") 
  colormsg(YELLOW "*** Ask your admin to install the hdf5 package from IceCube ports or 
       run `cmake -DSYSTEM_PACKAGES` to use the HDF5 libraries and headers
       packaged with your system.")

ENDIF(HDF5_FOUND)

