#ifndef I3PHOTONICSTABLESERVICE_H
#define I3PHOTONICSTABLESERVICE_H
/**
 *@file
 *@brief I3PhotonicsTableService
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

#define BIGNUMBER 10000

#ifdef PHOTONICS_SERVICE_ROOT
#include "TObject.h" //Needed for ClassDef and ClassImp macros
#define PHOTONICS_SERVICE_NO_I3
#endif

#include <string>
#include <cmath>
#include <vector>

#include <photonics-service/I3PhotonicsService.h>
#include <photonics-service/I3PhotonicsL1Reader.h>
#include <photonics-service/I3PhotonicsL2Reader.h>
#ifdef photonics_defined_level2_lmax
#ifdef photonics_defined_level1_lmax
/**
 * I3PhotonicsTableService provides access to maximum l-value in tables, and
 * uses them internally, to avoid querying photonics outside table range (gives 
 * no light anyway). This can only be done if photonics functions 
 * exist to the access table range. We use flags from photonicsCPPio.h to 
 * determine whether current photonics version contains such functions. 
 * If not, no check is made in I3PhotonicsTableService, and a very large
 * ("infinite") table range is returned.
 */
#define LMAX_CHECK
#endif
#endif

/**
 *@brief Service interfacing to the photonics table access functions
 */
class I3PhotonicsTableService : public I3PhotonicsService
{
public:
/** 
 *@brief Constructor - initialise data members and create photonics level2 object
 */
  I3PhotonicsTableService();
  I3PhotonicsTableService(const std::string& tablesDirectory,
    const std::string& driverFileDirectory, const std::string& level1DriverFile,
    const std::string& level2DriverFile, int interpolMode, int verbosity,
    double angularSelectLow, double angularSelectHigh, double zSelectLow,
    double zSelectHigh, int photonicsTableSelection, bool isPhotorecTable);
/**
 * Destructor - unloads photonics tables and deletes photonics object
 */
  virtual ~I3PhotonicsTableService();

/** 
 *@brief Select photonics interpolation mode
 *@param mode       Interpolation mode
 *
 * A bit mask where each bit corresponds to interpolation in
 * one of the dimensions of the tables:
 * 1     l
 * 2     rho
 * 4     phi
 * 8     nothing (time, always on)
 * 16    depth
 * 32    zenith
 * 128   choose tables to select those giving the best desription
 *       of closest approach          
 */
  void SetInterpolationMode(int mode);

/** 
 *@brief Set photonics verbosity (deprecated)
 *@param level       Photonics level affected
 *@param verbosity   Degree of verbosity 
 */
  virtual bool SetPhotonicsVerbosity(int level, int verbosity);

/** 
 *@brief Set angular selection for loading of photonics tables
 *@param level       Photonics level affected
 *@param low         Minimum zenith angle (degrees)
 *@param high        Maximum zenith angle (degrees)
 */
  virtual bool SetAngularSelection(int level, float low, float high);

/** 
 *@brief Get lower limit of angular selection used for loading of photonics tables
 *@return Minimum zenith angle (degrees)
 */
  virtual double GetAngularSelectionLow() { return angularSelectLow_; }

/** 
 *@brief Get upper limit of angular selection used for loading of photonics tables
 *@return Maximum zenith angle (degrees)
 */
  virtual double GetAngularSelectionHigh() { return angularSelectHigh_; }


/** 
 *@brief Set depth selection for loading of photonics tables
 *@param level       Photonics level affected
 *@param low         Minimum depth (m)
 *@param high        Maximum depth (m)
 */
  virtual bool SetDepthSelection(int level, float low, float high);


/** 
 *@brief Get lower limit of z selection used for loading of photonics tables
 *@return Minimum zenith angle (degrees)
 */
  virtual double GetDepthSelectionLow() { return depthSelectLow_; }

/** 
 *@brief Get upper limit of z selection used for loading of photonics tables
 *@return Minimum zenith angle (degrees)
 */
  virtual double GetDepthSelectionHigh() { return depthSelectHigh_; }

/** 
 *@brief Load photonics tables for specified level
 *
 *Tables must reside below "tables" subdirectory
 *(or link) in cwd. I3PhotonicsServiceFactory changes working directory to achieve this
 *before calling LoadTables.
 *
 *@param level        Photonics level
 *@param driverFile   Name of driver file 
 *@param topLevel     Name of top level table directory (containing "tables" subdirectory) 
 */
  bool LoadTables(int level, std::string driverFile, std::string topLevel );

/** 
 *@brief Clears the photonics table memory. Not needed for normal operation
 *@param level      photonics level for tables to be cleared
 */
  void ClearTables(int level);

/** 
 *@brief Select OM coordinates
 *@param xOM,yOM,zOM        Coordinates in metres
 */
  virtual void SelectModuleCoordinates(double xOM,double yOM,double zOM);

/** 
 *@brief Select light source
 * Set source properties, calculate geometrical quantities based on 
 * OM previously selected by SelectModuleCoordinates(), call photonics, and return result.
 * 
 *@param meanPEs                  Mean number of PEs
 *@param emissionPointDistance    Distance between nominal cherenkov emission point and OM
 *@param geoTime                  Nominal arrival time for cherenkov light relative 
 *                                to time of track starting point
 *@param source                   Source parameters (type PhotonicsSource)
 */
  virtual void SelectSource( 
		    double &meanPEs,
		    double &emissionPointDistance,
		    double &geoTime,
		    PhotonicsSource const &source);
		
/** 
 *@brief Get photon arrival time 
 * Draws a photon arrival time (in ns) using table pointers set up 
 * by previous call to SelectSource.
 *@param[in]  random     Random number 
 *@param[out] timeDelay  Photon arrival time relative to direct light time
 */
  virtual void GetTimeDelay(double random, double &timeDelay);

/**
 *@brief Get residual times for hit (w.r.t direct propagation from nominal 
 *       emission point)
 *@param[in]  random      Random number generator
 *@param[out] timeDelay   Residual time
 *@param[in]  n		  Number of time delays to sample
 */
  virtual void GetTimeDelays(I3RandomServicePtr random, double *timeDelays,
      int n);

/** 
 *@brief Get photon probability density in time
 * 
 * Get time probability density for single photon arrival time (in ns^-1),
 * based on the table pointers set up in latest call to SelectSource().
 * 
 *@param[out] density    Probability density (ns^-1)
 *@param[in]  timeDelay  Time offset from direct light arrival time
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

  /**
   *@brief Get photorec information 
   *   
   * Gets photorec information based on OM coordinates specified by previous
   * call to SelectOMCoordinates, delay time, and photonics source information
   * in PhotonicsSource struct. 
   *
   *@param[out] yield        Light yield as given by table.
   *                         For level 2 cascade line sources this is usually
   *                         normalised to an energy loss of 1 GeV/m
   *@param[out] probDensity  Probability density in time (ns^-1)
   *@param[in] delay         Delay time
   *@param[in] source        Photonics source
   *@return  Success or failure of photonics table access
   */
  virtual bool GetPhotorecInfo(
		       double &yield,
		       double &probDensity,
		       double delay,
		       PhotonicsSource const &source);

  /**
   *@brief Get several photorec probability densities
   *
   *  Gets mean number of PEs and the probability density for a specified set
   *  of times. Uses OM coordinates specified by previous, call to 
   *  SelectOMCoordinates and photonics source information in PhotonicsSource struct.
   *
   *@param[out] yield          Light yield as given by table.
   *                           For level 2 cascade line sources this is usually
   *                           normalised to an energy loss of 1 GeV/m
   *@param[out] probDensities  Vector with probability densities (in ns^-1)
   *@param[in]  delays         Vector of delay times to give densities for
   *@param[in]  source         Photonics source
   *@return  Success or failure of photonics table access
   */
  virtual bool GetPhotorecDensities(
		       double &yield,
		       std::vector<double> &probDensities,
		       std::vector<double> delays,
		       PhotonicsSource const &source);

  /**
   *@brief Get maximum l in level 1 photonics tables
   *@return maximum l
   */
  virtual double GetLmaxLevel1();
  /**
   *@brief Get maximum l in level 2 photonics tables
   *@return maximum l
   */
  virtual double GetLmaxLevel2();

 private:
/** 
 *@brief Find energy at nominal point of cherenkov emission 
 *  Adjust a in dE/dx=-a-bE such that E=0 for x=length and 
 *  return energy at nominal point for cherenkov emission or zero if
 *  point beyond track end.
 *@param E       Energy at start of track
 *@param length  Track length
 */
  float EmissionEnergy(double E,double length);

/** 
 *@brief Load level 1 photonics tables
 *Paths must be set by LoadTables() before call to this function
 */
  bool LoadLevel1Tables();
  
/** 
 *@brief Load level 2 photonics tables
 *Paths must be set by LoadTables() before call to this function
 */
  bool LoadLevel2Tables();

  /** @brief Pointer to photonics object for level 1 tables 
   */
  I3PhotonicsL1Reader* l1_photonics_;

  /** @brief Pointer to photonics object for level 2 tables 
   */
  I3PhotonicsL2Reader* l2_photonics_;

  //Table initialisation variables
  int interpolationMode_;
  bool level1TablesLoaded_,level2TablesLoaded_;
  std::string level1DriverFileName_,level2DriverFileName_;
  std::string photonicsTopLevelDirectory_;

  //Photonics tables l-range
  double lmaxLevel1_,lmaxLevel2_;  

  //Photonics photon yield
  float rawYield_;

  //Table access variables
  int level_;  //level of active tables
  int type_;   //type of active tables if level 1

  //Table access variables (returned by photonics)
  int tableSetId_,tableId_,stopTableId_,lBin_,lBinStop_,rhoBin_;
  int rhoBinStop_,phiBin_,phiBinStop_;	


  double angularSelectLow_;
  double angularSelectHigh_;
  double depthSelectLow_;
  double depthSelectHigh_;
  bool isPhotorecTable_;

#ifdef PHOTONICS_SERVICE_ROOT
    ///ROOT class definition macro
    ClassDef(I3PhotonicsTableService,2); ///I3PhotonicsTableService - access photonics tables
#endif
};

#ifndef PHOTONICS_SERVICE_NO_I3
I3_POINTER_TYPEDEFS(I3PhotonicsTableService);
#endif

//Define shared pointers

#endif
