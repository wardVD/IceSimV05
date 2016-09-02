#include <iostream>
#include <NewNuFlux/NewNuFlux.h>

int main(){
	//print out all of the available models
	NewNuFlux::printModels();
	
	using NewNuFlux::FluxFunction;
	//get the Honda 2006 model
	boost::shared_ptr<FluxFunction> flux(NewNuFlux::makeFlux("honda2006"));
	
	//add the effects of a knee in the CR spectrum
	boost::dynamic_pointer_cast<NewNuFlux::KneeReweightable>(flux)->setKneeReweightingModel("gaisserH3a_elbert");
	
	//increase the contribution from Kaons by 10%
	boost::dynamic_pointer_cast<NewNuFlux::PionKaonAdjustable>(flux)->setRelativeKaonContribution(1.1);
	
	//print out a simple table of flux values for different energies at a fixed zenith angle
	//Results are similar to the first column of Table XXII in arXiv:astro-ph/0611418
	double cz=1.0; //cosine of zenith angle
	std::cout << std::endl;
	for(double en=10; en<=1.1e4; en*=pow(10.,.1)){
		double f=flux->getFlux(I3Particle::NuMu,en,cz);
		std::cout << en << ' ' << f << '\n';
	}
	
	return(0);
}