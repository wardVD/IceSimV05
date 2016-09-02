/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3NuGGenie.cxx 103608 2013-05-03 16:54:54Z claudio.kopper $
 *
 * @file I3NuGGenie.cxx
 * @version $Revision$
 * @date $Date: 2013-05-03 11:54:54 -0500 (Fri, 03 May 2013) $
 * @author Claudio Kopper
 *
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>
 *  
 */


// nugen
#include "neutrino-generator/Particle.h"
#include "neutrino-generator/InteractionGeo.h"
#include "neutrino-generator/InteractionInfo.h"
#include "neutrino-generator/Steering.h"
#include "neutrino-generator/I3NuGGenie.h"
#include "neutrino-generator/utils/Utils.h"
#include "neutrino-generator/utils/Calculator.h"
#include "neutrino-generator/utils/Defaults.h"
#include "TVector3.h"


// for setenv
#include <stdlib.h>

#include <cmath>
#include <string>
#include <sstream>
#include <limits>

#include "icetray/I3Units.h"
#include "icetray/serialization.h"
#include <boost/serialization/variant.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>

// dataclasses
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

// Genie
#include "Conventions/XmlParserStatus.h"
#include "Conventions/GBuild.h"
#include "Conventions/Controls.h"
#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "GHEP/GHepParticle.h"
#include "GHEP/GHepRecord.h"
#include "EVGDrivers/GEVGDriver.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "Numerical/RandomGen.h"
#include "Numerical/Spline.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGLibrary.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"

// ROOT
#include "TPythia6.h"

// GENIE logging needs to go to I3Logging
#include "genie-icetray/I3LoggingAppender.h"
#include "log4cpp/Category.hh"
#include "log4cpp/BasicLayout.hh"

#include <iostream>
#include <sstream>

// helpers
#include "genie-icetray/ConvertToGST.h"
#include "genie-icetray/ConvertPDGEncodingToI3ParticleType.h"

// random numbers I3->GENIE
#include "genie-icetray/TRandomWrapperForI3RandomService.h"
#include "phys-services/I3RandomService.h"

using namespace earthmodel;
using namespace nugen;

// The module
I3_MODULE(I3NuGGenie);

//______________________________________________________________
I3NuGGenie::I3NuGGenie(const I3Context& context) 
: I3ConditionalModule(context), myTRandom_(NULL)
{
    // Genie uses the same stupid system of locating its installation
    // path as ROOT: an environment variable. This one's called "GENIE".
    // We know exactly where GENIE is installed (cmake already checked),
    // so we are just going to set/override the GENE variable here. -ck
    setenv("GENIE", GENIE_PATH, 1); // 1 means: *do* overwrite
    log_info("GENIE environment variable auto-set to \"%s\".",
             GENIE_PATH);
    
    unsetenv("GSEED"); // we configure the rng ourselves.. Get rid of any GSEED env variables
    unsetenv("GEVGL"); 
    
    std::string I3_BUILD;
    {
        const char *tmp = getenv("I3_BUILD");
        if (!tmp) log_fatal("I3_BUILD environment is not set. Are you within an IceTray environment?");
        I3_BUILD = tmp;
    }
    
    if (getenv("GMSGCONF") != NULL) 
        log_warn("You have the \"GMSGCONF\" environment variable set. It will be over-written now!");

    setenv("GMSGCONF", (I3_BUILD+"/genie-icetray/resources/config/").c_str(), 1); // 1 means: *do* overwrite
    log_debug("GMSGCONF=%s", getenv("GMSGCONF"));

    eventNumber_=0;
    
    // define parameters
    splineFilename_= I3_BUILD + "/neutrino-generator/resources/genie/xsec-spline-default-allnu-icerock-n1000-e300.xml";
    AddParameter("SplineFilename",
                 "Spline XML filename for GENIE.",
                 splineFilename_);
    
    LHAPDFPath_= I3_BUILD + "/neutrino-generator/resources/genie/lhapdf/";
    AddParameter("LHAPDFPath",
                 "Path to the LHAPDF data files. The directory should contain *.LHgrid and *.LHpdf files.",
                 LHAPDFPath_);

    setenv("LHAPATH", LHAPDFPath_.c_str(), 1); // 1 means: *do* overwrite
    
    // material
    materialOption_ = "Default";
    AddParameter("MaterialOption",
                 "Default:H2O and SiO2, Clark:H2O and clark number",
                 materialOption_);

    // steering
    steeringName_ = "";
    AddParameter("SteeringName",
                 "Name of I3NuGSteeringService",
                 steeringName_);

    randomService_ = boost::python::object(); // initialize with None
    AddParameter("RandomService",
                 "The I3RandomService we are going to use.\n"
                 "If None, the module tries to get it from the context.",
                 randomService_);

    primaryNuName_ = Defaults::primaryNuName;
    AddParameter("PrimaryNuName",
                 "Name of primary neutrino",
                 primaryNuName_);

    // The next three parameters are used to calculate spline.
    // energy range must cover the input neutrino's energy range.

    minEnergyLog_ = 0;
    AddParameter("MinEnergyLog","minimum log10 energy for spline", minEnergyLog_);

    maxEnergyLog_ = 2.5;
    AddParameter("MaxEnergyLog","maximum log10 energy for spline", maxEnergyLog_);

    nKnots_for_spline_ = 300;
    AddParameter("NumberOfKnots","number of knots for spline", nKnots_for_spline_);
    
    // add an outbox
    AddOutBox("OutBox");
    
    // this magic line will make Pythia6 shut up:
    TPythia6::Instance()->SetMSTU(12, 12345);
    
    // set up GENIE logging.
    log_debug("Setting up GENIE logging..");
    
    {
        unsetenv("GPRODMODE"); // always use Messenger.xml, not Messenger_production.xml
        
        // getting the first instance of the GENIE messenger sets up the GENIE appender
        // (and overwrites the current one). So we set up the GENIE one first and then
        // supply our own.

        
        // it is _really_ hard to make GENIE quiet.
        // to be able to do it, we need to redirect cout here..
        std::ostringstream things_genie_said;
        std::streambuf *sb = std::cout.rdbuf(things_genie_said.rdbuf());

        
        genie::Messenger *GENIEMessenger = genie::Messenger::Instance();
        if (!GENIEMessenger) log_fatal("GENIE Messenger singleton does not exist!");
        
        std::cout << std::endl;
        std::cout.flush();
        
        // .. and restore it here
        std::cout.rdbuf(sb);
        
        log_debug("GENIE blurted out the following while it set up its Messenger class:");
        log_debug("====================== BEGIN GENIE STUFF ===========================");
        std::vector<std::string> strs;
        {
            const std::string things_genie_said_str = things_genie_said.str();
            boost::split(strs, things_genie_said_str, boost::is_any_of("\n"));
        }
        
        BOOST_FOREACH(std::string stri, strs)
        {
            log_debug("%s", stri.c_str());
        }
        log_debug("====================================================================");
        
        
    }
    
#define USE_CUSTOM_LOG4CPP_APPENDER
#ifdef USE_CUSTOM_LOG4CPP_APPENDER
    // now, we can replace the log4cpp appender installed by GENIE
    {
        log4cpp::Appender * appender;
        appender = new log4cpp::I3LoggingAppender("default", true);
        appender->setLayout(new log4cpp::BasicLayout());
        log4cpp::Category &MSG = log4cpp::Category::getRoot();
        MSG.removeAllAppenders();
        MSG.setAdditivity(false);
        MSG.setAppender(appender);
    }
#endif
    
    log_debug("GENIE logging setup complete.");

    //std::cout << "-- Please ignore the following annoying messages brought to you by the wonderful ROOT:" << std::endl;
    if (!genie::PDGLibrary::Instance()) log_fatal("Could not create PDGLibrary singleton.");
    //std::cout << "-- Thank you for ignoring the previous messages!" << std::endl;
    
}


//______________________________________________________________
I3NuGGenie::~I3NuGGenie()
{
    // reset the custom RNG
    genie::RandomGen::Instance()->SetRndKine(NULL);
    genie::RandomGen::Instance()->SetRndHadro(NULL);
    genie::RandomGen::Instance()->SetRndDec(NULL);
    genie::RandomGen::Instance()->SetRndFsi(NULL);
    genie::RandomGen::Instance()->SetRndLep(NULL);
    genie::RandomGen::Instance()->SetRndISel(NULL);
    genie::RandomGen::Instance()->SetRndGeom(NULL);
    genie::RandomGen::Instance()->SetRndFlux(NULL);
    genie::RandomGen::Instance()->SetRndEvg(NULL);
    genie::RandomGen::Instance()->SetRndNum(NULL);
    genie::RandomGen::Instance()->SetRndPyth(NULL);
    genie::RandomGen::Instance()->SetRndGen(NULL);
    
    delete myTRandom_;
    myTRandom_=NULL;
    
    
    // it is _really_ hard to make GENIE quiet.
    // to be able to do it, we need to redirect cout..
    std::ostringstream things_genie_said;
    
    // save cout's current rdbuf
    std::streambuf *sb = std::cout.rdbuf(things_genie_said.rdbuf());
    
    // restore cout
    std::cout.rdbuf(sb);
    
    // this seems to help making the destructors quiet
    genie::gAbortingInErr = true;

}

//____________________________________________________________________
void I3NuGGenie::Configure()
{
    log_trace("%s", __PRETTY_FUNCTION__);
    
    GetParameter("SplineFilename", splineFilename_);
    GetParameter("LHAPDFPath", LHAPDFPath_);
    GetParameter("MaterialOption", materialOption_);
    GetParameter("SteeringName", steeringName_);
    GetParameter("MinEnergyLog", minEnergyLog_);
    GetParameter("MaxEnergyLog", maxEnergyLog_);
    GetParameter("NumberOfKnots", nKnots_for_spline_);
    GetParameter("RandomService", randomService_);
    GetParameter("PrimaryNuName", primaryNuName_);

    if (steeringName_ != "") {
       if (context_.Has(steeringName_)) {
          steer_ = context_.Get<SteeringPtr>(steeringName_);
       } else {
          log_error("You have to add I3NuGSteeringService to tray "
                    "before I3NuGInteractionInfo");
       }
    } else {
       log_error("You have to set SteeringName!");
    }
    // get pointer to earthmodel servcice
    earth_ = steer_->GetEarthModelService();
    stepsize_ = steer_->GetStepSize();

    // get the random service
    if (randomService_ == boost::python::object()) // is it None?
    {
        log_warn("No I3RandomService has been specified using the \"RandomService\" parameter. Trying to get the service from the context.");
        random_ = context_.Get<I3RandomServicePtr>();
        if (!random_) log_fatal("No I3RandomService on the context. Either install one using tray.AddService() or specify a service using the \"RandomService\" parameter.");
        log_warn("Found an I3RandomService on the context.");
    }
    else
    {
        random_ = boost::python::extract<I3RandomServicePtr>(randomService_);
        if (!random_) log_fatal("The value of the \"RandomService\" parameter is not of type I3RandomService!");
    }
    
    if (splineFilename_=="")
        log_fatal("You need to specify a spline filename uisng the \"SplineFilename\" parameter.");

    setenv("GSPLOAD", splineFilename_.c_str(), 1); // 1 means: *do* overwrite
    
    if (LHAPDFPath_=="") {
        char *dummy = getenv("I3_PORTS");
        if (!dummy) log_fatal("$I3_PORTS not set, LHAPDFPATH parameter can not be auto-configured.");
        const std::string I3_PORTS = dummy;

        LHAPDFPath_ = I3_PORTS + "/share/lhapdf/PDFsets";
        log_info("\"LHAPDFPATH\" parameter not specified. Auto-configuring to \"%s\".",
                 LHAPDFPath_.c_str());
    }
    
    // set the LHAPATH environment variable
    setenv("LHAPATH", LHAPDFPath_.c_str(), 1); // 1 means: *do* overwrite

    // check paths

    
    // use the IceTray rng for Genie and Pythia
    if (myTRandom_) delete myTRandom_;
    myTRandom_ = new TRandomWrapperForI3RandomService(random_);
    
    genie::RandomGen::Instance()->SetRndKine(myTRandom_);
    genie::RandomGen::Instance()->SetRndHadro(myTRandom_);
    genie::RandomGen::Instance()->SetRndDec(myTRandom_);
    genie::RandomGen::Instance()->SetRndFsi(myTRandom_);
    genie::RandomGen::Instance()->SetRndLep(myTRandom_);
    genie::RandomGen::Instance()->SetRndISel(myTRandom_);
    genie::RandomGen::Instance()->SetRndGeom(myTRandom_);
    genie::RandomGen::Instance()->SetRndFlux(myTRandom_);
    genie::RandomGen::Instance()->SetRndEvg(myTRandom_);
    genie::RandomGen::Instance()->SetRndNum(myTRandom_);
    genie::RandomGen::Instance()->SetRndPyth(myTRandom_);
    genie::RandomGen::Instance()->SetRndGen(myTRandom_);

   
    //load splines (from the XML file)
    genie::XSecSplineList* xspl = genie::XSecSplineList::Instance();
    xspl->LoadFromXml(splineFilename_);
    //xspl->AutoLoad();

    // set up GENIE
    GenerateDrivers();

}

//____________________________________________________________________
void I3NuGGenie::DAQ(I3FramePtr frame)
{
  log_trace("%s", "DAQ is called");
   
  //
  // 1. get neutrino energy, direction and position
  //
  I3ParticleConstPtr inj_nu = frame->Get<I3ParticleConstPtr>(primaryNuName_); 

  // convert to NuG particle
  nugen::Particle nu(*inj_nu, steer_);
  nugen::InteractionGeo &intgeo = nu.GetInteractionGeoRef();
  nugen::InteractionInfo &intinfo = nu.GetInteractionInfoRef();

  I3Position enterpos = intgeo.GetEnterDetVolDC();
  I3Position exitpos  = intgeo.GetExitDetVolDC();
  int      pdg      = GetPDGCode(inj_nu->GetType());
  double   energy   = inj_nu->GetEnergy();


  //
  // 2. calculate total column depth, surviving probability etc.
  //    and fill stepping info
  //
  SteppingMap stepmap;
  double total_cdep;
  
  CalcSteppingInfo(pdg, enterpos, exitpos, energy,
                   stepmap, total_cdep);

  //
  // 3. find interaction position in uniform distribution 
  //    of column depth  g/cm3 * cm = g/cm2
  //
  double random1 = myTRandom_->Rndm();
  double cdep = total_cdep * random1;

  if (cdep >= total_cdep) {
     // protection for lower_bound()
     cdep = total_cdep - 1e-9; 
  } else if (cdep < 0) {
     cdep = 0.;
  }
  log_trace("total_cdep %g, cdep %g", total_cdep, cdep);

  //
  // 4. select target material at the column depth
  //    and get proper GEVGDriver
  //
  SteppingMap::iterator iter1= stepmap.lower_bound(cdep);
  StepInfo theStep = iter1->second;
  //theStep.Print();

  // choose a target!
  double total_nsigma = theStep.fTotalNsigma_;
  double random2 = myTRandom_->Rndm();
  double nsigma = total_nsigma * random2;

  if (nsigma >= total_nsigma) {
     // protection for lower_bound()
     nsigma = (1.0-1e-9) * total_nsigma;  
  } else if (nsigma < 0) {
     nsigma = 0;
  }

  log_trace("total_nsigma %g, nsigma %g", total_nsigma, nsigma);

  std::map<double, boost::shared_ptr<genie::GEVGDriver> >::iterator iter2;
  iter2 = theStep.fDriverMap_.lower_bound(random2 * nsigma); 
  boost::shared_ptr<genie::GEVGDriver> driver = iter2->second;

  //
  // 5. Generate an event!
  //

  // it looks like lab-frame is OK...
  // since the input particle must be neutrino, 
  // magnitude of momentum is same as energy.
  I3Position momentum = energy * I3Direction(exitpos - enterpos);

  // to feed genie, convert it to TVector3
  TVector3 tp(momentum.GetX(), momentum.GetY(), momentum.GetZ());
  TLorentzVector nup4(tp, energy);
  boost::shared_ptr<genie::EventRecord> event;
  event = boost::shared_ptr<genie::EventRecord>(driver->GenerateEvent(nup4));

  log_trace("event generated");

  //
  // 6. Set interaction position.
  //

  I3Position particle_pos(inj_nu->GetPos());
  I3Position intpos(theStep.GetPos(cdep)); 
  double dist = (intpos - particle_pos).Magnitude();
  double dt = dist / I3Constants::c;
  double ptime = inj_nu->GetTime();
  TVector3 vtx(intpos.GetX(), intpos.GetY(), intpos.GetZ());
  TLorentzVector vertex(vtx, ptime + dt); 
  event->SetVertex(vertex);

  log_trace("vertex [%f,%f,%f]", vtx.X(), vtx.Y(), vtx.Z());

  // cross section at the vertex for the specific event
  const genie::Spline * totxsecspl = driver->XSecSumSpline();
  double xsec = totxsecspl->Evaluate(energy);
  //event->SetXSec(theStep.fTotalXSec_);
  event->SetXSec(xsec);
  log_trace("cross section %f [cm2]", xsec);

  //
  // 7. Calculate event weight
  // 

  // interaction weight 
  StepInfo finalStep = stepmap[total_cdep];
  double tot_psurv = finalStep.Compensate(finalStep.GetPsurv(total_cdep));  
  double intWeight = 1. - tot_psurv;
  event->SetProbability(intWeight);

  // this should be same as intWeight, but due to round off
  // it may differ...

  // position weight
  // dpsurv(x) = - d/dx(Psurv(x)) = n*sigma*exp(-n*sigma*x)
  // normalized dpsurv(x) = (1/intWeight)*dpsurv(x)
  // posWeight = (1/intWeight) * dpsurv(x) / (1/total_cdep)
  //           = total_cdep * dpsurv(x) / intWeight

  // get dpsurv at x = cdep
  double dpsurv = theStep.Compensate(theStep.GetdPdX(cdep));
  double posWeight = total_cdep  * dpsurv / intWeight;
  log_debug("total_cdep = %.15g, dpsurv=%.15g, intWeight=%.15g\n",
         total_cdep, dpsurv, intWeight);

  //
  // FillWeight
  //

  // suck up production info from wmap
  I3MapStringDoubleConstPtr constwmap = frame->Get<I3MapStringDoubleConstPtr>("I3MCWeightDict");
  intinfo.RestoreWeights(constwmap);

  I3MapStringDoublePtr wmap = nugen::Utils::PutWeightDict(frame,
                                             "I3MCWeightDict");
  
  // set interaction info to intinfo
  intinfo.SetTotalXsecCGS(xsec);
  intinfo.SetInteractionWeight(intWeight);
  intinfo.SetInteractionPositionWeight(posWeight);

  // set oneweight in [GeV cm^2 str]
  nugen::Utils::StoreOneWeight(wmap, intinfo);

  // generator volume
  double totlen = intgeo.GetMaxLength();
  double injectionArea = nugen::Utils::GetMapValue(*wmap,std::string("InjectionAreaCGS")) * I3Units::cm2;
  (*wmap)["TrueActiveLengthBefore"]  = intgeo.GetModDetLenBefore();
  (*wmap)["TrueActiveLengthAfter"]   = intgeo.GetModDetLenAfter();
  (*wmap)["GeneratorVolume"] = injectionArea * totlen; // in m^3

  //
  //
  // 8. convert to GST
  //

  log_debug("Converting event to GST..");
  I3GENIEResultDict gstMap;
  const bool result = I3GENIE_Helper::ConvertToGST(*event, static_cast<int32_t>(eventNumber_), gstMap);
  if (!result)
     log_fatal("Could not convert event to GST!");
  log_debug("..event converted.");


  //
  // 9. Make tree
  //

  I3MCTreePtr tree = GenerateMCTree(gstMap);


  //
  // 10. Add objects to frame
  // (weight map is already added)
  frame->Delete(Defaults::mcTreeName);
  frame->Put(Defaults::mcTreeName, tree);

  // increment the event number
  ++eventNumber_;
    
  PushFrame(frame, "OutBox");
}

//____________________________________________________________________
void I3NuGGenie::Finish()
{
}

//____________________________________________________________________
int I3NuGGenie::GetPDGCode(I3Particle::ParticleType ptype)
{
   if (ptype == I3Particle::NuE) return 12;
   if (ptype == I3Particle::NuEBar) return -12;
   if (ptype == I3Particle::NuMu) return 14;
   if (ptype == I3Particle::NuMuBar) return -14;
   if (ptype == I3Particle::NuTau) return 16;
   if (ptype == I3Particle::NuTauBar) return -16;

   log_error("wrong input type? %d", ptype);
   return -1;
}

//____________________________________________________________________
void I3NuGGenie::PrintMaps()
{
  log_debug("============ print material map  ================\n");

  MatRatioMap::iterator i1;
  for (i1 = matratiomap_.begin(); i1 != matratiomap_.end(); ++i1) {
     EarthModelService::MediumType medtype = i1->first;
     log_debug("material type %s \n", EarthModelService::GetMediumTypeString(medtype).c_str());
     MatRatio matratio = i1->second;
     MatRatio::iterator i2;
     for (i2 = matratio.begin(); i2 != matratio.end(); ++i2) {
        int materialid = i2->first;
        double ratio = i2->second;
        log_debug("material ID %d, ratio %f \n", materialid, ratio);

     }

  }

  /*
  DriverMap::iterator i3;
  for (i3 = drivers_.begin(); i3 != drivers_.end(); ++i3) {
     std::pair<int, int> key = i3->first;
     boost::shared_ptr<genie::GEVGDriver> driver = i3->second;
  
     printf("init_state(matid, pid) = (%d, %d)\n", 
             key.first, key.second);
     driver->Print(std::cerr);
  }
  */
}

//____________________________________________________________________
void I3NuGGenie::GenerateDrivers()
{

  //------------------------------ 
  // 1) define targets and particles 
  //------------------------------ 
  matratiomap_.clear();

  std::vector<int> particles;
  particles.push_back(12);  // nue
  particles.push_back(-12); // nuebar
  particles.push_back(14);  // numu
  particles.push_back(-14); // numubar
  particles.push_back(16);  // nutau
  particles.push_back(-16); // nutaubar

  std::vector<int> targets;
  targets.push_back(1000010010);  // H
  targets.push_back(1000080160);  // O
  targets.push_back(1000140280);  // Si
  if (materialOption_ == "Clark") {
     // real rock with clark number
     targets.push_back(1000130270);  // Al
     targets.push_back(1000260560);  // Fe
     targets.push_back(1000200400);  // Ca
     targets.push_back(1000110230);  // Na
     targets.push_back(1000190390);  // K
     targets.push_back(1000120240);  // Mg
     targets.push_back(1000812040);  // Ti
     targets.push_back(1000150310);  // P
  }

  // following maps are used for column-depth calculation
  MatRatio ice;      // H2O
  ice[1000080160] = 0.8881016;  // 0, 88.8% in weight
  ice[1000010010] = 0.1118983;  // 2H, 11% in weight


  MatRatio rock;     
  if (materialOption_ == "Default") {
     // SiO2
     rock[1000140280] = 0.4674349; // Si, 33%
     rock[1000080160] = 0.5325651; // 20, 66%
  } else if (materialOption_ == "Clark") {
     // Clark number
     rock[1000080160] = 0.476; // 0
     rock[1000140280] = 0.277; // Si
     rock[1000130270] = 0.081; // Al
     rock[1000260560] = 0.05;  // Fe
     rock[1000200400] = 0.036; // Ca
     rock[1000110230] = 0.028; // Na
     rock[1000190390] = 0.026; // K
     rock[1000120240] = 0.021; // Mg
     rock[1000812040] = 0.004; // Ti
     rock[1000150310] = 0.001; // P
  }

  matratiomap_[EarthModelService::ICE] = ice;
  matratiomap_[EarthModelService::ROCK] = rock;

  //------------------------------ 
  // 2. generate GEVGDrivers 
  // for each particle & target combination
  // (for each InitState)
  //------------------------------ 

  drivers_.clear();

  for (unsigned int i=0; i<particles.size(); ++i) {
     for (unsigned int j=0; j<targets.size(); ++j) {
        genie::InitialState init_state(targets[j], particles[i]);
        log_debug("making driver for target %d particle %d", 
                   targets[j], particles[i]);
        
        boost::shared_ptr<genie::GEVGDriver> driver = boost::shared_ptr<genie::GEVGDriver>(new genie::GEVGDriver());
        driver->Configure(init_state);
        driver->UseSplines();

        // generate spline
        log_debug("call CreateSplines") ;
        driver->CreateSplines(-1,-1,true); // spline in log10

        // generate cross-section sum
        genie::Range1D_t rE = driver->ValidEnergyRange();

        double emin = log10(rE.min); 
        double emax = log10(rE.max); 
        if (minEnergyLog_ <emin || 
            maxEnergyLog_ >emax) {

           log_fatal("log10 energy range [%f, %f] is out of range " 
                     " of cross section spline range [%f, %f]",
                      minEnergyLog_ , maxEnergyLog_, emin, emax);
           assert(minEnergyLog_>=emin &&
                  maxEnergyLog_<=emax);
        }        

        // decide the energy range for the sum spline
        // extend the spline a little bit above the
        // maximum beam energy (but below the maximum
        // valid energy) to avoid evaluation of the 
        // cubic spline around the viscinity of
        // knots with zero y values 
        emax = (1.1*maxEnergyLog_< emax) ?
                1.1*maxEnergyLog_: emax;


        log_info("call CreateXSecSumSpline with knots = %d, " 
                 "emin = %f, emax = %f", nKnots_for_spline_,
                 pow(10, emin) , pow(10, emax)) ;

        driver->CreateXSecSumSpline(nKnots_for_spline_, 
                            pow(10, emin), pow(10, emax), true);

        // store the driver
        std::pair<int,int> key(targets[j], particles[i]);
        drivers_[key] = driver;
     }
  }

  PrintMaps();
}

//____________________________________________________________________________
void I3NuGGenie::GetMaterial(I3Position localp,
                 EarthModelService::MediumType &med,
                 double &density)
{
   I3Position posCE = earth_->GetEarthCoordPosFromDetCoordPos(localp);
   med = earth_->GetMedium(posCE);
   //log_trace("got material type %s\n", earth_->GetMediumTypeString(med).c_str());

   density = earth_->GetEarthDensityInCGS(posCE);
   //log_trace("density is %f\n", density);

   return;
}

//___________________________________________________________________________
double I3NuGGenie::GetNSigma(double xsec, int A, double r)
{
//  Na   (Avogadro number, atoms/mole) 
//  A    (mass number, g/mole if pure material)    
//  xsec (total interaction cross section, need to convert to cm^2)
//  r    (mass ratio of the atom in compound, 0 <= r <= 1)
//
//  r / A : mol per r * 1[g]
//  kNA * r / A : number of target particle per 1g of compound
//
  xsec /= genie::units::cm2;
  //printf("GetNSigma: kNA=%g, xsec=%g, r=%g, A=%d\n",
  //        kNA, xsec, r, A);
  return I3Constants::NA * xsec * r / A;   // [cm^2/g]
}

//____________________________________________________________________________
void I3NuGGenie::CalcSteppingInfo(int pid,
                                  const I3Position &spos,
                                  const I3Position &epos,
                                  double energy,
                                  SteppingMap &stepmap,
                                  double &cdep)
{

   stepmap.clear();
   cdep = 0;

   I3Direction dir(epos - spos);
   double totlen = (epos - spos).Magnitude();

   // sample density and cross section at every 1m. It's bruteforce
   // but simple if geometory is not simple. 
   // at the boundary of material, try to reduce the size of step.
   
   double dx = stepsize_; // [m]
   double defaultdx = dx;
   double len = 0;
   double density = 0; // [g/cm3]
   double dcdep = 0;   // column depth within a step

   int    istep = 0;   // number of steps have been stepped

   // initialize material`
   EarthModelService::MediumType mat;
   GetMaterial(spos, mat, density);
   
   EarthModelService::MediumType lastmat = mat;
   double  lastdensity = density;

   // dummy laststep for initialization
   StepInfo laststep;
   laststep.fIstep_ = 0;
   laststep.fTotalXSec_ = 0;
   laststep.fTotalNsigma_ = 0;
   laststep.fStartPos_ = spos;
   laststep.fEndPos_ = spos;
   laststep.fStartLen_ = 0;
   laststep.fEndLen_ = 0;
   laststep.fColumnDepthIn_ = 0;
   laststep.fColumnDepthOut_ = 0;
   laststep.fPsurvIn_ = 1.0;
   laststep.fPsurvOut_ = 1.0;
   laststep.fPsurvPedestalIn_ = 1.0;
   laststep.fPsurvPedestalOut_ = 1.0;

   // do stepping! 
   while (len < totlen) {

      // step forward.
      len += dx;
      GetMaterial(spos + len*dir, mat, density);
      I3Position curpos = spos + len*dir;

      //printf("pos (%f,%f,%f), len %f, mat %s\n",curpos.GetX(), curpos.GetY(), curpos.GetZ(), len, earth_->GetMediumTypeString(mat).c_str());

      // check material and adjust step size.
      if (mat != lastmat) {
         // make step 1/10. This routine is called until
         // a step cross over the material boundary.
         len -= dx;
         dx /= 10;
         len += dx;
         GetMaterial(spos + len*dir, mat, density);
         // restore dx
         dx = defaultdx;
      }

      // then, if the material and density are same as the last
      // step, merge the step to the last one.
      bool merge_step = (istep != 0 && 
                        density == lastdensity &&
                        mat == lastmat);

      // new step
      StepInfo step;

      if (merge_step) {
         //printf("this step is merged\n");

         // first, remove exsisting step from stepmap.
         stepmap.erase(laststep.fColumnDepthOut_);

         // you can use laststep's calculation.
         step = laststep; // copy info

         // calc len from the start of the last step
         double dlen = len - step.fStartLen_;
         dcdep = dlen*density/I3Units::cm; // [m] to [cm]

      } else {

         //printf("Add new step \n");

         step.fIstep_ = laststep.fIstep_ + 1;
         step.fTotalXSec_ = 0;
         step.fTotalNsigma_ = 0;
         step.fStartPos_ = laststep.fEndPos_;
         step.fStartLen_ = laststep.fEndLen_;
         step.fColumnDepthIn_ = laststep.fColumnDepthOut_;
         step.fPsurvIn_ = laststep.fPsurvOut_;
         step.fPsurvPedestalIn_ = laststep.fPsurvPedestalOut_;

         // calc len from the end of the last step
         double dlen = len - step.fStartLen_;
         dcdep = dlen*density/I3Units::cm; // *100 : [m] to [cm]
         //printf("dcdep = %f\n",dcdep);
  
         // loop over all atoms in the target
         // matmap[mat] gives map::<matPDGcode, density ratio>
         // mat is ice or rock

         MatRatio::iterator iter;
         for (iter = matratiomap_[mat].begin(); 
              iter != matratiomap_[mat].end(); ++iter) {

            // 1. make initial state for i-th atom
            int mpdg = iter->first;      // material atom pdg code
            std::pair<int, int> initstate(mpdg, pid);
            //printf("material pdg = %d, particle pdg = %d\n",(int)mpdg, (int)pid);


            // 2. get event generator
            boost::shared_ptr<genie::GEVGDriver> driver = drivers_[initstate];
            //printf("material pdg = %d, particle pdg = %d\n",(int)mpdg, (int)pid);
        

            // 3. get crosssection in [cm^2]
            const genie::Spline * totxsecspl = driver->XSecSumSpline();
            double xsec = totxsecspl->Evaluate(energy);
            step.fTotalXSec_ += xsec;

            // 4. calculate n*sigma for current atom
            double mass_ratio = iter->second;
            int    atom_no = genie::pdg::IonPdgCodeToA(mpdg);
            double nsigma = GetNSigma(xsec, atom_no, mass_ratio);

            // 5. set n*sigma and driver in map 
            step.fTotalNsigma_ += nsigma; 
            step.fDriverMap_[step.fTotalNsigma_] = driver;
             
         }
      }

      // set end position, column depth
      step.fEndLen_ = len; 
      step.fEndPos_ = spos + len*dir; 
      step.fColumnDepthOut_ = step.fColumnDepthIn_ + dcdep;

      // calculate surviving probability within this step
      // TotalNsigma [cm^2/g] * dcdep [g/cm^2]
      //double prob = exp(- step.fTotalNsigma_ * dcdep * (kExpPedestal+1));
      double prob, ped;
      StepInfo::CalcPsurv(step.fTotalNsigma_, dcdep, prob, ped);

      //printf("stepping : dcdep = %g, totnsigma = %g, prob = %.15g\n", 
      //        dcdep, step.fTotalNsigma_, prob);

      step.fPsurvOut_ = step.fPsurvIn_ * prob;
      step.fPsurvPedestalOut_ = step.fPsurvPedestalIn_ * ped;

      //printf("stepping : dpsurv1 = %.15g, dpsurv2 = %.15g, diff = %.15g\n", 
      //        dpsurv1, dpsurv2, dpsurv1 - dpsurv2);


      // store the step to SteppingMap
      // key : total column depth at step end
      // from the entrance ofinteraction volume

      stepmap[step.fColumnDepthOut_] = step; 

      // check remaining distance 
      if (totlen - len < dx) {
         dx = totlen - len;
      }

      lastmat = mat;
      lastdensity = density;
      laststep = step;
      ++istep;

   }         


   log_debug("show all steps--- ");
   SteppingMap::iterator iter = stepmap.begin();
   for (iter=stepmap.begin(); iter != stepmap.end(); ++iter) {
      (iter->second).Print();
   }
   log_debug("show all steps end--- ");

   // copy info
   cdep = laststep.fColumnDepthOut_;

}

//____________________________________________________________________
namespace {
    template <typename T>
    const T &GetFromVariantMap(const std::string &name, const I3GENIEResultDict &dict)
    {
        I3GENIEResultDict::const_iterator it = dict.find(name);
        if (it == dict.end())
            log_fatal("I3GENIEResultDict does not contain an entry named \"%s\".",
                      name.c_str());
        
        try 
        {
            return boost::get<const T>(it->second);
        }
        catch (boost::bad_get &)
        {
            log_fatal("I3GENIEResultDict does contain entry named \"%s\" with wrong type.",
                      name.c_str());
        }
    }
    
};

//______________________________________________________________________
I3MCTreePtr I3NuGGenie::GenerateMCTree(const I3GENIEResultDict &gstMap)
{
  I3MCTreePtr mcTree = I3MCTreePtr(new I3MCTree());

  // add the primary
  I3Particle primary;
  {
     const int pdgType = GetFromVariantMap<int32_t>("neu", gstMap);
     const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgType);
     if (type == I3Particle::unknown)
        log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgType);

     primary.SetType(type);
     primary.SetShape(I3Particle::Primary);        // make these two compatible with neutrino-generator
     primary.SetLocationType(I3Particle::InIce);

     primary.SetPos(GetFromVariantMap<double>("vtxx", gstMap)*I3Units::m,
           GetFromVariantMap<double>("vtxy", gstMap)*I3Units::m,
           GetFromVariantMap<double>("vtxz", gstMap)*I3Units::m);

     primary.SetDir(GetFromVariantMap<double>("pxv", gstMap),
           GetFromVariantMap<double>("pyv", gstMap),
           GetFromVariantMap<double>("pzv", gstMap));

     primary.SetEnergy(GetFromVariantMap<double>("Ev", gstMap)*I3Units::GeV);
     primary.SetTime(GetFromVariantMap<double>("vtxt", gstMap)*I3Units::m/I3Constants::c);

     primary.SetLength(NAN);

     mcTree->insert_after(primary);
  }

  // add leptonic secondary
  {
     const int pdgType = GetFromVariantMap<int32_t>("pdgl", gstMap);

     I3Particle secondary;

     const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgType);

     if (type == I3Particle::unknown)
        log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgType);
     secondary.SetType(type);

     if ((type==I3Particle::MuPlus) || (type==I3Particle::MuMinus) ||
           (type==I3Particle::TauPlus) || (type==I3Particle::TauMinus))
        secondary.SetShape(I3Particle::StartingTrack);
     else if ((type==I3Particle::EPlus) || (type==I3Particle::EMinus))
        secondary.SetShape(I3Particle::Cascade);
     else if ((type==I3Particle::NuE) || (type==I3Particle::NuEBar) ||
           (type==I3Particle::NuMu) || (type==I3Particle::NuMuBar) ||
           (type==I3Particle::NuTau) || (type==I3Particle::NuTauBar))
        secondary.SetShape(I3Particle::Null); // it's a secondary neutrino
     else
        log_fatal("Leptonic secondary is not leptonic.. Internal error. Particle type is %s",
              secondary.GetTypeString().c_str());
     secondary.SetLocationType(I3Particle::InIce);

     secondary.SetPos(GetFromVariantMap<double>("vtxx", gstMap)*I3Units::m,
           GetFromVariantMap<double>("vtxy", gstMap)*I3Units::m,
           GetFromVariantMap<double>("vtxz", gstMap)*I3Units::m);

     secondary.SetDir(GetFromVariantMap<double>("pxl", gstMap),
           GetFromVariantMap<double>("pyl", gstMap),
           GetFromVariantMap<double>("pzl", gstMap));

     secondary.SetEnergy(GetFromVariantMap<double>("KEl", gstMap)*I3Units::GeV); // kinetic energy
     secondary.SetTime(GetFromVariantMap<double>("vtxt", gstMap)*I3Units::m/I3Constants::c);

     secondary.SetLength(NAN);

     mcTree->append_child(primary, secondary);
  }


  // add hadronic secondaries
  {
     const std::vector<double> &Ef = GetFromVariantMap<std::vector<double> >("KEf", gstMap); // kinetic energy
     const std::vector<double> &pxf = GetFromVariantMap<std::vector<double> >("pxf", gstMap);
     const std::vector<double> &pyf = GetFromVariantMap<std::vector<double> >("pyf", gstMap);
     const std::vector<double> &pzf = GetFromVariantMap<std::vector<double> >("pzf", gstMap);
     const std::vector<int32_t> &pdgf = GetFromVariantMap<std::vector<int32_t> >("pdgf", gstMap);

     const std::size_t numFinal = Ef.size();
     if ((numFinal != pxf.size()) ||
           (numFinal != pyf.size()) ||
           (numFinal != pzf.size()) ||
           (numFinal != pdgf.size()))
        log_fatal("final state vectors have differing sizes(). Cannot continue.");

     for (std::size_t i=0;i<numFinal;++i)
     {
        I3Particle secondary;

        const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgf[i]);
        if (type == I3Particle::unknown)
           log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgf[i]);
        secondary.SetType(type);

        secondary.SetShape(I3Particle::Cascade);
        secondary.SetLocationType(I3Particle::InIce);

        secondary.SetPos(GetFromVariantMap<double>("vtxx", gstMap)*I3Units::m,
              GetFromVariantMap<double>("vtxy", gstMap)*I3Units::m,
              GetFromVariantMap<double>("vtxz", gstMap)*I3Units::m);

        secondary.SetDir(pxf[i], pyf[i], pzf[i]);

        secondary.SetEnergy(Ef[i]*I3Units::GeV);
        secondary.SetTime(GetFromVariantMap<double>("vtxt", gstMap)*I3Units::m/I3Constants::c);

        secondary.SetLength(NAN);

        mcTree->append_child(primary, secondary);
     }
  }
  return mcTree;
}








