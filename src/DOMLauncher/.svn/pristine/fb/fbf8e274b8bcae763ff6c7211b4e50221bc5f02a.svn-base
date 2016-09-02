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

TEST_GROUP(PulseTemplateTests);

class HitGenerator2 : public I3Module{
private:
        double hitCharge;
public:
        HitGenerator2(const I3Context& context):
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
                        for(unsigned int i=0; i<100; i++){
                                I3MCPulse hit;
                                hit.charge=hitCharge;
                                hit.time=100.0*i;
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
I3_MODULE(HitGenerator2)

class PulseTemplateChecker : public I3Module{
public:
        PulseTemplateChecker(const I3Context& context):I3Module(context){}

        virtual void DAQ(boost::shared_ptr<I3Frame> frame){
                 
            
            
                const I3Map<OMKey,std::vector<I3MCPulse> >& pulses_mc=frame->Get<I3Map<OMKey,std::vector<I3MCPulse> > >("MCHitSeriesMap");
                double charge_before=0.0;


                for(I3Map<OMKey,std::vector<I3MCPulse> >::const_iterator dom=pulses_mc.begin(), end=pulses_mc.end(); dom!=end; dom++)
                        for(std::vector<I3MCPulse>::const_iterator pulse=dom->second.begin(), end=dom->second.end(); pulse!=end; pulse++){
                                charge_before+=pulse->charge;
                        }
                        
                const I3Map<OMKey,std::vector<I3RecoPulse> >& pulses=frame->Get<I3Map<OMKey,std::vector<I3RecoPulse> > >("pulses");
                const I3Map<OMKey,std::vector<I3RecoPulse> >& pulsesT=frame->Get<I3Map<OMKey,std::vector<I3RecoPulse> > >("pulsesT");
                double charge_t=0.0, charge=0.0;
                uint n_pulse_sumT = 0,n_pulse_sum = 0; 
                for(I3Map<OMKey,std::vector<I3RecoPulse> >::const_iterator dom=pulsesT.begin(), end=pulsesT.end(); dom!=end; dom++)
                        for(std::vector<I3RecoPulse>::const_iterator pulse=dom->second.begin(), end=dom->second.end(); pulse!=end; pulse++){
                                charge_t+=pulse->GetCharge();
                                n_pulse_sumT++;
                        }
                
                for(I3Map<OMKey,std::vector<I3RecoPulse> >::const_iterator dom=pulses.begin(), end=pulses.end(); dom!=end; dom++)
                        for(std::vector<I3RecoPulse>::const_iterator pulse=dom->second.begin(), end=dom->second.end(); pulse!=end; pulse++){
                                charge+=pulse->GetCharge();
                                n_pulse_sum++;
                        }
                double charge_ratio=charge_t/charge;
                std::cerr << "charge Tabulated: " << charge_t << std::endl;
                std::cerr << "charge Analytical: " << charge << std::endl;
                std::cerr << "charge ratio (Tabulated/Analytical): " << charge_ratio << std::endl;
                std::cerr << "charge ratio (Tabulated/mc): " << charge_t/charge_before << std::endl;
                std::cerr << "charge ratio (Analytical/mc): " << charge/charge_before << std::endl;
                std::cerr << "Number of pulses with tabulation: " << n_pulse_sumT << std::endl;
                std::cerr << "Number of pulses without tabulation: " << n_pulse_sum << std::endl;
                
                ENSURE(charge_ratio>.97 && "Charge ratio shouldn't be too low");
                ENSURE(charge_ratio<1.03 && "Charge ratio shouldn't be too high");
        }
};
I3_MODULE(PulseTemplateChecker)

TEST(PulseTemplateTest){
        I3Tray tray;

        std::string I3_TESTDATA;
        if(getenv("I3_TESTDATA") != NULL) {
          I3_TESTDATA = std::string(getenv("I3_TESTDATA"));
        }else{
          ENSURE( false, "Neither I3_PORTS nor I3_TESTDATA is defined! On of them must be...");
        }

        GetIcetrayLogger()->SetLogLevel(I3LOG_DEBUG);
        tray.AddService("I3GSLRandomServiceFactory","rng")
          ("Seed",5);
        tray.AddModule("I3InfiniteSource","FrameMaker")
          ("Stream",I3Frame::DAQ)
          ("Prefix", I3_TESTDATA + std::string("/sim/GeoCalibDetectorStatus_IC86.55697_corrected_V2.i3.gz"));
        tray.AddModule("HitGenerator2","Hitter");
        
        tray.AddModule("DOMLauncher", "Untabulated")
          ("Input","MCHitSeriesMap")
          ("Output","InIceRawData")
          ("UseTabulatedPT",false);

        tray.AddModule("DOMLauncher", "Tabulated")
          ("Input","MCHitSeriesMap")
          ("Output","InIceRawDataT")
          ("UseTabulatedPT",true);

        tray.AddModule("I3WaveCalibrator","Domcal")
          ("Launches","InIceRawData")
          ("WaveformRange","CalibratedWaveformRange")
          ("Waveforms","Waveforms")
          ("Errata","err");

        tray.AddModule("I3Wavedeform", "Deform")
          ("WaveformTimeRange","CalibratedWaveformRange")
          ("Waveforms", "Waveforms")
          ("Output", "pulses");
        
        tray.AddModule("I3WaveCalibrator","DomcalT")
          ("WaveformRange","CalibratedWaveformRangeT")
          ("Launches","InIceRawDataT")
          ("Waveforms","WaveformsT");

        tray.AddModule("I3Wavedeform","DeformT")
          ("WaveformTimeRange","CalibratedWaveformRangeT")
          ("Waveforms", "WaveformsT")
          ("Output", "pulsesT");

        tray.AddModule("PulseTemplateChecker","Checker");
        tray.Execute(3+20);
        tray.Finish();
}
