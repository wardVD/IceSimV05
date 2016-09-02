
i3_project(weighting PYTHON_DIR python DOCS_DIR resources/docs)

i3_test_scripts(
	resources/test/compare_oneweight.py
	resources/test/corsika_weight_calculator.py
)
