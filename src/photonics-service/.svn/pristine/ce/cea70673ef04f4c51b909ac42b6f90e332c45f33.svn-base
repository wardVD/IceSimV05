#ifndef I3PHOTONICSSERVICE_H
#define I3PHOTONICSSERVICE_H

/**
 *@file
 *@brief I3PhotonicsService
 *       A class for accessing photonics tables.
 *
 *       Heavily modelled on PSInterface by Thomas Burgess, but with
 *       reduced generality (photonics only), error checking, 
 *       and execution time. Methods using photonics are declared as virtual
 *       to enable re-implementation in derived dummy class.
 *@author Klas Hultqvist
 *@version $Id$
 *@date $Date$
 *(c) the IceCube Collaboration
 *
 */

#include <icetray/I3Logging.h>
#include <icetray/I3PointerTypedefs.h>
#include <dataclasses/physics/I3Particle.h>
#include <phys-services/I3RandomService.h>

#include "photonics-service/geo_type.h"

#include <string>
#include <cfloat>
#include <cmath>
#include <vector>

class I3OMGeo;

/**
 *@brief A type for passing the light source parameters 
 */
struct PhotonicsSource {
	PhotonicsSource();

	/**
	 *@brief Fill a PhotonicsSource struct from an I3Particle
	 */
	PhotonicsSource(const I3Particle &);

	/**
	 *@brief Fill a PhotonicsSource struct from basic quantities
	 *@param x,y,z            Source coordinates (metres)
	 *@param zenith,azimuth   Direction which source points from (degrees)
	 *@param speed		  Particle speed (fraction of c)
	 *@param Energy           Energy (GeV)
	 *@param length           Source length (zero for point source) (metres)
	 *@param type             Source type (0=muon, 1=EM cascade)  
	 */
	PhotonicsSource(double x, double y, double z, double zenith,
	    double azimuth, double speed, double length, double Energy,
	    int type);

	/**
	 * @brief Coordinates of source
	 */
	double x,y,z;
	/**
	 * @brief Direction source is pointing
	 */
	double dirx,diry,dirz;
	/**
	 * @brief Perpendicular vector
	 *
	 * Unit vector perpendicular to source direction 
	 * and pointing a close to vertically upwards as possible.
	 */
	double perpx,perpy,perpz;
	/**
	 * @brief Decomposition of source direction
	 */
	double sintheta,costheta,sinphi,cosphi;
	/**
	 * @brief Zenith angle angle source is pointing away from (zero if
	 * pointing vertically down)
	 */
	double zenith;
	/**
	 * @brief Energy (or energy per metre) of source particle 
	 * 
	 * The energy is used to re-scale the light output
	 * appropriately. For cascade tables the re-scaling is linear in
	 * the cascade energy. For level 2 lineshower tables ("light-sabres"),
	 * with cascades uniformly distributed along the track,
	 * this implies that the re-scaling gives the light output for a
	 * deposited energy E per metre. This is not true for level 2 muon
	 * tables, since for such tables the re-scaling is based on the
	 * muon energy, and includes the effects of the continuous energy loss.
	 */
	double E;

	/**
	 *@brief  Length of source
	 */
	double length;

	/**
  	 *@brief  Speed of source, as a fraction of c
 	 */
	double speed;

	/**
	 *@brief  Photonics type (0=muon,1=EM cascade).
	 *
	 * This refers to the photonics definitions (in photoamasim.h and
	 * light.c):
	 *     MUON=0, EMS=1, HADS=2
	 * photonics-service knows about these three types and uses the
	 * photonics light() function to calculate the light output.
	 *
	 * Note, however, that as of December 2008 there are no hadronic tables,
	 * and the EM tables are used also for hadronic cascades, but with
	 * corrections applied externally by photonics-service.
	 * 
	 * For types outside the range 0,1,2 no conversion from table content to
	 * light output is made. In addition, for such types level1 EM tablesi
	 * are generally assumed. For certain "known types" different from 0
	 * and 1, however, photon tables can be generated, and in this case the
	 * "known type" will be passed to photonics. These "known types" are
	 * currently 9,10,11,12 and have been used for monopole tables.
	 *
	 * Photonics also defines the following source_type enumeration:  
	 * ISOTROPIC = 1,CHERENKOV = 2,BEAMED    = 3,COSTHETA  = 4,
	 * EM_SHOWER = 5, HAD_SHOWER= 6, FLASHER_LED=7, FLASHER_BOARD=8.
	 * but these are not used here.
	 *
	 */
	int type;
};

inline bool operator ==(const PhotonicsSource &A, const PhotonicsSource &B) {
	return (A.x==B.x && A.y==B.y && A.z==B.z && A.dirx==B.dirx &&
	    A.diry==B.diry && A.dirz==B.dirz && A.E==B.E &&
	    A.length==B.length && A.type==B.type);
}

/**
 *@brief Service interfacing to the photonics table access functions
 */
class I3PhotonicsService {
protected:
	SET_LOGGER("I3PhotonicsService");
public:
	I3PhotonicsService();
	virtual ~I3PhotonicsService();

	/**
	 *@brief Fill a PhotonicsSource struct from basic quantities
	 *@param x,y,z            Source coordinates (metres)
	 *@param zenith,azimuth   Direction which source points from (degrees)
	 *@param Energy           Energy (GeV)
	 *@param length           Source length (zero for point source) (metres)
	 *@param type             Source type (0=muon, 1=EM cascade)  
	 *
	 * NB: DEPRECATED IN FAVOR OF PHOTONICSSOURCE METHODS
	 */
	static PhotonicsSource FillPhotonicsSource(double x, double y, double z,
	    double zenith, double azimuth, double length, double Energy,
	    int type) __attribute__((__deprecated__));

	/** 
	 *@brief Select OM coordinates
	 *@param xOM,yOM,zOM        Coordinates in metres
	 */
	virtual void SelectModuleCoordinates(double xOM,double yOM,double zOM);

	/** 
	 *@brief Select OM coordinates
	 * By default, this calls SelectModuleCoordinates. Subclasses that know
	 * about more dimensions should override this function.
	 *@param geo        Module coordinates, orientation, etc
	 */
	virtual void SelectModule(const I3OMGeo &geo);

	/** 
	 *@brief Select light source
	 * Set source properties, calculate geometrical quantities based on 
	 * OM previously selected by SelectModuleCoordinates(), call photonics,
	 * and return result.
	 * 
	 *@param meanPEs                  Mean number of PEs
	 *@param emissionPointDistance    Distance between nominal cherenkov
	 *                                emission point and OM
	 *@param geoTime                  Nominal arrival time for cherenkov
	 *                                light relative to time of track
	 *                                starting point
	 *@param source                   Source parameters
	 */
	virtual void SelectSource(double &meanPEs,
	    double &emissionPointDistance, double &geoTime,
	    PhotonicsSource const &source) = 0;
	virtual void SelectSource( double &meanPEs, double gradients[6],
	    double &emissionPointDistance, double &geoTime,
	    PhotonicsSource const &source, bool getAmp=true);
	void SelectSource(double &meanPEs, double &emissionPointDistance,
	    double &geoTime, double x, double y, double z, double zenith,
	    double azimuth, double length, double Energy, int type)
	    __attribute__((__deprecated__));

	/** 
	 *@brief Get photon arrival time 
	 * Draws a photon arrival time (in ns) using table pointers set up 
	 * by previous call to SelectSource.
	 *@param[in]  random     Random number (0-1)
	 *@param[out] timeDelay  Photon arrival time relative to direct light
	 *                       time
	 */
	virtual void GetTimeDelay(double random, double &timeDelay) = 0;
	virtual void GetTimeDelays(I3RandomServicePtr random,
	    double *timeDelays, int n);

	/** 
	 *@brief Get photon probability density in time
	 * 
	 * Get time probability density for single photon arrival time
	 * (in ns^-1), based on the table pointers set up in latest call to
	 * SelectSource().
	 * 
	 *@param[out] density    Probability density (ns^-1)
	 *@param[in]  timeDelay  Time offset from direct light arrival time
	 */
	virtual void GetProbabilityDensity(double &density, double timeDelay) = 0;

	/**
	 *@brief Fill an array with probability quantiles for a previously
	  selected OM/source combination
	 *@param[in] time_edges    Array giving the edges of the time bins
	 *@param[in] t_0           Direct time (time at which unscattered
	 *                         photons from the source would reach the DOM)
	 *@param[out] amplitudes   Array to fill with mean PE counts
	 *@param[in] n_bins        Size of the amplitude array. There are
	 *                         n_bins + 1 time edges.
	 */

	virtual bool GetProbabilityQuantiles(double *time_edges, double t_0,
	    double *amplitudes, size_t n_bins) = 0;
	virtual bool GetProbabilityQuantiles(double *time_edges, double t_0,
	    double *amplitudes, double gradients[][6], size_t n_bins);

	/** 
	 * @brief Get the gradient of the number of photoelectrons expected at
	 * the OM coordinates specified by a previous call to
	 * SelectModuleCoordinates.
	 * @param gradient (out) Store the gradient in this array. The gradient
	 *                       is given with respect to the source position
	 *                       (d/dx, d/dy, d/dz in PE/meter), direction of
	 *                       source origin (d/dtheta and d/dphi in
	 *                       PE/radian) and source energy (in PE/GeV).
	 */ 
	  
	virtual bool GetMeanAmplitudeGradient(double gradient[6]) = 0;
	virtual bool GetMeanAmplitudeHessian(double gradient[6], double
	    hessian[6][6]) = 0;

	/** 
	* @brief Get the gradient of photon detection probability at the OM
	*        coordinates specified by a previous call to SelectOMCoordinates
	*        in each of the time bins specified by time_edges.
	* @param time_edges (in)  Array giving the edges of the time bins.
	* @param t_0        (in)  Direct time (time at which unscattered photons
	*                         from the source would reach the DOM)
	* @param gradients  (out) Store the gradients for each time bin in this
	*                         array. Gradients are given with respect to
	*                         source position (d/dx, d/dy, d/dz in
	*                         meter^-1), event time (d/dt in ns^-1), and
	*                         direction of source origin (d/dtheta, d/dphi
	*                         in radian^-1).
	* @param n_bins     (in)  Size of the leading dimension of gradients.
	*                         There are n_bins+1 elements in time_edges.
	*/

	virtual bool GetProbabilityQuantileGradients(double *time_edges,
	    double t_0, double gradients[][6], size_t n_bins) = 0;
	virtual bool GetProbabilityQuantileHessians(double *time_edges,
	    double t_0, double values[], double gradients[][6],
	    double hessians[][6][6], size_t n_bins) = 0;

	/**
	 *@brief Get photorec information 
	 *   
	 * Gets photorec information based on OM coordinates specified by
	 * previous call to SelectOMCoordinates, delay time, and photonics
	 * source information in PhotonicsSource struct. 
	 *
	 *@param[out] yield        Light yield as given by table.
	 *                         For level 2 cascade line sources this is
	 *                         usually normalised to an energy loss of
	 *                         1 GeV/m
	 *@param[out] probDensity  Probability density in time (ns^-1)
	 *@param[in] delay         Delay time
	 *@param[in] source        Photonics source
	 *@return  Success or failure of photonics table access
	 */
	virtual bool GetPhotorecInfo(double &yield, double &probDensity,
	    double delay, PhotonicsSource const &source);
	virtual bool GetPhotorecDensities( double &yield,
	    std::vector<double> &probDensities, std::vector<double> delays,
	    PhotonicsSource const &source);
	bool GetPhotorecInfo(double &yield, double &probdensity,
	    double delay, double x, double y, double z, double zenith,
	    double azimuth, double length, double Energy, int type)
	    __attribute__((__deprecated__));

	/** 
	 *@brief Set angular selection for loading of photonics tables.
	 *       This is advisory only, and does nothing by default.
	 *@param level       Photonics level affected
	 *@param low         Minimum zenith angle (degrees)
	 *@param high        Maximum zenith angle (degrees)
	 */
	virtual bool SetAngularSelection(int level, float low, float high) {
		return true;
	}
	virtual double GetAngularSelectionLow() { return 0; }
	virtual double GetAngularSelectionHigh() { return 180; }

	/** 
	 *@brief Set depth selection for loading of photonics tables.
	 *       This is advisory only, and does nothing by default.
	 *@param level       Photonics level affected
	 *@param low         Minimum depth (m)
	 *@param high        Maximum depth (m)
	 */
	virtual bool SetDepthSelection(int level, float low, float high) {
		return true;
	}
	virtual double GetDepthSelectionLow() { return -DBL_MAX; }
	virtual double GetDepthSelectionHigh() { return DBL_MAX; }

	/**
	 *@brief Get maximum l in level 1 photonics tables
	 *@return maximum l
	 */
	virtual double GetLmaxLevel1() { return DBL_MAX; }
	/**
	*@brief Get maximum l in level 2 photonics tables
	*@return maximum l
	*/
	virtual double GetLmaxLevel2() { return DBL_MAX; }

	/**
	 *@brief return photonics geometry parameters (+ emission point
	 * parameters) as doubles
	 */
	void GetPhotonicsInput(double& zenith, double& rho, double& phiOM,
	    double& l, double& z, double& eOffset, double& eDistance);

	/**
	 *@brief Calculate photonics input variables (floats) and check if
	 *       outside table range in l coordinate
	 *@param[in] CheckLmax      Bool to enable checking lmax
	 *@param[in] xOM,yOM,zOM    OM coordinates
	 *@param[in] source         Source information
	 *@return False if OM is outside table range for given source, true
	 *        otherwise.
	 */
	bool CalculatePhotonicsInput(bool CheckLmax, double xOM, double yOM,
	    double zOM, const PhotonicsSource &source);

	std::vector<double> GetTableCoordinates() const {
		std::vector<double> vec;
		for (int i = 0; i < 5; i++)
			vec.push_back(coordPxC_[i]);
		return vec;
	}
	geo_type GetGeometry() const { return geometry_; }
	parity_type GetParity() const { return parity_; }

protected:
	// Constants (inherited by I3DummyPhotonicsService). These are
	// usually the commented values.
	double b_;//= 3.3E-4; // as in dE/dx=-a-bE
	double nPhase_;//=1.3195;
	double nGroup_;//=1.35634;
	double cVacuum_;//=0.299792458;
	double tanCh_; //= sqrt(nPhase_*nPhase_ - 1.0); 
	double sinCh_; //= tanCh_ / nPhase_;

	geo_type geometry_; // geometry type of table
	parity_type parity_; // parity of the table: does azimuth zero point
	                     // up or down?
	double coordPxC_[5]; // Photonics coordinates
	double zStopPxC_, lStopPxC_; // Finite track coords

	double l_,rho_;
	double cosAzi_;
	double r_,cosZenith_;

	double emissionPointOffset_; // Distance from source position to nominal
	                             // Cherenkov emission point
	double emissionPointDistance_; // Distance from nominal Cherenkov
	                               // emission point to OM

	// OM coordinates 
	double xOM_,yOM_,zOM_;
};

I3_POINTER_TYPEDEFS(I3PhotonicsService);
I3_DEFAULT_NAME(I3PhotonicsService);

#endif
