/**
 * @file I3PhotonicsL1Reader.h
 * @brief This header file contains a class for reading photonics level1 tables
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @author RCB
 * @author KWW
 * @version $Id$
 * @date $Date$
 * 
 * The following is a list of file from photonic-1.73 which I 
 * copied verbatium to create this header file:
 *   \li photonics-1.73/lib/photonics.h 
 *   \li photonics-1.73/lib/reader.h
 *   \li photonics-1.73/amasim/table_io.c
 *   \li photonics-1.73/amasim/photoamasim.h 
 *
 * For documentation of photonics files see
 * http://photonics.sourceforge.net/index.php
 * 
 * @copyright Copyright (c) The IceCube Collaboration
 */

#ifndef I3_PHOTONICS_L1_READER_H
#define I3_PHOTONICS_L1_READER_H

#include <photonics-service/I3PhotonicsReader.h> 

/** 
 * @brief Class for Reading Photonics Level1 files for use with photonics-service
 * 
 * This class is created from various photonics header files. Photonics relied 
 * on global variables. All global variables are now member variables in this 
 * class. Functions nescessary for reading level1 files which don't use the 
 * globals were added as private static member functions.
 * 
 */
class I3PhotonicsL1Reader {

  SET_LOGGER("I3PhotonicsL1Reader") 

 public:

  // Data Structures

  struct Data_type{
    float *cont;            /**< Data pointer */
    float *err;             /**< Error matrix pointer */
    size_t  mmaped;         /**< Bytes mmaped in cont and err. If zero, data
			       were allocated with malloc(), not mmap() */
    photonics::ph_size_t offset[VARS]; /**< Array encoding size of each subtable */
  };
  
  /**
   * @brief Structure for runtime configuration data 
   */
  struct Opt_type{
    bool long_run;
    char in_file_name[MAXLINE];
    char out_file_name[MAXLINE];
    char ice_file_name[MAXLINE];
    char eff_file_name[MAXLINE];
    char tmp_out_file_name[MAXLINE];
    double flasher_theta[12]; /**< holds configuration for flasher board source */
    double flasher_phi[12];
    photonics::ph_int active_number;     /**< the number of flashers turned on for a flasher board source */
    photonics::ph_int dummy_padding;     /**< NOT USED */
    bool hole_ice;            /**< hole ice cylinder around source is either on or off */
    double hole_ice_r;        /**< radius of hole ice cylinder */
    double hole_scattering;   /**< scattering length for the hole ice */
  };
  
  /**
   * @brief Structure for storing offsets need for data retreival 
   */
  struct Io_type{
    photonics::ph_size_t h_offset;       /**< Header size in bytes */
    photonics::ph_size_t offset[VARS];   /**< Byte offset for each binning variable */
    photonics::ph_int n_chunk;           /**< Number of elements in one data chunk */
    photonics::Header_type *h;           /**< Pointer to header of opened file */
  };

  struct Interp_type{
    double f[TWO_POWER_VARS];
    int bin[VARS][2];
    unsigned short mask;
    photonics::ph_short dummy_A;
    photonics::ph_int dummy_B;
  };
  
  struct Read_type{
    float val[VARS];
    int bin[VARS];
    int n[VARS];
    unsigned short interp_mask;
    photonics::ph_short dummy_A;
    float binCenter[VARS];
    photonics::ph_int dummy_B;
    float binSpacing[VARS];
    photonics::ph_int dummy_C;
  };

  /**
   * @brief  behaviour control 
   */
  struct pref_level1_t {
    int zedgelimiting; /**< Use lowest/highest z directly if track below/above */
    int greetingdone;
    double external_ref_ng ; /**< the interface defines residual time by using this */
    double z_load_offset;    /**< add z offset to the z coordinates as the tables are loaded */
  };
    
  struct Amasim_Table_Type{ 
    float zLow,zHigh;
    float aLow,aHigh;
    photonics::ph_int type;
    photonics::ph_int padding; /**< currently not used int */
    photonics::Header_type h_abs;
    photonics::Header_type h_prob;
    Data_type abs;
    Data_type prob;
  };

  struct Tableset_level1_type {
    Amasim_Table_Type *table;
    photonics::Geo_type *geo;
    photonics::Header_type *header;

    unsigned long memused;
    unsigned long memforset;

    int photorec_mode;     /**< option for the photorec functionallity */

    int *n_tables;

    double z_src_max; /**< highest valid z_src */
    double z_src_min; /**< highest valid z_src */

    double a_src_max;
    double a_src_min;

    int     n_z_layers; /**< number of z layers in this tableset */
    int     dummy_A;    /**< not used */
    int    *z_layer_index;   /**< indeces to first table of each z layer */
    int    *z_layer_n_thetas; /**< number of angles in each z layer  */
    double *z_layer_centers; /**< central z value in each z layer  */

  };
  
  /**
   * @brief A level1 tableset container contains several level1 table sets
   *
   * mytablesets_levbel2 is a static global which is intended
   * to handle any number of tablesets.
   *
   * The struct members are:
   * \li current     : index of current tableset
   * \li n_tablesets : total number of tablesets initialized
   * \li tableset    : an array of tablesets [0,...,n_tablesets-1]
   */
  struct Tableset_level1_container_type {
    unsigned int current;
    unsigned int n_tablesets;
    double request_angle_limit_low ;
    double request_angle_limit_high;
    double request_depth_limit_low ;
    double request_depth_limit_high;
    Tableset_level1_type *tableset;
  };
  
 private:

  /* Former Global I/O variables */
  FILE *in;
  Io_type io;
  float *data_chunk;
  float *err_chunk;
  Amasim_Table_Type *table;
  int *n_tables ;  /*Number of tables read*/
  photonics::Geo_type (*geo);
  Tableset_level1_container_type mytablesets_level1;
  pref_level1_t  pref_level1;
  
 public:

  /**
   * @brief Constructor initilized formarly global variables which photonics uses
   */ 
  I3PhotonicsL1Reader();

  /**
   * @brief Destructor frees memory used by photonics
   */
  ~I3PhotonicsL1Reader();

  geo_type get_level1_geometry();

  void free_tablesets_level1();

  void set_level1_angular_interval(float alow,float ahigh);

  void set_level1_depth_interval(float zlow,float zhigh);

  /**
   * @brief Routine that reads in all tables specified in driver file 
   */
  int load_tables(const char *);

  /**
   * @brief Returns mean photon density for specified source depth, angle, and type
   *
   * @returns -1 for no table found (z_source/angle out of bounds) 
   */
  double get_photon_density(float,float,int,float,float,float,int,int*,int*,int*,
			    int*)const;
  
  /** 
   * @brief Returns time delay based on input random number for selected table and bin
   */
  double get_photon_delay(double,int,int,int,int)const;
  
  int get_photon_delays(double (*uniform_random)(const void *r, double a, double b),
			void (*multinomial_random)(const void *r, size_t k, unsigned int N,
						   const double p[], unsigned int n[]),
			const void *r, int N, double *delays,
			int id,int z_bin,int rho_bin,int phi_bin)const;
  
  /** @brief Returns probability of given time delay based for selected table and bin*/
  double get_delay_prob(double,int,int,int,int)const;

  double get_level1_lmax()const;

 private:

  int init_tableset_level1();
  
  int swap_tableset_level1( int index );

  /**
   * @brief find the index to the tables [upper and lower] for a requested source depth.
   */
  inline int find_l1z_table(float z_source, int *idA, int *idB, double *wA, double *wB)const;

  /**
   * @brief Find the angle number [upper and lower] for a requsted angle.
   *
   * The returned indecis are offsets from the lowest angle. Add the
   * index from find_z_table to acces the right table.
   */
  inline int find_l1theta_table(float a_source, int *TidA, int *TidB, double *wA, double *wB)const;

  void close_input(FILE*);

  /** 
   * @brief Returns a pointer to header of currently opened 
   * input file, or NULL if none available.           
   */
  bool read_header(photonics::Header_type *h,FILE *fh)const;
  
  /** 
   * @brief Read chunk from current stream position  
   */
  float *read_current_chunk(FILE *fh);

  /** 
   * @brief Verify that it's possible to read data and setup 
   * variables for input                             
   */
  FILE *test_input(const Opt_type *opt);

  /** 
   * @brief Returns a pointer to header of currently opened  
   * input file, or NULL if none available.           
   */
  photonics::Header_type *get_header(void)const; 

  /**
   * @brief Entire photon array is readout out               
   */
  bool mmap_file(const Opt_type *opt,Data_type *data,bool errs)const;

  /** 
   * @brief Entire photon table file is readout
   * but without errors                 
   */
  bool read_file_without_errors(const Opt_type*,Data_type*);

  /**
   * @brief Entire photon array is readout out               
   */
  bool read_table(float*,FILE*);


  // private static functions

  /**
   * @brief Table comparator for sorting of data tables 
   */
  static int level1_table_comp(const void *a,const void *b);

  static bool open_binary_input(const char *file,FILE **fh);
  
  /**
   * @brief performs byte swapping on relevant members of level1 headers for Power-PC etc.
   */
  static void byteswap32_l1header(photonics::Header_type * h);

  /** 
   * @brief Functions calculating next boundry value based in scaling used. 
   */
  static double next_boundary_lin(const double base,const double step);

  /** 
   * @brief Set up azimuthal regions based on user input   
   *  NB: Values kept as cosines for faster computation down the line 
   */
  static bool evaluate_slices(const photonics::Header_type *h,photonics::Geo_type *geo);
    
  /** 
   * @brief  Set up cylindrical active volume based on user input                            
   */
  static bool cylindrical_geometry(const photonics::Header_type *h,photonics::Geo_type *geo);

  /**
   * @brief Evaluate area of each surface edge        
   */
  static bool cylindrical_areas(const photonics::Header_type *h,photonics::Geo_type *geo);

  /**
   * @brief  Set up spherical active volume based on user input                                 
   */
  static bool spherical_geometry(const photonics::Header_type *h,photonics::Geo_type *geo);

  /** 
   * @brief Evaluate area of each surface edge          
   */
  static bool spherical_areas(const photonics::Header_type *h,photonics::Geo_type *geo);

  /**
   * @brief Set up cubic active volume based on user input 
   */
  static bool cubic_geometry(const photonics::Header_type *h,photonics::Geo_type *geo);

  /**
   * @brief Evaluate area of each surface edge              
   */
  static bool cubic_areas(const photonics::Header_type *h,photonics::Geo_type *geo);

  /** 
   * @brief Calculates timing bin coordinates based on user input                                  
   */
  static bool timing_geometry(const photonics::Header_type *h,photonics::Geo_type *geo);
    
  /**
   * @brief set up requested geometry 
   */
  static bool set_up_geometry(const photonics::Header_type*,photonics::Geo_type*);
  
  /**
   * @brief Coordinates to bins conversions with lookup of interpolation variables 
   */
  static bool get_coord(const photonics::Header_type*,const photonics::Geo_type*,Read_type*);

  /** @brief Full interpolation routine for table entry.
   *
   *  Input: Structure with bin info
   *
   *  Output: Interpolation factors and bins to use as a structure, and status
   *  as a return value.
   *
   *  Intepolation logic:
   *  This is full multidimensional bilinear interpolation. The simpification
   *  is that dimensions are treated as linear grids. Cross-bins are taken care
   *  off automatically.
   *  Find the weight for the current and an adjecent bin. Always weight
   *  current bin higher, and select the adjecent bin closer to the
   *  actual value. Edge bins are not interpolared towards the outside.
  */
  static bool interpolate(const Read_type*,Interp_type*);  

  /**
   * @brief Allocate memory for our main data array                         
   */
  static bool data_alloc(Data_type *data,const photonics::Header_type *h);

  /**
   * @brief Determine the size of memory that WAS allocated by identical call to data_alloc 
   */
  static unsigned long data_allocated(Data_type *data,const photonics::Header_type *h);
  
  /**
   * @brief Free memory from our main data array                            
   */
  static void data_free(Data_type *data);
  
};
  
#endif /*I3_PHOTONICS_L1_READER_H*/


