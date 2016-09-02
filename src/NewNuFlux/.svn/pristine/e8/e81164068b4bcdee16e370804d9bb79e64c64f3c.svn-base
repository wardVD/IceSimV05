#include <NewNuFlux/particleType.h>

#include <boost/preprocessor/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/stringize.hpp>

#define I3PARTICLE_H_I3Particle_ParticleType                                      \
	(UnknownWithPdgEncoding)(unknown)(Gamma)(EPlus)(EMinus)(MuPlus)(MuMinus)(Pi0) \
	(PiPlus)(PiMinus)(K0_Long)(KPlus)(KMinus)(Neutron)(PPlus)(PMinus)(K0_Short)   \
	(Eta)(Lambda)(SigmaPlus)(Sigma0)(SigmaMinus)(Xi0)(XiMinus)(OmegaMinus)        \
	(NeutronBar)(LambdaBar)(SigmaMinusBar)(Sigma0Bar)(SigmaPlusBar)(Xi0Bar)       \
	(XiPlusBar)(OmegaPlusBar)(DPlus)(DMinus)(D0)(D0Bar)(DsPlus)(DsMinusBar)       \
	(LambdacPlus)(WPlus)(WMinus)(Z0)(NuE)(NuEBar)                                 \
	(NuMu)(NuMuBar)(TauPlus)(TauMinus)(NuTau)(NuTauBar)                           \
	(He3Nucleus)(He4Nucleus)(Li6Nucleus)(Li7Nucleus)(Be9Nucleus)(B10Nucleus)      \
	(B11Nucleus)(C12Nucleus)(C13Nucleus)(N14Nucleus)(N15Nucleus)(O16Nucleus)      \
	(O17Nucleus)(O18Nucleus)(F19Nucleus)(Ne20Nucleus)(Ne21Nucleus)(Ne22Nucleus)   \
	(Na23Nucleus)(Mg24Nucleus)(Mg25Nucleus)(Mg26Nucleus)(Al26Nucleus)(Al27Nucleus)\
	(Si28Nucleus)(Si29Nucleus)(Si30Nucleus)(Si31Nucleus)(Si32Nucleus)(P31Nucleus) \
	(P32Nucleus)(P33Nucleus)(S32Nucleus)(S33Nucleus)(S34Nucleus)(S35Nucleus)      \
	(S36Nucleus)(Cl35Nucleus)(Cl36Nucleus)(Cl37Nucleus)(Ar36Nucleus)(Ar37Nucleus) \
	(Ar38Nucleus)(Ar39Nucleus)(Ar40Nucleus)(Ar41Nucleus)(Ar42Nucleus)(K39Nucleus) \
	(K40Nucleus)(K41Nucleus)(Ca40Nucleus)(Ca41Nucleus)(Ca42Nucleus)(Ca43Nucleus)  \
	(Ca44Nucleus)(Ca45Nucleus)(Ca46Nucleus)(Ca47Nucleus)(Ca48Nucleus)(Sc44Nucleus)\
	(Sc45Nucleus)(Sc46Nucleus)(Sc47Nucleus)(Sc48Nucleus)(Ti44Nucleus)(Ti45Nucleus)\
	(Ti46Nucleus)(Ti47Nucleus)(Ti48Nucleus)(Ti49Nucleus)(Ti50Nucleus)(V48Nucleus) \
	(V49Nucleus)(V50Nucleus)(V51Nucleus)(Cr50Nucleus)(Cr51Nucleus)(Cr52Nucleus)   \
	(Cr53Nucleus)(Cr54Nucleus)(Mn52Nucleus)(Mn53Nucleus)(Mn54Nucleus)(Mn55Nucleus)\
	(Fe54Nucleus)(Fe55Nucleus)(Fe56Nucleus)(Fe57Nucleus)(Fe58Nucleus)             \
	(CherenkovPhoton)(Nu)(Monopole)(Brems)(DeltaE)(PairProd)(NuclInt)(MuPair)     \
	(Hadrons)(ContinuousEnergyLoss)(FiberLaser)(N2Laser)(YAGLaser)                \
	(STauPlus)(STauMinus)

namespace NewNuFlux{
	hid_t getHDF5TypeForParticleType(){
		//blar, not thread safe; don't care right now
		static bool initialized=false;
		static hid_t type;
		if(initialized)
			return(type);

		type=H5Tenum_create(H5T_NATIVE_INT); //TODO: might be non-optimal

		I3Particle::ParticleType dummy;
		#define reg_value(r,data,value) \
		H5Tenum_insert(type, BOOST_PP_STRINGIZE(value), (dummy=I3Particle::value,&dummy));

		BOOST_PP_SEQ_FOR_EACH(reg_value,unused,I3PARTICLE_H_I3Particle_ParticleType);

		initialized=true;
		return(type);
	}
} //namesapce NewNuFlux
