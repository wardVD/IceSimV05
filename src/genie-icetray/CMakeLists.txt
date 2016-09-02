#
#  $Id: CMakeLists.txt 141399 2016-02-01 20:50:48Z nega $
#
#  Copyright (C) 2011
#  Claudio Kopper  <claudio.kopper@icecube.wisc.edu>
#
#  This file is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>
#
# only compile this if all tools are found
if (LOG4CPP_FOUND AND XML2_FOUND AND ROOT_FOUND AND GENIE_FOUND)

i3_project(genie-icetray
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

  # check for Q-Frame support
  if(EXISTS ${CMAKE_SOURCE_DIR}/dataio/private/dataio/QConverter.cxx)
      # we assume that the existence of this module means that we have
      # to deal with a Q-frame-enabled meta-project
      ADD_DEFINITIONS(-DIS_Q_FRAME_ENABLED)
      colormsg(GREEN "+-- Q-frame-enabled")
  else(EXISTS ${CMAKE_SOURCE_DIR}/dataio/private/dataio/QConverter.cxx)
      colormsg(YELLOW "+-- old-style, non-Q-frame module")
  endif(EXISTS ${CMAKE_SOURCE_DIR}/dataio/private/dataio/QConverter.cxx)

  # we need three extra ROOT libs
  find_library(ROOT_GEOM_LIB Geom PATHS ${ROOTSYS}/lib NO_DEFAULT_PATH)
  find_library(ROOT_EG_LIB EG PATHS ${ROOTSYS}/lib NO_DEFAULT_PATH)
  find_library(ROOT_EGPYTHIA6_LIB EGPythia6 PATHS ${ROOTSYS}/lib NO_DEFAULT_PATH)
  find_library(LHAPDF_LIB LHAPDF PATHS ${I3_PORTS}/lib NO_DEFAULT_PATH)
  find_library(PYTHIA_SO_LIB Pythia6 PATHS ${I3_PORTS}/lib NO_DEFAULT_PATH)

  # let the code know where GENIE is located in order to be able to read some data files
  add_definitions(-DGENIE_PATH="${GENIE_PATH}")

  i3_add_library(genie-icetray
    private/genie-icetray/*.cxx
    USE_TOOLS python boost root log4cpp xml2 genie
    USE_PROJECTS icetray dataio dataclasses phys-services

    LINK_LIBRARIES ${PYTHIA_SO_LIB} ${ROOT_GEOM_LIB} ${ROOT_EG_LIB} ${ROOT_EGPYTHIA6_LIB} ${LHAPDF_LIB}
    )
  # SET_TARGET_PROPERTIES(genie-icetray PROPERTIES LINK_FLAGS -Wl,-u,pydata_)

  i3_add_pybindings(genie_icetray
    private/pybindings/I3GENIEGenerator.cxx
    private/pybindings/I3GENIEResultDict.cxx
    private/pybindings/module.cxx
    USE_TOOLS boost root python
    USE_PROJECTS icetray dataclasses genie-icetray
  )

option(GENIE_ICETRAY_EXEC "Build genie executables" ON)

if (GENIE_ICETRAY_EXEC)
  i3_executable(mkspl
    private/mkspl/gMakeSplines.cxx
    USE_TOOLS python boost root log4cpp xml2 genie
    USE_PROJECTS icetray genie-icetray

    LINK_LIBRARIES ${PYTHIA_SO_LIB} ${ROOT_GEOM_LIB} ${ROOT_EG_LIB} ${ROOT_EGPYTHIA6_LIB} ${LHAPDF_LIB}
    )

  i3_executable(spladd
    private/mkspl/gSplineAdd.cxx
    USE_TOOLS python boost root log4cpp xml2 genie
    USE_PROJECTS icetray genie-icetray

    LINK_LIBRARIES ${PYTHIA_SO_LIB} ${ROOT_GEOM_LIB} ${ROOT_EG_LIB} ${ROOT_EGPYTHIA6_LIB} ${LHAPDF_LIB}
    )
  
  #i3_test_scripts(resources/test/*.py)
endif (GENIE_ICETRAY_EXEC)

else (LOG4CPP_FOUND AND XML2_FOUND AND ROOT_FOUND AND GENIE_FOUND)
  if (NOT LOG4CPP_FOUND)
    colormsg(YELLOW "+-- log4cpp not installed.")
  endif (NOT LOG4CPP_FOUND)
  if (NOT XML2_FOUND)
    colormsg(YELLOW "+-- libxml2 not installed.")
  endif (NOT XML2_FOUND)
  if (NOT ROOT_FOUND)
    colormsg(YELLOW "+-- ROOT not installed.")
  endif (NOT ROOT_FOUND)
  if (NOT GENIE_FOUND)
    colormsg(YELLOW "+-- genie not installed.")
  endif (NOT GENIE_FOUND)
  colormsg(RED "+-- not compiling genie-icetray")
endif(LOG4CPP_FOUND AND XML2_FOUND AND ROOT_FOUND AND GENIE_FOUND)
