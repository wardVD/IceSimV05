#include "recclasses/I3LaputopParams.h"
#include "recclasses/LaputopEnums.h"
#include "recclasses/I3TopLateralFitParams.h"
#include <dataclasses/physics/I3Particle.h>

#include <I3Test.h>
#include <icetray/I3Frame.h>
#include <icetray/portable_binary_archive.hpp>

TEST_GROUP(I3FrameTransformations)

TEST(ConvertTopLateralFitParams2LaputopParams)
{
	using namespace Laputop;

	I3TopLateralFitParamsPtr tlfp(new I3TopLateralFitParams);
	tlfp->S125 = 10.0;
	tlfp->Beta = 5.0;
	tlfp->Beta_err = 1.0;

	I3Frame frame;
	frame.Put("TestParams", tlfp);
	I3LaputopParamsConstPtr lp = frame.Get<I3LaputopParamsConstPtr>("TestParams");
	ENSURE(bool(lp) == true);
	ENSURE_EQUAL(std::log10(tlfp->S125), lp->GetValue(Parameter::Log10_S125));
	ENSURE_EQUAL(tlfp->Beta, lp->GetValue(Parameter::Beta));
	ENSURE_EQUAL(tlfp->Beta_err, lp->GetError(Parameter::Beta));

	I3ParticlePtr particle(new I3Particle);
	particle->SetPos(1.0, 2.0, 3.0);
	particle->SetDir(0.0, 0.0, 1.0);
	frame.Put("Test", particle);
	I3LaputopParamsConstPtr lp2 = frame.Get<I3LaputopParamsConstPtr>("TestParams");
	ENSURE(bool(lp2) == true);
	ENSURE_EQUAL(std::log10(tlfp->S125), lp2->GetValue(Parameter::Log10_S125));
	ENSURE_EQUAL(tlfp->Beta, lp2->GetValue(Parameter::Beta));
	ENSURE_EQUAL(tlfp->Beta_err, lp2->GetError(Parameter::Beta));
	ENSURE_EQUAL(particle->GetX(), lp2->GetValue(Parameter::Xc));
	ENSURE_EQUAL(particle->GetY(), lp2->GetValue(Parameter::Yc));
}
