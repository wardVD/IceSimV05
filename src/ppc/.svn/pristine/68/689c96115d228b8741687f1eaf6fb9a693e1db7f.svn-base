i3_project(ppc
  DOCS_DIR resources/docs
)

i3_add_library(ppc
  private/ppc/i3ppc.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses simclasses phys-services
)

i3_add_library(ppc-eff
  private/ppc-eff/*.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses
)

i3_add_library(ppc-pick
  private/ppc-pick/*.cxx
  USE_TOOLS boost python
  USE_PROJECTS icetray dataclasses interfaces
)


if(CMAKE_VERSION VERSION_GREATER 2.8.0)
  find_package(CUDA QUIET)
else(CMAKE_VERSION VERSION_GREATER 2.8.0)
  colormsg(RED "+-- cmake version <2.8, diabling CUDA detection")
  SET(CUDA_FOUND FALSE)
endif(CMAKE_VERSION VERSION_GREATER 2.8.0)

if(OPENCL_FOUND)
  colormsg(GREEN "+-- OpenCL SDK found, enabling optional support")
  i3_add_library(xppc
                 private/ppc/ocl/ppc.cxx
                 USE_TOOLS opencl
                 COMPILE_FLAGS "-fPIC -DXLIB"
  )


elseif(CUDA_FOUND)
  colormsg(GREEN "+-- CUDA SDK found, enabling optional support")

  ## HACK: "multiple 'C' linkage" errors w/ fast-math
  if(APPLE)
    ## HACK: fixes an rpath issue with CUDA > 3.x
    set(CUDA_LIBRARIES ${CUDA_LIBRARIES} /usr/local/cuda/lib/libtlshook.dylib)
    SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -Xptxas=-v -arch=sm_12 -O2)
  else(APPLE)
    SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -Xptxas=-v -arch=sm_12 -O2 --use_fast_math --compiler-options=-O2,--fast-math )
  endif(APPLE)

  CUDA_ADD_EXECUTABLE(ppc-exe private/ppc/gpu/ppc.cu)
  set(old_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  SET(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -DXLIB )
  CUDA_ADD_LIBRARY(xppc private/ppc/gpu/ppc.cu)
  set(CMAKE_CXX_FLAGS "${old_CMAKE_CXX_FLAGS}")

else(OPENCL_FOUND)
  colormsg(CYAN "+-- SDK not found, disabling optional GPU support")
  add_executable(ppc-exe private/ppc/gpu/ppc.cxx)
  #SET(CMAKE_SHARED_LIBRARY_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DXLIB")
  set(old_CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
  if(APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dynamic -dynamiclib -o libxppc.dylib")
  else(APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-soname,xppc")
  endif(APPLE)
  add_library(xppc private/ppc/gpu/ppc.cxx)
  set(CMAKE_EXE_LINKER_FLAGS "${old_CMAKE_EXE_LINKER_FLAGS}")

endif(OPENCL_FOUND)

target_link_libraries(ppc xppc)
