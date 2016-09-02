/**
 * $Id$
 * @author Kotoyo Hoshina 01/Jun/06                                     
 */

#ifndef RECCLASSES_I3OPHELIARECORESULT_H
#define RECCLASSES_I3OPHELIARECORESULT_H

#include "recclasses/I3OpheliaParticle.h"
#include "recclasses/I3OpheliaFirstGuessTrack.h"
#include <dataclasses/I3Vector.h>

/**                                                                            
 * @brief Stores reco result of EHE analysis (see project ophelia).
 *
 * It also has place folders to store a First Guess Track and most significant
 * Stocastic Cascade. The final reco results can be store daughter area 
 * temporary, because the dataclass will change again...
 */                                                                       
class I3OpheliaRecoResult :  public I3OpheliaParticle
{

 public:

   /**
    * Destructor. 
    */
   virtual ~I3OpheliaRecoResult() {}

   /**
    * Returns first guess track which has the velocity and center of brightness
    */
   const I3OpheliaFirstGuessTrack& GetFirstGuessTrack() const
   { return firstGuessTrack_; }

   /**
    * Returns most significant stochastic cascade
    */
   const I3OpheliaParticle& GetStochasticCascade() const
   { return stochasticCascade_; }

   /**
    * Set FirstGuessTrack
    */
   void SetFirstGuessTrack(const I3OpheliaFirstGuessTrack& track)
   { firstGuessTrack_ = track; }

   /**
    * Set stochastic cascade
    */
   void SetStochasticCascade(const I3OpheliaParticle& cascade)
   { stochasticCascade_ = cascade; }

   /**
    * dump information of the particle
    */
   virtual void DumpOut(std::string indent = "") const;

   bool operator==(const I3OpheliaRecoResult&) const;

 private:

   I3OpheliaFirstGuessTrack firstGuessTrack_;
   I3OpheliaParticle stochasticCascade_;

   friend class boost::serialization::access;

   template <class Archive>
      void serialize(Archive& ar, unsigned version);
};

I3_POINTER_TYPEDEFS(I3OpheliaRecoResult);

#endif //I3OPHELIAVECT_H
