/**
 * $Id$
 * copyright (C) 2006
 * the IceCube collaboration
 * @version $Revision$
 *
 * @author Doug Rutledge
 * @date 26Aug2006
 */

#ifndef RECCLASSES_I3FILLRATIOINFO_H_INCLUDED
#define RECCLASSES_I3FILLRATIOINFO_H_INCLUDED

#include "icetray/I3FrameObject.h"
#include <boost/serialization/access.hpp>

/**
 *  @brief Stores results of I3FillRatioModule (see project fill-ratio).
 *
 *  It is equivalent to a *FitParams from one of the reconstruction modules.
 */
class I3FillRatioInfo : public I3FrameObject
{
  public:
    /**
     * Create an object of this class.
     */
    I3FillRatioInfo()
    { Clear(); }
  
    /**
     * Destroys an object of this class.
     */
    ~I3FillRatioInfo(){}

    /**
     * Clear is a convenience method for resetting the values to an
     * invalid value, in this -1 for all parameters.  It is currently
     * only used in the constructor, but is left for external projects
     * that may need to make use of it.
     */
    void Clear()
    {
       meanDistance_              = -1.0; /* negative value is always invalid */
       rmsDistance_               = -1.0; /* negative value is always invalid */
       nChDistance_               = -1.0; /* negative value is always invalid */
       energyDistance_            = -1.0; /* negative value is always invalid */
       fillRadiusFromRMS_         = -1.0; /* negative value is always invalid */
       fillRadiusFromMean_        = -1.0; /* negative value is always invalid */
       fillRadiusFromNCh_         = -1.0; /* negative value is always invalid */
       fillRadiusFromMeanPlusRMS_ = -1.0; /* negative value is always invalid */
       fillRadiusFromEnergy_      = -1.0; /* negative value is always invalid */
       fillRatioFromRMS_          = -1.0; /* negative value is always invalid */
       fillRatioFromMean_         = -1.0; /* negative value is always invalid */
       fillRatioFromMeanPlusRMS_  = -1.0; /* negative value is always invalid */
       fillRatioFromNCh_          = -1.0; /* negative value is always invalid */
       fillRatioFromEnergy_       = -1.0; /* negative value is always invalid */
       hitCount_                  = -1;   /* negative value is always invalid */
    }
   
    //Getters and setters
    /**
     * Get the mean distance between hit OMs ant the supplied vertex.
     * The I3FillRatioModule calculates the mean distance between Hit OMs 
     * and the vertex.
     * This is related to a cascade SPE radius, and can thus be calubrated
     * to give a first guess of the energy.
     * This is used internally by the iI3FillRatioModule to define one of 
     * its spheres.
     */
    const double GetMeanDistance() const
    { return meanDistance_; }
    
    /**
     * Set the mean distance. Used by the I3FillRatioModule to fill this
     * object.
     */ 
    void SetMeanDistance(double meanDistance)
    { meanDistance_ = meanDistance; }

    /**
     * Get the SPE Radius, estimated from the NCh.
     * This is used internally by the module to define one of its spheres.
     */
    const double GetNChDistance() const
    { return nChDistance_; }

    /**
     * Set the nCh-estimated SPE Radius. Used by the I3FillRatioModule to fill this object.
     */ 
    void SetNChDistance(double nChDistance)
    { nChDistance_ = nChDistance; }

    /**
     * Get the SPE radius, obtained from the energy estimate.
     * This is used internally by the module to define one of its spheres.
     */
    const double GetEnergyDistance() const
    { return energyDistance_; }

    /**
     * Set the energy-estimated SPE Radius. Used by the I3FillRatioModule to 
     * fill this object.
     */ 
    void SetEnergyDistance(double energyDistance)
    { energyDistance_ = energyDistance; }

    /**
     * Get the RMS of the distance between hit OMs ant the supplied vertex.
     * This module calculates the distribution of distances between Hit OMs 
     * and the vertex.
     * This is used internally by the module to define one of its spheres.
     */
    const double GetRMSDistance() const
    { return rmsDistance_; }

    /**
     * Set the RMS of the distance. Used by the I3FillRatioModule to fill this
     * object.
     */
    void SetRMSDistance(double rmsDistance)
    { rmsDistance_ = rmsDistance; }

    /**
     * Get the Radius of the sphere defined by the RMS (times some 
     * user-defined number).
     */
    const double GetFillRadiusFromRMS() const
    { return fillRadiusFromRMS_; }
    
    /**
     * Set the value of the radius of the RMS-defined sphere.
     */ 
    void SetFillRadius(double fillRadius)
    { fillRadiusFromRMS_ = fillRadius; }

    /**
     * Get the Radius of the sphere defined by the NCh (times some 
     * user-defined number).
     */
    const double GetFillRadiusFromNCh() const
    { return fillRadiusFromNCh_; }

    /**
     * Set the value of the radius of the NCh-defined sphere.
     */
    void SetFillRadiusFromNCh(double fillRadiusFromNCh)
    { fillRadiusFromNCh_ = fillRadiusFromNCh; }

    /**
     * Get the radius of the sphere defined by the mean (times some 
     * user-defined number). 
     */
    const double GetFillRadiusFromMean() const
    { return fillRadiusFromMean_; }

    /**
     * Set the value of the radius of the Mean-defined sphere.
     */
    void SetFillRadiusFromMean(double fillRadiusFromMean)
    { fillRadiusFromMean_ = fillRadiusFromMean; }

    /**
     * Get the radius of the sphere defined by the mean (times some 
     * user-defined number). 
     */
    const double GetFillRadiusFromMeanPlusRMS() const
    { return fillRadiusFromMeanPlusRMS_; }

    /**
     * Set the value of the radius of the MeanPlusRMS-defined sphere.
     */
    void SetFillRadiusFromMeanPlusRMS(double fillRadiusFromMeanPlusRMS)
    { fillRadiusFromMeanPlusRMS_ = fillRadiusFromMeanPlusRMS; }

    /**
     * Get the radius of the sphere defined by the mean (times some 
     * user-defined number). 
     */
    const double GetFillRadiusFromEnergy() const
    { return fillRadiusFromEnergy_; }

    /**
     * Set the value of the radius of the Energy-defined sphere.
     */
    void SetFillRadiusFromEnergy(double fillRadiusFromEnergy)
    { fillRadiusFromEnergy_ = fillRadiusFromEnergy; }

    /**
     * Get the fraction of oms that are inside the RMS-defined sphere 
     * that are hit.
     */
    const double GetFillRatioFromRMS() const 
    { return fillRatioFromRMS_; }
   
    /**
     * Get the fraction of oms that are inside the RMS-defined sphere      
     * that are hit.
     */
    void SetFillRatioFromRMS(double fillRatioFromRMS)
    { fillRatioFromRMS_ = fillRatioFromRMS; }

    /**
     * Get the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    const double GetFillRatioFromMean() const
    { return fillRatioFromMean_; }

    /**
     * Set the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    void SetFillRatioFromMean(double fillRatioFromMean)
    { fillRatioFromMean_ = fillRatioFromMean; }

    /**
     * Get the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    const double GetFillRatioFromMeanPlusRMS() const
    { return fillRatioFromMeanPlusRMS_; }

    /**
     * Set the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    void SetFillRatioFromMeanPlusRMS(double fillRatioFromMeanPlusRMS)
    { fillRatioFromMeanPlusRMS_ = fillRatioFromMeanPlusRMS; }

    /**
     * Get the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    const double GetFillRatioFromEnergy() const
    { return fillRatioFromEnergy_; }

    /**
     * Set the fraction of oms that are inside the Mean-defined sphere
     * that are hit.
     */
    void SetFillRatioFromEnergy(double fillRatioFromEnergy)
    { fillRatioFromEnergy_ = fillRatioFromEnergy; }

    /**
     * Get the fraction of oms that are inside the NCh-defined sphere
     * that are hit.
     */
    const double GetFillRatioFromNCh() const
    { return fillRatioFromNCh_; }

    /**
     * Set the fraction of oms that are inside the NCh-defined sphere
     * that are hit.
     */
    void SetFillRatioFromNCh(double fillRatioFromNCh)
    { fillRatioFromNCh_ = fillRatioFromNCh; }

    /**
     * Get the number of OMs that were present in this event.
     */
    const double GetHitCount() const
    { return hitCount_; }
   
    /**
     * Set the number of OMs that were present in this event.
     */
    void SetHitCount(int hitCount) 
    { hitCount_ = hitCount; }

    bool operator==(const I3FillRatioInfo&) const;

  private:

    /**
     * The mean of the distance distribution.
     */
    double meanDistance_;

    /**
     * The RMS of the distance distribution.
     */
    double rmsDistance_;

    /**
     * The NCh-defined spe distance.
     */
    double nChDistance_;

    /**
     * The Energy-defined spe distance.
     */
    double energyDistance_;

    /**
     * The radius (in meters) of the rms-defined sphere.
     */
    double fillRadiusFromRMS_;
  
    /**
     * The radius (in meters) of the mean-defined sphere.
     */
    double fillRadiusFromMean_;

    /**
     * The radius (in meters) of the mean-plus-rms-defined sphere.
     */
    double fillRadiusFromMeanPlusRMS_;

    /**
     * The radius (in meters) of the mean-defined sphere.
     */
    double fillRadiusFromNCh_;

    /**
     * The radius (in meters) of the mean-defined sphere.
     */
    double fillRadiusFromEnergy_;

    /**
     * The fraction of OMs inside of the RMS-defined sphere which are hit.
     */
    double fillRatioFromRMS_;

    /** 
     * The fraction of OMs inside of the Mean-Defined sphere which are hit.
     */
    double fillRatioFromMean_;

    /** 
     * The fraction of OMs inside of the MeanPlusRMS-Defined sphere which are hit.
     */
    double fillRatioFromMeanPlusRMS_;

    /** 
     * The fraction of OMs inside of the NCh-Defined sphere which are hit.
     */
    double fillRatioFromNCh_;

    /** 
     * The fraction of OMs inside of the NCh-Defined sphere which are hit.
     */
    double fillRatioFromEnergy_;

    /**
     * The number of hit OMs in this event.
     */
    int hitCount_;

    /**
     * Used by the dataio objects to read and write this object to a file.
     * Don't use this method yourself. It will be called by the dataio classes.
     *
     * @param Archive --the archive that this class will be written to.
     * @param version -- the version of this class (useful for 
     * backwards-compatibility).
     */
    friend class boost::serialization::access;
    template<class Archive>
      void serialize(Archive& archive, unsigned version);
};

I3_POINTER_TYPEDEFS(I3FillRatioInfo);

#endif // FILLRATIO_I3FILLRATIOINFO_H_INCLUDED
