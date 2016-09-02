i3_project(corsika-reader
	DOCS_DIR resources/docs
	PYTHON_DIR python
)

i3_add_library(corsika-reader
	private/corsika-reader/I3CORSIKAReader.cxx

	USE_TOOLS boost python 
	USE_PROJECTS icetray dataio dataclasses phys-services
)

i3_executable_script(check resources/scripts/check.py)

i3_test_scripts(
	resources/test/testcorsika.py
)

