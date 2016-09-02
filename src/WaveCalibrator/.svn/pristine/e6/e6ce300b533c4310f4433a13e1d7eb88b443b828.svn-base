#include <I3Test.h>

#define private public /* It's eeeevil, eeeevil is goin' on. */
#include "WaveCalibrator/I3WaveCalibrator.h"
#undef private

TEST_GROUP(I3WaveCalibrator);


static
boost::shared_ptr<I3Context>
fake_context()
{
	/* Need pybindings to instantiate the module */
	PyImport_ImportModule("icecube.WaveCalibrator");
	boost::shared_ptr<I3Context> ctx(new I3Context());
	ctx->Put("I3Configuration", boost::shared_ptr<I3Configuration>(new I3Configuration()));
	typedef std::map<std::string, std::pair<FrameFifoPtr, I3ModulePtr> > connections_t;
	ctx->Put("OutBoxes", boost::shared_ptr<connections_t>(new connections_t));
	
	return ctx;
}

TEST(ATWDSize)
{
	boost::shared_ptr<I3Context> ctx = fake_context();
	I3WaveCalibrator plow(*ctx);
	
	I3DOMLaunch dl;
	dl.GetRawATWD(0).resize(64);
	ENSURE_EQUAL(plow.GetATWDSize(dl), 64u);
	
	dl.GetRawATWD(2).resize(128);
	ENSURE_EQUAL(plow.GetATWDSize(dl), 128u);
	
	ENSURE_EQUAL(plow.GetATWDSize(dl, 0), 64u);
}

TEST(WaveformStatusEncoding)
{
	boost::shared_ptr<I3Context> ctx = fake_context();
	I3WaveCalibrator plow(*ctx);
	
	std::vector<I3WaveCalibrator::WaveformBin> bins(128);
	std::vector<I3Waveform::StatusCompound> compounds;
	unsigned i;
	
	std::vector<I3WaveCalibrator::WaveformBin>::const_iterator bin_it = bins.begin();
	for ( ; bin_it != bins.end(); bin_it++) {
		ENSURE_EQUAL(bin_it->status, I3Waveform::SATURATED);
		ENSURE_EQUAL(bin_it->channel, 0);
	}
	
	plow.ExtractWaveformStatus(bins, compounds);
	
	ENSURE_EQUAL(compounds.size(), 1u, "One uniform range.");
	ENSURE_EQUAL(compounds.back().GetInterval().first, 0u);
	ENSURE_EQUAL(compounds.back().GetInterval().second, 128u);
	ENSURE_EQUAL(compounds.back().GetStatus(), I3Waveform::SATURATED);
	ENSURE_EQUAL(compounds.back().GetChannel(), 0);
	
	for (i = 0; i < 10; i++)
		bins[i].status = I3Waveform::VIRGINAL;
	
	compounds.clear();
	plow.ExtractWaveformStatus(bins, compounds);
	
	ENSURE_EQUAL(compounds.size(), 2u, "Two ranges.");
	ENSURE_EQUAL(compounds[0].GetInterval().first, 0u);
	ENSURE_EQUAL(compounds[0].GetInterval().second, 10u);
	ENSURE_EQUAL(compounds[0].GetStatus(), I3Waveform::VIRGINAL);
	ENSURE_EQUAL(compounds[0].GetChannel(), 0);
	ENSURE_EQUAL(compounds[1].GetInterval().first, 10u);
	ENSURE_EQUAL(compounds[1].GetInterval().second, 128u);
	ENSURE_EQUAL(compounds[1].GetStatus(), I3Waveform::SATURATED);
	ENSURE_EQUAL(compounds[1].GetChannel(), 0);
	
	for (i = 10; i < 120; i++)
		bins[i].channel = 1;
	for (i = 120; i < 128; i++)
		bins[i].channel = 2;
		
	compounds.clear();
	plow.ExtractWaveformStatus(bins, compounds);
	ENSURE_EQUAL(compounds.size(), 3u, "three ranges.");
	ENSURE_EQUAL(compounds[0].GetInterval().first, 0u);
	ENSURE_EQUAL(compounds[0].GetInterval().second, 10u);
	ENSURE_EQUAL(compounds[0].GetStatus(), I3Waveform::VIRGINAL);
	ENSURE_EQUAL(compounds[0].GetChannel(), 0);
	ENSURE_EQUAL(compounds[1].GetInterval().first, 10u);
	ENSURE_EQUAL(compounds[1].GetInterval().second, 120u);
	ENSURE_EQUAL(compounds[1].GetStatus(), I3Waveform::SATURATED);
	ENSURE_EQUAL(compounds[1].GetChannel(), 1);
	ENSURE_EQUAL(compounds[2].GetInterval().first, 120u);
	ENSURE_EQUAL(compounds[2].GetInterval().second, 128u);
	ENSURE_EQUAL(compounds[2].GetStatus(), I3Waveform::SATURATED);
	ENSURE_EQUAL(compounds[2].GetChannel(), 2);
}
