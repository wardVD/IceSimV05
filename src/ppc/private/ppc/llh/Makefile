ifdef I3_PORTS
ver	=	gsl-1.14
dir	=	-I$(I3_PORTS)/include/$(ver) -L$(I3_PORTS)/lib/$(ver)
else
dir	=
endif
gsl	=	$(dir) -lgsl -lgslcblas

ifdef opcl
opts	=	-I$(opcl)/include -L$(opcl)/lib/x86_64 -lOpenCL
else
opts	=	-framework OpenCL
endif

gcpp	=	$(CXX) -O2 #--fast-math
nvcc	=	nvcc -O2 --use_fast_math

xtr	=	-DPDIR

all:	gpu

cpu:
	$(gcpp) llh.cxx -o llh $(xtr) $(gsl) ../gpu/ppc.o

gpu:
	$(nvcc) llh.cxx -o llh $(xtr) $(gsl) ../gpu/ppc.o

ocl:
	$(gcpp) llh.cxx -o llh $(xtr) $(gsl) ../ocl/ppc.o $(opts)

inv:
	c++ inv.cxx -o inv $(gsl)
