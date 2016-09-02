i3_project(vuvuzela
    PYTHON_DEST icecube/vuvuzela
    PYTHON_DIR python
    DOCS_DIR resources/docs
)

i3_add_library(vuvuzela
	private/vuvuzela/Inject.cxx 
	private/vuvuzela/Vuvuzela.cxx 
	private/vuvuzela/VuvuzelaFunctions.cxx 
	USE_TOOLS boost python
	USE_PROJECTS icetray dataclasses phys-services simclasses
)

i3_add_pybindings(vuvuzela
  private/pybindings/module.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses vuvuzela
)

i3_test_scripts(resources/scripts/generateNoiseOnly.py)

i3_test_executable(test 
	private/test/main.cxx
	private/test/CompareMCHits.cxx
	private/test/GetTimeRange.cxx
	private/test/HitSeriesAddition.cxx
	private/test/MakeNonThermals.cxx
	private/test/MakeThermals.cxx
	USE_TOOLS boost python gsl
	USE_PROJECTS vuvuzela
	)

set_tests_properties(vuvuzela::generateNoiseOnly.py PROPERTIES LABELS RUN_LONG)
      