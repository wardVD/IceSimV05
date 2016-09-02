#include <I3Test.h>
#include <icetray/I3Tray.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/calibration/I3Calibration.h>
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <simclasses/I3MCPulse.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/I3DOMFunctions.h>

/* This test checks that running DOMLauncher, WaveCalibrator and wavedeform in sequence
preserves charge to within a small (currently 3%) margin, for a variety of light levels.
PMTResponseSimulator is deliberately omitted, since many of its functions vary the amount
of charge present (applying the SPE distribution, pre- and afterpulses, saturation). */

TEST_GROUP(ChargeRoundTrip);

class HitGenerator : public I3Module{
private:
	double hitCharge;
public:
	HitGenerator(const I3Context& context):
	I3Module(context),
	hitCharge(1){
		AddOutBox("OutBox");
	}

	virtual void DAQ(boost::shared_ptr<I3Frame> frame){
		std::cout << "generating hits with charge " << hitCharge << std::endl;
		const I3Geometry& geo=frame->Get<I3Geometry>();
		const I3Calibration& cal=frame->Get<I3Calibration>();
		const I3DetectorStatus& det=frame->Get<I3DetectorStatus>();
		const I3Vector<OMKey> bad=frame->Get<I3Vector<OMKey> >("BadDomsList");
		boost::shared_ptr<I3Map<OMKey,std::vector<I3MCPulse> > > hits(new I3Map<OMKey,std::vector<I3MCPulse> >);

		for(I3Map<OMKey, I3OMGeo>::const_iterator domgeo=geo.omgeo.begin(), geoend=geo.omgeo.end(); domgeo!=geoend; domgeo++){
			if(domgeo->first.GetString()!=42 || domgeo->first.GetOM()>60) //let's use just the in-ice part of one string
				continue;
			if(std::find(bad.begin(),bad.end(),domgeo->first)!=bad.end())
				continue;
			double gain=PMTGain(det.domStatus.find(domgeo->first)->second,cal.domCal.find(domgeo->first)->second);
			if(gain==0.0 || std::isnan(gain))
				continue;
			for(unsigned int i=0; i<200; i++){
				I3MCPulse hit;
				hit.charge=hitCharge;
				hit.time=30.0*i;
				(*hits)[domgeo->first].push_back(hit);
			}
		}

		frame->Put("MCHitSeriesMap",hits);
		//this is useless, but keeps WaveCalibrator quiet
		frame->Put("I3EventHeader",boost::shared_ptr<I3EventHeader>(new I3EventHeader));
		PushFrame(frame);
		hitCharge++; // next time around, use more charge
	}
};
I3_MODULE(HitGenerator)

class ChargeConservationChecker : public I3Module{
public:
	ChargeConservationChecker(const I3Context& context):I3Module(context){}

	virtual void DAQ(boost::shared_ptr<I3Frame> frame){
		const I3Map<OMKey,std::vector<I3MCPulse> >& mchits=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("MCHitSeriesMap");
		const I3Map<OMKey,std::vector<I3RecoPulse> >& pulses=frame->Get<I3Map<OMKey,std::vector<I3RecoPulse> > >("WavedeformPulses");
		double charge_before=0.0, charge_after=0.0;

		for(I3Map<OMKey,std::vector<I3MCPulse> >::const_iterator dom=mchits.begin(), end=mchits.end(); dom!=end; dom++)
			for(std::vector<I3MCPulse>::const_iterator hit=dom->second.begin(), end=dom->second.end(); hit!=end; hit++)
				charge_before+=hit->charge;
		for(I3Map<OMKey,std::vector<I3RecoPulse> >::const_iterator dom=pulses.begin(), end=pulses.end(); dom!=end; dom++)
			for(std::vector<I3RecoPulse>::const_iterator pulse=dom->second.begin(), end=dom->second.end(); pulse!=end; pulse++)
				charge_after+=pulse->GetCharge();
		double charge_ratio=charge_after/charge_before;
		std::cout << "Input charge: "<< charge_before<<std::endl;
        std::cout << "charge ratio (after/before): " << charge_ratio << std::endl;

		ENSURE(charge_ratio>.97 && "Charge ratio shouldn't be too low");
		ENSURE(charge_ratio<1.03 && "Charge ratio shouldn't be too high");
	}
};
I3_MODULE(ChargeConservationChecker)

TEST(ChargeRoundTrip){
	I3Tray tray;

        std::string I3_TESTDATA;
        if(std::getenv("I3_TESTDATA") != NULL){
          I3_TESTDATA = std::string(std::getenv("I3_TESTDATA"));
        }else{
          ENSURE( false, "Neither I3_PORTS nor I3_TESTDATA is defined! On of them must be...");
        }

	tray.AddService("I3GSLRandomServiceFactory","rng")
          ("Seed",5);
	tray.AddModule("I3InfiniteSource","FrameMaker")
          ("Stream",I3Frame::DAQ)
          ("Prefix", I3_TESTDATA + std::string("/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"));
	tray.AddModule("HitGenerator","Hitter");
	tray.AddModule("DOMLauncher", "Guildenstern")
          ("Input","MCHitSeriesMap")
          ("Output","InIceRawData");
	tray.AddModule("I3WaveCalibrator","Domcal");
	tray.AddModule("I3Wavedeform","Deform");
	tray.AddModule("ChargeConservationChecker","Checker");
	tray.Execute(3+20);
	tray.Finish();
}
