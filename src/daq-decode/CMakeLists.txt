#
# $Id: CMakeLists.txt 95466 2012-11-14 13:24:57Z nwhitehorn $
#
i3_project(daq-decode
  DOCS_DIR resources/docs
  PYTHON_DIR python)

i3_add_library(daq-decode 
  private/I3DAQDecoderTestStandAlone/*.cxx private/daq-decode/*.cxx private/daq-decode/*/*.cxx
  USE_TOOLS boost
  USE_PROJECTS icetray dataclasses phys-services interfaces 
)

i3_test_executable(test private/test/*.cxx
  USE_PROJECTS daq-decode)

i3_test_scripts(resources/test/*.py)
