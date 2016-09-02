#ifndef NuGUtils_H
#define NuGUtils_H
/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 * @brief Utils header file
 * utility functions
 * 
 */

#include "phys-services/I3RandomService.h"
#include "dataclasses/physics/I3Particle.h"

#include <string>
#include <map>

namespace nugen {

I3_FORWARD_DECLARATION(Particle);
I3_FORWARD_DECLARATION(Steering);
I3_FORWARD_DECLARATION(InteractionInfo);

namespace Utils {

  /**
   * @brief convert particle type name to particle type
   */
  I3Particle::ParticleType GetParticleType(const std::string &s) ;

  /*
   * particle type checkers
   */
   inline bool IsElectron(I3Particle::ParticleType t) 
     { return (t==I3Particle::EMinus||t==I3Particle::EPlus); }
   inline bool IsMuon(I3Particle::ParticleType t) 
     { return (t==I3Particle::MuMinus||t==I3Particle::MuPlus); }
   inline bool IsTau(I3Particle::ParticleType t) 
     { return (t==I3Particle::TauMinus||t==I3Particle::TauPlus); }

   inline bool IsNuE(I3Particle::ParticleType t)
     { return (t==I3Particle::NuE||t==I3Particle::NuEBar); }
   inline bool IsNuMu(I3Particle::ParticleType t) 
     { return (t==I3Particle::NuMu||t==I3Particle::NuMuBar); }
   inline bool IsNuTau(I3Particle::ParticleType t)
     { return (t==I3Particle::NuTau||t==I3Particle::NuTauBar); }

   inline bool IsEFlavor(I3Particle::ParticleType t) 
     { return (IsElectron(t) || IsNuE(t)); }
   inline bool IsMuFlavor(I3Particle::ParticleType t)
     { return (IsMuon(t) || IsNuMu(t)); }
   inline bool IsTauFlavor(I3Particle::ParticleType t)
     { return (IsTau(t) || IsNuTau(t)); }

  /**
   * @brief put a particle to Frame, with its weight info
   */
  void PutParticle(I3FramePtr frame, 
                   boost::shared_ptr<Particle> nu, 
                   const std::string &pname, 
                   I3MapStringDoublePtr wmap);

  /**
   * @brief get a particle to Frame, with its weight info
   */
  boost::shared_ptr<Particle> GetParticle(I3FramePtr frame,
                          const std::string &pname, 
                          boost::shared_ptr<Steering> steer,
                          I3MapStringDoublePtr wmap);

  /**
   * convert Particle to MCTree and
   * put the MCTree to the frame.
   */
  void PutMCTree(I3FramePtr frame,
                 boost::shared_ptr<Particle> nu, 
                 const std::string &treename);

  /**
   * convert ParticleList to MCTree and
   * put the MCTree to the frame.
   */
  void PutMCTree(I3FramePtr frame,
                 I3Vector<boost::shared_ptr<Particle> >list, 
                 const std::string &treename);

  /**
   * This function puts a copied MCTree but deleted all
   * unwanted neutrinos.
   * Note that original tree is deleted and replaced to 
   * new tree.
   */
  void PutCleanedMCTree(I3FramePtr frame,
                        boost::shared_ptr<Particle> nu, 
                        const std::string &treename,
                        bool keepdark = false);

  /**
   * Put MCWeightDict to frame.
   * If an old MCWeightDict exists, all information in the old
   * weight dict is copied to new one and you may add
   * additional info using returned pointer
   * to MCWeightDict.
   */
  I3MapStringDoublePtr PutWeightDict(I3FramePtr frame,
                                     const std::string &weightname);

  /**
   * Calculate OneWeight and save it to MCWeightDict.
   */
  void StoreOneWeight(I3MapStringDoublePtr wmap,
                      InteractionInfo intinfo);



  /**
   * get value from const map.
   * if a given map doesn't have an key x, it calls log_error.
   */
  template <class X, class Y> 
  const Y & GetMapValue(const std::map<X,Y> &m, X x)
  {
      typename std::map<X,Y>::const_iterator i = m.find(x);
      if (i == m.end()) {
         log_error("Failed to extract map value.");
      }
      return i->second;
  }

  /**
   * get value from const map.
   * if a given map doesn't have an key x, it doesn't change anything.
   */
  template <class X, class Y> 
  void GetMapValue(const std::map<X,Y> &m, X x, Y &y)
  {
      typename std::map<X,Y>::const_iterator i = m.find(x);
      if (i != m.end()) {
         y = i->second;
      }
  }

}
}

#endif 
