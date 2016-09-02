/**
 * copyright (C) 2015
 * Melanie Day <melanie.day@icecube.wisc.edu>
 *
 * @file ConvertToMCTree.cxx
 * @version $Revision$
 * @author Melanie Day 
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

#include "genie-icetray/ConvertToMCTree.h"
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

template <typename T>
const T &I3GENIE_Helper::GetFromVariantMap(const std::string &name, const I3GENIEResultDict &dict)
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
    
bool I3GENIE_Helper::ConvertToMCTree(I3MCTreePtr mcTree,  I3MapStringDoublePtr weightDict, I3GENIEResultDictConstPtr resultDict)
{
    // add the primary
    I3Particle primary;
    {
        const int pdgType = I3GENIE_Helper::GetFromVariantMap<int32_t>("neu", *resultDict);
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
        
        primary.SetPos(I3GENIE_Helper::GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                       I3GENIE_Helper::GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                       I3GENIE_Helper::GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);

        primary.SetDir(I3GENIE_Helper::GetFromVariantMap<double>("pxv", *resultDict),
                       I3GENIE_Helper::GetFromVariantMap<double>("pyv", *resultDict),
                       I3GENIE_Helper::GetFromVariantMap<double>("pzv", *resultDict));

        primary.SetEnergy(I3GENIE_Helper::GetFromVariantMap<double>("Ev", *resultDict)*I3Units::GeV);
        primary.SetTime(I3GENIE_Helper::GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);

        primary.SetLength(NAN);

        mcTree->insert_after(primary);
    }

    // add leptonic secondary
    {
        const int pdgType = I3GENIE_Helper::GetFromVariantMap<int32_t>("pdgl", *resultDict);

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
        
        secondary.SetPos(I3GENIE_Helper::GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                         I3GENIE_Helper::GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                         I3GENIE_Helper::GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);
        
        secondary.SetDir(I3GENIE_Helper::GetFromVariantMap<double>("pxl", *resultDict),
                       I3GENIE_Helper::GetFromVariantMap<double>("pyl", *resultDict),
                       I3GENIE_Helper::GetFromVariantMap<double>("pzl", *resultDict));
        
        secondary.SetEnergy(I3GENIE_Helper::GetFromVariantMap<double>("KEl", *resultDict)*I3Units::GeV); // kinetic energy
        secondary.SetTime(I3GENIE_Helper::GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);
        
        secondary.SetLength(NAN);
        
        mcTree->append_child(primary, secondary);
    }
    
    // add hadronic secondaries
    {
        const std::vector<double> &Ef = I3GENIE_Helper::GetFromVariantMap<std::vector<double> >("KEf", *resultDict); // kinetic energy
        const std::vector<double> &pxf = I3GENIE_Helper::GetFromVariantMap<std::vector<double> >("pxf", *resultDict);
        const std::vector<double> &pyf = I3GENIE_Helper::GetFromVariantMap<std::vector<double> >("pyf", *resultDict);
        const std::vector<double> &pzf = I3GENIE_Helper::GetFromVariantMap<std::vector<double> >("pzf", *resultDict);
        const std::vector<int32_t> &pdgf = I3GENIE_Helper::GetFromVariantMap<std::vector<int32_t> >("pdgf", *resultDict);
        
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
            
            secondary.SetPos(I3GENIE_Helper::GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                             I3GENIE_Helper::GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                             I3GENIE_Helper::GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m);
            
            secondary.SetDir(pxf[i], pyf[i], pzf[i]);
            
            secondary.SetEnergy(Ef[i]*I3Units::GeV);
            secondary.SetTime(I3GENIE_Helper::GetFromVariantMap<double>("vtxt", *resultDict)*I3Units::m/I3Constants::c);
            
            secondary.SetLength(NAN);

            mcTree->append_child(primary, secondary);
        }
    }
    
    // now, fill the weight dictionary
    // disclaimer: most of this code is shamelessly stolen from neutrino-generator
    {
        
        //Here We fill event-wise weight related variables
        //everything need to be in double in the mean time...
        const double InjectionR    = I3GENIE_Helper::GetFromVariantMap<double>("_gvolr", *resultDict)*I3Units::m;
        const double PowerLawIndex = I3GENIE_Helper::GetFromVariantMap<double>("_plawind", *resultDict);
        const double MinEnergyLog  = I3GENIE_Helper::GetFromVariantMap<double>("_elogmin", *resultDict);
        const double MaxEnergyLog  = I3GENIE_Helper::GetFromVariantMap<double>("_elogmax", *resultDict);
        const double MinZenith     = I3GENIE_Helper::GetFromVariantMap<double>("_zenmin", *resultDict)*I3Units::rad;
        const double MaxZenith     = I3GENIE_Helper::GetFromVariantMap<double>("_zenmax", *resultDict)*I3Units::rad;
        const double MinAzimuth    = I3GENIE_Helper::GetFromVariantMap<double>("_azimin", *resultDict)*I3Units::rad;
        const double MaxAzimuth    = I3GENIE_Helper::GetFromVariantMap<double>("_azimax", *resultDict)*I3Units::rad;
        
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
        const double genieWeight = I3GENIE_Helper::GetFromVariantMap<double>("wght", *resultDict);
        const double globalProbabilityScale = I3GENIE_Helper::GetFromVariantMap<double>("_glbprbscale", *resultDict);

        // cross section is stored in units of 1e-38 cm^2.
        // convert it to mb = 1e-31 m^2 = 1e-27cm^2 = 1e11 * 1e-38 cm^2
        
        // total cross section for the current probe and target
        // const double TotalCrosssection           = I3GENIE_Helper::GetFromVariantMap<double>("_totxsec", *resultDict)*1.e-11;
        const double TotalInteractionProbability = genieWeight*globalProbabilityScale;

        // cross section for the current interaction
        const double Crosssection                = I3GENIE_Helper::GetFromVariantMap<double>("xsec", *resultDict)*1.e-11;
        const double InteractionProbability      = genieWeight*I3GENIE_Helper::GetFromVariantMap<double>("prob", *resultDict);
        
        const double TotalDetectionLength        = I3GENIE_Helper::GetFromVariantMap<double>("_gvoll", *resultDict);

        // if (TotalCrosssection < Crosssection)
        //     log_error("Internal error: TotalCrosssection < Crosssection! (%e < %e)", TotalCrosssection, Crosssection);
        
        const double half_l = TotalDetectionLength/2.; // half length of cylinder
        TVector3 cyl_end(I3GENIE_Helper::GetFromVariantMap<double>("pxv", *resultDict),
                         I3GENIE_Helper::GetFromVariantMap<double>("pyv", *resultDict),
                         I3GENIE_Helper::GetFromVariantMap<double>("pzv", *resultDict)); // direction of neutrino
        cyl_end.SetMag(half_l);                 // multiply by half length cylinder gives end point
        TVector3 cyl_beg = -1.*cyl_end;                // the opposite point is the beginning
        TVector3 int_pos(I3GENIE_Helper::GetFromVariantMap<double>("vtxx", *resultDict)*I3Units::m,
                         I3GENIE_Helper::GetFromVariantMap<double>("vtxy", *resultDict)*I3Units::m,
                         I3GENIE_Helper::GetFromVariantMap<double>("vtxz", *resultDict)*I3Units::m); // interaction position
        
        // calculate the relative position along the cylinder axis of the perpendicular projection of the
        // vertex onto this axis
        const double u_min = int_pos * cyl_end / (half_l*half_l);
        if (fabs(u_min)>1.)
            log_fatal("Vertex point outside generation cylinder - should not happen! u_min=%f", u_min);
        
        const double LengthInVolume = half_l * (u_min+1.);
        
        const int32_t tgtpdg = I3GENIE_Helper::GetFromVariantMap<int32_t>("tgt", *resultDict);
        
        
        const double GenVolRadius                = I3GENIE_Helper::GetFromVariantMap<double>("_gvolr", *resultDict);
        const double PrimaryNeutrinoEnergy       = I3GENIE_Helper::GetFromVariantMap<double>("Ev", *resultDict)*I3Units::GeV;
        
        const bool is_CC = I3GENIE_Helper::GetFromVariantMap<bool>("cc", *resultDict);
        const bool is_NC = I3GENIE_Helper::GetFromVariantMap<bool>("nc", *resultDict);
        if ((is_CC) && (is_NC)) log_fatal("Internal error, event is CC *and* NC!");
        double InteractionType = 0;
        // double InteractionCrosssection = 0.;
        if (is_CC) {
            InteractionType=1;
            // InteractionCrosssection = I3GENIE_Helper::GetFromVariantMap<double>("_totxsec_cc_nc", *resultDict)*1.e-11;
        } else if (is_NC) {
            InteractionType=2;
            // InteractionCrosssection = I3GENIE_Helper::GetFromVariantMap<double>("_totxsec_cc_nc", *resultDict)*1.e-11;
        }

        
        
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
        (*weightDict)["NEvents"] = I3GENIE_Helper::GetFromVariantMap<double>("_ngennu", *resultDict);
        
    }
    
  return true;
}

