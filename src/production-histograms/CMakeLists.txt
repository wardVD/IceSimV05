i3_project(production-histograms
  PYTHON_DIR python
  PYTHON_DEST icecube/production_histograms
  DOCS_DIR resources/docs
)

i3_test_scripts(
  resources/tests/test_histogram.py
  resources/tests/test_histogram_module.py
  resources/tests/test_expression_histogram.py
  resources/tests/test_daq_histogram.py
  resources/tests/test_physics_histogram.py
  # Get test data, enable this, and close ticket #1623
  #resources/tests/test_example_scripts.py
)
