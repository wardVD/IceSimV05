#include <I3Test.h>
#include <string>
#include <icetray/I3Module.h>
#include <icetray/I3Tray.h>
#include <icetray/I3Units.h>
#include <iostream>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include <simclasses/I3MCPE.h>

TEST_GROUP(I3Diplopia);


class ParticleInserter: public I3Module
{ 
	private: 
		double timewindow_; 

	public: 
		ParticleInserter(const I3Context& context) ; 
		void Configure();
		void DAQ(I3FramePtr frame) ;
};

I3_MODULE(ParticleInserter);

ParticleInserter::ParticleInserter(const I3Context& context) : 
			I3Module(context), 
			timewindow_(30.*I3Units::microsecond) 
{ 
	AddOutBox("OutBox"); 
} 
void ParticleInserter::Configure() {} 

void ParticleInserter::DAQ(I3FramePtr frame) 
{ 
	double min_time = -timewindow_;
        double max_time = timewindow_;

        // Make a tree and add particles to it
        I3MCTreePtr mctree(new I3MCTree());

        I3Particle primary1;
        primary1.SetType(I3Particle::PPlus);
	I3MCTreeUtils::AddPrimary(*mctree,primary1);

        I3Particle secondary1;
        secondary1.SetType(I3Particle::MuPlus);
	I3MCTreeUtils::AppendChild(*mctree,primary1,secondary1);

        I3Particle primary2;
        primary2.SetType(I3Particle::PMinus);
	I3MCTreeUtils::AddPrimary(*mctree,primary2);

        I3Particle secondary2;
        secondary2.SetType(I3Particle::MuMinus);
	I3MCTreeUtils::AppendChild(*mctree,primary2,secondary2);

        // Add some photo-electrons only associated with second primary
	I3MCPESeriesMapPtr pemap(new I3MCPESeriesMap);
        I3MCPESeries peseries;
	OMKeyPtr om; 
        
        // put 1000 within the time window
        for (unsigned int i=0;i< 1000; i++)
	{
            I3MCPE pe(secondary2);
            peseries.push_back(pe);
	}
        om = OMKeyPtr(new OMKey(21,30));
        (*pemap)[*om] = peseries;

        frame->Put("I3MCTree",mctree);
        frame->Put("I3MCPESeriesMap",pemap);
        
        I3MapStringDoublePtr weights(new I3MapStringDouble());
        (*weights)["Weight"] = 1.0;
        (*weights)["TimeScale"] = 1.0;
        frame->Put("CorsikaWeightMap",weights);

        PushFrame(frame);
}

class CheckMPHitFilter: public I3Module
{ 
	private: 
		double timewindow_; 

	public: 
		CheckMPHitFilter(const I3Context& context) ; 
		void Configure();
		void DAQ(I3FramePtr frame) ;
};

I3_MODULE(CheckMPHitFilter);

CheckMPHitFilter::CheckMPHitFilter(const I3Context& context) : 
			I3Module(context), 
			timewindow_(30.*I3Units::microsecond) 
{ 
	AddOutBox("OutBox"); 
} 
void CheckMPHitFilter::Configure() {} 

void CheckMPHitFilter::DAQ(I3FramePtr frame) 
{ 
        I3MCPESeriesMap pemap = frame->Get<I3MCPESeriesMap>("I3MCPESeriesMap");
        I3MCTree mctree = frame->Get<I3MCTree>("I3MCTree");

	std::vector<I3Particle> primarylist = 
		I3MCTreeUtils::GetPrimaries(mctree);
        ENSURE(primarylist.size() == 1,
			"There should only be one primary left.\n" + I3MCTreeUtils::Dump(mctree));

        ENSURE(primarylist.front().GetType() == I3Particle::PMinus,
			"The only primary left should be a PMinus\n" + I3MCTreeUtils::Dump(mctree));
}

TEST(CleanHits)
{ 
	std::string I3_TESTDATA(getenv("I3_TESTDATA"));
	std::string gcdfile(I3_TESTDATA+"/sim/GeoCalibDetectorStatus_IC86.55697_corrected.i3.gz");

	I3Tray tray; 
	tray.AddModule("I3InfiniteSource", "source") 
		("Prefix", gcdfile);
	tray.AddModule("ParticleInserter","insert");
	tray.AddModule("MPHitFilter","filter")
		("RemoveBackgroundOnly", false);
	tray.AddModule("CheckMPHitFilter","check");
	tray.Execute(100);
}
