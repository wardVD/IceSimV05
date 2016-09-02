
i3_project(earthmodel-service
  DOCS_DIR resources/docs
  )


i3_add_library(earthmodel-service
	private/earthmodel-service/EarthModelCalculator.cxx 
	private/earthmodel-service/EarthModelService.cxx 

	USE_TOOLS boost python

	USE_PROJECTS icetray dataclasses
)

i3_add_pybindings(earthmodel_service
  private/pybindings/EarthModelService.cxx
  private/pybindings/EarthModelCalculator.cxx
  private/pybindings/module.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses earthmodel-service
)

i3_test_executable(test 
	private/test/*.cxx
	USE_PROJECTS earthmodel-service dataclasses
)

i3_test_scripts(resources/tests/*.py)


