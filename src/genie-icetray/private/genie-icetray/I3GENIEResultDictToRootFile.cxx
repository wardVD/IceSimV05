/**
 * copyright (C) 2011
 * Claudio Kopper <claudio.kopper@nikhef.nl>
 * $Id: I3GENIEResultDictToRootFile.cxx 103608 2013-05-03 16:54:54Z claudio.kopper $
 *
 * @file I3GENIEResultDictToRootFile.cxx
 * @version $Revision$
 * @date $Date: 2013-05-03 12:54:54 -0400 (Fri, 03 May 2013) $
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

#include "genie-icetray/I3GENIEResultDictToRootFile.h"
#include "genie-icetray/I3GENIEResultDict.h"

#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
#include "genie-icetray/ConvertPDGEncodingToI3ParticleType.h"
#endif

#include "dataclasses/I3Constants.h"

#include <boost/foreach.hpp>

// dataclasses
#include "dataclasses/I3Map.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

// ROOT stuff
#include "TVector3.h"

// The module
I3_MODULE(I3GENIEResultDictToRootFile);


I3GENIEResultDictToRootFile::I3GENIEResultDictToRootFile(const I3Context& context) 
: I3ConditionalModule(context)
{
    // define parameters
    resultDictName_="I3GENIEResultDict";
    AddParameter("ResultDictName",
                 "Name of the input I3GENIEResultDict frame object.",
                 resultDictName_);

    mcTreeName_="I3MCTreeNew";
    AddParameter("MCTreeName",
                 "Name of the output I3MCTree frame object.",
                 mcTreeName_);

    weightDictName_="I3MCWeightDictExplicit";
    AddParameter("WeightDictName",
                 "Name of the output I3MCWeightDict frame object.",
                 weightDictName_);
    
    // add an outbox
    AddOutBox("OutBox");

}


I3GENIEResultDictToRootFile::~I3GENIEResultDictToRootFile()
{

}

void I3GENIEResultDictToRootFile::Configure()
{
    log_trace("%s", __PRETTY_FUNCTION__);
    
    GetParameter("ResultDictName", resultDictName_);
    GetParameter("MCTreeName", mcTreeName_);
    GetParameter("WeightDictName", weightDictName_);
    
    if (resultDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"ResultDictName\" parameter.");

    if (mcTreeName_=="")
        log_fatal("You have to configure a non-empty name for the \"MCTreeName\" parameter.");

    if (weightDictName_=="")
        log_fatal("You have to configure a non-empty name for the \"WeightDictName\" parameter.");

    
    
}

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

#ifdef IS_Q_FRAME_ENABLED
void I3GENIEResultDictToRootFile::DAQ(I3FramePtr frame)
#else
void I3GENIEResultDictToRootFile::Physics(I3FramePtr frame)
#endif
{
    log_trace("%s", __PRETTY_FUNCTION__);

    // get input data
    I3GENIEResultDictConstPtr resultDict = frame->Get<I3GENIEResultDictConstPtr>(resultDictName_);
    if (!resultDict)
        log_fatal("Could not find an I3GENIEResultDict object with name \"%s\" in the frame.",
                  resultDictName_.c_str());

    // prepare output objects
    I3MCTreePtr mcTree = I3MCTreePtr(new I3MCTree());
    I3MapStringDoublePtr weightDict = I3MapStringDoublePtr(new I3MapStringDouble());
    
    // add the primary
    I3Particle primary;
    {
        const int pdgType = GetFromVariantMap<int32_t>("neu", *resultDict);
#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
        const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgType);
        if (type == I3Particle::unknown)
            log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgType);
        
        primary.SetType(type);
#else
        primary.SetPdgEncoding(pdgType);
#endif
        primary.SetShape(I3Particle::Primary);        // make these two compatible with neutrino-generator
        primary.SetLocationType(I3Particle::InIce);
        
        primary.SetPos(GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                       GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                       GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);

        primary.SetDir(GetFromVariantMap<double>("pxv", *resultDict),
                       GetFromVariantMap<double>("pyv", *resultDict),
                       GetFromVariantMap<double>("pzv", *resultDict));

        primary.SetEnergy(GetFromVariantMap<double>("Ev", *resultDict)*I3Units::GeV);
        primary.SetTime(GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);

        primary.SetLength(NAN);

        mcTree->insert_after(primary);
    }

    // add leptonic secondary
    {
        const int pdgType = GetFromVariantMap<int32_t>("pdgl", *resultDict);

        I3Particle secondary;


        const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgType);

#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
        if (type == I3Particle::unknown)
            log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgType);
        secondary.SetType(type);
#else
        secondary.SetPdgEncoding(pdgType);
#endif
        
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
        
        secondary.SetPos(GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                         GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                         GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);
        
        secondary.SetDir(GetFromVariantMap<double>("pxl", *resultDict),
                       GetFromVariantMap<double>("pyl", *resultDict),
                       GetFromVariantMap<double>("pzl", *resultDict));
        
        secondary.SetEnergy(GetFromVariantMap<double>("KEl", *resultDict)*I3Units::GeV); // kinetic energy
        secondary.SetTime(GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);
        
        secondary.SetLength(NAN);
        
        mcTree->append_child(primary, secondary);
    }
    
    // add hadronic secondaries
    {
        const std::vector<double> &Ef = GetFromVariantMap<std::vector<double> >("KEf", *resultDict); // kinetic energy
        const std::vector<double> &pxf = GetFromVariantMap<std::vector<double> >("pxf", *resultDict);
        const std::vector<double> &pyf = GetFromVariantMap<std::vector<double> >("pyf", *resultDict);
        const std::vector<double> &pzf = GetFromVariantMap<std::vector<double> >("pzf", *resultDict);
        const std::vector<int32_t> &pdgf = GetFromVariantMap<std::vector<int32_t> >("pdgf", *resultDict);
        
        const std::size_t numFinal = Ef.size();
        if ((numFinal != pxf.size()) ||
            (numFinal != pyf.size()) ||
            (numFinal != pzf.size()) ||
            (numFinal != pdgf.size()))
            log_fatal("final state vectors have differing sizes(). Cannot continue.");
        
        for (std::size_t i=0;i<numFinal;++i)
        {
            I3Particle secondary;

#ifndef I3PARTICLE_SUPPORTS_PDG_ENCODINGS
            const I3Particle::ParticleType type = I3GENIE_Helper::ConvertPDGEncodingToI3ParticleType(pdgf[i]);
            if (type == I3Particle::unknown)
                log_fatal("PDG encoding %i cannot be converted to an I3Particle::ParticleType!", pdgf[i]);
            secondary.SetType(type);
#else
            secondary.SetPdgEncoding(pdgf[i]);
#endif

            secondary.SetShape(I3Particle::Cascade);
            secondary.SetLocationType(I3Particle::InIce);
            
            secondary.SetPos(GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                             GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                             GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);
            
            secondary.SetDir(pxf[i], pyf[i], pzf[i]);
            
            secondary.SetEnergy(Ef[i]*I3Units::GeV);
            secondary.SetTime(GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);
            
            secondary.SetLength(NAN);

            mcTree->append_child(primary, secondary);
        }
    }
    
    // now, fill the weight dictionary
    // disclaimer: most of this code is shamelessly stolen from neutrino-generator
    {
        
        //Here We fill event-wise weight related variables
        //everything need to be in double in the mean time...
        const double InjectionR    = GetFromVariantMap<double>("_gvolr", *resultDict)*I3Units::m;
        const double PowerLawIndex = GetFromVariantMap<double>("_plawind", *resultDict);
        const double MinEnergyLog  = GetFromVariantMap<double>("_elogmin", *resultDict);
        const double MaxEnergyLog  = GetFromVariantMap<double>("_elogmax", *resultDict);
        const double MinZenith     = GetFromVariantMap<double>("_zenmin", *resultDict)*I3Units::rad;
        const double MaxZenith     = GetFromVariantMap<double>("_zenmax", *resultDict)*I3Units::rad;
        const double MinAzimuth    = GetFromVariantMap<double>("_azimin", *resultDict)*I3Units::rad;
        const double MaxAzimuth    = GetFromVariantMap<double>("_azimax", *resultDict)*I3Units::rad;
        
        (*weightDict)["InjectionSurfaceR"] = InjectionR;
        (*weightDict)["PowerLawIndex"]     = PowerLawIndex;
        (*weightDict)["MinEnergyLog"]      = MinEnergyLog;
        (*weightDict)["MaxEnergyLog"]      = MaxEnergyLog;
        (*weightDict)["MinZenith"]         = MinZenith;
        (*weightDict)["MaxZenith"]         = MaxZenith;
        (*weightDict)["MinAzimuth"]        = MinAzimuth;
        (*weightDict)["MaxAzimuth"]        = MaxAzimuth;

        // not implemented here

        // global genie weight, accounts for the cross section
        const double genieWeight = GetFromVariantMap<double>("wght", *resultDict);
        const double globalProbabilityScale = GetFromVariantMap<double>("_glbprbscale", *resultDict);

        // cross section is stored in units of 1e-38 cm^2.
        // convert it to mb = 1e-31 m^2 = 1e-27cm^2 = 1e11 * 1e-38 cm^2
        
        // total cross section for the current probe and target
        // const double TotalCrosssection           = GetFromVariantMap<double>("_totxsec", *resultDict)*1.e-11;
        const double TotalInteractionProbability = genieWeight*globalProbabilityScale;

        // cross section for the current interaction
        const double Crosssection                = GetFromVariantMap<double>("xsec", *resultDict)*1.e-11;
        const double InteractionProbability      = genieWeight*GetFromVariantMap<double>("prob", *resultDict);
        
        const double TotalDetectionLength        = GetFromVariantMap<double>("_gvoll", *resultDict);

        // if (TotalCrosssection < Crosssection)
        //     log_error("Internal error: TotalCrosssection < Crosssection! (%e < %e)", TotalCrosssection, Crosssection);
        
        const double half_l = TotalDetectionLength/2.; // half length of cylinder
        TVector3 cyl_end(GetFromVariantMap<double>("pxv", *resultDict),
                         GetFromVariantMap<double>("pyv", *resultDict),
                         GetFromVariantMap<double>("pzv", *resultDict)); // direction of neutrino
        cyl_end.SetMag(half_l);                 // multiply by half length cylinder gives end point
        TVector3 cyl_beg = -1.*cyl_end;                // the opposite point is the beginning
        TVector3 int_pos(GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                         GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                         GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m); // interaction position
        
        // calculate the relative position along the cylinder axis of the perpendicular projection of the
        // vertex onto this axis
        const double u_min = int_pos * cyl_end / (half_l*half_l);
        if (fabs(u_min)>1.)
            log_fatal("Vertex point outside generation cylinder - should not happen! u_min=%f", u_min);
        
        const double LengthInVolume = half_l * (u_min+1.);
        
        const int32_t tgtpdg = GetFromVariantMap<int32_t>("tgt", *resultDict);
        
        
        const double GenVolRadius                = GetFromVariantMap<double>("_gvolr", *resultDict);
        const double PrimaryNeutrinoEnergy       = GetFromVariantMap<double>("Ev", *resultDict)*I3Units::GeV;
        
        const bool is_CC = GetFromVariantMap<bool>("cc", *resultDict);
        const bool is_NC = GetFromVariantMap<bool>("nc", *resultDict);
        if ((is_CC) && (is_NC)) log_fatal("Internal error, event is CC *and* NC!");
        double InteractionType = 0;
        // double InteractionCrosssection = 0.;
        if (is_CC) {
            InteractionType=1;
            // InteractionCrosssection = GetFromVariantMap<double>("_totxsec_cc_nc", *resultDict)*1.e-11;
        } else if (is_NC) {
            InteractionType=2;
            // InteractionCrosssection = GetFromVariantMap<double>("_totxsec_cc_nc", *resultDict)*1.e-11;
        }
	const bool is_DIS = GetFromVariantMap<bool>("dis", *resultDict);
	const bool is_QEL = GetFromVariantMap<bool>("qel", *resultDict);
	const bool is_RES = GetFromVariantMap<bool>("res", *resultDict);
	const bool is_COH = GetFromVariantMap<bool>("coh", *resultDict);
	const bool is_DFR = GetFromVariantMap<bool>("dfr", *resultDict);
	const bool is_IMD = GetFromVariantMap<bool>("imd", *resultDict);
	const bool is_NUEL = GetFromVariantMap<bool>("nuel", *resultDict);
	const bool is_SEA = GetFromVariantMap<bool>("sea", *resultDict);
	const int  hit_nuc = GetFromVariantMap<int>("hitnuc",*resultDict);
	const int hit_quark = GetFromVariantMap<int>("hitqrk",*resultDict);

        
        (*weightDict)["TargetPDGCode"]              = tgtpdg;
        (*weightDict)["GENIEWeight"]                = genieWeight;
        (*weightDict)["GlobalProbabilityScale"]     = globalProbabilityScale;
        (*weightDict)["GeneratorVolume"]            = GenVolRadius*GenVolRadius*I3Constants::pi*TotalDetectionLength;
        (*weightDict)["Crosssection"]               = Crosssection;
        // (*weightDict)["TotalCrosssection"]          = TotalCrosssection;
        (*weightDict)["InteractionProbabilityWeight"] = InteractionProbability;
        (*weightDict)["TotalInteractionProbabilityWeight"] = TotalInteractionProbability;
        
        (*weightDict)["TotalDetectionLength"]       = TotalDetectionLength;
        (*weightDict)["LengthInVolume"]             = LengthInVolume;
        (*weightDict)["EnergyLost"]                 = 0.0;
        (*weightDict)["InteractionType"]            = InteractionType;
        (*weightDict)["IsDIS"]                      = is_DIS;
	(*weightDict)["IsQEL"]                      = is_QEL;
	(*weightDict)["IsRES"]                      = is_RES;
	(*weightDict)["IsCOH"]                      = is_COH;
	(*weightDict)["IsDFR"]                      = is_DFR;
	(*weightDict)["IsIMD"]                      = is_IMD;
	(*weightDict)["IsNUEL"]                     = is_NUEL;
	(*weightDict)["IsSEA"]                      = is_SEA;
	(*weightDict)["HitNuc"]                     = hit_nuc;
	(*weightDict)["HitQuark"]                   = hit_quark;
	
        // (*weightDict)["InteractionCrosssection"]    = InteractionCrosssection;
        //20
        
        // get the interaction probability from the neutrino-generator : compatible with IceSim I.9.6 and +
        //probWeight = TotalInteractionProbability;
        
        // generation area is a circle with radius InjectionSurfaceR
        const double areaNorm = GenVolRadius * GenVolRadius * I3Constants::pi / I3Units::cm2 ; 
        
        // generation solid angle
        const double solidAngle = (cos(MinZenith)-cos(MaxZenith))*(MaxAzimuth-MinAzimuth);
        
        // generation neutrino spectral index
        //powerLawIndex = PowerLawIndex;
        
        const double energyPrimaryMax = pow(10.0, MaxEnergyLog) / I3Units::GeV;
        const double energyPrimaryMin = pow(10.0, MinEnergyLog) / I3Units::GeV;
        
        double energyIntegral=0;
        if (PowerLawIndex == 1.) {
            // if E^-1 then integral over Emin and Emax is
            energyIntegral = log(energyPrimaryMax/energyPrimaryMin);
        } else {
            // if not E^-1 then integral over Emin and Emax is
            energyIntegral = (pow(energyPrimaryMax, (1.-PowerLawIndex)) -
                              pow(energyPrimaryMin, (1.-PowerLawIndex))) / (1.-PowerLawIndex);
        }
        
        
        //power law index is probabaly 1 indicating a spectrum generated at E^-1
        const double energyFactor = pow( PrimaryNeutrinoEnergy / I3Units::GeV , -PowerLawIndex );
        
        // OneWeight
        const double OneWeight = (TotalInteractionProbability / energyFactor) * (energyIntegral * areaNorm * solidAngle);
        
        (*weightDict)["PrimaryNeutrinoEnergy"] = PrimaryNeutrinoEnergy / I3Units::GeV;
        (*weightDict)["OneWeight"] = OneWeight;
        (*weightDict)["NEvents"] = GetFromVariantMap<double>("_ngennu", *resultDict);
        
    }
    
    
    // add things to the frame
    frame->Put(mcTreeName_, mcTree);
    frame->Put(weightDictName_, weightDict);
    
    // push the frame
    PushFrame(frame, "OutBox");
}

