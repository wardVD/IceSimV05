#ifndef ANFLUX_H
#define ANFLUX_H

#include <NewNuFlux/NewNuFlux.h>
extern "C" {
#include <photospline/bspline.h>
#include <photospline/splinetable.h>
}

namespace NewNuFlux{

class ANFlux : public FluxFunction {
public:
	ANFlux(const std::string& fluxName);
	static boost::shared_ptr<FluxFunction> makeFlux(const std::string& fluxName);
	
	///Computes the expected flux for neutrinos of the given type, energy, and zenith angle
	virtual double getFlux(particleType type, double energy, double cosZenith) const;

private:
	class Evaluator{
	private:
		splinetable data;
		Evaluator(const Evaluator&);
		Evaluator& operator=(const Evaluator&);
	public:
		Evaluator(const std::string&);
		~Evaluator();
		size_t GetNDim() const { return data.ndim; };
		std::pair<double, double> GetExtents(size_t dim) const;
		double operator()(double energy, double cosZenith) const;
	};
	
	std::map<particleType, boost::shared_ptr<Evaluator> > fluxes_;
};

}

#endif // ANFLUX_H
