NNF_DIR       = $(shell pwd -P)
SrcSuf        = cpp
ObjSuf        = o
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o

LIBDIR		  = lib
INCDIR		  = public
SRCDIR		  = private/NewNuFlux
OBJDIR		  = build

OS=$(shell uname -s)
ifeq ($(OS),Darwin) 
	DllSuf        = dylib
	SOFLAGS		  = -shared -flat_namespace -undefined dynamic_lookup -multiply_defined suppress -Wl,-install_name,$(NF_DIR)/lib/libNewNuFlux.$(DllSuf)
else
	SOFLAGS		  = -shared 
endif

USERCXXFLAGS := $(CXXFLAGS)
USERLDFLAGS := $(LDFLAGS)
CXXFLAGS = -O2 -g -I$(INCDIR) -Wall -fPIC -Dstandalone -DNEWNUFLUX_DATADIR=\"$(NNF_DIR)\" $(USERCXXFLAGS)
LDFLAGS += -lphotospline

NEUTFLUXSO  = $(LIBDIR)/libNewNuFlux.$(DllSuf)

SRCS = $(SRCDIR)/FluxFunction.cpp $(SRCDIR)/logging.cpp $(SRCDIR)/particleType.cpp $(SRCDIR)/Fluxes/LegacyConventionalFlux.cpp $(SRCDIR)/Fluxes/LegacyPromptFlux.cpp $(SRCDIR)/Fluxes/ANFlux.cpp $(SRCDIR)/Fluxes/LEFlux.cpp $(SRCDIR)/Fluxes/SplineFlux.cpp $(SRCDIR)/Fluxes/IPLEFlux.cpp

OBJSPAT = $(patsubst %$(SrcSuf),%$(ObjSuf), $(SRCS))
OBJS = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%, $(OBJSPAT))

all: $(NEUTFLUXSO)

$(NEUTFLUXSO): $(OBJS)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $(NEUTFLUXSO)
	@$(CXX) $(SOFLAGS) $(LDFLAGS) -o $(NEUTFLUXSO) $(OBJS)

clean:
	@echo "removing intermediate build files and products"
	@rm -rf build lib *.o *.$(DllSuf)

.SUFFIXES: .$(SrcSuf)

#.$(SrcSuf).$(ObjSuf): 
$(OBJDIR)/%$(ObjSuf): $(SRCDIR)/%$(SrcSuf)
	@test -d $(@D) || mkdir -p $(@D)
	@echo compiling $< to $@
	@$(CXX) $(CXXFLAGS) -o $(@) -I$(INCDIR) ${INCALL} -c $<
