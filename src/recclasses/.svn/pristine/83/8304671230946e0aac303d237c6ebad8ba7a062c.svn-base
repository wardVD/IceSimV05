/**
 * $Id$
 * copyright  (C) 2005
 * the icecube collaboration
 *  $Id: I3CscdLlhFitParams.h 111030 2013-09-21 20:54:48Z claudio.kopper $
 *
 * @version $Revision: 1.2 $
 * @date $Date: 2013-09-21 22:54:48 +0200 (Sat, 21 Sep 2013) $
 * @author mggreene
*/

#ifndef I3_CSCD_LLH_FIT_PARAMS_H
#define I3_CSCD_LLH_FIT_PARAMS_H

#include "icetray/I3FrameObject.h"
#include "dataclasses/Utility.h"

/**
 * @brief Stores results of CscdLlh fit (see project cscd-llh).
 *
 * The most important variables are stored in the I3BasicCascade superclass:
 * Use I3BasicCascade::GetPos() to get the vertex position;  
 * Use I3BasicCascade::GetT() to get the vertex time.
 */
class I3CscdLlhFitParams : public I3FrameObject
{
  public:
    
    I3CscdLlhFitParams() 
    {
      Clear();
    };

    ~I3CscdLlhFitParams() 
    {};

    /**
     * Reset values of member variables.
     */
    void Clear() 
    {
      hitCount_ = INT_MIN;
      hitOmCount_ = INT_MIN;
      unhitOmCount_ = INT_MIN;
      status_ = INT_MIN;
  
      errT_ = NAN;
      errX_ = errY_ = errZ_ = NAN;
      errTheta_ = errPhi_ = NAN;
      errEnergy_ = NAN;

      negLlh_ = NAN;
      reducedLlh_ = NAN;

      return;
    }
  
    /**
     * getHitCount returns the number of hits in the event.
     */
    int GetHitCount() const 
    { 
      return hitCount_;
    }
  
    void SetHitCount(int hitCount) 
    {
      hitCount_ = hitCount;
    }

    /**
     * getHitOmCount returns the number of hit OM's in the event.
     */
    int GetHitOmCount() const 
    {
      return hitOmCount_; 
    }
 
    void SetHitOmCount(int hitOmCount) 
    {
      hitOmCount_ = hitOmCount;
    }

    /**
     * getUnhitOmCount returns the number of active OM's 
     * that were <I>not</I> hit
     */
    int GetUnhitOmCount() const 
    {
      return unhitOmCount_;
    }
    void SetUnhitOmCount(int unhitOmCount) 
    {
      unhitOmCount_ = unhitOmCount;
    }

    /**
     * getStatus returns the minimizer fit status.
     */
    int GetStatus() const 
    {
      return status_;
    }
    void SetStatus(int status) 
    {
      status_ = status;
    }
  
    /**
     * getErrT returns the error in the vertex time.
     */
    double GetErrT() const 
    {
      return errT_;
    }
    void SetErrT(double errT) 
    {
      errT_ = errT;
    }

    /**
     * getErrX returns the error in the vertex x-coordinate.
     */
    double GetErrX() const 
    {
      return errX_;
    }
    
    void SetErrX(double errX) 
    {
      errX_ = errX;
    }

    /**
     * getErrY returns the error in the vertex y-coordinate.
     */
    double GetErrY() const 
    {
      return errY_;
    }
    void SetErrY(double errY) 
    { 
      errY_ = errY;
    }

    /**
     * getErrZ returns the error in the vertex z-coordinate.
     */
    double GetErrZ() const 
    {
      return errZ_;
    }
    void SetErrZ(double errZ) 
    {  
      errZ_ = errZ;
    }
  
    /**
     * getErrTheta returns the error in the polar angle.
     */
    double GetErrTheta() const 
    {
      return errTheta_;
    }
    void SetErrTheta(double errTheta) 
    { 
      errTheta_ = errTheta;
    }

    /**
     * getErrPhi returns the error in the vertex time.
     */
    double GetErrPhi() const 
    {  
      return errPhi_;
    }
    void SetErrPhi(double errPhi) 
    {
      errPhi_ = errPhi;  
    }

    /**
     * getErrEnergy returns the error in the vertex time.
     */
    double GetErrEnergy() const 
    {
      return errEnergy_;
    }
    void SetErrEnergy(double errEnergy) 
    {
      errEnergy_ = errEnergy;
    }
 
    /**
     * getNegLlh returns the negative log likelihood at the minimum.
     */
    double GetNegLlh() const 
    {
      return negLlh_;
    }
    void SetNegLlh(double negLlh) 
    {
      negLlh_ = negLlh;
    }
 
    /**
     * getReducedLlh returns the reduced log likelihood.
     */
    double GetReducedLlh() const 
    {
      return reducedLlh_;
    }
    void SetReducedLlh(double reducedLlh) 
    {
      reducedLlh_ = reducedLlh;
    }
  
    void CopyTo(I3CscdLlhFitParams& destination) const
    {
      I3CscdLlhFitParams* dest = 
        dynamic_cast<I3CscdLlhFitParams*>(&destination);
      
      if (dest) 
      {
        dest->hitCount_ = hitCount_;
        dest->hitOmCount_ = hitOmCount_;
        dest->unhitOmCount_ = unhitOmCount_;
        dest->status_ = status_;
        dest->errT_ = errT_;
        dest->errX_ = errX_;
        dest->errY_ = errY_;
        dest->errZ_ = errZ_;
        dest->errTheta_ = errTheta_;
        dest->errPhi_ = errPhi_;
        dest->errEnergy_ = errEnergy_;
        dest->negLlh_ = negLlh_;
        dest->reducedLlh_ = reducedLlh_;
      }
    }
  
   private:
  
    int hitCount_;
    int hitOmCount_;
    int unhitOmCount_;
    int status_;
  
    double errT_;
    double errX_, errY_, errZ_;
    double errTheta_, errPhi_;
    double errEnergy_;

    double negLlh_;
    double reducedLlh_;

    friend class boost::serialization::access;
    template<class Archive> 
      void serialize(Archive& archive, unsigned version);
}; // class I3CscdLlhFitParams

I3_POINTER_TYPEDEFS(I3CscdLlhFitParams);

#endif
