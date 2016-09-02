GENERATED += dump_flux

#---------------------------------------------------------------
# stand-alone
ifeq (,${I3_BUILD})
	CXXFLAGS += -I../../public
	LDFLAGS += -L../../lib -lNewNuFlux -lphotospline

dump_flux : dump_flux.cxx
	$(CXX) $(CXXFLAGS) $(LDFLAGS) dump_flux.cxx -o dump_flux
#---------------------------------------------------------------
# within IceTray
else
GENERATED += .cached_flags_dump_flux
CXXFLAGS += -DICETRAY

.cached_flags_dump_flux : $(I3_BUILD)/bin/icetray-config makefile
	icetray-config NewNuFlux > .cached_flags_dump_flux

dump_flux : dump_flux.cxx .cached_flags_dump_flux
	$(CXX) $(CXXFLAGS) `cat .cached_flags_dump_flux` dump_flux.cxx -o dump_flux
endif
#---------------------------------------------------------------

all : dump_flux

clean : 
	rm -f $(GENERATED)

