/**
 * $Id$
 * @author Kotoyo Hoshina 23/Aug/05                                     
 *         Kotoyo Hoshina 24/Apr/06 Modified to comply dataclass v2 
 */

#ifndef RECCLASSES_I3OPHELIAPARTICLE_H
#define RECCLASSES_I3OPHELIAPARTICLE_H

#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <dataclasses/physics/I3Particle.h>
#include <dataclasses/I3Vector.h>
#include <icetray/I3FrameObject.h>
#include <vector>

// forward declation needed here because I3OpheliaParticle stores
// a vector of I3OpheliaParticlePtrs
class I3OpheliaParticle;
I3_POINTER_TYPEDEFS(I3OpheliaParticle);

/**                                                           
 *  @brief Stores final reco result of EHE analysis (see project ophelia).
 *
 *  A simple track class to store final reco result of EHE analysis.
 *  It also has storage for a First Guess Track and the most significant
 *  Stocastic Cascade. In the vector area, you can store final reco results,
 *  I3OpheliaParticle and I3OpheliaCascade, for example.
 */                                                                       
class I3OpheliaParticle :  public I3FrameObject 
{

 public:

   //------------------------------------------------------------------------------

   /**
    * Destructor. 
    */
     virtual ~I3OpheliaParticle() {}
   //------------------------------------------------------------------------------

   /**
    * Add function to add daughter particle 
    */
     void AddDaughter(I3OpheliaParticlePtr p) { daughters_.push_back(p); }
   //------------------------------------------------------------------------------

   /**
    * clear vectors. It deletes all persistency inside vector.
    */
     void ClearDaughters() { daughters_.clear(); }
   //------------------------------------------------------------------------------

   /**
    * get daughter vector.
    */
     const std::vector<I3OpheliaParticlePtr>& GetDaughters() const { return daughters_; }
     std::vector<I3OpheliaParticlePtr>& GetDaughters() { return daughters_; }
   //------------------------------------------------------------------------------

   /**
    * dump information of the particle
    */
     virtual void DumpOut(std::string indent = "") const;
   //------------------------------------------------------------------------------

   /**
    * Get reference to I3Particle. 
    * CAUTION: returned I3Particle is a snap shot of current I3Particle.
    */
     I3Particle& GetI3Particle();
   //------------------------------------------------------------------------------

   /**
    * Get const reference to I3Particle. 
    * CAUTION: returned I3Particle is a snap shot of current I3Particle.
    */
     const I3Particle& GetConstI3Particle() const;
   //------------------------------------------------------------------------------

   /**
    * copy current snapshot of given I3Particle to the datamamber.
    * 
    */
     void SetI3Particle(const I3Particle &p);
   //------------------------------------------------------------------------------

     bool operator==(const I3OpheliaParticle&) const;

 private:

   std::vector<I3OpheliaParticlePtr> daughters_;  // array of daughter pointer
   I3Particle                   i3particle_; // this field will be removed in future

   friend class boost::serialization::access;
                                                                                                  
   template <class Archive>
     void serialize(Archive& ar, unsigned version);
};

BOOST_CLASS_VERSION(I3OpheliaParticle, 1);

typedef I3Vector<I3OpheliaParticlePtr> I3OpheliaParticlePtrVect;
I3_POINTER_TYPEDEFS(I3OpheliaParticlePtrVect);

#endif
