#
# $Id$
#

i3_project(rootwriter 
  DOCS_DIR resources/docs
  PYTHON_DIR python
)

if(ROOT_FOUND)

  i3_add_library(rootwriter
    private/rootwriter/*.cxx
  
    USE_TOOLS boost root python
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS icetray dataio dataclasses tableio)

  i3_add_pybindings(rootwriter
    private/pybindings/I3ROOTTableService.cxx
    private/pybindings/module.cxx
    USE_TOOLS boost python root
    USE_PROJECTS icetray rootwriter tableio
    )

  i3_test_executable(test 
    private/test/*.cxx
    USE_TOOLS root
    USE_PROJECTS dataclasses icetray phys-services rootwriter tableio)


else(ROOT_FOUND)

  colormsg(CYAN "+-- ROOT has been disabled in your configuration.")
  colormsg(CYAN "+-- Skipping build of ${PROJECT_NAME}")

endif(ROOT_FOUND)

