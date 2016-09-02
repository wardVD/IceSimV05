/** $Id$
 * @file
 * @author Jakob van Santen <vansanten@wisc.edu>
 *
 * $Revision$
 * $Date$
 */

#include <MuonGun/EnergyDistribution.h>
#include <phys-services/I3RandomService.h>

namespace I3MuonGun {

EnergyDistribution::~EnergyDistribution() {};

double
EnergyDistribution::operator()(double d, double ct, 
    unsigned m, double r, double e) const
{
	return std::exp(GetLog(d, ct, m, r, e));
}

bool
SplineEnergyDistribution::operator==(const EnergyDistribution &o) const
{
	const SplineEnergyDistribution *other = dynamic_cast<const SplineEnergyDistribution*>(&o);
	if (!other)
		return false;
	else
		return (singles_ == other->singles_ && bundles_ == other->bundles_);
}

SplineEnergyDistribution::SplineEnergyDistribution(const std::string &singles, const std::string &bundles)
    : singles_(singles), bundles_(bundles)
{
	if (singles_.GetNDim() != 3u)
		log_fatal("'%s' does not appear to be a single-muon energy distribution", singles.c_str());
	if (bundles_.GetNDim() != 5u)
		log_fatal("'%s' does not appear to be a muon bundle energy distribution", bundles.c_str());
	// Extrapolate with a constant below the minimum supported energy
	minLogEnergy_ = std::min(singles_.GetExtents(2).first, bundles_.GetExtents(2).first);
}

double
SplineEnergyDistribution::GetLog(double depth, double cos_theta, 
    unsigned multiplicity, double radius, double energy) const
{
	double coords[5] = {cos_theta, depth, static_cast<double>(multiplicity),
	    std::min(radius, bundles_.GetExtents(3).second),
	    std::max(minLogEnergy_, std::log(energy))};
	double logprob;
	
	if (multiplicity < 2) {
		coords[2] = coords[4];
		if (singles_.Eval(coords, &logprob) != 0)
			return -std::numeric_limits<double>::infinity();
	} else if (bundles_.Eval(coords, &logprob) != 0)
		return -std::numeric_limits<double>::infinity();
	
	// Bundle spline is fit to log(dP/dr^2 dlogE)
	if (multiplicity > 1)
		logprob += std::log(2*radius);
	
	return logprob;
}

double
SplineEnergyDistribution::Generate(I3RandomService &rng __attribute__((unused)), double depth __attribute__((unused)), double cos_theta __attribute__((unused)), 
    unsigned multiplicity __attribute__((unused)), double radius __attribute__((unused))) const
{
	log_fatal("Sampling is not yet implemented");
}

BMSSEnergyDistribution::BMSSEnergyDistribution() : 
    beta_(0.42), g0_(-0.232), g1_(3.961), e0a_(0.0304), e0b_(0.359), e1a_(-0.0077), e1b_(0.659),
    a0_(0.0033), a1_(0.0079), b0a_(0.0407), b0b_(0.0283), b1a_(-0.312), b1b_(6.124),
    q0_(0.0543), q1_(-0.365), c0a_(-0.069), c0b_(0.488), c1_(-0.117),
    d0a_(-0.398), d0b_(3.955), d1a_(0.012), d1b_(-0.35)
{}

bool
BMSSEnergyDistribution::operator==(const EnergyDistribution &o) const
{
	return dynamic_cast<const BMSSEnergyDistribution*>(&o);
}

double
BMSSEnergyDistribution::GetLog(double depth, double cos_theta, 
    unsigned m, double r, double energy) const
{
	// Convert to water-equivalent depth
	double h = (200*I3Units::m/I3Units::km)*0.832 + (depth-(200*I3Units::m/I3Units::km))*0.917;
	double bX = beta_*h/cos_theta;
	double g, eps;
	if (m == 1) {
		g = g0_*log(h) + g1_;
		eps = e0a_*exp(e0b_*h)/cos_theta + e1a_*h + e1b_;
	} else {
		m = std::min(m, 4u);
		double a = a0_*h + a1_;
		double b = (b0a_*m + b0b_)*h + (b1a_*m + b1b_);
		double q = q0_*h + q1_;
		g = a*r + b*(1 - 0.5*exp(q*r));
		double c = (c0a_*h + c0b_)*exp(c1_*r);
		double d = (d0a_*h + d0b_)*pow(r, d1a_*h + d1b_);
		eps = c*acos(cos_theta) + d;
	}
	double norm = (g-1)*pow(eps, g-1)*exp((g-1)*bX)*pow(1-exp(-bX), g-1);
	return std::log(norm*exp((1-g)*bX)*pow(energy + eps*(1-exp(-bX)), -g));
}

double
BMSSEnergyDistribution::Generate(I3RandomService &rng __attribute__((unused)), double depth __attribute__((unused)), double cos_theta __attribute__((unused)), 
    unsigned multiplicity __attribute__((unused)), double radius __attribute__((unused))) const
{
	log_fatal("Sampling is not yet implemented");
}

OffsetPowerLaw::OffsetPowerLaw() : gamma_(NAN), offset_(NAN), emin_(NAN), emax_(NAN)
{}

OffsetPowerLaw::OffsetPowerLaw(double gamma, double offset, double emin, double emax)
    : gamma_(gamma), offset_(offset), emin_(emin), emax_(emax)
{
	if (gamma <= 0)
		log_fatal("Power law index must be > 0");
	else if (gamma == 1) {
		nmin_ = std::log(emin + offset);
		nmax_ = std::log(emax + offset);
		norm_ = 1./(nmax_ - nmin_);
	} else {
		nmin_ = std::pow(emin + offset, 1-gamma);
		nmax_ = std::pow(emax + offset, 1-gamma);
		norm_ = (1-gamma)/(nmax_ - nmin_);
	}
	lognorm_ = std::log(norm_);
}

bool
OffsetPowerLaw::operator==(const OffsetPowerLaw &other) const
{
	return (gamma_ == other.gamma_ && offset_ == other.offset_
	    && emin_ == other.emin_ && emax_ == other.emax_ );
}

double
OffsetPowerLaw::operator()(double energy) const
{
	if (energy <= emax_ && energy >= emin_)
		return norm_*std::pow(energy + offset_, -gamma_);
	else
		return 0.;
}

double
OffsetPowerLaw::GetLog(double energy) const
{
	if (energy <= emax_ && energy >= emin_)
		return lognorm_ - gamma_*std::log(energy + offset_);
	else
		return -std::numeric_limits<double>::infinity();
}

double
OffsetPowerLaw::Generate(I3RandomService &rng) const
{
	if (gamma_ == 1)
		return std::exp(rng.Uniform()*(nmax_ - nmin_) + nmin_) - offset_;
	else
		return std::pow(rng.Uniform()*(nmax_ - nmin_) + nmin_, 1./(1.-gamma_)) - offset_;
}

template <typename Archive>
void
EnergyDistribution::serialize(Archive &ar __attribute__ ((unused)), unsigned version __attribute__ ((unused)))
{}
	
template <typename Archive>
void
SplineEnergyDistribution::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");
	
	ar & make_nvp("EnergyDistribution", base_object<EnergyDistribution>(*this));
	ar & make_nvp("SingleEnergy", singles_);
	ar & make_nvp("BundleEnergy", bundles_);
}

template <typename Archive>
void
OffsetPowerLaw::serialize(Archive &ar, unsigned version)
{
	if (version > 0)
		log_fatal_stream("Version "<<version<<" is from the future");
	
	ar & make_nvp("Gamma", gamma_);
	ar & make_nvp("Offset", offset_);
	ar & make_nvp("MinEnergy", emin_);
	ar & make_nvp("MaxEnergy", emax_);
	
	*this = OffsetPowerLaw(gamma_, offset_, emin_, emax_);
}

}

I3_SERIALIZABLE(I3MuonGun::EnergyDistribution);
I3_SERIALIZABLE(I3MuonGun::SplineEnergyDistribution);
I3_SERIALIZABLE(I3MuonGun::OffsetPowerLaw);
