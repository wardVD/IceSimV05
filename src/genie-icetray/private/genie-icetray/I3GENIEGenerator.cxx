/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEGenerator.cxx 143430 2016-03-18 16:15:50Z melanie.day $
 *
 * @file I3GENIEGenerator.cxx
 * @version $Revision$
 * @date $Date: 2016-03-18 11:15:50 -0500 (Fri, 18 Mar 2016) $
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

#include "genie-icetray/I3GENIEGenerator.h"

#include "genie-icetray/I3GENIEResultDict.h"

// for setenv
#include <stdlib.h>

#include <cmath>
#include <string>
#include <sstream>
#include <fstream>
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
#include "Algorithm/AlgConfigPool.h"
#include "Registry/Registry.h"
#include "Conventions/XmlParserStatus.h"
#include "Conventions/GBuild.h"
#include "Conventions/Controls.h"
#include "Conventions/Units.h"
#include "EVGCore/EventRecord.h"
#include "EVGCore/InteractionList.h"
#include "GHEP/GHepParticle.h"
#include "EVGDrivers/GFluxI.h"
#include "EVGDrivers/GEVGDriver.h"
#include "EVGDrivers/GMCJDriver.h"
#include "EVGDrivers/GMCJMonitor.h"
#include "EVGDrivers/GEVGPool.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "Ntuple/NtpWriter.h"
#include "Ntuple/NtpMCFormat.h"
#include "Numerical/RandomGen.h"
#include "Numerical/Spline.h"
#include "PDG/PDGCodes.h"
#include "PDG/PDGUtils.h"
#include "PDG/PDGLibrary.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"
//#include "Utils/CmdLineArgParserUtils.h"
//#include "Utils/CmdLineArgParserException.h"

//#include "Geo/PointGeomAnalyzer.h"
//#include "FluxDrivers/GMonoEnergeticFlux.h"
//#include "HelperClasses/CrossSectionAccessor.h"

// ROOT
#include "TPythia6.h"

// GENIE logging needs to go to I3Logging
#include "genie-icetray/I3LoggingAppender.h"
#include "log4cpp/Category.hh"
#include "log4cpp/BasicLayout.hh"

#include <iostream>
#include <sstream>

// internal classes
#include "genie-icetray/GCylindPowerLawFlux.h"
#include "genie-icetray/GCylindPowerLawFlux.h"
#include "genie-icetray/GConstantDensityGeometryAnalyzer.h"
//#include "genie-icetray/MyGMCJDriver.h"
#include "EVGDrivers/GMCJDriver.h"
#include "genie-icetray/I3GENIESystWeights.h"

// helpers
#include "genie-icetray/ConvertToGST.h"
#include "genie-icetray/ConvertToMCTree.h"

// random numbers I3->GENIE
#include "genie-icetray/TRandomWrapperForI3RandomService.h"

// The module
I3_MODULE(I3GENIEGenerator);

I3GENIEGenerator::I3GENIEGenerator(const I3Context& context) 
: I3ConditionalModule(context),
geniePath_(string(GENIE_PATH)),
myTRandom(NULL)
{
    // Genie uses the same stupid system of locating its installation
    // path as ROOT: an environment variable. This one's called "GENIE".
    // We know exactly where GENIE is installed (cmake already checked),
    // so we are just going to set/override the GENE variable here. -ck
    log_trace("Starting GENIE production");
    setenv("GENIE", GENIE_PATH, 1); // 1 means: *do* overwrite
    log_info("GENIE environment variable auto-set to \"%s\".",
             GENIE_PATH);
    
    unsetenv("GSEED"); // we configure the rng ourselves.. Get rid of any GSEED env variables
    
    if (getenv("GXMLPATH") != NULL) 
        log_warn("You have the \"GXMLPATH\" environment variable set. Make sure you know what you are doing!");

    if (getenv("GXMLPATHS") != NULL) 
        log_warn("You have the \"GXMLPATHS\" environment variable set. Make sure you know what you are doing!");

    std::string I3_SRC;
    {
        const char *tmp = getenv("I3_SRC");
        if (!tmp) log_fatal("I3_SRC environment is not set. Are you within an IceTray environment?");
        I3_SRC = tmp;
    }
    
    // allow the user to customize options by putting xml files into a resource directory
    if (getenv("GALGCONF") != NULL) 
        log_warn("You have the \"GALGCONF\" environment variable set. It will be over-written now!");

    setenv("GALGCONF", (I3_SRC+"/genie-icetray/resources/config/").c_str(), 1); // 1 means: *do* overwrite
    log_debug("GALGCONF=%s", getenv("GALGCONF"));
    

    eventNumber_=0;

    AddParameter("GENIEPATH",
                 "Genie's very own ROOTSYS equivalent..",
                 geniePath_);
    
    // define parameters
    splineFilename_="";
    AddParameter("SplineFilename",
                 "Spline XML filename for GENIE.",
                 splineFilename_);
    
    LHAPDFPath_="";
    AddParameter("LHAPDFPath",
                 "Path to the LHAPDF data files. The directory should contain *.LHgrid and *.LHpdf files.",
                 LHAPDFPath_);

    resultDictName_="I3GENIEResultDict";
    AddParameter("ResultDictName",
                 "Name of the output I3GENIEResultDict frame object.",
                 resultDictName_);

    mcTreeName_="I3MCTree";
    AddParameter("MCTreeName",
                 "Name of the output I3MCTree frame object.",
                 mcTreeName_);
    
    mcWeightDictName_="I3MCWeightDict";
    AddParameter("MCWeightDictName",
                 "Name of the output I3MCWeightDict frame object.",
                 mcWeightDictName_);

    nuEnergyMin_=1.*I3Units::GeV;
    AddParameter("NuEnergyMin",
                 "neutrino energy range (start)",
                 nuEnergyMin_);
    
    nuEnergyMax_=1000.*I3Units::GeV;
    AddParameter("NuEnergyMax",
                 "neutrino energy range (end)",
                 nuEnergyMax_);
    
    powerLawIndex_=2.;
    AddParameter("PowerLawIndex",
                 "spectrum power law index (as in \"E^-index\")",
                 powerLawIndex_);

    materialDensity_=0.93*I3Units::g/I3Units::cm3;
    AddParameter("MaterialDensity",
                 "Ice/water density.",
                 materialDensity_);
    
    zenithMin_=0.*I3Units::deg;
    AddParameter("ZenithMin",
                 "zenith min",
                 zenithMin_);
    
    zenithMax_=180.*I3Units::deg;
    AddParameter("ZenithMax",
                 "zenith max",
                 zenithMax_);
    
    azimuthMin_=0.*I3Units::deg;
    AddParameter("AzimuthMin",
                 "azimuth min",
                 azimuthMin_);
    
    azimuthMax_=360.*I3Units::deg;
    AddParameter("AzimuthMax",
                 "azimuth max",
                 azimuthMax_);
    
    genVolRadius_=1200.*I3Units::m;
    AddParameter("GenVolRadius",
                 "radius of the generation volume cylinder",
                 genVolRadius_);
    
    genVolLength_=2000.*I3Units::m;
    AddParameter("GenVolLength",
                 "length of the generation volume cylinder",
                 genVolLength_);
    
    genVolDepth_=1950.*I3Units::m;
    AddParameter("GenVolDepth",
                 "depth of the generation volume (@ z==0)",
                 genVolDepth_);
    
    neutrinoFlavor_="NuE"; // nu_e and anti-nu_e
    AddParameter("NeutrinoFlavor",
                 "String describing the neutrino flavor to generate. Choose from \"NuE\", \"NuMu\" or \"NuTau\".",
                 neutrinoFlavor_);

    nEvents_=10000;
    AddParameter("NEvents",
                 "Number of generated neutrinos. Use this for weighting.\n"
                 "The actual number of events produced by this module will\n"
                 "be <= this number (not all neutrinos will interact).",
                 nEvents_);

    forceSingleProbScale_=true;
    AddParameter("ForceSingleProbScale",
                 "Forces a single interaction probability scale.\n"
                 "The actual number of generated frames will be much less\n"
                 "than \"NEvents\", but your events will basically be usable un-weighted.",
                 forceSingleProbScale_);
    
    
    // default water
    //                                             O16         H1
#if __cplusplus < 201103L
    targetMixIngredients_ = boost::assign::list_of(1000080160)(1000010010);
    targetMixQuantities_ = boost::assign::list_of(1)(2); // H2O
#else
    targetMixIngredients_ = std::vector<int>{1000080160,1000010010};
    targetMixQuantities_ = std::vector<double>{1,2}; // H2O
#endif
    AddParameter("TargetMixIngredients",
                 "list of PDG codes specifying the target material composition",
                 targetMixIngredients_);

    AddParameter("TargetMixQuantities",
                 "list of quantities of the different atoms of your material composition",
                 targetMixQuantities_);

    randomService_ = boost::python::object(); // initialize with None
    AddParameter("RandomService",
                 "The I3RandomService we are going to use.\n"
                 "If None, the module tries to get it from the context.",
                 randomService_);
    
    enableSystWeights_ = false;
    // List of Genie systematic weights to calculate
    AddParameter("SystematicNames",
                 "List of Genie systematic weights to calculate, by name",
                 systNames_);
    // List of Genie systematic steps to calculate
    AddParameter("SystematicSteps",
                 "List of systematic step values to calculate",
                 systSteps_);
    // fraction of nus in sample (other to nubars)
    nuFraction_ = 0.5;
    AddParameter("NuFraction", 
                 "Fraction of neutrinos in sample 0.5 means equal number\n"
                 "of neutrinos and anti-neutrinos, 1.0 means pure nu sample\n"
                 "and 0.0 corresponds to pure anti-neutrino sample",
                 nuFraction_);
    outputGST_=false;
    AddParameter("OutputGST", 
                 "Choose whether or not to output I3GENIEResultDict",
                 outputGST_);
    // add an outbox
    AddOutBox("OutBox");
    
    // this magic line will make Pythia6 shut up:
    log_debug("Telling Pythia6 to be quiet");
    TPythia6::Instance()->SetMSTU(12, 12345);
    
   
}


I3GENIEGenerator::~I3GENIEGenerator()
{
    // it is _really_ hard to make GENIE quiet.
    // to be able to do it, we need to redirect cout..
    std::ostringstream things_genie_said;
    
    // save cout's current rdbuf
    std::streambuf *sb = std::cout.rdbuf(things_genie_said.rdbuf());
#ifdef GENIE_RNG_OVERRIDE_AVAILABLE
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
#endif
    
    delete myTRandom;
    myTRandom=NULL;

    // Clean up systematic uncertainty weight calculator
    if(enableSystWeights_){
      systWeights_.reset();
    }

    
    mcj_driver.reset();
    flux_driver.reset();
    geom_driver.reset();
    
    // restore cout
    std::cout.rdbuf(sb);
    
    // this seems to help making the destructors quiet
    genie::gAbortingInErr = true;

}

void I3GENIEGenerator::Configure()
{
    log_trace("%s", __PRETTY_FUNCTION__);
    
    GetParameter("GENIEPATH", geniePath_);
    if (geniePath_.size()) {
      setenv("GENIE", geniePath_.c_str(), 1); // 1 means: *do* overwrite
      log_info("GENIE environment variable auto-set to \"%s\".", geniePath_.c_str());
    }

////////////////////////////////////////////////////////////////////////////////////////////////////
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
 

////////////////////////////////////////////////////////////////////////////////////////////////////


    GetParameter("SplineFilename", splineFilename_);
    GetParameter("LHAPDFPath", LHAPDFPath_);
    GetParameter("ResultDictName", resultDictName_);
    GetParameter("MCTreeName", mcTreeName_);
    GetParameter("MCWeightDictName", mcWeightDictName_);
    GetParameter("NuEnergyMin", nuEnergyMin_);
    GetParameter("NuEnergyMax", nuEnergyMax_);
    GetParameter("PowerLawIndex", powerLawIndex_);
    GetParameter("MaterialDensity", materialDensity_);
    GetParameter("ZenithMin", zenithMin_);
    GetParameter("ZenithMax", zenithMax_);
    GetParameter("AzimuthMin", azimuthMin_);
    GetParameter("AzimuthMax", azimuthMax_);
    GetParameter("GenVolRadius", genVolRadius_);
    GetParameter("GenVolLength", genVolLength_);
    GetParameter("GenVolDepth", genVolDepth_);
    GetParameter("NeutrinoFlavor", neutrinoFlavor_);
    GetParameter("TargetMixIngredients", targetMixIngredients_);
    GetParameter("TargetMixQuantities", targetMixQuantities_);
    GetParameter("NEvents", nEvents_);
    GetParameter("ForceSingleProbScale", forceSingleProbScale_);
    GetParameter("RandomService", randomService_);
    GetParameter("SystematicNames", systNames_);
    GetParameter("SystematicSteps", systSteps_);
    GetParameter("NuFraction", nuFraction_);
    GetParameter("OutputGST", outputGST_);
    if(systNames_.size()>0 && systSteps_.size()>0){
      enableSystWeights_ = true;
    }
    
    // get the random service
	if ((nuFraction_ < 0.0) || (nuFraction_ > 1.0)) {
		log_fatal("Please set correct fraction of neutrinos in the sample (from 0 to 1)");
	}
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
    
    if ((neutrinoFlavor_ != "NuE") &&
        (neutrinoFlavor_ != "NuMu") &&
        (neutrinoFlavor_ != "NuTau"))
        log_fatal("Please set the \"NeutrinoFlavor\" parameter to either \"NuE\", \"NuMu\" or \"NuTau\".");
    
    if (outputGST_ and resultDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"ResultDictName\" parameter.");

    if (!outputGST_ and mcTreeName_=="")
        log_fatal("You have to configure a non-empty name for the \"MCTreeName\" parameter.");

    if (!outputGST_ and mcWeightDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"MCWeightDictName\" parameter.");
    
    if (splineFilename_=="")
        log_fatal("You need to specify a spline filename uisng the \"SplineFilename\" parameter.");
    
    if (LHAPDFPath_=="")
    {
        char *dummy = getenv("I3_PORTS");
        if (!dummy) log_fatal("$I3_PORTS not set, LHAPDFPATH parameter can not be auto-configured.");
        const std::string I3_PORTS = dummy;

        LHAPDFPath_ = I3_PORTS + "/share/lhapdf/PDFsets";
        log_info("\"LHAPDFPATH\" parameter not specified. Auto-configuring to \"%s\".",
                 LHAPDFPath_.c_str());
    }
    
    if (genVolLength_ <= 0.)
        log_fatal("Value of \"GenVolLength\" is invalid! (val=%fm)", genVolLength_/I3Units::m);
    if (genVolRadius_ <= 0.)
        log_fatal("Value of \"GenVolRadius\" is invalid! (val=%fm)", genVolRadius_/I3Units::m);
    if (genVolDepth_ <= 0.)
        log_fatal("Value of \"GenVolDepth\" is invalid! (val=%fm)", genVolDepth_/I3Units::m);
    
    if (targetMixIngredients_.empty())
        log_fatal("No target ingredients specified (i.e., TargetMixIngredients.size()==0)");
    if (targetMixIngredients_.size() != targetMixQuantities_.size())
        log_fatal("The \"TargetMixIngredients\" and \"TargetMixQuantities\" vectors need to have the same size!");
    
    double totalIonA = 0;
    for (std::size_t i=0; i<targetMixIngredients_.size();++i)
    {
        if ((targetMixQuantities_[i] <= 0.))
            log_fatal("mix atom quantity #%zu value invalid (val=%f)",
                      i, targetMixQuantities_[i]);
        
        if (!genie::pdg::IsIon(targetMixIngredients_[i]))
            log_fatal("PDG code %i (@ #%zu) is not an ion!", targetMixIngredients_[i], i);
        
        totalIonA += targetMixQuantities_[i]*static_cast<double>(genie::pdg::IonPdgCodeToA(targetMixIngredients_[i]));
    }
    
    for (std::size_t i=0; i<targetMixIngredients_.size();++i)
    {
        const double thisIonA = targetMixQuantities_[i]*static_cast<double>(genie::pdg::IonPdgCodeToA(targetMixIngredients_[i]));
        const double weightFraction = thisIonA/totalIonA;
        
        // calculate the column depth for this atom:
        const double weightedDensity = materialDensity_*weightFraction;
        
        // Genie expects the denisty in units of kg/m^3, convert it here
        targetMixMap_.insert(std::make_pair(targetMixIngredients_[i], weightedDensity/(I3Units::kg/I3Units::m3)));
    }
    if (targetMixMap_.size() != targetMixIngredients_.size())
        log_fatal("You specified at least one target mix ingredient more than once.");
    
    // set the LHAPATH environment variable
    setenv("LHAPATH", LHAPDFPath_.c_str(), 1); // 1 means: *do* overwrite
    
    // use the IceTray rng for Genie and Pythia
    if (myTRandom) delete myTRandom;
    myTRandom = new TRandomWrapperForI3RandomService(random_);
    
#ifdef GENIE_RNG_OVERRIDE_AVAILABLE
    genie::RandomGen::Instance()->SetRndKine(myTRandom);
    genie::RandomGen::Instance()->SetRndHadro(myTRandom);
    genie::RandomGen::Instance()->SetRndDec(myTRandom);
    genie::RandomGen::Instance()->SetRndFsi(myTRandom);
    genie::RandomGen::Instance()->SetRndLep(myTRandom);
    genie::RandomGen::Instance()->SetRndISel(myTRandom);
    genie::RandomGen::Instance()->SetRndGeom(myTRandom);
    genie::RandomGen::Instance()->SetRndFlux(myTRandom);
    genie::RandomGen::Instance()->SetRndEvg(myTRandom);
    genie::RandomGen::Instance()->SetRndNum(myTRandom);
    genie::RandomGen::Instance()->SetRndPyth(myTRandom);
    genie::RandomGen::Instance()->SetRndGen(myTRandom);
#else
#warning You seem not to be using the specially patched
#warning version of Genie.
#warning I3RandomServices cannot be used with this version.
#warning Using a single random number as the rng seed.
#warning DO NOT USE FOR PRODUCTION
    {
        genie::RandomGen::Instance()->SetSeed(random_->Integer(0xfffffffe)+1); // seed 0 is invalid
    }
#endif
    
    // set up GENIE
    log_trace("Making Flux Driver()");
    flux_driver = MakeFluxDriver();
    log_trace("Making Geometry Driver()");
    geom_driver = MakeGeomDriver();

    if (!flux_driver) log_fatal("Could not set up GENIE flux driver");
    if (!geom_driver) log_fatal("Could not set up GENIE geometry driver");
    
    //load splines (from the XML file)
    std::ifstream sp_file(splineFilename_.c_str());
    if(sp_file)
    {
        genie::XSecSplineList* xspl = genie::XSecSplineList::Instance();
        xspl->LoadFromXml(splineFilename_);
    }
    else{
      log_fatal("Spline file %s does not exist. Without a spline file Genie takes prohibitively long to run. Please use a valid spline file" , splineFilename_.c_str());
    } 
    // create the monte carlo job driver
    //mcj_driver = boost::shared_ptr<genie::MyGMCJDriver>(new genie::MyGMCJDriver());
    mcj_driver = boost::shared_ptr<genie::GMCJDriver>(new genie::GMCJDriver());
    mcj_driver->UseFluxDriver(flux_driver.get());
    mcj_driver->UseGeomAnalyzer(geom_driver.get());
    mcj_driver->UseSplines();
    if (forceSingleProbScale_)
        mcj_driver->ForceSingleProbScale(); // do not set this if weighted events should be generated
    mcj_driver->KeepOnThrowingFluxNeutrinos(true); // always return a neutrino
    log_trace("Configuring the mcj driver");

    mcj_driver->Configure();
    
    log_debug("Genie MCJDriver global probability scale = %e", mcj_driver->GlobProbScale());

    // Configure systematic weight calculator, if needed
    if(enableSystWeights_){
      systWeights_ = boost::shared_ptr<genie::I3GENIESystWeights>(new genie::I3GENIESystWeights());
      systWeights_->SetSystematics(systNames_);
      systWeights_->SetSteps(systSteps_);
      systWeights_->Configure();
    }
}

boost::shared_ptr<genie::GFluxI> I3GENIEGenerator::MakeFluxDriver()
{
    boost::shared_ptr<genie::flux::GCylindPowerLawFlux> flux =
    boost::shared_ptr<genie::flux::GCylindPowerLawFlux>
    (new genie::flux::GCylindPowerLawFlux(neutrinoFlavor_,
                                          genVolLength_/I3Units::m,
                                          genVolRadius_/I3Units::m,
                                          powerLawIndex_,
                                          nuEnergyMin_/I3Units::GeV,
                                          nuEnergyMax_/I3Units::GeV,
                                          zenithMin_/I3Units::rad,
                                          zenithMax_/I3Units::rad,
                                          azimuthMin_/I3Units::rad,
                                          azimuthMax_/I3Units::rad,
                                          nuFraction_));
    
    return flux;
}

boost::shared_ptr<genie::GeomAnalyzerI> I3GENIEGenerator::MakeGeomDriver()
{
    boost::shared_ptr<genie::geometry::GConstantDensityGeometryAnalyzer> geom_driver =
    boost::shared_ptr<genie::geometry::GConstantDensityGeometryAnalyzer>
    (new genie::geometry::GConstantDensityGeometryAnalyzer(targetMixMap_, genVolLength_/I3Units::m));

    return geom_driver;
}


#ifdef IS_Q_FRAME_ENABLED
void I3GENIEGenerator::DAQ(I3FramePtr frame)
#else
void I3GENIEGenerator::Physics(I3FramePtr frame)
#endif
{
    log_trace("%s", __PRETTY_FUNCTION__);
    
#if 0
    // check charm decay
    genie::AlgConfigPool * confp = genie::AlgConfigPool::Instance();
    const genie::Registry * gc = confp->GlobalParameterList();
    
    genie::RgKeyList klist = gc->FindKeys("DecayParticleWithCode=");
    genie::RgKeyList::const_iterator kiter = klist.begin();
    for( ; kiter != klist.end(); ++kiter) 
    {
        std::string myStr = *kiter;
        bool decay = gc->GetBool(myStr);
        
        log_warn("DecayParticleWithCode: %s: value=%s", myStr.c_str(), decay?"YES!":"NO..");
    }
#endif
    
    // generate a single event
    boost::shared_ptr<genie::EventRecord> genie_event;
    for (;;) {
        log_debug("Generating a GENIE event...");
        genie_event = boost::shared_ptr<genie::EventRecord>(mcj_driver->GenerateEvent());
        if (!genie_event) log_fatal("Received (null) event from GENIE.");
        
        if (genie_event->IsUnphysical()) {
            log_warn("Received unphysical event. Trying again.");
            continue;
        }
        
        break;
    }
    log_debug(".. event generated!");
    
    if (mcj_driver->NFluxNeutrinos() < 0) log_fatal("WTF?!");
    if (static_cast<uint64_t>(mcj_driver->NFluxNeutrinos()) > nEvents_) {
        log_info("GENIE has generated the requested number of neutrinos (%lu). Requesting suspension.",
                 static_cast<unsigned long>(nEvents_));
        RequestSuspension();
        // do not push the frame!
        return;
    }
    
    log_debug("NFluxNeutrinos=%li, eventNumber=%lu", mcj_driver->NFluxNeutrinos(), static_cast<unsigned long>(eventNumber_+1));
    /*
    int32_t pdgCode = genie_event->Particle(1)->Pdg();
    if (pdgCode==2212) pdgCode=1000010010; // replace p with H1
    //log_warn("TgtPDGCode=%i",pdgCode);
    {
        genie::GeomAnalyzerI *geomPtr = mcj_driver->GeomAnalyzerPtr();
        genie::GFluxI *fluxPtr = mcj_driver->FluxDriverPtr();
        
        genie::PathLengthList fCurPathLengths = geomPtr->ComputePathLengths(fluxPtr->Position(), fluxPtr->Momentum());
        
        genie::PathLengthList::const_iterator pliter = fCurPathLengths.find(pdgCode);
        double path_length=NAN;
        if (pliter!=fCurPathLengths.end())
            path_length = pliter->second*I3Units::kg/I3Units::m2;
        
        //log_warn("PathLength=%f kg/m^2", path_length/(I3Units::kg/I3Units::m2));
    }
    */
    
    // const genie::GEVGPool *GPool = mcj_driver->GEVGPoolPtr();
    // if (!GPool) log_fatal("Internal error: GEVGPoolPtr is (null)!");
    
    // get xsec sum over all modelled processes for given neutrino+target)
    const double probeEnergy = genie_event->Probe()->Energy();
    
    // genie::GEVGDriver * evgdriver = GPool->FindDriver(genie_event->Summary()->InitState());
    // const double sum_xsec = evgdriver->XSecSumSpline()->Evaluate(probeEnergy); 
    
    // double sum_xsec_cc_nc=0.;
    // {
    //     const genie::InteractionList * ilist = evgdriver->Interactions();
    //     const genie::ProcessInfo &event_proc = genie_event->Summary()->ProcInfo();
    //     if ((!(event_proc.IsWeakCC())) && (!(event_proc.IsWeakNC())))
    //         log_fatal("Process should either be CC or NC.");
        
    //     for(genie::InteractionList::const_iterator ilistiter = ilist->begin();
    //         ilistiter != ilist->end(); ++ilistiter)
    //     {
    //         const genie::Interaction * interaction = *ilistiter;
    //         const genie::Spline * spl = evgdriver->XSecSpline(interaction);
    //         const genie::ProcessInfo &proc = interaction->ProcInfo();

    //         if ((!(proc.IsWeakCC())) && (!(proc.IsWeakNC())))
    //             log_fatal("Process should either be CC or NC.");
            
    //         if ((proc.IsWeakCC()) && (event_proc.IsWeakCC())) {
    //             sum_xsec_cc_nc += spl->Evaluate(probeEnergy);
    //         } else if ((proc.IsWeakNC()) && (event_proc.IsWeakNC())) {
    //             sum_xsec_cc_nc += spl->Evaluate(probeEnergy);
    //         }
            
    //     }
    // }
    
    // convert to GST
    log_debug("Converting event to GST..");
    I3GENIEResultDictPtr gstMap(new I3GENIEResultDict());
    const bool result = I3GENIE_Helper::ConvertToGST(*genie_event, static_cast<int32_t>(eventNumber_), *gstMap);
    if (!result)
        log_fatal("Could not convert event to GST!");
    log_debug("..event converted.");
    
    // add some more information to the map
    gstMap->insert(std::make_pair("_gvolr", genVolRadius_/I3Units::m));
    gstMap->insert(std::make_pair("_gvoll", genVolLength_/I3Units::m));
    gstMap->insert(std::make_pair("_gvold", genVolDepth_/I3Units::m));
    gstMap->insert(std::make_pair("_plawind", powerLawIndex_));
    gstMap->insert(std::make_pair("_elogmin", std::log10(nuEnergyMin_/I3Units::GeV)));
    gstMap->insert(std::make_pair("_elogmax", std::log10(nuEnergyMax_/I3Units::GeV)));
    gstMap->insert(std::make_pair("_zenmin", zenithMin_/I3Units::rad));
    gstMap->insert(std::make_pair("_zenmax", zenithMax_/I3Units::rad));
    gstMap->insert(std::make_pair("_azimin", azimuthMin_/I3Units::rad));
    gstMap->insert(std::make_pair("_azimax", azimuthMax_/I3Units::rad));
    gstMap->insert(std::make_pair("_ngennu", static_cast<double>(nEvents_))); // store NEvents as a double
    // gstMap->insert(std::make_pair("_totxsec", sum_xsec / (1e-38*genie::units::cm2) ));
    // gstMap->insert(std::make_pair("_totxsec_cc_nc", sum_xsec_cc_nc / (1e-38*genie::units::cm2) ));
    gstMap->insert(std::make_pair("_glbprbscale", mcj_driver->GlobProbScale() ));

    // Calculate systematic weights, if requested
    if(enableSystWeights_){
      systWeights_->Calculate(*genie_event, *gstMap);
    }
    // prepare output objects
    I3MCTreePtr mcTree = I3MCTreePtr(new I3MCTree());
    I3MapStringDoublePtr weightDict = I3MapStringDoublePtr(new I3MapStringDouble());
    //Get MCInfo from I3GENIEResultDict 
    const bool success = I3GENIE_Helper::ConvertToMCTree(mcTree,weightDict,gstMap);
    if(!success)
        log_fatal("Could not convert event to MCTree!");

    // add objects to the frame
    if(outputGST_){
      frame->Put(resultDictName_, gstMap);
    }
    else{
      frame->Put(mcTreeName_, mcTree);
      frame->Put(mcWeightDictName_, weightDict);
    }
    // increment the event number
    ++eventNumber_;
    if (eventNumber_ > static_cast<uint64_t>(std::numeric_limits<int32_t>::max())) {
        log_error("Cannot increase event number, reached limit of int32_t! Re-starting from zero!");
        eventNumber_=0;
    }
    
    PushFrame(frame, "OutBox");
}

void I3GENIEGenerator::Finish()
{
  log_trace("I3GENIEGenerator::Finish()");
    if (mcj_driver->NFluxNeutrinos() < 0) log_fatal("WTF?!");
    if (static_cast<uint64_t>(mcj_driver->NFluxNeutrinos()) <= nEvents_) {
        log_error("***");
        log_error("*** GENIE has NOT generated the requested number of neutrinos yet.");
        log_error("*** You should not use the NEvents weight (\"_ngennu\" I3GENIEResultDict entry) for weighting!");
        log_error("*** Requested: %lu", static_cast<unsigned long>(nEvents_));
        log_error("*** Generated: %lu", static_cast<unsigned long>(mcj_driver->NFluxNeutrinos()));
        log_error("***");
    }
}
