
#ifndef I3PHOTOSPLINESERVICE_H
#define I3PHOTOSPLINESERVICE_H
/**
 *@file
 *@brief I3PhotoSplineService
 *       A class for accessing fitted photonics tables.
 *
 *@author Eike Middell
 *(c) the IceCube Collaboration
 */


#include "photonics-service/I3PhotonicsService.h"
#include <icetray/I3ServiceBase.h>

class I3PhotoSplineTable; //forward declaration
typedef boost::shared_ptr<I3PhotoSplineTable> I3PhotoSplineTablePtr;

class I3PhotoSplineService : public I3PhotonicsService, public I3ServiceBase {
    public:
        I3PhotoSplineService(const std::string &amplitudeTable,
            const std::string &timingTable, double timingSigma=0,
            double maxRadius=std::numeric_limits<double>::infinity());
        I3PhotoSplineService();
        I3PhotoSplineService(const I3Context &);
        virtual ~I3PhotoSplineService();

        void Configure();

        /**
         *@brief Load fitted photonics tables (spline coeffs)
         */
        virtual bool LoadSplineTables(std::string ampFileName, std::string timeFileName);

        /**
         *@brief Apply a psuedo-gaussian smearing to the photon arrival
         * time distribution.
         */
        void ConvoluteTime(double sigma);

        /**
         *@brief Select OM coordinates
         *@param OMx,OMy,OMz    OM coordinates
         */
        virtual void SelectModuleCoordinates(double OMx, double OMy, double OMz);
        /**
         *@brief Select source
         *Select source, return light yield, and set time distribution parameters for
         *later calls to timing methods. 
         *@param meanPEs                    Mean number of PEs
         *@param emissionPointDistance      Distance from nominal point of cherenkov point
         *                                  to OM
         *@param geoTime                    "Geometric time"
         *@param source                     Source parameters
         */
        virtual void SelectSource( double &meanPEs,
                                   double &emissionPointDistance,
                                   double &geoTime,
                                   PhotonicsSource const &source);

        virtual void SelectSource( double &meanPEs,
                                   double gradients[6],
                                   double &emissionPointDistance,
                                   double &geoTime,
                                   PhotonicsSource const &source,
				   bool getAmp=true);

        /**
         *@brief Get residual time for hit (w.r.t direct propagation from nominal 
         *       emission point)
         *@param[in]  random      Uniform random number 
         *@param[out] timeDelay   Residual time
         */
        virtual void GetTimeDelay(double random, double &timeDelay);

        /**
         *@brief Get residual times for hit (w.r.t direct propagation from nominal 
         *       emission point)
         *@param[in]  random      Random number generator
         *@param[out] timeDelay   Residual time
         *@param[in]  n		  Number of time delays to sample
         */
        virtual void GetTimeDelays(I3RandomServicePtr random,
	    double *timeDelays, int n);

        /**
         *@brief Get probability density for previously selected OM/source combination
         *@param[out] density       Probability density (ns^-1)for given residual time 
         *@param[in] timeDelay      Residual time
         */

        virtual void GetProbabilityDensity(double &density, double timeDelay);

	 /**
	  *@brief Fill an array with probability quantiles for a previously selected OM/source
	  	  combination, optionally convoluted with a Gaussian
	  *@param[in] time_edges	Array giving the edges of the time bins
	  *@param[in] t_0		Direct time (time at which unscattered photons from the source would reach the DOM)
	  *@param[out] amplitudes	Array to fill with mean PE counts
	  *@param[in] n_bins		Size of the amplitude array. There are n_bins + 1 time edges.
	  */

	virtual bool GetProbabilityQuantiles(double *time_edges, double t_0,
	    double *amplitudes, size_t n_bins);
	
	virtual bool GetProbabilityQuantiles(double *time_edges, double t_0,
	    double *amplitudes, double gradients[][6], size_t n_bins);

        /** 
         * @brief Get the gradient of the number of photoelectrons expected at the OM
         *  coordinates specified by a previous call to SelectOMCoordinates.
         * @param gradient (out) Store the gradient in this array. The gradient is given
         *                       with respect to the source position (d/dx, d/dy, d/dz
         *                       in PE/meter), direction of source origin (d/dtheta and
         *                       d/dphi in PE/radian) and source energy (in PE/GeV).
         */ 

	virtual bool GetMeanAmplitudeGradient(double gradient[6]);
	
        virtual bool GetMeanAmplitudeHessian(double gradient[6], double hessian[6][6]);

        /** 
         * @brief Get the gradient of photon detection probability at the OM coordinates
         *  specified by a previous call to SelectOMCoordinates in each of the time bins
         *  specified by time_edges.
         * @param time_edges (in)  Array giving the edges of the time bins.
         * @param t_0        (in)  Direct time (time at which unscattered photons
         *                         from the source would reach the DOM)
         * @param gradients  (out) Store the gradients for each time bin in this array.
         *                         Gradients are given with respect to source position
         *                         (d/dx, d/dy, d/dz in meter^-1), event time (d/dt in
         *                         ns^-1), and direction of source origin (d/dtheta,
         *                         d/dphi in radian^-1).
         * @param n_bins     (in)  Size of the leading dimension of gradients. There
         *                         are n_bins+1 elements in time_edges.
         */

        virtual bool GetProbabilityQuantileGradients(double *time_edges, double t_0,
            double gradients[][6], size_t n_bins);

        virtual bool GetProbabilityQuantileHessians(double *time_edges,
            double t_0, double values[], double gradients[][6], double hessians[][6][6], size_t n_bins);


    protected:
        I3PhotoSplineTablePtr amplitudeSplineTable_;
        I3PhotoSplineTablePtr timingSplineTable_;

        SET_LOGGER("I3PhotoSplineService");

    private:
		double maxRadius_;
        // PhotonicsInput parameters - to be filled by I3PhotonicsService::GetPhotonicsInput
	double photonicsCoords_[4];
        // double zenith_, rho_, phiOM_, l_,  lStop_, z_, zStop_,  eOffset_, eDistance_;
        //remember info to be filled by SelectSource
        double geoTime_;
        double meanPEs_;

        int geotype_;
       
        // remember source type
        int sourceType_;
	// remember source length - lightsaber is differentiated from cascades via length
        int sourceLength_;
        // remember the last source requested (useful for sequential PhotoRec calls)
        PhotonicsSource lastSource_;

        double rawYield_; // unscaled npe returned by photonics

	void FillTableCoordinates(double tablecoordinates[6], bool timing);

	void ConvertMeanAmplitudeGradient(double raw_gradient[6],
	    double gradient[6], const PhotonicsSource &source);
	
	void ScaleMeanAmplitudeGradient(double gradient[6],
	    const double &rawYield, const double &meanPEs,
	    const PhotonicsSource &source);
};

I3_POINTER_TYPEDEFS(I3PhotoSplineService);


#endif
