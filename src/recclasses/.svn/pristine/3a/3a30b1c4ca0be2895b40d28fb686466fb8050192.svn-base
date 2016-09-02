#
# $Id$
#

i3_project(recclasses
  PYTHON_DIR python
  DOCS_DIR resources/docs
  )

set(PROJECT_DEPENDENCIES icetray tableio dataclasses phys-services dataio)
set(TOOL_DEPENDENCIES boost python)

if(ROOT_FOUND)
  # some classes still support a deprecated ROOT interface
  set(TOOL_DEPENDENCIES ${TOOL_DEPENDENCIES} root)
endif(ROOT_FOUND)

i3_add_library(recclasses
  private/recclasses/*.cxx
  private/recclasses/converter/*.cxx
  USE_TOOLS ${TOOL_DEPENDENCIES}
  USE_PROJECTS ${PROJECT_DEPENDENCIES}
  )

i3_test_executable(test
  private/test/*.cxx
  USE_TOOLS ${TOOL_DEPENDENCIES}
  USE_PROJECTS recclasses ${PROJECT_DEPENDENCIES}
  )

i3_add_pybindings(recclasses
  private/pybindings/*.cxx
  USE_TOOLS ${TOOL_DEPENDENCIES}
  USE_PROJECTS recclasses ${PROJECT_DEPENDENCIES}
  )

# compile CLASSES_TO_REGISTER and pass it to module.cxx;
# see module.cxx for details on how this works
no_dotfile_glob(PY_CXX_FILES private/pybindings/*.cxx )
set(CLASSES_TO_REGISTER "")
foreach(PY_CXX_FILE ${PY_CXX_FILES})
  get_filename_component(BASE_NAME ${PY_CXX_FILE} NAME_WE)
  if(${BASE_NAME} MATCHES module)
    set(MODULE_CXX ${PY_CXX_FILE})
  else()
    set(CLASSES_TO_REGISTER "${CLASSES_TO_REGISTER}(${BASE_NAME})")
  endif()
endforeach()
set_source_files_properties(${MODULE_CXX}
  PROPERTIES
  COMPILE_DEFINITIONS CLASSES_TO_REGISTER=${CLASSES_TO_REGISTER}
  )

i3_test_scripts(resources/test/*.py)
