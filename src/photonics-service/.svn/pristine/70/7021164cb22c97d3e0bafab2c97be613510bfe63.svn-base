/**
 * @file I3PhotonicsL2Reader.h
 * @brief Class for Reading Photonics Level2 files for use with photonics-service
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @version $Id$
 * @date $Date$
 *
 * This file contains a cleaned up version of:
 *   \li photonics-1.73/level2/level2_table_io.h
 *   \li photonics-1.73/level2/photonicsCPPio.h
 *   \li photonics-1.73/level2/level2.h
 *
 * This file contains user functions for loading and reading
 * level2 photonics tables. Level2 historically ment muons, but
 * it is now enhanced to treat any kind of light source.
 *
 * For documentation of photonics files see
 * http://photonics.sourceforge.net/index.php
 *
 * @copyright Copyright (c) The IceCube Collaboration
 */

#ifndef I3_PHOTONICS_L2_READER_H
#define I3_PHOTONICS_L2_READER_H

#include <photonics-service/I3PhotonicsReader.h>

#define L2VARS 4
/** 2**(L2VARS-1)    8 means: rho,l,phi
    depth, theta and time is done on the side */
#define  L2_INTERP_BINS      8

/**
 * @brief Class for Reading Photonics Level2 files for use with photonics-service
 *
 * This class is a modified version of photonics_cppio which was a class provided by
 * photonics for accessing level2 photonics tables. It used weird a preprocessor
 * directive scheme to recycle straight c files into the class. I think this was
 * created so that the globals in the original c file would now be class members.
 *
 * All the preprocessor stuff was removed so it is now a straightforward c++ class.
 * Necessary functions from level2/level2_reader.c were also included as private
 * static member functions.
 *
 */
class I3PhotonicsL2Reader {

  SET_LOGGER("I3PhotonicsL2Reader")

 public:

  /**
   * @brief Enumeration for type of tableset
   */
  typedef enum {
    START = 0,
    STOP = 1
  } tableset_type;

  /**
   * @brief this is not written to disk, format therefore changable
   */
  struct Level2_geo_type{
    photonics::ph_int n[L2VARS];         /**< Number of bins in each dimension */
    double d[L2VARS];         /**< Bin size for all dimensions */
    double *edge[L2VARS];     /**< Bin edges for fast binary search of table location */
    double *times;            /**< representing point for each time bin. */
  };

  struct Level2_read_type{
    float val[L2VARS];    /**< Physical coords of interest */
    int bin[L2VARS];      /**< Corresponding bin for physical coord */
    int n[L2VARS];        /**< Number of bins in each dimesnion */
    unsigned short interp_mask; /**< Bin interpolation mask */
    float lowEdge[L2VARS];/**< Physical value of lower edge of a bin */
    photonics::ph_int dummy_A;       /**< NOT USED */
    float binSize[L2VARS];/**< Physical size of a bin */
    photonics::ph_int dummy_B;       /**< NOT USED */
  };

  struct Level2_interp_type{
    double f[L2_INTERP_BINS]; /**< Interpolation weight */
    int bin[L2VARS][2];    /**< Bins used in linear interpolation */
    unsigned short mask;   /**< Interpolation mask */
    photonics::ph_short dummy_A;
    photonics::ph_int dummy_B;
  };

  struct Level2_data_type{
    float *cont;                          /**< Pointer to data table */
    photonics::ph_size_t offset[L2VARS];  /**< Array encoding size of each subtable */
  };

  struct Amasim_Level2_Table_type {
    photonics::MetaHead_type MetaHead_data;
    char dummy_padding[256];
    float zLow,zHigh;
    float aLow,aHigh;
    float dummy_padding2;
    Level2_data_type abs,prob;
  };

  /**
   * @brief Level2 table enumerations
   */
  enum photonics_l2_table_flavor{
    ABS  = 1,
    DIFF = 2,
    PROB = 3
  };

  // note! also check the byteswapping function. they must match!
  struct Level2_header_type{          /* the dummy_ types are there to make the structure
					 as similar as possible to the level1 header. This header
					 is is written to disk with the tables (as in level1). */
    photonics::MetaHead_type MetaHead;
    char dummy_padding[28]; /**< not used */
    float gausconv;         /**< convolved with gausian of this width */
    float ref_np;           /**< phase refractive index used for true <--> residual calc*/
    float ref_ng;           /**< phase refractive index used for true <--> residual calc*/
    bool dummy_a      ;     /**<                    NOT USED                     */
    photonics::source_type source;     /**< Emission source type                            */
    bool dummy_b;           /**<                    NOT USED                     */
    float dummy_c1;         /**< Step for photon tracking (m)                    */
    float e;                /**< Shower energy for shower sources in (GeV)       */
    float dummy_c2;         /**<                    NOT USED                     */
    float theta;            /**< Source axis zenith angle                        */
    float dummy_c3[2];      /**<                    NOT USED                     */
    geo_type dummy_d;       /**<                    NOT USED                     */
    photonics::ph_int n[VARS];         /**< Number of bins in each dimension                */
    float range[L2VARS][2]; /**< Recordable limits for each dimension            */
    float dummy_range[VARS-L2VARS][2]; /**<         NOT USED                     */
    float dummy_e[VARS][2]; /**<                    NOT USED                     */
    float z0;               /**< depth of active volume center wrt ice table     */
    photonics::scale_type d_scale;     /**< distance scaling parameter                      */
    photonics::scale_type t_scale;     /**< time scaling parameter                          */
    float lambda;           /**< light wavelength, or spectral "index"           */
    photonics::ph_int efficiency;      /**< Efficinency calibrations applied                */
    photonics::ph_int dummy_e2;        /**< NOT USED                                        */
    photonics_l2_table_flavor type;        /**< Table type                                      */
    photonics::ph_long n_dummy_f1;     /**<                    NOT USED                     */
    photonics::scale_type l_scale;     /**< lengthwize scaling parameter                    */
    photonics::time_ref_type tref;     /**< Timing reference                                */
  } ;

  struct Tableset_level2_type {
    Amasim_Level2_Table_type *table;
    Level2_geo_type *geo;
    Level2_header_type *header;
    tableset_type type;

    unsigned long memused;
    unsigned long memforset;

    double last_probcalc;
    double delay_probcalc; /**< time used by probcalc, must be in internal format */

    int photorec_mode;     /**< option for the photorec functionallity */

    int pair_id;
    int *n_tables;
    double z_src_max; /**< highest valid z_src */
    double z_src_min; /**< highest valid z_src */

    double a_src_max; /**< highest valid a_src */
    double a_src_min; /**< highest valid a_src */

    double ref_dist;  /**< reference distance, distance of influence in l*/
    double ref_dist_fraction; /**< tiny fraction of ref_dist             */
    int     n_z_layers; /**< number of z layers in this tableset */

    double inv_sin_ch;  /**< inverted sin cherenkov angle */

    int    *z_layer_index;   /**< indeces to first table of each z layer */
    int    *z_layer_n_thetas; /**< number of angles in each z layer  */
    double *z_layer_centers; /**< central z value in each z layer  */

  };

  /**
   * @brief behaviour control
   */
  struct pref_level2_t {
    int zl_matching;   /**< Try to find best z/l table combination */
    int zedgelimiting; /**< Use lowest/highest z directly if track below/above */
    int greetingdone;
    double external_ref_ng ; /**< the interface defines residual time by using this */
  } ;

  /**
   * @brief A level2 tableset container contains several level2 table sets
   *
   * mytablesets_levbel2 is a static global which is intended
   * to handle any number of tablesets.
   *
   * The struct members are:
   * \li current     : index of current tableset
   * \li n_tablesets : total number of tablesets initialized
   * \li tableset    : an array of tablesets [0,...,n_tablesets-1]
   */
  struct Tableset_level2_container_type {
    int current;
    int n_tablesets;
    double request_angle_limit_low ;
    double request_angle_limit_high;
    double request_depth_limit_low ;
    double request_depth_limit_high;
    Tableset_level2_type *tableset;
  } ;

 private:

  const char *infstop_mark ;
  const char *startinf_mark;
  const char *photorec_mark;
  struct Amasim_Level2_Table_type *table;
  int *n_tables ;
  struct Level2_geo_type (*geo);
  struct pref_level2_t pref_level2;
  struct Tableset_level2_container_type mytablesets_level2;

 public:

  I3PhotonicsL2Reader();

  ~I3PhotonicsL2Reader();

  /**
   * @brief Free all memory used by level2 table sets
   */
  void free_tablesets_level2();

  /**
   * @brief Initialize a new tableset
   *
   * This will allocate memory for a new tableset, table info and tables.
   *
   * @note That actual table content is allocated in routines called by
   * I3PhotonicsL2Reader::load_level2_tables and not here. Because of
   * this don't call free_tablesets_level2::free_tablesets_level2 before
   * I3PhotonicsL2Reader::load_level2_tables unless you want a runtime
   * error.
   *
   * @note Also note that tables allways will get the size ::MAX_TABLES. This is to
   * make the code backwardscompatible, it is possible to implement this
   * dynamic but then more changes to existing photonics routines.
   *
   * @returns index of newly initialized tableset
   */
  int init_tableset_level2();

  /**
   * @brief Swap active tableset
   *
   * This will change the current tableset index
   * I3PhotonicsL2Reader::mytablesets_level2 and set the
   * pointers I3PhotonicsL2Reader::table and I3PhotonicsL2Reader::geo and
   * I3PhotonicsL2Reader::n_tables to the corresponding data in
   * mytablesets_level2.tableset[mytablesets_level2.current]
   *
   * @returns If index is larger then the number of tablesets -1
   * will be returned, otherwise index will be returned.
   */
  int swap_tableset_level2( int index );

  /**
   * @brief Routine that reads in all tables specified in driver file
   *
   * @returns 1 on success 0 or -1 on error.
   */
  int load_level2_tables(const char *driver_file);

  /**
   * @brief Table lookup for finite @a line-like sources such as muons
   *
   * The user specifies any finite (starting and or stopping) track
   * The routine figures out how to best
   * fulfill the requrest, given the already loaded tables, and returns
   * the amplitude and handles for time/prob querries.
   *
   * We follow the photonics convention to have coordinates as
   * floats. Amplitudes, times, probabilities etc as doubles.
   *
   *   @note This function \a must only be used with tables for line like
   *   sources. (The user must know that)
   *   For point like tables (such as LEDs and pointlike cascades)
   *   you should instead use get_level2_general_pointlike_photon_density()
   *
   * The output ids and bins are used with:
   *                         \li I3PhotonicsL2Reader::get_level2_general_delay
   *                         \li I3PhotonicsL2Reader::get_level2_general_delay_prob
   */
  double get_level2_general_photon_density(
    float angle_track,      /**< [IN]   track theta [degrees]                       */
    float rho,		    /**< [IN]   track rho                                   */
    float phi,		    /**< [IN]   track phi   [radians]                       */
    float l_start,	    /**< [IN]   start                                       */
    float l_stop,	    /**< [IN]                                               */
    float z_start,          /**< [IN]   track start depth                           */
    float z_stop,           /**< [IN]   track stop depth                            */
    int interp_mask,	    /**< [IN]   photonics interpolation mask                */
    int *tableset_id,       /**< [OUT]  'A' bins refer to the table with higher amp */
    int *table_id_A,        /**< [OUT]  contains depth/theta information            */
    int *table_id_B,	    /**< [OUT]                                              */
    int *l_bin_A,	    /**< [OUT]                                              */
    int *l_bin_B,	    /**< [OUT]                                              */
    int *rho_bin_A,	    /**< [OUT]                                              */
    int *rho_bin_B,	    /**< [OUT]                                              */
    int *phi_bin_A,	    /**< [OUT]                                              */
    int *phi_bin_B 	    /**< [OUT]                                              */
    );

  /**
   * @brief This is a general funtion for level2 delay queries.
   *
   * Works with the handles given by get_level2_general_photon_density().
   * This function is identical in form to double get_level2_general_delay_prob().
   *
   * This function is a wrapper for
   *      \li I3PhotonicsL2Reader::get_level2_photon_delay
   *      \li I3PhotonicsL2Reader::get_finite_level2_photon_delay
   * @returns The function itself returns delay
   * -1 if question not relevant (no amp)
   * @note this assumes that -1 is not a valid
   * respons. This is _currently_ fine.
   */
  double get_level2_general_delay(
    double r,               /**< [IN]   the random number                           */
    int tableset_id,        /**< [IN]                                               */
    int table_id_A,         /**< [IN]                                               */
    int table_id_B,	    /**< [IN]                                               */
    int l_bin_A,	    /**< [IN]                                               */
    int l_bin_B,	    /**< [IN]                                               */
    int rho_bin_A,	    /**< [IN]                                               */
    int rho_bin_B,	    /**< [IN]                                               */
    int phi_bin_A,	    /**< [IN]                                               */
    int phi_bin_B 	    /**< [IN]                                               */
    );

  /**
   * @brief This is a general funtion for level2 delay queries.
   *
   * Works with the handles given by get_level2_general_photon_density().
   * This function is identical in form to double get_level2_general_delay().
   */
  double get_level2_general_delay_prob(
    double delay,           /**< [IN]   the time delay                              */
    int tableset_id,        /**< [IN]                                               */
    int table_id_A,         /**< [IN]                                               */
    int table_id_B,	    /**< [IN]                                               */
    int l_bin_A,	    /**< [IN]                                               */
    int l_bin_B,	    /**< [IN]                                               */
    int rho_bin_A,	    /**< [IN]                                               */
    int rho_bin_B,	    /**< [IN]                                               */
    int phi_bin_A,	    /**< [IN]                                               */
    int phi_bin_B 	    /**< [IN]                                               */
    );

  /**
   * @brief Reader for the 'photorec' reconstruction pdf framework
   *
   * Supports multi linear interpolation in all dimentions, of both
   * abs and prob.
   *
   * This function is used for reconstruction with tables of point like
   * sources (like showers), or degenerate (only one l-bin) muons.
   *
   * For finite line sources, use get_level2_photorec_finite()
   *
   * Most of the new code is implemented elsewhere in this file, while
   * this function is a selector and wrapper to the new functionallity.
   *
   * When the number of l-bins of the table is 1, the functionallity
   * previously implemented in get_level2_photorec_degenerate is
   * provided.
   *
   * @see get_level2_photorec_finite
   *
   * @returns true on success
   */
  int get_level2_photorec(
    float angle_track,    /**< [IN]   track theta [degrees]                     */
    float rho,            /**< [IN]   track rho                                 */
    float phi,            /**< [IN}   track phi   [radians]                     */
    float l_start,        /**< [IN]   start                                     */
    float z_start,        /**< [IN]   track start depth                         */
    float delay,          /**< [IN]   residual time                             */
    double *amp,          /**< [OUT]  amplitude (poisson mean)                  */
    double *prob,         /**< [OUT]  differential prob: dp/dt integrates to 1  */
    int interp_mask,      /**< [IN]   photonics interpolation mask              */
    double energy         /**< [IN]   vertex energy (GeV). 0 means no scaling.
                                       the particle type is read from the table  */
    ) const;

  /**
   * @brief Reader for the 'photorec' reconstruction pdf framework
   *
   * Supports multi linear interpolation in all dimentions, of both
   * abs and prob.
   *
   * The intention is to provide an interface to table reading with
   * full interpolation of abs and pdf for any line like (muonic) request
   *
   * This is to support reconstruction where
   * the track start and stop positions are parameters to be fitted.
   *
   * Most of the new code is implemented elsewhere in this file, while
   * this function is a selector and wrapper to the new functionallity.
   *
   * @see get_level2_photorec
   *
   * @returns true on success
   */
  int get_level2_photorec_finite(
    float angle_track,    /**< [IN]  track theta [degrees] */
    float rho,            /**< [IN]  track rho                                 */
    float phi,            /**< [IN]  track phi   [radians]                     */
    float l_start,        /**< [IN]  start                                     */
    float l_stop,         /**< [IN]                                            */
    float z_start,        /**< [IN]  track start depth                         */
    float z_stop,         /**< [IN]  track stop depth                          */
    float delay,          /**< [IN]  residual time                             */
    double *amp,          /**< [OUT] amplitude (poisson mean)                  */
    double *prob,         /**< [OUT] differential prob: dp/dt  unit: [1/ns]    */
    int interp_mask,      /**< [IN]  photonics interpolation mask              */
    double energy         /**< [IN]  vertex energy (GeV). 0 means no scaling
                                     the particle type is read from the table  */
    );


  /***************************************************************************/
  /** Block of functions used to set preferences and query tables status:   **/
  /***************************************************************************/

  void set_level2_angular_interval(float alow,float ahigh);
  void get_level2_angular_interval(float *alow,float *ahigh)const;

  void set_level2_depth_interval(float zlow,float zhigh);
  void get_level2_depth_interval(float *zlow,float *zhigh)const;

  /**
   * @brief Return the amount of memory (bytes) used for tableset in
   * photonics level2
   */
  void get_level2_memory_usage(unsigned long *mem) const;

  /**
   * @brief returns estimate of number of bytes needed to load an interval of
   * given size, assuming equidistant theta binning.
   *
   * The results should be exact as long as the first possible angle
   * given by the driver file is zero, or a multiple of angular bin size
   */
  void get_level2_angular_memory_requirement(float alow,float ahigh,unsigned long *mem)const;

  bool get_level2_sourcetype(int *sourcetype, int *time_reference )const;

  double get_level2_lmax()const;

  /**
   * @brief Returns mean photon density for specified source depth, angle, and type
   */
  double get_level2_photon_density(float,float,float,float,float,int,
				   int*,int*,int*,int*) const;

 private:

  /**
   * @brief Returns mean photon density for specified source depth, angle, and type
   *
   * This function is responsible for finding the table bins and amplitudes
   * for point like sources.
   * It can be called directly for point like sources (such as LEDs and
   * pointlike cascades). But it should not be called directly for line like
   * sources such as finite or infinite muons. Use the apropriate
   * muon-specific function instead.
   *
   * @returns -1 for no table found (z_source/angle out of bounds)
   */
  int find_z_table(float z_source, int *TidA, int *TidB, double *wA, double *wB) const;

  /**
   * @brief Find the angle number [upper and lower] for a requsted angle.
   *
   * The returned indecis are offsets from the lowest angle. Add the
   * index from find_z_table to acces the right table.
   */
  int find_theta_table(float theta_source, int *idA, int *idB, double *wA, double *wB)const;

  /**
   * @brief Returns a (random) time delay based on input (random) cumulative
   *  probability distribution number for selected table and spatial bin.
   */
  double get_level2_photon_delay(double,int,int,int,int)const;

  /**
   * @brief Reads the time distribution directly for a selected time bin.
   *
   * @note this differentiates the content (even if you loaded a
   * differential table!) This function is used to obtain the
   * differential time distribution with simulation tables. */
  double get_level2_photon_delay_prob(double,int,int,int,int)const;

  /**
   * @brief Returns time delay based on input random number for selected table
   *  and bin of starting and stopping point.
   *
   * Contribution of source light beyond the stopping point is removed from
   * timing array before calculating the delay.
   */
  double get_finite_level2_photon_delay(double,int,int,int,int,
					int,int,int,int)const;

  /**
   * @brief Returns probability of given time delay based on input random number for
   *  selected table and bin of starting and stopping point. Finite muons.
   *
   *  As get_level2_photon_delay_prob() but for finate muons.
   */
  double get_finite_level2_photon_delay_prob(double,int,int,int,int,
					     int,int,int,int)const;

  /**
   * @brief gives the number of angles loaded and the angle bin size
   */
  void get_level2_angle_status(int*, float*)const;

  /**
   * @brief Switch to paired tableset of desired type
   *
   * @returns the type we managed to select.
   *
   * Harmless if current tableset is of 'type' already,
   * otherwise a swap to pair_id will be done.
   *
   */
  tableset_type swap_tableset_pair_level2( tableset_type type );

  // private static functions

  /**
   * @brief Number of cerenkov photons emitted by muon per meter of pathlength
   *
   * With correction for low energy loss effects. See CHW thesis, section 7.2
   * @& Number of cerenkov photons emitted by a shower of energy E.
   * See CHW thesis, section 7.1
   */
  static double light(int,double);

  /**
   * @brief light scaling for any type of source.
   */
  static double light_for_source(int sourcetype, double e);

  /**
   * @brief Precalculates level2 table binnings for fast lookup
   */
  static int level2_geometry(const Level2_header_type*,Level2_geo_type*);

  /**
   * @brief Coordinates to bins conversions with remainders.
   *
   * Copied and simplified from reader.c
   */
  static bool get_level2_coord(const Level2_geo_type*,Level2_read_type*);

  /**
   * @brief Full interpolation routine for table entry.
   *
   * Input: Structure with bin info
   * Output: Interpolation factors and bins to use as a structure, and status
   * as a return value.
   * Intepolation logic:
   * Find the weight for the current and an adjecent bin. Always weight
   * current bin higher, and select the adjecent bin closer to the
   * actual value. Edge bins are not interpolared towards the outside.
   * Copied from reader.c, with minor changes.
   */
  static bool level2_interpolate(const Level2_read_type*,Level2_interp_type*);

  /**
   * @brief Routine that reads single level2 file
   */
  static int read_level2_file(const char *file,Level2_header_type *h,Level2_data_type *data,unsigned long *memused);

  /**
   * @brief Table comparator for sorting of data tables
   */
  static int level2_table_comp(const void *a,const void *b);

  /**
   * @brief performs byte swapping on relevant members of level2 headers for Power-PC etc.
   */
  static void byteswap32_l2header(Level2_header_type * h);

};

#endif //PHOTONICS_TABLE_READER_H
