i3_project(filter-tools
  PYTHON_DIR python
  PYTHON_DEST icecube/filter_tools
  DOCS_DIR resources/docs)


i3_add_library(filter-tools
  private/filter-tools/*.cxx
  USE_TOOLS boost
  USE_PROJECTS icetray dataclasses interfaces
  )

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS filter-tools phys-services
  )
