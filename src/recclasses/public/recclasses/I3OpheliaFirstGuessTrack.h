/**
 * $Id$
 * @author Kotoyo Hoshina 23/Aug/05                                     
 *         Kotoyo Hoshina 24/Apr/06 Modified to comply dataclass v2 
 */

#ifndef RECCLASSES_I3OPHELIAFIRSTGUESSTRACK_H
#define RECCLASSES_I3OPHELIAFIRSTGUESSTRACK_H

#include "I3OpheliaParticle.h"
#include <dataclasses/I3Position.h>

#ifdef I3_USE_ROOT
  #include "TVector3.h"
#endif

/**
 * @brief Stores a simple track description (see project ophelia).
 *                                                                          
 * A simple track class which holds line fit velocity and center of brightness.
 */
class I3OpheliaFirstGuessTrack :  public I3OpheliaParticle
{

 public:

   /**
    * Constructor. 
    * Default value:
    *    ParticleShape : Cascade
    *    ParticleType  : Brems
    */
     I3OpheliaFirstGuessTrack(); 
   //------------------------------------------------------------------------------

   /**
    * Destructor. 
    */
     virtual ~I3OpheliaFirstGuessTrack() {}
   //------------------------------------------------------------------------------

   /**
    * dump information of the particle
    */
     virtual void DumpOut(std::string indent = "") const;
   //------------------------------------------------------------------------------

#ifdef I3_USE_ROOT
   /**
    * Returns line fit velocity
    * @return TVector3 --- line fit velocity
    */
     TVector3 GetVelocity() const
     { return TVector3(xvel_, yvel_, zvel_); }
   //------------------------------------------------------------------------------

   /**
    * Returns a 3D position of center of brightness
    * @return TVector3 --- position of center of brightness
    */
     TVector3 GetCenterOfBrightness() const
     { return TVector3(xpos_, ypos_, zpos_); }
   //------------------------------------------------------------------------------

   /**
    * Returns a 3D position of DOM with the largest NPE
    * @return TVector3 --- position of DOM
    */
     TVector3 GetLargestNPEDOMposition() const
     { return TVector3(brightestDOMxpos_, brightestDOMypos_, brightestDOMzpos_); }
   //------------------------------------------------------------------------------

   /**
    * Set velocity */
     void SetVelocity(const TVector3& v)
     { xvel_ = v.X();  yvel_ = v.Y(); zvel_ = v.Z(); }
   //------------------------------------------------------------------------------

   /**
    * Set center of brightness
    */
     void SetCenterOfBrightness(const TVector3& p)
     { xpos_ = p.X();  ypos_ = p.Y(); zpos_ = p.Z(); }
   //------------------------------------------------------------------------------

   /**
    * Set position of the DOM with the largest NPE - the brightest
    */
     void SetLargestNPEDOMposition(const TVector3& p)
     { brightestDOMxpos_ = p.X();  brightestDOMypos_ = p.Y(); brightestDOMzpos_ = p.Z(); }
   //------------------------------------------------------------------------------
#endif

     double GetVelocityX() const { return xvel_; }
     double GetVelocityY() const { return yvel_; }
     double GetVelocityZ() const { return zvel_; }
     void SetVelocity(double x, double y, double z) { xvel_ = x;  yvel_ = y; zvel_ = z; }
     double GetCenterOfBrightnessX() const { return xpos_; }
     double GetCenterOfBrightnessY() const { return ypos_; }
     double GetCenterOfBrightnessZ() const { return zpos_; }
     void SetCenterOfBrightness(double x, double y, double z) { xpos_ = x;  ypos_ = y; zpos_ = z; }
     double GetLargestNPEDOMpositionX() const { return brightestDOMxpos_; }
     double GetLargestNPEDOMpositionY() const { return brightestDOMypos_; }
     double GetLargestNPEDOMpositionZ() const { return brightestDOMzpos_; }
     void SetLargestNPEDOMposition(double x, double y, double z)
     { brightestDOMxpos_ = x;  brightestDOMypos_ = y; brightestDOMzpos_ = z; }

   /**
    * Returns first guess fit is acceptable
    * @return bool -- if fail, do not use this results
    */
    bool IsFitSuccessful() const { return fitsuccess_; }
   //------------------------------------------------------------------------------

   /**
    * Returns first guess fit quality parameters
    * @return double -- first guess fit quality parameters
    */
    double GetFitQuality() const { return fitquality_; }
   //------------------------------------------------------------------------------

   /**
    * Set if this fit sucess
    */
    void SetFitSuccessful(const bool success) { fitsuccess_ = success; }
   //------------------------------------------------------------------------------
                                                                                                              
   /**
    * Set first guess fit quality parameters
    * this is not automatically calculated by the method but
    * you can calculate whatevern your preffered parameter
    * and put it here
    */
    void SetFitQuality(const double fitq) { fitquality_ = fitq; }
   //------------------------------------------------------------------------------

    bool operator==(const I3OpheliaFirstGuessTrack&) const;

 private:

   double     xvel_;     // line fit velocity x
   double     yvel_;     // line fit velocity y
   double     zvel_;     // line fit velocity z
   double     xpos_;     // center of brightness x
   double     ypos_;     // center of brightness y
   double     zpos_;     // center of brightness z
   double     brightestDOMxpos_; // x-position of the brightest DOM in this track
   double     brightestDOMypos_; // x-position of the brightest DOM in this track
   double     brightestDOMzpos_; // x-position of the brightest DOM in this track
   bool       fitsuccess_; //checker parameter if this fit is reasonable
   double     fitquality_; //first guess fit quality parameter

   friend class boost::serialization::access;
                                                                                                  
   template <class Archive>
     void serialize(Archive& ar, unsigned version);
};

BOOST_CLASS_VERSION(I3OpheliaFirstGuessTrack, 2);

I3_POINTER_TYPEDEFS(I3OpheliaFirstGuessTrack);

typedef I3Vector<I3OpheliaFirstGuessTrackPtr> I3OpheliaFirstGuessTrackPtrVect;
I3_POINTER_TYPEDEFS(I3OpheliaFirstGuessTrackPtrVect);

#endif //I3OPHELIATRACK_H
