if(BLAS_FOUND AND LAPACK_FOUND)
if(SUITESPARSE_FOUND)
	i3_project(wavedeform
		DOCS_DIR resources/docs
		PYTHON_DIR python
	)

	i3_add_library(wavedeform
		private/wavedeform/I3Wavedeform.cxx
		private/wavedeform/nnls.c
		private/wavedeform/nnls_wrapper.cxx

		USE_TOOLS boost python suitesparse blas lapack
		USE_PROJECTS icetray dataclasses 
	)
	
	i3_add_pybindings(wavedeform
		private/pybindings/module.cxx
		USE_TOOLS boost python suitesparse blas lapack
		USE_PROJECTS icetray dataclasses wavedeform
	)

	i3_test_scripts(
		resources/test/wavedeform_extraction_test.py
		resources/test/wavedeform_qtot_test.py
		resources/test/wavedeform_time_test.py
		resources/test/wavedeform_pulsesplit_enthusiasm_test.py
	)
else(SUITESPARSE_FOUND)
	colormsg(CYAN "+-- SuiteSparse not found. Skipping wavedeform.
       If you need it you can install SuiteSparse through I3Ports.")
endif(SUITESPARSE_FOUND)
else(BLAS_FOUND AND LAPACK_FOUND)
	colormsg(CYAN "+-- BLAS and/or LAPACK not found. Skipping wavedeform.")
endif(BLAS_FOUND AND LAPACK_FOUND)
