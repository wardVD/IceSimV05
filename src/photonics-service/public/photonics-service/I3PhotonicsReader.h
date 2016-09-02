/**
 * @file  I3PhotonicsCommonReader .h
 * @brief This header file contains defnitions needed for reading both L1 and L2 photonics tables
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @author RCB
 * @author KWW
 * @version $Id: I3PhotonicsReader.h 142977 2016-03-11 05:04:44Z nega $
 * @date $Date: 2016-03-10 23:04:44 -0600 (Thu, 10 Mar 2016) $
 * 
 * The following is a list of file from photonic-1.73 which I 
 * copied verbatium to create this header file:
 *   \li photonics-1.73/lib/photonics.h 
 *
 * For documentation of photonics files see
 * http://photonics.sourceforge.net/index.php
 * 
 * @copyright Copyright (c) The IceCube Collaboration
 */

#ifndef I3_PHOTONICS_READER_H
#define I3_PHOTONICS_READER_H

#include <photonics-service/geo_type.h>

/* Some general definitions */
#define MAX_TABLES 10000
#define photonics_reader_InF 0x7F800000
#define EPS        1.0e-6        /**< small value to avoid round off errors */
#define MAXLINE    1024
/** 
 * @brief  Number of dimensions used 
 */
#define VARS 6
/**
 * @brief 2**VARS 
 */
#define TWO_POWER_VARS 1<<6    
#define PHOTONICS_VERSION "I3PhotonicsReader $Rev: 142977 $"
#define HEADER_TYPE_LENGTH 328

/* Some mathematical operations  */
#define sqr(x) ((x)*(x))
#define Rad(x) ((x)*0.01745329251994)
#define Deg(x) ((x)*57.29577951308232)

/** 
 * @brief Namespace for putting functions used by both the L1 and L2 photonics readers
 */
namespace photonics {
  SET_LOGGER("photonics")

  typedef int8_t ph_tiny ;
  typedef int16_t ph_short ;
  typedef int32_t ph_int ;
  
  typedef int32_t ph_size_t ;

  /* this is explicitly tested in isSupportedEnvironment() */
  typedef long long ph_long ; 

  /* Various enumeration types */
  typedef enum{
    ISOTROPIC = 1,
    CHERENKOV = 2,
    BEAMED    = 3,
    COSTHETA  = 4,
    EM_SHOWER = 5,
    HAD_SHOWER= 6,
    FLASHER_LED=7,
    FLASHER_BOARD=8,
    FLASHER_BOARD_WITH_HOLE_ICE=9,
    STANDARD_CANDLE_WITH_HOLE_ICE=10,
    STANDARD_CANDLE_DOWNWARDS_WITH_HOLE_ICE=11
  } source_type;
  
  typedef enum{
    LINEAR    = 1,
    QUADRATIC = 2
  } scale_type;

  typedef enum{
    ORIGIN       = 1,
    POCA         = 2,
    DIRECT       = 3
  } time_ref_type;

  typedef struct {
    char File_Format_Label[4];
    ph_tiny File_Format_Revision; /**< currently 0 */
    ph_tiny dummy0;               /**< future high byte for revision */
    ph_tiny bitsys;
    ph_tiny endian;/**< endian = 1 is little, 2 is big */
    ph_tiny level;
    ph_tiny dummy2;
    ph_tiny dummy3;
    ph_tiny dummy4;
    char Photonics_Version[32];
    char dummy_padding[40]; /**< not used */
  } MetaHead_type;

  /** 
   * @brief Header/setup information 
   */
  typedef struct {
    MetaHead_type MetaHead;
    char dummy_padding[16]; /**< not used */
    float sphere_frac  ;    /**< fraction of all scatters that will be with <cos>=0*/
    float impsampl_Lf  ;    /**< Importance sampling factor for exp() steps length */
    float impsampl_Tf  ;    /**< Importance sampling factor for HG scattering      */
    float dummy_l2padd;     /**< not used [in level1, gauss factor in level2]      */
    float ref_np;           /**< phase refractive index used for true <--> residual calc*/
    float ref_ng;           /**< phase refractive index used for true <--> residual calc*/
    bool record_errors;     /**< Statistical errors are tracked */
    source_type source;     /**< Emission source type */
    bool extended_source;   /**< Simulaton of extended shower sources */
    float step;             /**< Step for photon tracking (m) */
    float e;                /**< Shower energy for shower sources in (GeV) */
    float volume;           /**< Radial size for volume source (m) */
    float angle;            /**< Source axis zenith angle */
    float source_rz[2];     /**< Source location wrt active volume center */
    geo_type geo;           /**< Geometry type */
    ph_int n[VARS];         /**< Number of bins in each dimension */
    float limits[VARS][2];  /**< Recordable limits for each dimension */
    float maxes[VARS][2];   /**< Active limits for each dimension */
    float depth;            /**< depth of active volume center wrt ice table */
    scale_type d_scale;     /**< distance scaling parameter */
    scale_type t_scale;     /**< time scaling parameter */
    float lambda;           /**< light wavelength, or negative; spectral "index" */
    ph_int efficiency;      /**< Efficinency calibrations applied */
    ph_long n_photon;       /**< number of photons to generate */
    ph_long n_entries;      /**< total number of MC entries                */
    ph_int refraction_mode; /**< if we do refraction between layers (0 == no) */
    ph_int User_ieC;        /**< not used */
  } Header_type;
  
  typedef struct{
    double val;        /**< Edge location in appropriate units */
    double **area;
    /**< Area of an edge surface as function of other 2 coordinates */
    double sine;       /**< Sine of a value (for angular bins) */
  } Edge_type;

  typedef struct{
    double cosine;    /**< Cosine of mid point value */
    double sine;      /**< Sine of mid point value */
  } Mid_type;
  
  typedef struct {     /**< region info */
    Edge_type high;    /**< High edge info */
    Edge_type low;     /**< Low edge info */
    Mid_type mid;      /**< Mid value info, used for angular bins */
  } Region_type;
  
  /**
   * @brief Overall geometry 
   */
  typedef struct {
    ph_int n[3];            /**< Number of geometrical bins per dimension */
    bool buffer[3][2];      /**< Indicator whether active geometry extends
			     *   beyond recordable region */
    ph_int dummy_padding;   /**< not used */
    Region_type *dim[3];    /**< Dimensional regions */
    double d[VARS];         /**< Dimensional steps */
    double *bracket[3];     /**< Arrays used for fast binary search of location */
    double *middle[3];      /**< Array used for fast interpolation lookup */
    double *timing;         /**< Time binning array */
    double *timing_middle;  /**< Centers of timing bins for fast interpolation lookup
			     *   some arrays handy for optimizations */
  } Geo_type;
  
  double next_boundary_sqr(const double,const double);
  
  /**
   * @brief Copies header info
   */
  void copy_header(const Header_type*,Header_type*);
  
  /**
   * @brief true if the examined meta head is little endian
   */
  bool checkMetaHeadLittle(MetaHead_type* MetaHead);

  /**
   * @brief true if the examined meta head is written on 32 bit machine 
   */
  bool checkMetaHead32bit(MetaHead_type* MetaHead);

  /**
   * @brief true if the examined meta head is written on 32 bit machine
   */
  bool checkMetaHead(MetaHead_type* MetaHead);

  /**
   * @brief Convert a residual time from one group index of refraction to another
   *
   * @returns residual time, as if it would have been calculated
   * with group refractive ind_new.
   */
  double residual_convert(double ind_old,       /**< refractive index for input timedelay */
			  double ind_new,       /**< refractive index to convert timedelay into */
			  double time_old,      /**< residual time in ind_old sens */
			  double rho,           /**< perpendicular distance track origin--OM */
			  double l,             /**< longitudinal distance track origin--OM */
			  double rev_sin_ang,   /**< 1/(sin(cherenkov_angle)) */
			  time_ref_type REFTYPE /**< type of residual calculation */
			  );
  
  /** 
   * @brief Binary search of an integrated probability distribution. 
   */
  int binary(const float*,int,double);
  
  /**
   * @brief Modified hunt routine from NR, for seeded binary search
   *
   * IMPORTANT: If array is sorted from low to high, bins are defined
   * as [low_edge,high_edge). If sorted from high to low, then bins
   * are (low_edge,high_edge]. E.g.
   * array={-1,0,1}  find with key=0 returns 1, i.e. 0 is in [0,1)
   * array={1,0,-1}  find with key=0 returns 0, i.e. 0 is in (-1,0]
   *
   * @note n is the highest allowed index. that is, (nbins-1)
   *  this function does not work when n=0;
   */
  int find(const double*,int,double,int);
  
  /**
   * @brief Functions for determination of supported environment 
   */
  bool isLittleEndian(void) ;

  bool isSupportedEnvironment(void);

  /** 
   * @brief 32 bit byte swapping. bytes ABCD -> DCBA.
   * 
   * Operates directly on p_swapit_ 
   */
  void px_swap32 (void * p_swapit_);

  /**
   * @brief 64 bit byte swapping. bytes ABCD EFGH -> DCBA HGFE
   *
   * Because the type is actually 'long long'
   * Operates directly on p_swapit_ 
   */
  void px_swap64 (void * p_swapit_);
  
}//namespace photonics

#endif //I3_PHOTONICS_READER_H


