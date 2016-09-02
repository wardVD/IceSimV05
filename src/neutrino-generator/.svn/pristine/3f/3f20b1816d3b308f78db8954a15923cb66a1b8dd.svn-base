/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief   Utils IMPLEMENTATION FILE
 */

#include <neutrino-generator/Steering.h>
#include <neutrino-generator/Particle.h>
#include <neutrino-generator/utils/Utils.h>
#include <neutrino-generator/utils/TreeUtils.h>
#include <neutrino-generator/utils/Calculator.h>
#include <dataclasses/physics/I3MCTree.h>
#include <icetray/I3Frame.h>
#include <algorithm>

using namespace std;

namespace nugen {

//_________________________________________________________
I3Particle::ParticleType
Utils::GetParticleType(const string &ss)
{
   
   string s = ToUpperCase(ss);

   if (s == "NUE")      return I3Particle::NuE;
   if (s == "NUEBAR")   return I3Particle::NuEBar;
   if (s == "NUMU")     return I3Particle::NuMu;
   if (s == "NUMUBAR")  return I3Particle::NuMuBar;
   if (s == "NUTAU")    return I3Particle::NuTau;
   if (s == "NUTAUBAR") return I3Particle::NuTauBar;

   log_error("ParticleType %s is not supported", s.c_str()); 

   return I3Particle::unknown;

}

//_________________________________________________________________
void Utils::PutParticle(I3FramePtr frame,
                                ParticlePtr nu, 
                                const string &pname, 
                                I3MapStringDoublePtr wmap)
{
  log_debug("Adding a particle to frame : name %s, type %s, "
            "energy %g, zenith %g shape %d location %d",
            pname.c_str(),
            nu->GetTypeString().c_str(),
            nu->GetEnergy(), nu->GetDir().GetZenith(),
            nu->GetShape(), nu->GetLocationType());
 
  // put mctree 
  I3ParticlePtr nup(new I3Particle(nu->GetI3Particle()));
  if (frame->Has(pname)) {
     // the particle is replaced
     frame->Delete(pname);
  }
  frame->Put(pname, nup);

  // copy weights from particle to weightmap 
  (nu->GetInteractionInfoRef()).FillInteractionInfo(wmap);
  (nu->GetInteractionGeoRef()).FillInteractionGeo(wmap);
}

//_________________________________________________________________
ParticlePtr Utils::GetParticle(I3FramePtr frame,
                               const string &pname, 
                               boost::shared_ptr<Steering> steer,
                               I3MapStringDoublePtr wmap)
{
  // 1. search in Steering buffer
  // not implemented yet

  // 2. search in frame
  I3ParticleConstPtr nup = frame->Get<I3ParticleConstPtr>(pname);
  if (! nup) {
     log_error("pname %s is not found", pname.c_str());
  } 

  // 3. create Particle 
  ParticlePtr nu = ParticlePtr(new Particle(*nup, steer));

  // 4. Restore interaction info and geo from wmap to Particle.
  (nu->GetInteractionInfoRef()).RestoreInteractionInfo(wmap);
  (nu->GetInteractionGeoRef()).RestoreInteractionGeo(wmap);

  return nu;
}

//_________________________________________________________________
void Utils::PutMCTree(I3FramePtr frame,
                          ParticlePtr nu, 
                          const string &treename)
{
  // make a list which conains only primary particle.
  ParticlePtrList list;
  list.push_back(nu);
  PutMCTree(frame, list, treename);
}

//_________________________________________________________________
void Utils::PutMCTree(I3FramePtr frame,
                      ParticlePtrList list, 
                      const string &treename)
{
  I3MCTreePtr tree;

  if (frame->Has(treename)) {
     // copy original tree
     I3MCTreeConstPtr tree_orig = 
          frame->Get<I3MCTreeConstPtr>(treename);
     tree = I3MCTreePtr(new I3MCTree(*tree_orig));

     //frame->Rename(treename, treename + "_orig");
     frame->Delete(treename);

  } else {
     // create a new tree
     tree = I3MCTreePtr(new I3MCTree);
  }

  // Start new MCTree
  TreeUtils::ListToTree(tree, list);
 
  frame->Put(treename, tree);
}

//_________________________________________________________________
void RemoveDarkNeutrinos(I3MCTreePtr mctree)
{
	// Remove un-selected neutrinos
	for (I3MCTree::iterator p = mctree->begin(); p != mctree->end(); )
		if (p->IsNeutrino() && p->GetShape() == I3Particle::Dark) {
			p = mctree->erase(p);
		} else {
			p++;
		}
	// Clean up newly childless parents
	for (I3MCTree::post_order_iterator p = mctree->begin_post(); p != mctree->end_post(); )
		if (p->GetLocationType() != I3Particle::InIce && mctree->number_of_children(p) == 0u) {
			p = mctree->erase(p);
			// TODO: what does this do? is it related to the similar
			//       line in MuonGun?
			//p.descend_all();
		} else {
			p++;
		}
}

//_________________________________________________________________
void Utils::PutCleanedMCTree(I3FramePtr frame,
                             ParticlePtr nu,
                             const string &treename,
                             bool keepdark)
{
  I3MCTreePtr tree;
  I3MCTreeConstPtr tree_orig;

  if (frame->Has(treename)) {
     tree_orig = frame->Get<I3MCTreeConstPtr>(treename);
     tree = I3MCTreePtr(new I3MCTree(*tree_orig));

     //frame->Rename(treename, treename + "_orig");
     frame->Delete(treename);

  } else {
     log_error("no tree found with name %s", treename.c_str());
  }

  // loop over all tree and remove / make dark 
  // unwanted neutrinos.

  I3MCTree::iterator iter;
  for (iter=tree->begin(); iter!=tree->end(); ++iter) {
     log_trace("particle type %s", iter->GetTypeString().c_str());

     if (! iter->IsNeutrino()) continue;
     if (nu) {
        if (iter->GetMajorID() == nu->GetMajorID() &&
            iter->GetMinorID() == nu->GetMinorID() ) {
           log_trace("matched selected nu");
           continue;
        }
     }

     // remaining particles are neutrinos we don't use.
     // make it dark...
     iter->SetShape(I3Particle::Dark);
  }

  if (!keepdark) {
     RemoveDarkNeutrinos(tree);
  }
  frame->Put(treename, tree);
}

//_________________________________________________________________
I3MapStringDoublePtr Utils::PutWeightDict(I3FramePtr frame,
                                              const string &weightname)
{
  // store weight dict
  I3MapStringDoublePtr wmap;

  if (frame->Has(weightname)) {
     I3MapStringDoubleConstPtr wmap_orig = 
           frame->Get<I3MapStringDoubleConstPtr>(weightname);
     wmap = I3MapStringDoublePtr(new I3MapStringDouble(*wmap_orig));
     frame->Delete(weightname);

  } else {
     wmap = I3MapStringDoublePtr(new I3MapStringDouble());
  }

  frame->Put(weightname, wmap);
  return wmap;
}

//_________________________________________________________________
void Utils::StoreOneWeight(I3MapStringDoublePtr wmap,
                           InteractionInfo intinfo)
{
    // Calculate one weight.
    // they are stored by SourceUtils.cxx
    double areaNormCGS = GetMapValue(*wmap,std::string("InjectionAreaCGS"));
    double solidAngle = GetMapValue(*wmap, std::string("SolidAngle"));

    double primaryE = GetMapValue(*wmap, std::string("PrimaryNeutrinoEnergy"));
    double eminLog = GetMapValue(*wmap, std::string("MinEnergyLog"));
    double emaxLog = GetMapValue(*wmap, std::string("MaxEnergyLog"));
    double gamma = GetMapValue(*wmap, std::string("PowerLawIndex"));

    // calculate energy factor: energy-integral / E^-gama
    double energyFactor = nugen::Calculator::CalcPowerLawEnergyFactor(
                           primaryE, gamma, eminLog, emaxLog);

    // Calc OneWeight
    double totalWeight = intinfo.GetTotalWeight();
    double oneWeight = totalWeight * energyFactor * areaNormCGS * solidAngle;

    (*wmap)["OneWeight"] = oneWeight;

    // OneWeightPerType is a new param. 
    // We historically use total number of events for Nu+NuBar (NEvents) for weight calculation,
    // however, it is not useful when flux of Nu and NuBar are not same: we have to use number of
    // files per particle type.
    // As a result, we have been applying NEvent compansation (x0.5) for Atmospheric neutrino weight. 
    // AtmosWeight = AtmFlux(PrimaryE, costheta) * OneWeight / (0.5*NEvents)
    //             = 2.0 * AtmFlux(PrimaryE, costheta) * OneWeight / (NEvents)
    //
    // OneWeightPerType contains the factor, so you don't have to calculate ratio of your 
    // selected particle type in a dataset. Since now we have new feature to control the input ratio
    // of nu and nubar, the ratio could be any number from 0 to 1.  
    //
    // as long as you assume Nu and NuBar flux are same (Nu:NuBar = 1:1), the new weight will be
    // AtmosWeight = AtmFlux(PrimaryE, costheta) * OneWeightPerType / (NEvents)
    // E2Weight = 0.5 * PrimaryE^(-2)_for_NuMu * OneWeightPerType / (NEvents) 
    // (0.5 gives "average" value of Nu and NuBar)

    (*wmap)["OneWeightPerType"] = oneWeight / intinfo.GetTypeWeight();

}

}
