/**
 * copyright  (C) 2012
 * the WimpSim::IceCube Collaboration
 * $Id: $
 *
 * @file I3WimpSimReaderTest.cxx
 * @author mzoll <marcel.zoll@fysik.su.se>
 * @date $Date: 2012-12-20$
 * @brief Tests the I3WimpSimReader for the exact frame content by creating a TestClass and emulating the icetray
 */

//DANGER This script has problems with double instanisation, and was therefore substituted by resources/test/test_earth_event.py and test_sun_event.py

#include <I3Test.h>
#include <vector>
#include <string>
#include <list>
#include <stdlib.h>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/assign/list_of.hpp>
#include <icetray/I3Tray.h>
#include <icetray/I3ServiceFactory.h>
#include "dataclasses/I3Constants.h"
#include <dataclasses/I3Time.h>
#include <dataclasses/physics/I3EventHeader.h>
#include <wimpsim-reader/I3WimpSimReader.h>
#include <wimpsim-reader/I3WimpParams.h>
#include <wimpsim-reader/WimpSimTools.h>
#include <icetray/I3Module.h>
#include <icetray/I3Units.h>

namespace constants = boost::math::constants;

// all below is a test implemetation which should finally inteface to the WimpSimReader Module

/// a test class which looks like the WimpSimReader but is not registered as a module: A Abomination; provides sneaky peaky routines inside
class WimpSimReaderTester : public I3WimpSimReader {
public:
  WimpSimReaderTester(const I3Context &context):I3WimpSimReader(context) {}

  /// implement all internal tests in this routine
  bool AsExpected() {return true;}

  //SPILL THE GUTS: expose all internals which never should have been exposed
  void SetInbox(boost::shared_ptr<std::deque<boost::shared_ptr<I3Frame> > > inbox) {inbox_ = inbox;} //FrameFifoPtr

  I3Configuration* GetConfigurationPtr() {return &configuration_;}
  boost::shared_ptr<std::deque<boost::shared_ptr<I3Frame> > > GetInbox () {return inbox_;}
  std::map<std::string, std::pair<boost::shared_ptr<std::deque<I3FramePtr> >, I3ModulePtr> > GetOutboxes() {return outboxes_;}
  std::map<std::string, std::pair<boost::shared_ptr<std::deque<I3FramePtr> >, I3ModulePtr> >* GetOutboxesPtr() {return &outboxes_;}
};

TEST_GROUP(WimpSimReader);

//======================================
// Infrastructure for running a module without a tray

static struct config_end{} end_config;

template<typename T>
class param_setter {
private:
	T& owner;
	I3Configuration& config;
public:
	param_setter<T>(T& t, I3Configuration& c):owner(t),config(c){}
	param_setter<T>(const param_setter<T>& rhs):owner(rhs.owner),config(rhs.config){}

	template <typename U>
	param_setter& operator()(const std::string& paramName, U value){
		config.Set(paramName, boost::python::object(value));
		return(*this);
	}
	void operator()(const config_end&){ owner.FinalizeConfiguration(); };
};

class ServiceFactoryWrapper{
private:
	std::string factoryClassName;
	I3Context& context;
	boost::shared_ptr<I3ServiceFactory> factory;
public:
	ServiceFactoryWrapper(I3Context& context, const std::string& factName):
	factoryClassName(factName),
	context(context),
	factory(I3::Singleton<I3ServiceFactoryFactory>::get_const_instance().Create(factoryClassName)(context))
	{}

	param_setter<ServiceFactoryWrapper> Configure(){
		//unfortunately this appears to be impossible without using a const_cast hack
		return(param_setter<ServiceFactoryWrapper>(*this,const_cast<I3Configuration&>(factory->GetConfiguration())));
	}
	void FinalizeConfiguration(){
		factory->Configure();
	}

	void installService(){
		factory->InitializeService(context);
	}
};

///the officiall wrapper around the Monster which will also expose Configuration routines and forced Proecssing
class WimpSimReaderTesterSetup{
private:
	boost::shared_ptr<std::deque<boost::shared_ptr<I3Frame> > > inbox;
	I3Context& context;
	boost::shared_ptr<WimpSimReaderTester> wsrt_instance;
	std::vector<boost::shared_ptr<I3Frame> > frames;
public:
	///constructor of the monster
	WimpSimReaderTesterSetup(I3Context& context):
	  inbox(new std::deque<boost::shared_ptr<I3Frame> >),
	  context(context),
	  wsrt_instance(new WimpSimReaderTester(context))
	{
	  wsrt_instance->GetConfigurationPtr()->ClassName(I3::name_of(typeid(*wsrt_instance)));
	  wsrt_instance->SetInbox(inbox);
	}

	///pass parameters to the monster
	param_setter<WimpSimReaderTesterSetup> Configure() {
	  return(param_setter<WimpSimReaderTesterSetup>(*this, *(wsrt_instance->GetConfigurationPtr())));
	}

	/// call this if you ended your configuration of the Monster
	void FinalizeConfiguration(){
	  wsrt_instance->Configure_();
	}

	/// expose the I3Module::Process() methode
	void process(){
		wsrt_instance->Process();
	}

	/// expose a methode which will give you the next frame in the pipeline and force the I3Module::Process() routine if neccessary
	boost::shared_ptr<I3Frame> RetrieveNextFrame(){
		//retrieve everything from the outboxes and append it to the constant stream, if we run out of frames: do process()
		if (frames.empty()) {
		  wsrt_instance->Process();
		  if ((*wsrt_instance->GetOutboxesPtr())["OutBox"].first->empty()) //nomore to process; stream has ended
		    return boost::shared_ptr<I3Frame>();
		  while (not (*wsrt_instance->GetOutboxesPtr())["OutBox"].first->empty()) {
		    frames.insert(frames.begin(),1,(*wsrt_instance->GetOutboxesPtr())["OutBox"].first->back());
		    (*wsrt_instance->GetOutboxesPtr())["OutBox"].first->pop_back();
		  }
		}
		boost::shared_ptr<I3Frame> frame = frames.back();
		frames.pop_back();
		return frame;
	}
};

//======================================
// Can the Monster survive in the outside world? Let's find out
//======================================

//give it some tools
#include <dataclasses/I3Double.h>
#include <dataclasses/I3String.h>
#include <dataclasses/physics/I3MCTree.h>
#include <dataclasses/physics/I3MCTreeUtils.h>
#include<dataclasses/physics/I3Particle.h>

//now to the real work
TEST(1_Exact_Frame_Content_Sun){ //do it for sun
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()
    //("NStreams",2)
    ("Seed",0)
    //("StreamNum",1)
    (end_config);
	rng.installService();
	//services are set up, the stage is ready
	WimpSimReaderTesterSetup wsrt(context); //bring the monster to life
	std::string infile = std::string(getenv("I3_SRC")).append("/wimpsim-reader/resources/test_fake_sun.dat");
	std::vector<std::string> filenamelist(1, infile);
	std::list<double> laber = boost::assign::list_of(-800.)(800.)(-800.)(800.)(-800.)(800.);
	std::vector<double> poslim(laber.begin(), laber.end());
	wsrt.Configure() //tell it what to done
    ("RandomServiceName", "I3GSLRandomServiceFactory")
	  ("FileNameList", filenamelist)
	  ("NEvents",0)
	  ("StartMJD",55999.)
	  ("EndMJD",56000)
	  ("Oversampling",0)
	  ("PositionLimits",poslim)
	  ("InjectionRadius",700*I3Units::meter)
	  ("LowerZenCut",0*I3Units::degree)
	  ("UpperZenCut",180*I3Units::degree)
	  ("SensitiveHeight",1300*I3Units::meter)
	  ("SensitiveRadius",700*I3Units::meter)
	  ("UseElectrons",true)
	  ("UseMuons",true)
	  ("UseTaus",true)
	  ("UseNC",true)
	  (end_config);
	// things are set up, let the rampage begin

	I3FrameConstPtr frame;

	frame=wsrt.RetrieveNextFrame(); //first frame the info frame

	frame=wsrt.RetrieveNextFrame(); //the real information frame

	I3EventHeaderConstPtr event_header = frame->Get<I3EventHeaderConstPtr>("I3EventHeader");
	I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>("I3MCTree");
	I3WimpParamsConstPtr wimp_params = frame->Get<I3WimpParamsConstPtr>("WIMP_params");
	ENSURE (event_header, "'I3EventHeader' Object exists");
	ENSURE (mctree, "'I3MCTree' Object exists");
	ENSURE (wimp_params, "'WIMP_params' Object exists");

	I3Particle &primary = *(I3MCTreeUtils::GetMostEnergeticPrimary(mctree));
	const I3Particle lepton = I3MCTreeUtils::GetDaughters(mctree, primary)[0];
	const I3Particle hadron = I3MCTreeUtils::GetDaughters(mctree, primary)[1];

	ENSURE((primary.GetTime() == lepton.GetTime()) and (lepton.GetTime()== hadron.GetTime()) and (hadron.GetTime() == 0*I3Units::ns));
	ENSURE((primary.GetFitStatus() == lepton.GetFitStatus()) and (lepton.GetFitStatus() == hadron.GetFitStatus()) and (hadron.GetFitStatus() == I3Particle::OK));
	ENSURE((primary.GetLocationType() == lepton.GetLocationType()) and (lepton.GetLocationType() == hadron.GetLocationType()) and (hadron.GetLocationType() == I3Particle::InIce));

	ENSURE(primary.GetShape() == I3Particle::Primary);
	ENSURE(hadron.GetShape() == I3Particle::Cascade);

	ENSURE(hadron.GetType() == I3Particle::Hadrons);

	ENSURE(wimp_params->GetSource() == I3WimpParams::SUN);
	ENSURE(wimp_params->GetMass() == 1000);
	ENSURE(wimp_params->GetChannel() == WimpSim::ConvertToDecayChannel(5));

	ENSURE((primary.GetPos() == lepton.GetPos()) and (lepton.GetPos()== hadron.GetPos()));

	ENSURE(event_header->GetStartTime() == wimp_params->GetTime());

	ENSURE_DISTANCE(event_header->GetStartTime().GetModJulianDayDouble(), 56000.00000000000, 1./24./3600.)
	ENSURE(event_header->GetRunID() == 0);
	ENSURE(event_header->GetSubRunID() == 0);
	ENSURE(event_header->GetEventID() == 1);
	ENSURE(event_header->GetSubEventID() == 0);

	ENSURE_DISTANCE(primary.GetPos().GetX(),-21.03693992830813,0.0000000000001);
	ENSURE_DISTANCE(primary.GetPos().GetY(),640.4677391517907,0.0000000000001);
	ENSURE_DISTANCE(primary.GetPos().GetZ(),390.888549014926,0.0000000000001);
	ENSURE(primary.GetType() == (I3Particle::NuEBar));
	ENSURE_DISTANCE(primary.GetEnergy(),0.100000E+03,0.000001E+2);
	ENSURE_DISTANCE(primary.GetDir().GetZenith(),0.0,0.0000001);
	ENSURE_DISTANCE(primary.GetDir().GetAzimuth(),I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(lepton.GetEnergy(),0.750000+02,0.000001E+2);
	ENSURE(lepton.GetShape() == (I3Particle::Cascade));
	ENSURE(lepton.GetType() == (I3Particle::EPlus));
	ENSURE_DISTANCE(lepton.GetDir().GetZenith(),0.0,0.0000001);
	ENSURE_DISTANCE(lepton.GetDir().GetAzimuth(),I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(hadron.GetEnergy(),0.250000E+02,0.000001E+2);
	ENSURE_DISTANCE(hadron.GetDir().GetZenith(),0.0,0.0000001);
	ENSURE_DISTANCE(hadron.GetDir().GetAzimuth(),I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(wimp_params->GetNuWeight(),0.100000E-33, 0.000001E-33);
	ENSURE_DISTANCE(wimp_params->GetLepWeight(),0.100000E-46, 0.000001E-46);
	ENSURE_DISTANCE(wimp_params->GetHadWeight(),0.100000E-46, 0.000001E-46);
	ENSURE_DISTANCE(wimp_params->GetVgen(),2463008640.414398, 1); // to one qm precise
}

//not enough action? lets do more...
TEST(2_Exact_Frame_Content_Earth){ //do it for earthlings
	I3Context context;
	ServiceFactoryWrapper rng(context,"I3GSLRandomServiceFactory");
	rng.Configure()
    //("NStreams",2)
    ("Seed",0)
    //("StreamNum",1)
    (end_config);
	rng.installService();
	WimpSimReaderTesterSetup wsrt(context);
	std::string infile = std::string(getenv("I3_SRC")).append("/wimpsim-reader/resources/test_fake_earth.dat");
	std::vector<std::string> filenamelist(1, infile);
	std::list<double> laber = boost::assign::list_of(-800.)(800.)(-800.)(800.)(-800.)(800.);
	std::vector<double> poslim(laber.begin(), laber.end());
	wsrt.Configure() //tell it what to do
    ("RandomServiceName", "I3GSLRandomServiceFactory")
	  ("FileNameList", filenamelist)
	  ("NEvents",0)
	  ("StartMJD",55694)
	  ("EndMJD",(55694+365.25))
	  ("Oversampling",0)
	  ("PositionLimits",poslim)
	  ("InjectionRadius",700*I3Units::meter)
	  ("LowerZenCut",0*I3Units::degree)
	  ("UpperZenCut",180*I3Units::degree)
	  ("SensitiveHeight",1300*I3Units::meter)
	  ("SensitiveRadius",700*I3Units::meter)
	  ("UseElectrons",true)
	  ("UseMuons",true)
	  ("UseTaus",true)
	  ("UseNC",true)
	  (end_config);

	I3FrameConstPtr frame;

	frame=wsrt.RetrieveNextFrame(); //first frame the info frame

	frame=wsrt.RetrieveNextFrame(); //the real information frame

	I3EventHeaderConstPtr event_header = frame->Get<I3EventHeaderConstPtr>("I3EventHeader");
	I3MCTreeConstPtr mctree = frame->Get<I3MCTreeConstPtr>("I3MCTree");
	I3WimpParamsConstPtr wimp_params = frame->Get<I3WimpParamsConstPtr>("WIMP_params");
	ENSURE (event_header, "'I3EventHeader' Object exists");
	ENSURE (mctree, "'I3MCTree' Object exists");
	ENSURE (wimp_params, "'WIMP_params' Object exists");

	I3Particle &primary = *(I3MCTreeUtils::GetMostEnergeticPrimary(mctree));
	const I3Particle lepton = I3MCTreeUtils::GetDaughters(mctree, primary)[0];
	const I3Particle hadron = I3MCTreeUtils::GetDaughters(mctree, primary)[1];
	//always

	ENSURE((primary.GetTime() == lepton.GetTime()) and (lepton.GetTime()== hadron.GetTime()) and (hadron.GetTime() == 0*I3Units::ns));
	ENSURE((primary.GetFitStatus() == lepton.GetFitStatus()) and (lepton.GetFitStatus() == hadron.GetFitStatus()) and (hadron.GetFitStatus() == I3Particle::OK));
	ENSURE((primary.GetLocationType() == lepton.GetLocationType()) and (lepton.GetLocationType() == hadron.GetLocationType()) and (hadron.GetLocationType() == I3Particle::InIce));

	ENSURE(primary.GetShape() == I3Particle::Primary);

	ENSURE(hadron.GetShape() == I3Particle::Cascade);
	ENSURE(hadron.GetType() == I3Particle::Hadrons);

	ENSURE(wimp_params->GetSource() == I3WimpParams::EARTH);
	ENSURE(wimp_params->GetMass() == 1000);
	ENSURE(wimp_params->GetChannel() == WimpSim::ConvertToDecayChannel(5));

	//alike
	ENSURE((primary.GetPos() == lepton.GetPos()) and (lepton.GetPos()== hadron.GetPos()));

	ENSURE_DISTANCE(event_header->GetStartTime().GetModJulianDayDouble(), 56059.15567154549, 1./24./3600.);
	ENSURE(event_header->GetRunID() == 0);
	ENSURE(event_header->GetSubRunID() ==0);
	ENSURE(event_header->GetEventID() == 1);
	ENSURE(event_header->GetSubEventID() == 0);

	ENSURE_DISTANCE(primary.GetPos().GetX(),-375.6808401551098,0.0000000000001);
	ENSURE_DISTANCE(primary.GetPos().GetY(),-21.03693992830813,0.0000000000001);
	ENSURE_DISTANCE(primary.GetPos().GetZ(),731.9631304591894,0.0000000000001);
	ENSURE(primary.GetType() == (I3Particle::NuEBar));
	ENSURE_DISTANCE(primary.GetEnergy(),0.100000E+03, 0.000001E+02);
	ENSURE_DISTANCE(primary.GetDir().GetZenith(),0.*I3Constants::pi,0.0000001);
	ENSURE_DISTANCE(primary.GetDir().GetAzimuth(),0.5*I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(lepton.GetEnergy(), 0.750000E+02, 0.000001E+02);
	ENSURE(lepton.GetShape() == (I3Particle::Cascade)); //HACK
	ENSURE(lepton.GetType() == (I3Particle::EPlus));
	ENSURE_DISTANCE(lepton.GetDir().GetZenith(),0.*I3Constants::pi,0.0000001);
	ENSURE_DISTANCE(lepton.GetDir().GetAzimuth(),0.75*I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(hadron.GetEnergy(), 0.250000E+02, 0.000001E+02);
	ENSURE_DISTANCE(hadron.GetDir().GetZenith(),0.*I3Constants::pi,0.0000001);
	ENSURE_DISTANCE(hadron.GetDir().GetAzimuth(),0.25*I3Constants::pi,0.0000001);

	ENSURE_DISTANCE(wimp_params->GetNuWeight(), 0.100000E-24, 0.000001E-24);
	ENSURE_DISTANCE(wimp_params->GetLepWeight(), 0.100000E-37, 0.000001E-37);
	ENSURE_DISTANCE(wimp_params->GetHadWeight(),0.100000E-37, 0.000001E-37);
	ENSURE_DISTANCE(wimp_params->GetVgen(), 2463008640.4143977, 1.);  // to one qm precise
}
