#
# $Id: CMakeLists.txt $
#
i3_project(topsimulator
  PYTHON_DIR python
  DOCS_DIR resources/docs
)

SET(LIB_${PROJECT_NAME}_SOURCEFILES
  private/topsimulator/*.cxx
  private/topsimulator/*/*.cxx
  )

SET(LIB_${PROJECT_NAME}_TOOLS boost python gsl)

if(ROOT_FOUND)
  LIST(APPEND LIB_${PROJECT_NAME}_TOOLS root)
endif(ROOT_FOUND)

i3_add_library(topsimulator
  ${LIB_${PROJECT_NAME}_SOURCEFILES}
  USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
#KR  USE_PROJECTS icetray dataclasses simclasses phys-services pmt-simulator tableio
  USE_PROJECTS icetray dataclasses simclasses phys-services tableio
)

i3_add_pybindings(topsimulator
  private/pybindings/vem_yield.cxx
  private/pybindings/ParticleUnthinner.cxx
  private/pybindings/GeoFunctions.cxx
  private/pybindings/ExtendedI3Particle.cxx
  private/pybindings/module.cxx
  USE_TOOLS ${LIB_${PROJECT_NAME}_TOOLS}
  USE_PROJECTS topsimulator
)

i3_test_executable(test
  private/test/I3IntersectCylinderTest.cxx
  private/test/SparseHistogramTest.cxx
  #private/test/I3PMTSimTest.cxx
  private/test/main.cxx
  #private/test/*.cxx
  USE_PROJECTS topsimulator sim-services simclasses
)        

i3_executable_script(simicetop resources/examples/sim-icetop.py)
i3_executable_script(simsingleparticles resources/examples/sim-single-particles.py)

i3_test_scripts(
  resources/test/test_validationsuite.py
  resources/test/test_pecompression.py
  resources/test/test_examples.py
  # resources/test/test_against_ref.py <- disabled until reference files are updated
)
set_tests_properties(topsimulator::test_pecompression.py PROPERTIES LABELS RUN_LONG)
set_tests_properties(topsimulator::test_examples.py PROPERTIES LABELS RUN_LONG)
# set_tests_properties(topsimulator::test_against_ref.py PROPERTIES LABELS RUN_LONG)
