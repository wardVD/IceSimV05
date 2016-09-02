/**
 * @file I3PhotonicsL2Reader.cxx
 * @brief Class for Reading Photonics Level2 files for use with photonics-service
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @version $Id$
 * @date $Date$
 * 
 * This file contains a cleaned up version of:
 *  \li photonics-1.73/level2/level2_photonics.h:
 *  \li photonics-1.73/level2/level2_reader.h
 *  \li photonics-1.73/level2/light.c
 *  \li photonics-1.73/level2/photonicsCPPio.cxx
 *  \li photonics-1.73/level2/level2_table_io.c
 *  \li photonics-1.73/level2/level2_reader.c
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

#include <cmath>
#include <cstdio>
#include <cerrno>
#include <cstring>
#include "icetray/I3Logging.h"
#include "dataclasses/I3Constants.h"
#include "photonics-service/I3PhotonicsL2Reader.h"

/*******************************************************************/
/* level2_reader.h:  Definitions for reading of level2 tables      */
/*******************************************************************/

#define L2_EXT_LISTMARKSTOP "@stopping:"
#define L2_EXT_LISTMARKSTART "@starting:"
#define L2_EXT_LISTMARKPHOTOREC "@photorec:"

/* Interpolation flags. used for level2 interp bitmask */
#define  L2_INTERP_NONE      0x00
/// l
#define  L2_INTERP_PRIMARY   0x01
/// rho
#define  L2_INTERP_SECONDARY 0x02
/// phi
#define  L2_INTERP_TERTIARY  0x04
/// time:
#define  L2_INTERP_TIME      0x08
/// depth: 16
#define  L2_INTERP_DEPTH     0x10
/// theta: 32
#define  L2_INTERP_ANGLE     0x20

/******************************************************************/
/* level2/light.c                                                 */
/******************************************************************/

#define MUON 0
#define EMS  1
#define HADS 2

/** Cerenkov photons per meter in 300 - 600 nm range */
#define photons 32440.0

/** Allowable numerical error for temp array */
#define ERROR_MARGIN 5e-10   

using namespace photonics;

/*****************************************************************
*     Constructor for the photonics wrapper class.
******************************************************************/
I3PhotonicsL2Reader::I3PhotonicsL2Reader():
  infstop_mark(L2_EXT_LISTMARKSTOP),
  startinf_mark(L2_EXT_LISTMARKSTART),
  photorec_mark(L2_EXT_LISTMARKPHOTOREC),
  n_tables(NULL)
{
  mytablesets_level2.current=0;
  mytablesets_level2.n_tablesets=0;
  mytablesets_level2.request_angle_limit_low  =-photonics_reader_InF;
  mytablesets_level2.request_angle_limit_high = photonics_reader_InF;
  mytablesets_level2.request_depth_limit_low  =-photonics_reader_InF;
  mytablesets_level2.request_depth_limit_high = photonics_reader_InF;

  pref_level2.zl_matching   = 1;     /* Try to find best z/l table combination */
  pref_level2.zedgelimiting = 1 ;    /* Use lowest/highest z directly if track below/above */
  pref_level2.greetingdone  = 0;
  pref_level2.external_ref_ng = I3Constants::n_ice_group ; /* the interface defines residual time by using this */    
}


I3PhotonicsL2Reader::~I3PhotonicsL2Reader(){
  if (mytablesets_level2.n_tablesets>=0){
    free_tablesets_level2();   
  }
}

/* Number of cerenkov photons emitted by muon per meter of pathlength, with
   correction for low energy loss effects. See CHW thesis, section 7.2 
 & Number of cerenkov photons emitted by a shower of energy E.
   See CHW thesis, section 7.1 */
double I3PhotonicsL2Reader::light(int id, double eee){
  double length;  /* Effective length of shower */

  if (id == EMS)
    length = 0.894*4.889*eee;  /* EM shower */
  else if(id == HADS)
    length = 0.860*4.076*eee;  /* Hadronic shower */  
  else
    length = 1.172 + 0.0324*log(eee);
    //length = 1.172 + 0.023*log(eee); /* Muon Mar. 17 2009 K.Hoshina CHW thesis and Ped's thesis WAS typo!!! */

  return photons*length;
}
  
/* light scaling for any type of source. */
double I3PhotonicsL2Reader::light_for_source(int sourcetype, double e){

  source_type ST=(source_type) sourcetype;

  switch(ST){
  case CHERENKOV : 
    return light(MUON , e);
  case HAD_SHOWER :
    return light(HADS, e);
  case EM_SHOWER :
    return light(EMS, e);      
  default:
    return light(EMS, e); /* default scaling is ems */
  }

}

/* Switch to tableset of desired type, in current table set pair */
I3PhotonicsL2Reader::tableset_type 
I3PhotonicsL2Reader::swap_tableset_pair_level2( tableset_type type )
{    
    const Tableset_level2_type curr = mytablesets_level2.tableset[
	mytablesets_level2.current ];
    
    if ( curr.type != type ) {
	if ( curr.pair_id != -1 ) {
	  if (mytablesets_level2.tableset[curr.pair_id].type == type) {
	    swap_tableset_level2(curr.pair_id );
	  } else {
	    return curr.type;
	  }
	} else {
	    log_fatal("Photonics Level2: level2_table_io.c"
		    "Trying to switch pair with no partner!");
	}
    }
    return type;
}

/* Initialize a new tableset */
int I3PhotonicsL2Reader::init_tableset_level2()
{
    int index; /* New level2 table set index */
    unsigned long mm=0,mthis=0;

    /* Make memory for level2 tableset */
    if ( mytablesets_level2.n_tablesets ==  0 ) {              

	/* First level2 tableset needs malloc */
      mytablesets_level2.tableset = ( Tableset_level2_type*) 
	    malloc ( sizeof( Tableset_level2_type ) );
	index = 0; 
	mytablesets_level2.n_tablesets = 1;
    } else { 
	/* Following table sets needs realloc */
	mytablesets_level2.tableset = ( Tableset_level2_type*)
	    realloc ( mytablesets_level2.tableset, 
		      (mytablesets_level2.n_tablesets+1)*
		      sizeof( Tableset_level2_type ) );
	index = mytablesets_level2.n_tablesets;
	mytablesets_level2.n_tablesets++;
    }    

    /*Make memory for tables and info in tableset*/

    mytablesets_level2.tableset[index].n_tables =(int*) 
	malloc( mthis=sizeof ( int ) ); 
    (*mytablesets_level2.tableset[index].n_tables) = 0;
    mm+=mthis;

    mytablesets_level2.tableset[index].geo =  ( Level2_geo_type *)
	malloc( mthis=sizeof ( Level2_geo_type ) ); 
    mm+=mthis;

    mytablesets_level2.tableset[index].header =(Level2_header_type*)
        malloc( mthis=sizeof (Level2_header_type) );
    mm+=mthis;

    mytablesets_level2.tableset[index].table =(Amasim_Level2_Table_type*) 
	malloc( mthis=sizeof (Amasim_Level2_Table_type)*MAX_TABLES );
    mm+=mthis;

    mytablesets_level2.tableset[index].memused=0; // for the actual data
    mytablesets_level2.tableset[index].memforset=mm; // for just the set

    /*Initiate variables */
    mytablesets_level2.tableset[index].z_src_max=0;
    mytablesets_level2.tableset[index].z_src_min=0;
    mytablesets_level2.tableset[index].a_src_max=0;
    mytablesets_level2.tableset[index].a_src_min=0;
    mytablesets_level2.tableset[index].ref_dist=0;
    mytablesets_level2.tableset[index].ref_dist_fraction=0;

    mytablesets_level2.tableset[index].photorec_mode=0;

    /*return tableset index*/
    return index;
}

/*Swap active tableset*/
int I3PhotonicsL2Reader::swap_tableset_level2( int index )
{
    /* Check index */
 
   if ( mytablesets_level2.n_tablesets ==  0 ) {
        return -1;
   }
  
   if ( index >= mytablesets_level2.n_tablesets ) {
	return -1;
    }
    
    /* Change current index to new index */
    mytablesets_level2.current = index;
    
    /* Repoint static global pointers */    
    geo      = 
	mytablesets_level2.tableset[mytablesets_level2.current].geo; 
    table    = 
	mytablesets_level2.tableset[mytablesets_level2.current].table;
    n_tables = 
	mytablesets_level2.tableset[mytablesets_level2.current].n_tables;
    
    return index;
}

/* Free all memory used by level2 table sets */
void I3PhotonicsL2Reader::free_tablesets_level2( )
{
    int i,j,k;

    /* Loop over all tablesets in array */
    for ( i = 0; i < mytablesets_level2.n_tablesets; i++ ) {
	/* Swap to ensure n_tables is up to date */
	swap_tableset_level2( i );

	/* Free memory used by each table */
	for ( j = 0; j < (*mytablesets_level2.tableset[i].n_tables); j++ ) {

	    free( mytablesets_level2.tableset[i].table[j].abs.cont ); 
	    free( mytablesets_level2.tableset[i].table[j].prob.cont ); 
	}

	/* Free tableset members */

	free( mytablesets_level2.tableset[i].n_tables ); 

	for(k=0;k<L2VARS;++k){
	  free( mytablesets_level2.tableset[i].geo->edge[k] ); 
	}
	free( mytablesets_level2.tableset[i].geo->times ); 

	free( mytablesets_level2.tableset[i].geo ); 
	free( mytablesets_level2.tableset[i].header ); 
	free( mytablesets_level2.tableset[i].table ); 	
	free( mytablesets_level2.tableset[i].z_layer_index );
	free( mytablesets_level2.tableset[i].z_layer_n_thetas );
	free( mytablesets_level2.tableset[i].z_layer_centers );


	/* trust that we are now done */
	mytablesets_level2.tableset[i].memused=0;
	mytablesets_level2.tableset[i].memforset=0;
    }
    
    if ( mytablesets_level2.n_tablesets > 0 ) {
      /* Free memory used by tableset array */
      free( mytablesets_level2.tableset );
    }
    
    /* Reset current tableset index */
    mytablesets_level2.current = 0;

    /* Reset number of tablesets counter */
    mytablesets_level2.n_tablesets = 0;
}

/* Return the amount of memory (bytes) used for tableset in
 * photonics level2
 */
void I3PhotonicsL2Reader::get_level2_memory_usage(unsigned long *mem) const
{
  int i;
  unsigned long total=0;
  
  /* Loop over all tablesets in array */
  for ( i = 0; i < mytablesets_level2.n_tablesets; i++ ) {
    /* Swap to ensure n_tables is up to date */
    total+=mytablesets_level2.tableset[i].memused;
  }

  *mem=total;
}

void I3PhotonicsL2Reader::set_level2_angular_interval(float alow,float ahigh)
{
  ahigh=(ahigh<alow)?alow:ahigh;

  mytablesets_level2.request_angle_limit_low = alow;
  mytablesets_level2.request_angle_limit_high = ahigh;
}

void I3PhotonicsL2Reader::get_level2_angular_interval(float *alow,float *ahigh)const
{
  if ( mytablesets_level2.n_tablesets ==  0 ) {
    *alow=1;
    *ahigh=-1;
  } else {
    *alow= mytablesets_level2.tableset[mytablesets_level2.current].a_src_min ; 
    *ahigh=mytablesets_level2.tableset[mytablesets_level2.current].a_src_max ;  
  }
}

void I3PhotonicsL2Reader::set_level2_depth_interval(float zlow,float zhigh)
{
  zhigh=(zhigh<zlow)?zlow:zhigh;

  mytablesets_level2.request_depth_limit_low = zlow;
  mytablesets_level2.request_depth_limit_high = zhigh;
}

void I3PhotonicsL2Reader::get_level2_depth_interval(float *zlow,float *zhigh)const
{
  if ( mytablesets_level2.n_tablesets ==  0 ) {
    *zlow=1;
    *zhigh=-1;
  } else {
    *zlow= mytablesets_level2.tableset[mytablesets_level2.current].z_src_min ; 
    *zhigh=mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ;  
  }
}

// gives the number of angles loaded and the angle bin size
void I3PhotonicsL2Reader::get_level2_angle_status(int *nangles,float *anglesize)const
{  
  int j=mytablesets_level2.current;
  if ( (mytablesets_level2.n_tablesets ==  0) || 
       (*mytablesets_level2.tableset[j].n_tables ==0) ) {
    *nangles=0;
    *anglesize=-1.0;
  } else {
    // assume all z layers have the same binning, and that
    // the angular (theta) bin size is the same.
    *nangles=mytablesets_level2.tableset[j].z_layer_n_thetas[0] ;
    *anglesize=mytablesets_level2.tableset[j].table->aHigh-
               mytablesets_level2.tableset[j].table->aLow;
  }
}

// returns estimate of number of bytes needed to load an interval of 
// given size, assuming equidistant theta binning.
// The results should be exact as long as the first possible angle
// given by the driver file is zero, or a multiple of angular bin size
void I3PhotonicsL2Reader::get_level2_angular_memory_requirement(float alow,
								float ahigh,
								unsigned long*mem)const{
  int nangles;
  float anglesize;
  unsigned long totalmem;
  float memperangle;
  
  int n;

  get_level2_angle_status(&nangles,&anglesize);
  get_level2_memory_usage(&totalmem);

  if((nangles<=0)||(anglesize<=0)){
    log_error("get_level2_angular_memory_requirement(%f,%f) failed",alow,ahigh);
  }

  memperangle=  totalmem / (1.0*nangles );

  n=1 + (int)(ceilf(ahigh/anglesize) - floorf(alow /anglesize)) ;
  n=n>0?n:0;

  totalmem=(unsigned long)(memperangle*n);
  
  *mem= totalmem;
}

/* Routine that reads in all tables specified in driver file */
int I3PhotonicsL2Reader::load_level2_tables(const char *driver_file){
  FILE *driver_fh;
  int j,k,m,q,r,i;
  int isstop=0; /* start=0, stop =1 */
  char line[MAXLINE];
  char tmp_name[MAXLINE];
  char in_file_name[MAXLINE];
  char tmp_line[MAXLINE];
  Level2_header_type *h;
  int index_start=-1;
  int index_stop=-1;
  int prfln;
  int loading_photorec=0;
  double low_ang_tab=-photonics_reader_InF,high_ang_tab=photonics_reader_InF;
  double low_dep_tab=-photonics_reader_InF,high_dep_tab=photonics_reader_InF;  
  float tmp1,tmp2,tmp3,tmp4;

  unsigned long memused;

  float tmpdepth;

  // initialize char arrays
  memset(line, '\0', MAXLINE);
  memset(tmp_name, '\0', MAXLINE);
  memset(in_file_name, '\0', MAXLINE);
  memset(tmp_line, '\0', MAXLINE);
  if(pref_level2.greetingdone<1){
    pref_level2.greetingdone++;
    log_info("Photonics Level2: Version '%s'",PHOTONICS_VERSION);
    if(!isSupportedEnvironment()){
      log_warn("Photonics is not supported in this environment -- continuing anyway.");
    }

    if(sizeof(Level2_header_type) != HEADER_TYPE_LENGTH ){
      log_fatal("Photonics: Error: compile time Level2_header_type of wrong size."); 
    }

  }    
  
  /* Open driver file */
  if((driver_fh=fopen(driver_file,"r"))==NULL){
    log_error("Photonics Level2: Unable to open table input driver file: '%s'",driver_file);
    return false;
  }
  log_debug("Photonics Level2: Loading driver file: '%s'",driver_file);

  /* preparsing of the file to find the angles */

  float a_lowest=photonics_reader_InF;
  float a_highest=-photonics_reader_InF;
  float z_lowest=photonics_reader_InF;
  float z_highest=-photonics_reader_InF;

  {
    i=0;
    while(!feof(driver_fh)){
      if(i>=MAX_TABLES){
	log_fatal("Photonics Level2: Maximum number of tables read without reaching EOF.");
	return false;
      }
    /* Read in a line, skipping comments */
      if(fgets(line,MAXLINE,driver_fh) && line[0]!='#' && strlen(line)>4 ){    
	sscanf(line,"%s %f %f %f %f",tmp_line,&tmp1,&tmp2,&tmp3,&tmp4);

	/* find the highest and lowest bounds whatsoever */
	a_lowest=(tmp3<a_lowest)?tmp3:a_lowest;
	a_highest=(tmp3>a_highest)?tmp3:a_highest;

	/* centers for depth */
	tmpdepth=(tmp1+tmp2)*0.5;
	z_lowest=(tmpdepth<z_lowest)?tmpdepth:z_lowest;
	z_highest=(tmpdepth>z_highest)?tmpdepth:z_highest;

	if (tmpdepth<=mytablesets_level2.request_depth_limit_low)
	  /* find the highest low bound allowed */
	  if(tmpdepth>low_dep_tab){	  	
	    low_dep_tab=tmpdepth;
	  }

	if(tmpdepth>=mytablesets_level2.request_depth_limit_high)
	  /* find the lowest high bound allowed */
	  if(tmpdepth<high_dep_tab){	  
	    high_dep_tab=tmpdepth;	    
	  }
	
	
	if(tmp3<=mytablesets_level2.request_angle_limit_low)
  	  /* find the highest low bound allowed */
	  if(tmp3>low_ang_tab){	  
	    low_ang_tab=tmp3;	    
	  }

	if(tmp3>=mytablesets_level2.request_angle_limit_high) 
	  /* find the lowest high bound allowed */
	  if(tmp3<high_ang_tab){	  
	    high_ang_tab=tmp3;       
	  }	

      }
      i++;
    }  /* end of preparsing */

    if((low_ang_tab>high_ang_tab)||(low_dep_tab>high_dep_tab)){
      log_error("no tables matched selection criteria");
      return false;
    }   

    /* low_ang_tab, high_ang_tab, low_dep_tab and high_dep_tab 
       are used later in this function */

    /* rewind the file */
    rewind(driver_fh);
  } // end of unconditional block

  /* Loop over all entries */
  while(!feof(driver_fh) ){
    /* Read in a line, skipping comments */
    if(fgets(line,MAXLINE,driver_fh) && line[0]!='#' && strlen(line)>4 ){

      /* Parse the line */      
      sscanf(line,"%s %f %f %f %f",tmp_name,&tmp1,&tmp2,&tmp3,&tmp4);
      
      if( tmp1 >= tmp2 ){
	log_fatal("Photonics Level2: Error. The driver file has layers of ill-defined size");
      }

      tmpdepth=(tmp1+tmp2)*0.5;

      if(!((tmp3>=low_ang_tab)&&
	   (tmp3<=high_ang_tab)&&
	   (tmpdepth>=low_dep_tab)&&
	   (tmpdepth<=high_dep_tab))){
	log_trace("Photonics Level2: debug: skipping %s",tmp_name);

	continue;	
      } 

      /* Log_Fatal if we find deprecated label: */
      if (0==strncmp("@degeninf:",tmp_name,strlen("@degeninf:"))) {
	log_fatal("Photonics Level2: Error: found '@degeninf:' mark (deprecated table) in driver file: '%s' bad line: '%s",driver_file,tmp_name);
      }

      /* first, we check if there is a @photorec: mark. if there is, remember and remove the tag  */
      /* note that @stopping and @starting is supported in combination with @photorec, like this: */
      /* @photorec:@starting: or                                                                  */
      /* @photorec:@stopping:                                                                     */

      if (0==strncmp(photorec_mark,tmp_name,strlen(photorec_mark))) {

	prfln=strlen(photorec_mark);
	
	/* remove the mark: */
	for ( j=0 ; tmp_name[prfln+j-1] != '\0' ; ++j ) {
	  tmp_name[j] = tmp_name[prfln+j] ;
	}
	
	if(loading_photorec!=1){
	  
	  if(pref_level2.greetingdone<2){
	    pref_level2.greetingdone++;
	    log_info("Photonics Level2: Using 'Photorec' mode.");
	  }
	}

	/* if we already loaded non photorec tables: */
	if ((loading_photorec==0) && ((( index_start!=-1 )||(index_stop!=-1 )))) {	  
	  log_fatal("Photonics Level2: error: you are not allowed to mix photorec and simulation tables in a single table set.");	 
	}
	loading_photorec=1;
      }
      else {/* if we are not loading a photorec table */
	/* if we already loaded photorec tables: */
	if ((loading_photorec==1) && ((( index_start!=-1 )||(index_stop!=-1 )))) {	  
	  log_fatal("Photonics Level2: error: you are not allowed to mix photorec and simulation tables in a single table set.");	 
	}
      }

      /* Is it marked as stopping? */
      if (0==strncmp(infstop_mark,tmp_name,strlen(infstop_mark)))
	isstop=1;
      else
	isstop=0;
      
      if (isstop==1) {
	if ( index_stop==-1 ) {
	  /* Is this the first file of this kind? */
	  index_stop = init_tableset_level2();
	}
	
	swap_tableset_level2( index_stop );
	h=     mytablesets_level2.tableset[index_stop].header;

	prfln=strlen(infstop_mark);
	
      } else {
	if ( index_start==-1 ) {
	  /* Is this the first file of this kind? */
	  index_start = init_tableset_level2();

	}	
	swap_tableset_level2( index_start );
	h=     mytablesets_level2.tableset[index_start].header;
	
	if (0==strncmp(startinf_mark,tmp_name,strlen(startinf_mark))) {
	  prfln=strlen(infstop_mark);
	} else {
	  prfln=0; /* this is the default, if no mark is set in the list file */
	}	
      }

      /* is this a photorec table? then mark it... */
      mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode=loading_photorec;

      /* remove the mark: */
      for ( j=0 ; tmp_name[prfln+j-1] != '\0' ; ++j ) {
	tmp_name[j] = tmp_name[prfln+j] ;
      }
      
      if ((*n_tables)>=MAX_TABLES) {
	log_fatal("Photonics Level2: error: Maximum number of tables read without reaching EOF.");	
      }
      
      sscanf(line,"%s %f %f %f %f",tmp_line,
	     &table[  (*n_tables)].zLow,&table[  (*n_tables)].zHigh,
	     &table[  (*n_tables)].aLow,&table[  (*n_tables)].aHigh); 
      
      /* Read in the tables */
      log_debug("Loading level2 table %3d, set %d %s",(*n_tables),mytablesets_level2.current,tmp_name);
      
      sprintf(in_file_name,"%s.abs",tmp_name);
      if(!read_level2_file(in_file_name,&(*h),&table[  (*n_tables)].abs,&memused)){
	log_error("Unable to read level2 file %s.",in_file_name);
	return false;
      }
      // Loop over table and check for inf and nan
      { 
	unsigned long i_warnings=0;
	long this_size= table[*n_tables].abs.offset[0]*h->n[0];
	
	for(k=0;k<this_size;++k){
	  if( std::isinf(table[(*n_tables)].abs.cont[k]) || std::isnan(table[(*n_tables)].abs.cont[k])){
	    table[(*n_tables)].abs.cont[k]=0;
	    i_warnings++;
	  } 
	  if( table[(*n_tables)].abs.cont[k] <0 ){ // negative probabilities quietly set to 0.
	    table[(*n_tables)].abs.cont[k]=0;
	  }
	}
	log_debug("cleared some nans: %d*%f",h->n[3],i_warnings/(1.0*h->n[3]));
      }

      mytablesets_level2.tableset[mytablesets_level2.current].memused+=memused;
      
      if(loading_photorec==1)
	sprintf(in_file_name,"%s.diff",tmp_name);
      else
	sprintf(in_file_name,"%s.prob",tmp_name);

      if(!read_level2_file(in_file_name,&(*h),&table[(*n_tables)].prob,&memused)){
	log_error("Unable to read level2 file %s.",in_file_name);
	return false;
      }
      // Loop over table and check for inf and nan. negative values set to 0.
      { 
	unsigned long i_warnings=0;
	long this_size= table[*n_tables].prob.offset[0]*h->n[0];

	for(k=0;k<this_size;++k){
	  if( std::isinf(table[(*n_tables)].prob.cont[k]) || std::isnan(table[(*n_tables)].prob.cont[k])){
	    table[(*n_tables)].prob.cont[k]=0;
	    i_warnings++;
	  } 
	  if( table[(*n_tables)].prob.cont[k] <0 ){ // negative probabilities quietly set to 0.
	    table[(*n_tables)].prob.cont[k]=0;
	  }
	}
	log_debug("clearing some nans: %d*%f",h->n[3],i_warnings/(1.0*h->n[3]));
	
	
      }

      mytablesets_level2.tableset[mytablesets_level2.current].memused+=memused;

      if( (*n_tables)==0){ /* Calculate geometry on the first pass */
	if(!level2_geometry(&(*h),&(*geo))){
	  log_fatal("Photonics Level2: Unable to obtain geometry info for level2 tables.");
	  return false;
	}

	mytablesets_level2.tableset[mytablesets_level2.current].ref_dist=(
	  fabs(mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][0])<
	  fabs(mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][1])?
	  fabs(mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][0]):
	  fabs(mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][1]));

	mytablesets_level2.tableset[mytablesets_level2.current].ref_dist_fraction=
	  mytablesets_level2.tableset[mytablesets_level2.current].ref_dist*0.00001;

      }      

      ++(*n_tables);  /* Count up */
    }
  }
  
  fclose(driver_fh);  /* List file reading done */
  

  /* Starting tables */  
  if ( index_start!=-1 ) {
    swap_tableset_level2( index_start );
    
    /* Finally sort tables for quicker search */
    qsort(table,(*n_tables),sizeof(Amasim_Level2_Table_type),level2_table_comp);

    mytablesets_level2.tableset[index_start].type=START;

    if (index_stop!=-1) {
      mytablesets_level2.tableset[index_start].pair_id=index_stop;
    } else {
      /* Neighbour with itself */
       mytablesets_level2.tableset[index_start].pair_id=index_start;
    }    
    log_info("Photonics Level2: loaded %4d starting tables, setid %3d%s",*n_tables,index_start,
	     mytablesets_level2.tableset[index_start].photorec_mode?" photorec mode":""); 
  }

  /* Stopping tables */
  if ( index_stop!=-1 ) { 
    swap_tableset_level2( index_stop );
  
    /* Finally sort tables for quicker search */
    qsort(table,(*n_tables),sizeof(Amasim_Level2_Table_type),level2_table_comp);

    mytablesets_level2.tableset[index_stop].type=STOP;

    if (index_start!=-1) {
      mytablesets_level2.tableset[index_stop].pair_id=index_start;
    } else {
      /* Neighbour with itself */
       mytablesets_level2.tableset[index_stop].pair_id=index_stop;
    }    

    log_info("Photonics Level2: loaded %4d stopping tables, setid %3d%s",*n_tables,index_stop,
	     mytablesets_level2.tableset[index_stop].photorec_mode?"   photorec mode":"");
  
  }
  

  /* ********************************************************* */
  /*                                                           */
  /* Precalculate and store some useful information.           */
  /*    Tables are already sorted above.                       */
  
  for ( m=0 ; m<=1 ; m++){ // helper loop j loop over tablesets.   

    j= ( m==0?index_start:index_stop );
    
    if (j != -1 && (swap_tableset_level2( j ) == j) ) {
     
      /* store 1/sin(ch) for quick access */
      mytablesets_level2.tableset[j].inv_sin_ch = 
        1/(sqrt(1 - (1.0/(mytablesets_level2.tableset[j].header->ref_np*mytablesets_level2.tableset[j].header->ref_np))));

      /* count the number of z layers */
      mytablesets_level2.tableset[j].n_z_layers=1;
      for (k=0; k<(*n_tables-1) ; k++) {	      
	if ( table[k].zHigh != table[k+1].zHigh ) {
	  mytablesets_level2.tableset[j].n_z_layers++;
	}
      }

      /* allocate memory for lookup tables */
      mytablesets_level2.tableset[j].z_layer_index = (int *)
	malloc( sizeof ( int )*mytablesets_level2.tableset[j].n_z_layers ); 

      mytablesets_level2.tableset[j].z_layer_n_thetas = (int *)
	malloc( sizeof ( int )*mytablesets_level2.tableset[j].n_z_layers ); 
      
      mytablesets_level2.tableset[j].z_layer_centers= (double *)
	malloc( sizeof ( double )*mytablesets_level2.tableset[j].n_z_layers ); 

      /* calculate and store  the index to the first bin in each z layer */
      /* tables are sorted, so this is easy */
      q=0;
      for (k=0; k<(*n_tables) ; k++) {      
	if (k==0 || table[k].zHigh != table[k-1].zHigh ){  
	  mytablesets_level2.tableset[j].z_layer_index[q]=k;
	  q++;
	}	
	if ((k==0)||(mytablesets_level2.tableset[j].a_src_max<table[k].aLow))
	    mytablesets_level2.tableset[j].a_src_max=table[k].aLow;
	if ((k==0)||(mytablesets_level2.tableset[j].a_src_min >table[k].aLow))
	    mytablesets_level2.tableset[j].a_src_min =table[k].aLow;
      }      

      /* store max and min valid z locations, and central z values */
      for (k=0; k<(mytablesets_level2.tableset[j].n_z_layers) ; k++) {	           

	r=mytablesets_level2.tableset[j].z_layer_index[k];
	for(q=0;(r+q)<(*n_tables);q++){
	  if (table[r].zHigh!=table[r+q].zHigh)
	    break;
	  mytablesets_level2.tableset[j].z_layer_n_thetas[k]=q+1;
	}
		
	tmpdepth=(table[r].zHigh+table[r].zLow)*0.5;

	mytablesets_level2.tableset[j].z_layer_centers[k]=tmpdepth;

	if ((k==0)||(mytablesets_level2.tableset[j].z_src_max<tmpdepth))
	  mytablesets_level2.tableset[j].z_src_max=tmpdepth;
	if ((k==0)||(mytablesets_level2.tableset[j].z_src_min>tmpdepth))
	  mytablesets_level2.tableset[j].z_src_min=tmpdepth;
	
      }

      /* verify that all z layers have the same number of angles */
      for (k=0; k<(-1 + mytablesets_level2.tableset[j].n_z_layers) ; k++) {	           
	if(mytablesets_level2.tableset[j].z_layer_n_thetas[k]!=
	   mytablesets_level2.tableset[j].z_layer_n_thetas[k+1]){
	  log_fatal("Photonics Level2: Error: limitation: homogeneously binned table set required.");
	  break;
	}
      }

      /* if there is only one angle in the driver file: */
      if ( a_highest == a_lowest ){
	mytablesets_level2.tableset[j].a_src_max=table[0].aHigh;	
      }
       
      log_info("Photonics Level2: Active interval: (setid: %d) %4g<=th<=%4g  %4g<=z<=%4g",
	       mytablesets_level2.current,
	       mytablesets_level2.tableset[j].a_src_min,
	       mytablesets_level2.tableset[j].a_src_max,
	       mytablesets_level2.tableset[j].z_src_min,
	       mytablesets_level2.tableset[j].z_src_max);

      /* suggestion: We could add more sanity checking here. For example:
         users must always load identical binning/sets for @starting
         and @stopping */
    }
  }  

  return ((index_start+index_stop)>-2?1:-1) ;
  
}


// Find the angle number [upper and lower] for a requsted angle. 
// The returned indecis are offsets from the lowest angle. Add the 
// index from find_z_table to acces the right table.
inline int I3PhotonicsL2Reader::find_theta_table(float a_source, int *TidA, int *TidB, double *wA, double *wB)const{

  int my_current=mytablesets_level2.current;

  // assuming all layers have the same number of angles.
  int n_thetas=mytablesets_level2.tableset[my_current].z_layer_n_thetas[0];
  
  double a_other_weight;	
  
  int id0=-1;
  int id1;

  if(n_thetas<2){
    id0=0;
    id1=0;
    a_other_weight=0;	  
  }else{
    if(a_source>=mytablesets_level2.tableset[my_current].a_src_max){
      id0=n_thetas-1;
      id1=id0;
      a_other_weight=0;
    }else{
      if(a_source<=mytablesets_level2.tableset[my_current].a_src_min){
	id0=0;
	id1=0;
	a_other_weight=0;
      }
    }
  }
  
  if(id0==-1){

    int j;

    for(j=n_thetas-1;j>=0 ;--j){      // Find angle: loop from higher angles down.
      if(table[j].aLow<=a_source){
	id0=j;
	break;	    
      }
    }
      
    id1=id0+1;

    if((id0<0)||(id0>=n_thetas)){
      log_fatal("Photonics Level2: Error: (a) Internal interpolation error (a_src)");    
    }

    a_other_weight=
      (a_source-table[id0].aLow)/
      (table[id1].aLow-table[id0].aLow);	    	
  }

  if((id0<0)||(id0>=n_thetas)){
    log_fatal("Photonics Level2: Error: (b) Internal interpolation error (a_src)");    
  }

  if(a_other_weight>0.5){
    *TidB=id0;
    *TidA=id1;
    *wB=1-a_other_weight;
    *wA=a_other_weight;
  }else{
    *TidA=id0;
    *TidB=id1;
    *wA=1-a_other_weight;
    *wB=a_other_weight;
  }

  return 0;

}

// find the index to the tables [upper and lower] for a requested source depth.
inline int I3PhotonicsL2Reader::find_z_table(float z_source, int *idA, int *idB, double *wA, double *wB)const{

  static int last_id0=-1;

  int my_current=mytablesets_level2.current;
  
  int id0=-1;
  int id1;
  
  double z_other_weight;	
  
  if(mytablesets_level2.tableset[my_current].n_z_layers<2){
    id0=0;
    id1=0;
    z_other_weight=0;	  
  }else{
    /* Use 1000*EPS (1mm) since precision does not allow adding 1E-6 to 
     something of the order of 1000 */  
    if(z_source>=mytablesets_level2.tableset[my_current].z_src_max-1000.*EPS){
      id0=mytablesets_level2.tableset[my_current].n_z_layers-1;
      id1=id0;
      z_other_weight=0;
    }else{
      if(z_source<=mytablesets_level2.tableset[my_current].z_src_min+1000.*EPS){
	id0=0;
	id1=0;
	z_other_weight=0;
      }
    }
  }

  if(id0==-1){
    id0=find(mytablesets_level2.tableset[my_current].z_layer_centers,
	     mytablesets_level2.tableset[my_current].n_z_layers -1,
	     z_source ,
	     last_id0);
    id1=id0+1;

    z_other_weight=
      (z_source-mytablesets_level2.tableset[my_current].z_layer_centers[id0  ])/
      (mytablesets_level2.tableset[my_current].z_layer_centers[id1]-
       mytablesets_level2.tableset[my_current].z_layer_centers[id0])  ;
  }

  if(id0<0){
    log_fatal("Photonics Level2: Error: (c) internal interp error. (z_src)");    
  }

  if(z_other_weight>0.5){
    *idB=mytablesets_level2.tableset[my_current].z_layer_index[id0];
    *idA=mytablesets_level2.tableset[my_current].z_layer_index[id1];	  
    *wB=1-z_other_weight;
    *wA=z_other_weight;
  }else{
    *idA=mytablesets_level2.tableset[my_current].z_layer_index[id0];
    *idB=mytablesets_level2.tableset[my_current].z_layer_index[id1];	  
    *wA=1-z_other_weight;
    *wB=z_other_weight;
  }

  last_id0=id0;
  return 0;
}


  
/* Returns mean photon density for specified source depth, angle, and type */
double I3PhotonicsL2Reader::get_level2_photon_density(float z_source,float angle_track,
						       float l,float rho,float phi,int interp_mask,
						       int *table_id,int *l_bin,int *rho_bin,
						       int *phi_bin)const{
							 

  /* Variables needed for table reading and intratable interpolation */
  Level2_read_type coord;
  Level2_interp_type interp;
  double value_amp=0;
  double value_prb;

  /* new variables for optimzing z lookup */
  int my_current   = mytablesets_level2.current;

  double high_time;
  double low_time;

  double delay;
  int do_times;

  /* I initilized to remove the warning
     no guarantee this is correct --KJM */
  double delay_frac = NAN;
  double delay_frac_comp = NAN;
  int bin = -1; 
  int bin2 = -1;

  /* Reset return variables */
  *table_id=*l_bin=*rho_bin=*phi_bin=-1;

  if(*n_tables==0){
    log_fatal("Photonics Level2: Error [3] No table loaded in the active table set");    
    *table_id=-1;
    return -1;
  }    

  log_trace("photonics called with depth: %f",z_source);
  if(std::isnan(z_source)){
    log_trace("photonics called with strange depth: %f",z_source);
    *table_id=-1;
    return -1;
  }
  log_trace("photonics called with angle: %f",angle_track);
  
  if(std::isnan(angle_track)){
    log_trace("photonics called with strange angle: %f",angle_track);
    *table_id=-1;
    return -1;
  }


  if(mytablesets_level2.tableset[my_current].photorec_mode==1)
    do_times=1;
  else
    do_times=0;

  mytablesets_level2.tableset[my_current].last_probcalc=-1;

  //////////////////////////////////////////////////////////////////////////
  //        First, deal with intrinsic spatial coordinates:
  //////////////////////////////////////////////////////////////////////////

  // we always time-interpolate when applicable
  interp_mask=interp_mask-(interp_mask&L2_INTERP_TIME);

  /* Fill in request vector */
  coord.val[0]=l;
  coord.val[1]=rho;
  coord.val[2]=phi;
  coord.val[3]=0; /* this would be time interpolation. Keep it 0 */

  /* Remove possible ambiguity in tertiary coordinate, phi */
  while(coord.val[2]>M_PI) coord.val[2]=2*M_PI-coord.val[2];
  /* Set interpolation mask */
  interp.mask = coord.interp_mask = interp_mask;

  /* Get bin coordinates of requested values, or return with no answer. */
  if(!get_level2_coord(&(*geo),&coord)){
    log_trace("Photonics Level2: debug: (outside table limits?): "
	      "l, rho, phi:: %g %g %g",coord.val[0],coord.val[1],coord.val[2]);

    *table_id=-1;
    return -1; 
  }

  /* Time dimension preparation for photorec mode:  
     interpolation of time distributions are flux weighted. */
  if (do_times){
  
    /* this is always in the internal type residual time format */
    delay=mytablesets_level2.tableset[my_current].delay_probcalc;

    /* set up time interpolation. (ignore interpolation bitmask) */

    static int lastbin=-1; // remember last location.
    bin=find((*geo).times,(*geo).n[3]-1,delay ,lastbin);
    lastbin=bin;

    /* Check validity of time bin found */
    if(bin<0 || bin>=(*geo).n[3]-1) do_times=0;

    if(bin>((*geo).n[3] -2 )){
      bin=((*geo).n[3] -2 );
      bin2=bin;
      delay_frac=1;
    } else {
      bin2=bin+1;      
      low_time=(*geo).times[bin];
      high_time=(*geo).times[bin2];    
      /* the weight fraction for the lower bin */
      delay_frac=1-(delay-low_time)/(high_time-low_time);
    }
    delay_frac_comp=1-delay_frac;
  }
  
  /*********************************************************************/

  // Find the 4 best tables in angle_track,z_source space:

  double Rplane = rho*cos(phi); 
  // projected distance to OM in the theta plane is:     
  double Dplane = sqrt(l*l + Rplane*Rplane) ;
  // angle to OM from origin is 
  double Aplane ;
  if(l>0){
    Aplane=asin(Rplane/Dplane);
  }else{    
    Aplane=M_PI-atan2(Rplane,Dplane);
  }

  // depth of OM with respect to z_src. (negative since up is 180)
  double Zomo = -cos(Rad(angle_track)+Aplane)*Dplane;

  log_trace(" : %f %f %f",-cos(Rad(angle_track)),angle_track,Aplane/I3Units::degree); 

  int TidA,TidB;
  double TwA,TwB;
  find_theta_table(angle_track, &TidA, &TidB, &TwA, &TwB);
  if(TidA<0){
    *table_id=-1;
    return -1;
  }
    
  // with edge angles the depths would be: (with respect to z_src)
  double ZomA = -cos(Rad(table[TidA].aLow)+Aplane)*Dplane;      
  double ZomB = -cos(Rad(table[TidB].aLow)+Aplane)*Dplane;      

  double shiftA =  Zomo -ZomA ; // how much higher should we go?
  double shiftB =  Zomo- ZomB ;

  if(!(interp_mask&128)){
    shiftA=0;
    shiftB=0;
  }

  int    idA0,idA1,idB0,idB1;
  double  wA0, wA1, wB0, wB1;
  find_z_table(z_source+shiftA, &idA0, &idA1, &wA0, &wA1);
  find_z_table(z_source+shiftB, &idB0, &idB1, &wB0, &wB1);
  if((idA0<0)||(idB0<0)){
    *table_id=-1;
    return -1;
  }

  double WW[4]; // weight
  int ID[4];    // table id

  if(interp_mask&L2_INTERP_ANGLE){
    if(interp_mask&L2_INTERP_DEPTH){
      WW[0]=TwA*wA0; ID[0]=TidA+idA0;
      WW[1]=TwA*wA1; ID[1]=TidA+idA1;
      WW[2]=TwB*wB0; ID[2]=TidB+idB0;
      WW[3]=TwB*wB1; ID[3]=TidB+idB1;
    }else{
      WW[0]=TwA    ; ID[0]=TidA+idA0;
      WW[1]=TwB    ; ID[1]=TidB+idB0;
      WW[2]=0      ; ID[2]=-1       ;
      WW[3]=0      ; ID[3]=-1       ;
    }      
  }else{
    if(interp_mask&L2_INTERP_DEPTH){
      WW[0]=    wA0; ID[0]=TidA+idA0;
      WW[1]=    wA1; ID[1]=TidA+idA1;
      WW[2]=0      ; ID[2]=-1       ;
      WW[3]=0      ; ID[3]=-1       ;
    }else{
      WW[0]=1      ; ID[0]=TidA+idA0;
      WW[1]=0      ; ID[1]=-1       ;
      WW[2]=0      ; ID[2]=-1       ;
      WW[3]=0      ; ID[3]=-1       ;
    }      
  }      

  // just for verification:
  {
    double wsum=0; int tmpi;
    for(tmpi=0;tmpi<4;tmpi++){
      log_trace("Photonics Level2: note. ID %d , W: %f",ID[tmpi],WW[tmpi]);
      wsum+=WW[tmpi];
    }
    if(fabs(wsum-1.0)>1e-6){
      log_error("Photonics Level2: error. Weight sum not 1. it is: %f",wsum);
    }
  }

  *table_id=ID[0]; // this is the table with the heighest weight.
  // ( we could also randomize it according to the weights )


  /* if the center value amplitude is not possitive (ie: zero,
     negative or nan), we have to assume there's no valid time
     distribution for the center value of the hit and the request
     fails. Note: in this implementation not even photorec is allowed
     to continue, we could loosen that up, but this is robust */
  {
    int test_pos=
      table[*table_id].abs.offset[0]*coord.bin[0]+
      table[*table_id].abs.offset[1]*coord.bin[1]+
      table[*table_id].abs.offset[2]*coord.bin[2] ; 
      
      if(!((table[*table_id].abs.cont[test_pos]>0))){
	log_debug("1/3, center bin (amp>0) not true: %f",table[*table_id].abs.cont[test_pos]);
	log_debug("2/3 l, rho, phi:: %d %d %d (bins)",coord.bin[0],coord.bin[1],coord.bin[2]);
	log_debug("3/3 l, rho, phi:: %g %g %g (vals)",coord.val[0],coord.val[1],coord.val[2]);	  
	*table_id=-1;
	return -1; 
      }
  }


  if(*table_id<0){
    log_error("this should not happen");
  }

  /* *************************************************************** */    

  if((interp_mask!=0)&&(level2_interpolate(&coord,&interp))){  /* Interpolate */
    
    /* Set up intrinsic interpolation matrix (index to bin) */
    unsigned int Vapos[L2_INTERP_BINS]; 
    unsigned int Vapost[L2_INTERP_BINS]; 

    int i;
    for(i=0;i<L2_INTERP_BINS;++i){	    
      Vapos[i]=
	table[0].abs.offset[0]*interp.bin[0][i&L2_INTERP_PRIMARY?1:0]+
	table[0].abs.offset[1]*interp.bin[1][i&L2_INTERP_SECONDARY?1:0]+
	table[0].abs.offset[2]*interp.bin[2][i&L2_INTERP_TERTIARY?1:0];
    }
    if(do_times){
      for(i=0;i<L2_INTERP_BINS;++i){	    
	Vapost[i]=Vapos[i]*(*geo).n[3]; 
      }
    }

    value_prb=0.0;
    value_amp=0.0;

    double tmp_prob1;
    double tmp_prob2;
    double tmp_A; 

    double tmp_amp;

    int k;
    for(k=0;k<4;k++){
      if(ID[k]<0){
	break;
      }
      if(WW[k]<1e-9){
	continue;
      }
      
      double tmpW=WW[k];
      int tmpI=ID[k];

      tmp_prob1=0;      
      tmp_prob2=0;      
      tmp_amp=0;

      for(i=0;i<L2_INTERP_BINS;++i){	    
	if(interp.f[i]<0.0) continue;
	tmp_A=interp.f[i]*table[tmpI].abs.cont[Vapos[i]];	 
	if(do_times){
	  tmp_prob2+=tmp_A*(  table[tmpI].prob.cont[Vapost[i]+bin2]);
	  tmp_prob1+=tmp_A*(  table[tmpI].prob.cont[Vapost[i]+bin]);	  
	}
	tmp_amp+=tmp_A;
      }
      value_amp+=tmp_amp*tmpW;
      if(do_times){
	value_prb+=((tmp_prob1*delay_frac)+(tmp_prob2*delay_frac_comp))*tmpW;
      }
    }    

  }else{  /* Non-interpolated value. Note; this is the only way to turn of time-interpolation. */

    /* Calculate position for readout */    
    int pos=0;
    int i;
    for(i=0;i<3;++i) pos+= table[*table_id].abs.offset[i]*coord.bin[i];
    value_amp=table[*table_id].abs.cont[pos];
      
    if(do_times){
      pos=0;
      for(i=0;i<3;++i) pos+= table[*table_id].prob.offset[i]*coord.bin[i];
      value_prb=value_amp*table[*table_id].prob.cont[pos+bin];
    }
  }

  if(do_times)
    mytablesets_level2.tableset[my_current].last_probcalc=value_prb/value_amp;
    
  log_trace("Photonics level2: value_amp calucated as: %f",value_amp);

  *l_bin = coord.bin[0];
  *rho_bin = coord.bin[1];
  *phi_bin = coord.bin[2];

  return value_amp;
}


/* Returns time delay based on input random number for selected table and bin*/
double I3PhotonicsL2Reader::get_level2_photon_delay(double r,int id,int l_bin,int rho_bin,int phi_bin)const{
  int bin;
  double low,high;
  double low_time,high_time;
  double f;

  double outtime;

  size_t pos;

  /* Calculate position for readout */
  pos=table[id].prob.offset[0]*l_bin+table[id].prob.offset[1]*rho_bin+
    table[id].prob.offset[2]*phi_bin;

  /* Find which bin random number landed in */
  /* NB: The output range will depend on bin edge used, see comment of 'binary'
     routine in lib/misc.c */
  bin=binary(&table[id].prob.cont[pos],(*geo).n[3],r);
  
  /* Now do linear interpolation of a fraction with the same random number */
  /* Algoritham for left edged bins 
  if(bin<0) return -1; / / / Illegal return; r<0
  low = table[id].prob.cont[pos+bin];
  high = bin<geo.n[3]-1?table[id].prob.cont[pos+bin+1]:1; 
  */
  /* Algoritham for right edged bins */
  if(bin==(*geo).n[3]){
    log_debug("Photonics Level2: get_level2_photon_delay: warn/error: illegal bin: %d (n=%d, r=%f, prob(n-1)):%f",
	      bin,(*geo).n[3],r,table[id].prob.cont[(*geo).n[3]-1]);
    
    return  (*geo).edge[3][(*geo).n[3]-1];/* Illegal return; r>1*/
  }; 


  ++bin; /* Need to move bin up by one since binary returns in
	    the range [-1,geo.n[3]-1] */
  low = bin>0?table[id].prob.cont[pos+bin-1]:0;
  high = table[id].prob.cont[pos+bin];


  /* if high is zero, the below algorithms fail, but we know the answer directly */ 
  if(high<=0){
    log_debug("Photonics Level2: info: get_level2_photon_delay: note: edge: %g, bin: %d",high,bin);
    return (*geo).edge[3][0];
  }

  /* Find bin edges */
  low_time = bin<(*geo).n[3]?(*geo).edge[3][bin]:(*geo).edge[3][bin-1];
  high_time =  bin<(*geo).n[3]?(*geo).edge[3][bin+1]:(*geo).edge[3][bin];

  /* Interpolate and return random delay time */
  f=(r-low)/(high-low);

  /* ************* convert to standard ng times ************ */

  outtime = low_time+f*(high_time-low_time);

  /* warning, in direct hit frame, cherenkov angle is only right with muon beta = 1 */
  outtime = residual_convert(
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      pref_level2.external_ref_ng,
      outtime,
      0.5 * ( ((*geo).edge[1][rho_bin] + rho_bin<1?0:(*geo).edge[1][rho_bin-1])) ,
      0.5 * ( ((*geo).edge[0][  l_bin] +   l_bin<1?0:(*geo).edge[0][  l_bin-1])) ,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);

  return outtime;
}

/* Returns probability of given time delay based for selected table and bin*/
double I3PhotonicsL2Reader::get_level2_photon_delay_prob(double delay,int id,int l_bin,int rho_bin,int phi_bin)const{
  int bin;
  double low,high;
  double low_time,high_time;
  size_t pos;

  /* convert to internal residual time format */
  delay = residual_convert(
      pref_level2.external_ref_ng,
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      delay,
      ((*geo).edge[1][rho_bin] ) ,
      ((*geo).edge[0][  l_bin] ) ,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);

  /* Calculate position for readout */
  pos=table[id].prob.offset[0]*l_bin+table[id].prob.offset[1]*rho_bin+
    table[id].prob.offset[2]*phi_bin;

  /* Find which bin random number landed in */

  static int lastbin=-1;
  bin=find((*geo).edge[3],(*geo).n[3],delay,lastbin);
  lastbin=bin;

  /* Check validity of bin found */
  if(bin<0 || bin>=(*geo).n[3]) return -1;

  /* Find bin edges */
  low_time = bin<(*geo).n[3]?(*geo).edge[3][bin]:(*geo).edge[3][bin-1];
  high_time =  bin<(*geo).n[3]?(*geo).edge[3][bin+1]:(*geo).edge[3][bin];

  /* Algoritham for right edged bins */
  low = bin>0?table[id].prob.cont[pos+bin-1]:0;
  high = table[id].prob.cont[pos+bin];

  /* Return differential distribution of table content */
  return (high-low)/(high_time-low_time);
}


/* Returns time delay based on input random number for selected table and bin
   of starting and stopping point..*/
double I3PhotonicsL2Reader::get_finite_level2_photon_delay(double r,int id1,int l_bin1,int rho_bin1,
							    int phi_bin1,int id2,int l_bin2,
							    int rho_bin2,int phi_bin2)const{
  int bin,i;
  double low,high;
  double low_time,high_time;
  double f;
  size_t pos1,pos2,pos1a,pos2a;
  float a1,a2;
  float outtime;

  static float *prob=NULL;

  /* If this is a first, call allocate memory for temporary array */
  if(prob==NULL){
    if((prob=(float *)malloc((*geo).n[3]*sizeof(float)))==NULL){
      log_fatal("Photonics Level2: Unable to allocate memory for temporary array.");
    }
  }
   
  /* Calculate positions for readout */
  pos1=table[id1].prob.offset[0]*l_bin1+table[id1].prob.offset[1]*rho_bin1+
    table[id1].prob.offset[2]*phi_bin1;
  pos2=table[id2].prob.offset[0]*l_bin2+table[id2].prob.offset[1]*rho_bin2+
    table[id2].prob.offset[2]*phi_bin2;
  pos1a=table[id1].abs.offset[0]*l_bin1+table[id1].abs.offset[1]*rho_bin1+
    table[id1].abs.offset[2]*phi_bin1;
  pos2a=table[id2].abs.offset[0]*l_bin2+table[id2].abs.offset[1]*rho_bin2+
    table[id2].abs.offset[2]*phi_bin2;

  /* Evaluate temporay array */
  a1=table[id1].abs.cont[pos1a];
  a2=table[id2].abs.cont[pos2a];

  for(i=0;i<(*geo).n[3];++i){
    // protect against negative amplitudes
    prob[i]=((a1-a2)<=0)?0:(a1*table[id1].prob.cont[pos1+i]-a2*table[id2].prob.cont[pos2+i])/(a1-a2);
  };

#ifdef DEBUG
  static int count_msg=0; 
  if(fabs(prob[i-1]-1)>ERROR_MARGIN){
    if(count_msg==9){
      log_debug("get_finite_level2_photon_delay: (From now on I'm not reporting this): ");
    }
    if(count_msg<=9){
      log_debug("get_finite_level2_photon_delay: Note: Tot prob not 1. (1-p=%g)",1.0-prob[i-1]);
    };
    
    if(count_msg<100) // prevent looping...
      count_msg++;
  }
#endif // DEBUG

  prob[i-1]=1.0; // this should always be the case...

  /* Find which bin random number landed in */
  /* NB: The output range will depend on bin edge used, see comment of 'binary'
     routine in lib/misc.c */
  bin=binary(prob,(*geo).n[3],r);

  /* Now do linear interpolation of a fraction with the same random number */
  /* Algoritham for left edged bins
  if(bin<0) return -1; //Illegal return; r<0
  low = table[id].prob.cont[pos+bin];
  high = bin<geo.n[3]-1?table[id].prob.cont[pos+bin+1]:1; 
  */
  /* Algorithm for right edged bins */
  if(bin==(*geo).n[3]){

    log_debug("Photonics Level2: warn/err (b) get_finite_level2_photon_delay: error: illegal bin: %d (n=%d, r=%f, prob(n-1)):%f",bin,(*geo).n[3],r,prob[(*geo).n[3]-1]);

    return  (*geo).edge[3][(*geo).n[3]-1];/* Illegal return; r>1*/
  }; 

  ++bin; /* Need to move bin up by one since binary returns in
	    the range [-1,geo.n[3]-1] */
  low = bin>0?prob[bin-1]:0;
  high = prob[bin];

  /* if high is zero, the below algorithms fail, but we know the answer directly */ 
  if(high<=0){
    log_debug("Photonics Level2: debug: get_finite_level2_photon_delay: note: edge: %g, bin: %d",high,bin);

    return (*geo).edge[3][0];
  }

  /* Find bin edges */
  low_time = bin<(*geo).n[3]?(*geo).edge[3][bin]:(*geo).edge[3][bin-1];
  high_time =  bin<(*geo).n[3]?(*geo).edge[3][bin+1]:(*geo).edge[3][bin];

  /* Interpolate and return random delay time */
  f=(r-low)/(high-low);
  outtime = low_time+f*(high_time-low_time);

  outtime = residual_convert(
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      pref_level2.external_ref_ng,
      outtime,
      0.5 * ( ((*geo).edge[1][rho_bin1] + rho_bin1<1?0:(*geo).edge[1][rho_bin1-1])) ,
      0.5 * ( ((*geo).edge[0][  l_bin1] +   l_bin1<1?0:(*geo).edge[0][  l_bin1-1])) ,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);

  return outtime;
}

/* Returns probability of given time delay based on input random number for 
   selected table and bin of starting and stopping point. Finite muons. */
double I3PhotonicsL2Reader::get_finite_level2_photon_delay_prob(double delay,int id1,int l_bin1,
								 int rho_bin1,int phi_bin1,int id2,
								 int l_bin2,int rho_bin2,
								 int phi_bin2)const{
  int bin;
  double low1,high1,low2,high2;
  double low_time,high_time;
  size_t pos1,pos2,pos1a,pos2a;
  float a1,a2;

  /* convert to internal residual time format */
  delay = residual_convert(
      pref_level2.external_ref_ng,
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      delay,
      ((*geo).edge[1][rho_bin1] ) ,
      ((*geo).edge[0][  l_bin1] ) ,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);


  /* Find which bin random number landed in */
  static int lastbin=-1;
  bin=find((*geo).edge[3],(*geo).n[3],delay,lastbin);
  lastbin=bin;

  /* Check validity of bin found */
  if(bin<0 || bin>=(*geo).n[3]) return -1;

  /* Find bin edges */
  low_time = bin<(*geo).n[3]?(*geo).edge[3][bin]:(*geo).edge[3][bin-1];
  high_time =  bin<(*geo).n[3]?(*geo).edge[3][bin+1]:(*geo).edge[3][bin];

  /* Calculate position for readout */
  pos1=table[id1].prob.offset[0]*l_bin1+table[id1].prob.offset[1]*rho_bin1+
    table[id1].prob.offset[2]*phi_bin1;
  pos2=table[id2].prob.offset[0]*l_bin2+table[id2].prob.offset[1]*rho_bin2+
    table[id2].prob.offset[2]*phi_bin2;
  pos1a=table[id1].abs.offset[0]*l_bin1+table[id1].abs.offset[1]*rho_bin1+
    table[id1].abs.offset[2]*phi_bin1;
  pos2a=table[id2].abs.offset[0]*l_bin2+table[id2].abs.offset[1]*rho_bin2+
    table[id2].abs.offset[2]*phi_bin2;

  /* Algoritham for right edged bins */
  low1 = bin>0?table[id1].prob.cont[pos1+bin-1]:0;
  high1 = table[id1].prob.cont[pos1+bin];
  low2 = bin>0?table[id2].prob.cont[pos2+bin-1]:0;
  high2 = table[id2].prob.cont[pos2+bin];
  a1 = table[id1].abs.cont[pos1a];
  a2 = table[id2].abs.cont[pos2a];

  /* Return dP/dt per unit time */
  return (a1*(high1-low1)-a2*(high2-low2))/((high_time-low_time)*(a1-a2));
}


/* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\/////////////////////////////////  */
/*  New way of dealing with _muons_.                                   */
/* /////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\  */

/* This is a general procedure for level2 amplitude queries.           */ 
/*                                                                     */
double I3PhotonicsL2Reader::get_level2_general_photon_density(
    float angle_track,    /* IN   track theta                               */
    float rho,            /* IN   track rho                                 */
    float phi,            /* IN   track phi                                 */
    float l_start,        /* IN   start                                     */
    float l_stop,         /* IN                                             */
    float z_start,        /* IN   track start depth                         */
    float z_stop,         /* IN   track stop depth                          */
    int interp_mask,      /* IN   photonics interpolation mask              */
    int *tableset_id,     /* OUT  'A' bins refer to the table of higher amp */
    int *table_id_A,      /* OUT  gets set to -1 if amp<=0 or no bin found  */
    int *table_id_B,      /* OUT  gets set to -1 if infinite track          */
    int *l_bin_A,         /* OUT                                            */
    int *l_bin_B,         /* OUT                                            */
    int *rho_bin_A,       /* OUT                                            */
    int *rho_bin_B,       /* OUT                                            */
    int *phi_bin_A,       /* OUT                                            */
    int *phi_bin_B )      /* OUT                                            */
{                         /*(OUT)  The function itself returns the amplitude*/
                          /*       or -1 if no bin found or amp<=0          */
  double amplitude_A;
  double amplitude_B;
  double amp=-1.0;

  int isstarting=0;  /* start/stop selection                               */
  int isinfinite=0;  /* track is infinite (but possibly starting/stopping) */
                     /* with regard to observer.                           */

  int i,tmp_state;
  float flt_a,flt_b;
  double z_candidate=0,z_candidate_extend=0;
  double l_candidate=0,l_candidate_extend=0;

  double cosinv;
  double l_tmp;
  double z_tmp;

  int    havepair=0;
  double track_length;
  double l_range_min;
  double l_range_max;
  double track_length_ref;

 /* I initilized to remove the warning
     no guarantee this is correct --KJM */
  double probcalc_A=NAN;
  double probcalc_B=NAN;

  if(*n_tables==0){
    log_fatal("Photonics Level2: [1] No table loaded in the active table set");    
    *table_id_A=-1;
    return -1;
  }

  /* if just one length bin, this function should not be used. */
  if(mytablesets_level2.tableset[mytablesets_level2.current].header->n[0]==1){
    *table_id_A=-1;
    log_fatal("Photonics Level2: Error, you cant do finite (starting/stopping) tracks with this table.");    
    //    return -1;
  }


  // todo: we may need more tests here...
  if(std::isnan(angle_track)){
    log_warn("photonics level 2 : called with strange angle: %f",angle_track);
    *table_id_A=-1;
    return -1;
  }
  if(std::isnan(z_start)){
    log_warn("photonics level 2 : called with strange z_start: %f",z_start);
    *table_id_A=-1;
    return -1;
  }

  track_length=l_start-l_stop;

  track_length_ref=
    mytablesets_level2.tableset[mytablesets_level2.current].ref_dist+
    mytablesets_level2.tableset[mytablesets_level2.tableset[mytablesets_level2.current].pair_id].ref_dist;

  log_trace("Photonics Level2: info: Considering new track.");

  log_debug("Photonics Level2: PRE: zstart: %6.1f zstop: %6.1f l_start: %6.1f l_stop: %6.1f",z_start,z_stop,l_start,l_stop);

  /* If we really have a pair */
  if(mytablesets_level2.tableset[mytablesets_level2.current].pair_id != mytablesets_level2.current){

    log_trace("Photonics Level2: info: We have a pair.");

    swap_tableset_pair_level2(START); /* just to make it well defined */

    /* suggestion; this could be optimized. Some checks are actually
       redundant. Also note that this algorithm
       assumes we loaded stopping and starting table sets with
       matching binning, same layers. */

    /**************************************************************
     * is any side of the track outside the tabulated volume? (if we have bulk ice, this is always false)
     **************************************************************/
    if ((mytablesets_level2.tableset[mytablesets_level2.current].z_src_max!=
	 mytablesets_level2.tableset[mytablesets_level2.current].z_src_min)&&
	((z_start >mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ) || 
	 (z_start <mytablesets_level2.tableset[mytablesets_level2.current].z_src_min ) ||
	 (z_stop  >mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ) || 
	 (z_stop  <mytablesets_level2.tableset[mytablesets_level2.current].z_src_min ))){

      /*************************************************************
       * is both sides outside the tabulated volume?...  
       **************************************************************/
      if (((z_start >mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ) || 
	   (z_start <mytablesets_level2.tableset[mytablesets_level2.current].z_src_min )) &&
	  ((z_stop  >mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ) || 
	   (z_stop  <mytablesets_level2.tableset[mytablesets_level2.current].z_src_min ))){

	log_debug("Photonics Level2:  info: Both sides outside detector.");

	/* ... then switch starting/stopping in the middle of the volume
	   and don't use the other member in the pair again*/	

	/* upgoing: */
	if ((z_stop>z_start)){	  
	  if (z_start-l_start*cos(Rad(angle_track)) > 
	      (mytablesets_level2.tableset[mytablesets_level2.current].z_src_max+
	       mytablesets_level2.tableset[mytablesets_level2.current].z_src_min)/2.0){
	    swap_tableset_pair_level2(START); /* just to make it well defined */
	    isstarting=1;
	    log_trace("Photonics Level2:  info: Upgoing,starting.");
	  }else{
	    isstarting=0;
	    log_trace("Photonics Level2:  info: Upgoing,stopping.");
	  }

	} else { /* downgoing or flat */
	  if (z_start-l_start*cos(Rad(angle_track)) < 	  
	      (mytablesets_level2.tableset[mytablesets_level2.current].z_src_max+
	       mytablesets_level2.tableset[mytablesets_level2.current].z_src_min)/2.0){
	    isstarting=1;
	    log_trace("Photonics Level2:  info: Downgoing,starting.");
	  }else{
	    isstarting=0;
	    log_trace("Photonics Level2:  info: Downgoing,stopping.");
	  }
	}
      }	else {

	/*************************************************************
	 * If _one_ end is utside detector, select the other type:
         * (if the starting side is outside, selecte stopping , vv. )
	 * but allow the other type to be considered later.
	 **************************************************************/

	if ((z_start >mytablesets_level2.tableset[mytablesets_level2.current].z_src_max ) || 
	    (z_start <mytablesets_level2.tableset[mytablesets_level2.current].z_src_min )){
	  isstarting=0;
	  log_trace("Photonics Level2:  info: started outside.");
	}
	else
	  {
	    isstarting=1;
	    log_trace("Photonics Level2:  info: stopped outside.");
	  }

      }
 
    } else { /* both ends inside valid ice: */

      /* Are we closer to the end than the start of the track?           */
      if (l_stop<(-l_start)) { /* starting */
	isstarting=1;      	
      } else {                 /* stopping */
	isstarting=0;      
      }
    } 
 
    if (isstarting==1){
      swap_tableset_pair_level2(START);
      log_trace("Photonics Level2:  info: Selected starting.");
    } else {
      swap_tableset_pair_level2(STOP);
      log_trace("Photonics Level2:  info: Selected stopping.");
    }

    havepair=0;         /* Don't consider the bad pair member again. */
  
  } else { /* if we have only a single table set */      
    
    isstarting=(mytablesets_level2.tableset[mytablesets_level2.current].type==START)?1:0;

  }

  /*   suggestion: 'havepair' always ends up beeing set to 0, so that
       should be changed/removed. This must be checked for all
       possible cases, with layered and bulk tables.
       havepair == 0 means we dont need to consider the other table type anymore in this request.
  */

  /* don't continue if we are clearly before the start of the selected table or clearly after the end*/
  if ((l_stop  >  mytablesets_level2.tableset[mytablesets_level2.current].ref_dist )||
      (l_start < -mytablesets_level2.tableset[mytablesets_level2.current].ref_dist )){

    log_debug("Photonics Level2: info: clearly after or clearly before...");

    *table_id_A=-1;
    return -1;        
  }

  *tableset_id =  mytablesets_level2.current; 

  l_range_max= 
    mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][1] -
    mytablesets_level2.tableset[mytablesets_level2.current].ref_dist_fraction;
  l_range_min= 
    mytablesets_level2.tableset[mytablesets_level2.current].header->range[0][0] +
    mytablesets_level2.tableset[mytablesets_level2.current].ref_dist_fraction;

  /* if we are far away from other vertex, we can select infinite directly */
  if(track_length>track_length_ref){
    if (((isstarting==1)&&(l_stop<l_range_min))||((isstarting==0)&&(l_start>l_range_max))){
      isinfinite=1;      
    }
  } else {
    isinfinite=0;
       
    /* For very short tracks, this code avoids a thin band of far to high values */
    /* close to the edge of valid table space.                                   */       
    if (4.0*track_length<track_length_ref) {
      if ((isstarting==1)&&(l_stop<(l_range_min))) {
	l_stop=l_range_min;      
      }
      if ((isstarting==0)&&(l_start>(l_range_max))) {
	l_start=l_range_max;      
      }
    }
  }

  /* ***************************************************************** */
  /* For infinite track regions, translate l that are to large to      */
  /* valid table locations. The two blocks are nearly identical.       */
  /* -- one for starting and one for stopping tablesets.               */
  /*                                                                   */
  if(pref_level2.zl_matching==1) {
    if(isinfinite==1||havepair==0){ 			  
      log_trace("Photonics Level2:  info: Doing lz_matching");

      if ((isstarting==1)&&(l_start>(l_range_max))) {
	if (fabs(90.0-angle_track)<0.01) {
	  l_start=l_range_max;
	} else {     
	  cosinv=1.0/(-cos(Rad(angle_track)));
	 
	  /* minimum allowed value */
	  l_candidate=mytablesets_level2.tableset[mytablesets_level2.current].ref_dist;
	  tmp_state=-1;

	  for(i=0;i<mytablesets_level2.tableset[mytablesets_level2.current].n_z_layers;++i){ 
	    z_tmp=
	      mytablesets_level2.tableset[mytablesets_level2.current].z_layer_centers[i];
	    l_tmp=l_start - (z_tmp-z_start)*cosinv;

	    if (((l_tmp<=l_range_max) && (l_tmp>l_candidate)) ){
	      z_candidate=z_tmp;
	      l_candidate=l_tmp;
	      tmp_state=1;
	    }

	    if (((tmp_state==-1) || (l_tmp<l_candidate_extend)) &&
		(l_tmp>l_range_max)){
	      z_candidate_extend=z_tmp;
	      l_candidate_extend=l_tmp;
	      tmp_state=0;
	    }
	  }

	  if (tmp_state==1){
	    l_start=l_candidate;
	    z_start=z_candidate; 
	  } else
	    if (tmp_state==0) {	  
	      l_start=l_range_max;	     
	      z_start=z_candidate_extend; 
	    } else {	
		/* This mean that we revert backwards to get valid
		   table information. 
		   This will hapend for example in any of these cases:
		     1. bulk ice, track longer than tabulated muon.
		     2. horisontal track in layered ice, track longer than tabulated muon.		   
		*/
	      log_debug("Photonics Level2:  Notice: reverted to l_range_max for l:%g z:%g theta:%g",
			l_start,z_start,angle_track);

	      l_start=l_range_max;
	    }

	} /* end of cosinv!=0 */
      } /* end of isstarting and l outside table */
      
      if ((isstarting==0)&&(l_stop<(l_range_min))) {
	if (fabs(90.0-angle_track)<0.01) {
	  l_stop=l_range_min;
	} else {     
	  cosinv=1.0/(-cos(Rad(angle_track)));
	 
	  /* minimum allowed value */
	  l_candidate=-mytablesets_level2.tableset[mytablesets_level2.current].ref_dist;
	  tmp_state=-1;

	  for(i=0;i<mytablesets_level2.tableset[mytablesets_level2.current].n_z_layers;++i){ 
	    z_tmp=
	      mytablesets_level2.tableset[mytablesets_level2.current].z_layer_centers[i];
	    l_tmp=l_stop  - (z_tmp-z_stop )*cosinv;

	    if (((l_tmp>=l_range_min) && (l_tmp<l_candidate)) ){
	      z_candidate=z_tmp;
	      l_candidate=l_tmp;
	      tmp_state=1;
	    }

	    if (((tmp_state==-1) || (l_tmp>l_candidate_extend)) &&
		(l_tmp<l_range_min)){
	      z_candidate_extend=z_tmp;
	      l_candidate_extend=l_tmp;
	      tmp_state=0;
	    }

	  }

	  if (tmp_state==1){
	    l_stop =l_candidate;
	    z_stop =z_candidate; 
	  } else {	  
	    if (tmp_state==0) {
	      l_stop =l_range_min;	     
	      z_stop =z_candidate_extend; 
	    } else {
		/* Read comment above, for starting case */
	      log_debug("Photonics Level2:  Notice: reverted to l_range_min for l:%g z:%g theta:%g",
			l_stop ,z_stop ,angle_track);
	      l_stop=l_range_min;
	    }
	  }

	} /* end of cosinv!=0 */
      } /* end of isstarting and l outside table */

    } /* end of isinfinite */     
    
    /*                                                               */
    /* End of code for translations of z and l                       */
    /*                                                               */
    /* ************************************************************* */

    log_debug("Photonics Level2: POST: zstart: %6.1f zstop: %6.1f l_start: %6.1f l_stop: %6.1f",z_start,z_stop,l_start,l_stop);
  
  }

  /* If we ended up getting a stopping table set we must exchange */
  /* start/stop point                                             */

  if (isstarting==0){
    flt_a=l_start; 
    flt_b=z_start; 
    l_start=l_stop;
    z_start=z_stop;
    l_stop=flt_a; 
    z_stop=flt_b;   
  }
  
  /* Now request starting if using starting tableset, stopping if stopping */

  amplitude_A =  get_level2_photon_density(
					   z_start,      /*    float  */  
					   angle_track,	 /*    float  */ 
					   l_start,	 /*    float  */
					   rho,		 /*    float  */  
					   phi,	         /*    float  */  
					   interp_mask,	 /*    int    */ 
					   table_id_A,   /*OUT int    */  
					   l_bin_A,	 /*OUT int    */  
					   rho_bin_A,	 /*OUT int    */  
					   phi_bin_A );  /*OUT int    */ 

  /* If we are in photorec mode, Store the differential probability value calculated */
  if(mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode==1){
    probcalc_A = mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc;
  }

  if (amplitude_A<=0) {
    log_trace("Photonics Level2: info: Amp A <= 0.");

    *table_id_A=-1;
    return -1;
  }

  /* Now request stopping if using starting tableset, starting if stopping */  
  if (isinfinite) {
    amplitude_B = 0;
  } else {
    
    amplitude_B =  get_level2_photon_density(
					     z_stop,     /*    float  */
					     angle_track,/*    float  */
					     l_stop,	 /*    float  */
					     rho,	 /*    float  */
					     phi,	 /*    float  */
					     interp_mask,/*    int    */
					     table_id_B, /*OUT int    */
					     l_bin_B,	 /*OUT int    */
					     rho_bin_B,	 /*OUT int    */
					     phi_bin_B );/*OUT int    */

    /* Store the photore differential probability value calculated */
    if(mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode==1){
      probcalc_B = mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc;
    }
  }

  if (amplitude_B<=0){
    log_trace("Photonics Level2:  Amp B <= 0.");
    *table_id_B=-1;
  }

  if (amplitude_A!=-1) {
    amp= amplitude_A - (amplitude_B<0?0:amplitude_B);

    /* Calculate the prob value as (aA*pA-aB*pB)/(aA-aB) */
    if(mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode==1){
      if(probcalc_A<0){
	mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc=-1;
      } else {
	mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc = 
	  ((amplitude_A*probcalc_A) - 
	   (amplitude_B<0?0:amplitude_B)*(probcalc_B<0?0:probcalc_B))/amp;
      }
    }
  }

  if (amp<=0) {       /* if the amplitude is 0 it makes no */
    *table_id_A=-1;   /* sense to ask for delay or prob    */
    return -1;
  }

  if(*table_id_B!=-1){
    /* if start and stop center bin points are equal, we will not
       be able to use that point for time requests. Therefore we have to report
       that we don't know the light flux for this request */
    if ((*l_bin_B   ==*l_bin_A   )&&
	(*rho_bin_B ==*rho_bin_A )&&
	(*phi_bin_B ==*phi_bin_A )&&    
	(*table_id_B==*table_id_A)){
      
      log_debug("Photonics Level2: debug: Non-interpolated amplitudes canceled.");	
      *table_id_A=-1; 
      return -1;
    }

  }

  log_trace("Photonics Level2: debug: Returning sane amplitude %g",amp);
  
  return amp;
}


/* This is a general funtion for level2 delay queries.                     */ 
double I3PhotonicsL2Reader::get_level2_general_delay(				
    double r,             /* IN   the random number                        */
    int tableset_id,      /* IN                                            */
    int table_id_A,       /* IN                                            */
    int table_id_B,	  /* IN                                            */
    int l_bin_A,	  /* IN                                            */
    int l_bin_B,	  /* IN                                            */
    int rho_bin_A,	  /* IN                                            */
    int rho_bin_B,	  /* IN                                            */
    int phi_bin_A,	  /* IN                                            */
    int phi_bin_B )	  /* IN                                            */
{                         /*(OUT)  The function itself returns delay       */
                          /*       -1 if question not relevant (no amp)    */
                          /* NOTE: this assumes that -1 is not a valid     */
                          /*       respons. This is _currently_ fine.      */
  double out_time;

  if (table_id_A==-1) {    
    log_error("Photonics Level2: get_level2_general_delay: Fatal error: negative major bin.");    
    return -1;
  }

  if ((tableset_id==-1)||(tableset_id != swap_tableset_level2( tableset_id ))) {
    log_fatal("Photonics Level2: get_level2_general_delay:: Could not swap to set %d",tableset_id);
  } 
  
  /* Returns time delay based on input random number for selected table and bin*/

  if (table_id_B==-1) { 

    out_time =  get_level2_photon_delay(
       r,         	  /*  double r,  */
       table_id_A,	  /*  int id,        */	   
       l_bin_A,   	  /*  int l_bin,     */
       rho_bin_A, 	  /*  int rho_bin    */
       phi_bin_A);	  /*  int phi_bin    */
								    
  } else {

    out_time =  get_finite_level2_photon_delay(
       r,                 /* double r,       */
       table_id_A,	  /* int    id1,     */	 
       l_bin_A,	      	  /* int    l_bin1,  */
       rho_bin_A,	  /* int    rho_bin1,*/	 
       phi_bin_A,	  /* int    phi_bin1,*/	 
       table_id_B,	  /* int    id2,     */	 
       l_bin_B,	      	  /* int    l_bin2,  */
       rho_bin_B,	  /* int    rho_bin2,*/	 
       phi_bin_B);	  /* int    phi_bin2 */	 
  }

  // sorry that we actually use magic numbers, but for now we have to
  // protect -1. if we really want -1, set it to -1+eps
  if(out_time==-1)
    out_time=out_time+EPS;

  return out_time;
    
}

/* This is a general funtion for level2 delay queries.                     */ 
double I3PhotonicsL2Reader::get_level2_general_delay_prob(				
    double delay,         /* IN   the time delay                           */
    int tableset_id,      /* IN                                            */
    int table_id_A,       /* IN                                            */
    int table_id_B,	  /* IN                                            */
    int l_bin_A,	  /* IN                                            */
    int l_bin_B,	  /* IN                                            */
    int rho_bin_A,	  /* IN                                            */
    int rho_bin_B,	  /* IN                                            */
    int phi_bin_A,	  /* IN                                            */
    int phi_bin_B ) 	  /* IN                                            */
{                         /*(OUT)  The function itself returns prob        */
                          /*       -1 if question not relevant (no amp)    */

  double out_prob;

  if (table_id_A==-1) {    
    return -1;
  }

  if ((tableset_id==-1)||(tableset_id != swap_tableset_level2( tableset_id ))) {
    log_fatal("Photonics Level2: get_level2_general_delay_prob:: Could not swap to set %d",tableset_id);
  } 
  
  /* Returns delay prob based on delay and selected table and bin*/
  if (table_id_B==-1) { 

    out_prob =  get_level2_photon_delay_prob(
       delay,         	  /*  double delay,  */
       table_id_A,	  /*  int id,        */	   
       l_bin_A,   	  /*  int l_bin,     */
       rho_bin_A, 	  /*  int rho_bin    */
       phi_bin_A);	  /*  int phi_bin    */

  } else {

    out_prob =  get_finite_level2_photon_delay_prob(
       delay,             /* double delay,   */ 
       table_id_A, 	  /* int    id1,     */ 
       l_bin_A,    	  /* int    l_bin1,  */ 
       rho_bin_A,  	  /* int    rho_bin1,*/ 
       phi_bin_A,  	  /* int    phi_bin1,*/ 
       table_id_B, 	  /* int    id2,     */ 
       l_bin_B,    	  /* int    l_bin2,  */ 
       rho_bin_B,  	  /* int    rho_bin2,*/ 
       phi_bin_B); 	  /* int    phi_bin2 */ 
  }
 
  return out_prob;
   
}

/* ********************* Now, photorec ************* */

/* Reader for the 'photorec' reconstruction pdf framework              */
/* Supports multi linear interpolation in all dimentions, of both      */
/* abs and prob.                                                       */
int I3PhotonicsL2Reader::get_level2_photorec(
   float angle_track,    /* IN   track theta                               */
   float rho,            /* IN   track rho                                 */
   float phi,            /* IN   track phi                                 */
   float l_start,        /* IN   start                                     */
   float z_start,        /* IN   track start depth                         */
   float delay,          /* IN   residual time                             */
   double *amp,          /* OUT  amplitude (poisson mean)                  */
   double *prob,         /* OUT  differential prob: dp/dt integrates to 1  */
   int interp_mask,      /* IN   photonics interpolation mask              */
   double energy)        /* IN   vertex energy (GeV). 0 means no scaling   */
  const                  /*      the particle type is read from the table  */
{

  //  interp_mask=0;

  int table_id_A;
  int l_bin_A;
  int rho_bin_A;
  int phi_bin_A;

  double amp_x;
  double prob_x;

  if(*n_tables==0){
    log_fatal("Photonics Level2: get_level2_photorec: No table loaded in the active table set");    
    *amp=-1;
    return false;
  }

  if (mytablesets_level2.tableset[mytablesets_level2.current].pair_id!=
      mytablesets_level2.current){
    log_fatal("Photonics Level2: Don't use get_level2_photorec() with paired (starting/stopping) tablesets."
	      "Use get_level2_photorec_finite() instead.");    
    
    *amp=-1;
    return false;
  }


  if(mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode!=1){
    log_fatal("Photonics Level2: Can't use a normal level2 table using"
	      "tools for differential 'photorec' tables. Contact your local photonics dealer.");    
    *amp=-1;
    return false;
  }

  mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc=-1;
  mytablesets_level2.tableset[mytablesets_level2.current].delay_probcalc=-1;
 
  /* translate z to let l = 0 if table is degenerate */
  if(mytablesets_level2.tableset[mytablesets_level2.current].header->n[0]==1){
    z_start=z_start - l_start*cos(Rad(angle_track));
    l_start=0;
  }


  /* convert to internal residual time format */
  delay = residual_convert(
      pref_level2.external_ref_ng,
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      delay,
      rho,
      l_start,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);

  mytablesets_level2.tableset[mytablesets_level2.current].delay_probcalc=delay; 
  
  amp_x=get_level2_photon_density( z_start,    /*    float   */  
				angle_track,   /*    float   */ 
				l_start,       /*    float   */
				rho,	       /*    float   */  
				phi,	       /*    float   */  
				interp_mask,   /*    int     */ 
				&table_id_A,   /*    OUT int */  
				&l_bin_A,      /*    OUT int */  
				&rho_bin_A,    /*    OUT int */  
				&phi_bin_A );  /*    OUT int */ 
  
  prob_x=mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc; 

  *prob=prob_x;

  if(prob_x<0)
    *prob=-1;
  
  if(amp_x<0){
    *amp=-1;
    return false;  
  }

  if(energy!=0){
    amp_x = amp_x * light_for_source((source_type)mytablesets_level2.tableset[mytablesets_level2.current].header->source,energy) ;
  }

  *amp =amp_x;

  return true;  
}

/* Reader for the 'photorec' reconstruction pdf framework              */
/* Supports multi linear interpolation in all dimentions, of both      */
/* abs and prob.                                                       */
int I3PhotonicsL2Reader::get_level2_photorec_finite(
    float angle_track,    /* IN   track theta                               */
    float rho,            /* IN   track rho                                 */
    float phi,            /* IN   track phi                                 */
    float l_start,        /* IN   start                                     */
    float l_stop,         /* IN                                             */
    float z_start,        /* IN   track start depth                         */
    float z_stop,         /* IN   track stop depth                          */
    float delay,          /* IN   residual time                             */
    double *amp,          /* OUT  amplitude (poisson mean)                  */
    double *prob,         /* OUT  differential prob: dp/dt  unit: [1/ns]    */
    int interp_mask,      /* IN   photonics interpolation mask              */
    double energy)        /* IN   the energy (GeV) or 0 for no scaling      */
{                         /*      the particle type is read from the table  */
                          /*(OUT)  The function itself returns the amplitude*/
                          /*       or -1 if no bin found or amp<=0          */
  //  interp_mask=0; 

  // dummy variables:
  int tableset_id, table_id_A, table_id_B, l_bin_A, l_bin_B, rho_bin_A, rho_bin_B, phi_bin_A, phi_bin_B ;

  double amp_x;
  double prob_x;

  if(*n_tables==0){
    log_fatal("Photonics Level2: get_level2_photorec_finite: No table loaded in the active table set");    
    *amp=-1;
    return false;
  }

  if(mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode!=1){
    log_fatal("Photonics Level2: Can't use a normal level2 table using"
	      "tools for differential 'photorec' tables. Contact your local photonics dealer.");    
    *amp=-1;
    return false;
  }


  mytablesets_level2.tableset[mytablesets_level2.current].photorec_mode=1;
  mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc=-1;
  mytablesets_level2.tableset[mytablesets_level2.current].delay_probcalc=delay;

  /* convert to internal residual time format */
  delay = residual_convert(
      pref_level2.external_ref_ng,
      mytablesets_level2.tableset[mytablesets_level2.current].header->ref_ng,
      delay,
      rho,
      l_start,
      mytablesets_level2.tableset[mytablesets_level2.current].inv_sin_ch,
      mytablesets_level2.tableset[mytablesets_level2.current].header->tref);

  mytablesets_level2.tableset[mytablesets_level2.tableset[mytablesets_level2.current].pair_id].delay_probcalc=delay; 

  amp_x = get_level2_general_photon_density(
     angle_track,    /* IN   track theta                               */
     rho,            /* IN   track rho                                 */
     phi,            /* IN   track phi                                 */
     l_start,        /* IN   start                                     */
     l_stop,         /* IN                                             */
     z_start,        /* IN   track start depth                         */
     z_stop,         /* IN   track stop depth                          */
     interp_mask,      /* IN   photonics interpolation mask              */
     &tableset_id,     /* OUT  'A' bins refer to the table of higher amp */
     &table_id_A,      /* OUT  gets set to -1 if amp<=0 or no bin found  */
     &table_id_B,      /* OUT  gets set to -1 if infinite track          */
     &l_bin_A,         /* OUT                                            */
     &l_bin_B,         /* OUT                                            */
     &rho_bin_A,       /* OUT                                            */
     &rho_bin_B,       /* OUT                                            */
     &phi_bin_A,       /* OUT                                            */
     &phi_bin_B );     /* OUT                                            */

  prob_x=mytablesets_level2.tableset[mytablesets_level2.current].last_probcalc;  

  *prob=prob_x;

  if(prob_x<0)
    *prob=-1;
  
  if(amp_x<0){
    *amp=-1;
    return false;  
  }

  if(energy!=0){
    amp_x = amp_x * light_for_source((source_type)mytablesets_level2.tableset[mytablesets_level2.current].header->source,energy) ;
  }

  *amp =amp_x;

  return true;  

}

bool I3PhotonicsL2Reader::get_level2_sourcetype(int *sourcetype, int *time_reference ) const { 

  if(*n_tables==0){
    *sourcetype=-1;
    *time_reference=-1;
    return false;
  }
  *sourcetype=mytablesets_level2.tableset[mytablesets_level2.current].header->source;
  *time_reference=mytablesets_level2.tableset[mytablesets_level2.current].header->tref;

  return true;
}

double I3PhotonicsL2Reader::get_level2_lmax() const {
  return mytablesets_level2.tableset[mytablesets_level2.current].ref_dist;
}



/*******************************************************************/
/* level2_reader.c: Table reader routines for level2 tables        */
/*******************************************************************/

/* Precalculates level2 table binnings for fast lookup */
int I3PhotonicsL2Reader::level2_geometry(const Level2_header_type *h,Level2_geo_type *geo){
  int i;
  float q;

  for(i=0;i<L2VARS;++i){
    geo->n[i]=h->n[i];
    if((geo->edge[i] = (double *)malloc((h->n[i]+1)*sizeof(double)))==NULL)
      log_fatal("Unable to allocate geometry bins.");
    geo->edge[i][0]=h->range[i][0];
  }


  /* Calculate longitudinal bin edges */
  geo->d[0]=(h->range[0][1]-h->range[0][0])/h->n[0];

  switch(h->l_scale){ 
  case LINEAR:
    for(i=1;i<=h->n[0];++i)
      geo->edge[0][i]=geo->edge[0][i-1]+geo->d[0];
    break;
  case QUADRATIC:    
    
    if(h->range[0][1]>=0 && h->range[0][0]<=0) /* Around center */
      q=(sqrt(h->range[0][1])+sqrt(-h->range[0][0]))/h->n[0];
    else  
      q=fabs(sqrt(fabs(h->range[0][1]))-sqrt(fabs(h->range[0][0])))/
	h->n[0];

    for(i=1;i<=h->n[0];++i)
      geo->edge[0][i]=next_boundary_sqr(geo->edge[0][i-1],q);
                      
    break;
  default:

    log_fatal("unknown geometry in level2_reader.");    
    break;
  }

  /**************************************************/


  /* Calculate radial bin edges */
  geo->d[1]=(h->d_scale==QUADRATIC?
	sqrt(h->range[1][1])-sqrt(h->range[1][0]):
	h->range[1][1]-h->range[1][0])/h->n[1];
  for(i=1;i<=h->n[1];++i)
    geo->edge[1][i]=geo->edge[1][i-1]+
      (h->d_scale==QUADRATIC?(2*i-1)*sqr(geo->d[1]):geo->d[1]);

  /* Calculate azimuthal bin edges */
  geo->d[2]=(h->range[2][1]-h->range[2][0])/(h->n[2]-1);
  for(i=1;i<h->n[2];++i)
    geo->edge[2][i]=geo->edge[2][i-1]+geo->d[2];
  /* Since azimuth data is calculate at bin edges, this just makes the left bin edged
     search take care of last bin automatically...Hopefully */
  geo->edge[2][h->n[2]]=geo->edge[2][h->n[2]-1];

  /* now, timing... we already alocated n+1 bins, so that the last lower
     edge can be used as absolute upper edge */

  // special case for negative quadratic:
  if((h->range[3][0]<0 )&&(h->t_scale==QUADRATIC)) {
    
    if(h->range[3][1]<0){
      log_fatal("sorry, negative upper times limit not supported with QUADRATIC");
    }
    
    double negfract=sqrt(-h->range[3][0])/
      (sqrt(-h->range[3][0])+sqrt(h->range[3][1]));
    
    int negind= (h->n[3] * negfract  )      +1;     // this adds a bit
    int posind= (h->n[3] * (1-negfract)  )  -1;     // more detail to the negative side
    
    double negd= sqrt(-h->range[3][0])/negind;
    double posd= sqrt( h->range[3][1])/(posind+1);
    
    geo->edge[3][posind]=0;
    geo->edge[3][negind]=0.; // added EM 100202 
    
    
    for(i=(negind-1);i>=0;--i){
      geo->edge[3][i]=geo->edge[3][i+1]-
	(2*abs(i-negind)-1)*sqr(negd);
    }
    for(i=(negind+1);i<=h->n[3];++i){
      geo->edge[3][i]=geo->edge[3][i-1]+
	(2*abs(i-negind)-1)*sqr(posd);
    }

    
  } else {
    /* Calculate timing bin edges (this is lower edge of bin)*/ 
    geo->d[3]=(h->t_scale==QUADRATIC?
	       sqrt(h->range[3][1])-sqrt(h->range[3][0]):
	       h->range[3][1]-h->range[3][0])/h->n[3];
    for(i=1;i<=h->n[3];++i)
      geo->edge[3][i]=geo->edge[3][i-1]+
	(h->t_scale==QUADRATIC?(2*i-1)*sqr(geo->d[3]):geo->d[3]);    
  }
  
  
  /* Find the representing point of each times bin*/
  /* we only alocate n bins for this vector, not n+1 */
  geo->times=(double *)malloc(h->n[3] * sizeof(double));
  
  for(i=1;i<(h->n[3]);++i) /* edge has n[3]+1 entries */
    geo->times[i]=geo->edge[3][i]+0.25*(geo->edge[3][i+1]-geo->edge[3][i-1]);

  /* since the bin search expects to find a lower edge, the first time
     is ofset downwards to allow time=h->range[3][0] to be found */
  geo->times[0]=h->range[3][0];

  return true;
}

/* Coordinates to bins conversions with remainders. Copied and simplified from reader.c */
bool I3PhotonicsL2Reader::get_level2_coord(const Level2_geo_type *geo,Level2_read_type *coord){
  int i,j;
  int type;

  for(i=0;i<L2VARS;++i){
    coord->n[i]=geo->n[i];
    type = 1 << i;
    j = find(geo->edge[i],coord->n[i],coord->val[i],-1);
    coord->bin[i]=j;
    if(coord->interp_mask&type && j>=0 && j<coord->n[i]){
      coord->lowEdge[i]=geo->edge[i][j];
      coord->binSize[i]=geo->edge[i][j+1]-coord->lowEdge[i];
    }
    /* Check that we actually found requested coordinate */
    if(coord->bin[i]<0 || coord->bin[i]>=coord->n[i]) return false;
  }

  return true;
}
  
/* Full interpolation routine for table entry. 
   Input: Structure with bin info
   Output: Interpolation factors and bins to use as a structure, and status
           as a return value.
   Intepolation logic:	   
	 Find the weight for the current and an adjecent bin. Always weight 
	 current bin higher, and select the adjecent bin closer to the
	 actual value. Edge bins are not interpolared towards the outside.  
   Copied from reader.c, with minor changes. 	 
*/
bool I3PhotonicsL2Reader::level2_interpolate(const Level2_read_type *coord,
					     Level2_interp_type *interp){
  int i,j,type;  /* Counter variables */
  int dir;     /* Adjecent bin selection variable */
  double a,b;  /* Numbers used to calculate weights */
  double high,low;  /* Weight used for bit 'high' and bit 'low'
		       high==adjecent bin, low==center bin */
  unsigned short not_mask = ~interp->mask; /* Inverted mask for quick checks */

  /* First we need to zero the the return structure */
  for(i=0;i<L2_INTERP_BINS;++i) interp->f[i]=(not_mask&i)==0?1.:0.;
  for(i=0;i<L2VARS;++i) interp->bin[i][0]=interp->bin[i][1]=coord->bin[i];

  /* Loop over possible interpolations and execute them */
  for(i=0;i<L2VARS;++i){
    type = 1 << i;
    if(!(interp->mask&type)) continue;
    b=coord->val[i]-coord->lowEdge[i];
    a=b/coord->binSize[i];

    if(i!=2){
      /* Determine weights and adjecent bins */
      if ( a > .5 && a <= 1 ){
	high = a - .5;
	low = 1 - high;
	dir=1;
      }else if ( a >= -1e-10 && a <= .5 ){
	low = a + .5;
	high = 1 - low;
	dir=-1;
      }else 
	return false;
    }else{ // Phi bins are calculated at left edges
      low=a;
      high=1-low;
      dir=-1; /* This line was a bug. was +1. 25 apr 2005 */

    }

    /* Store adjecent bin and check for ranges */
    interp->bin[i][1]=interp->bin[i][0]+dir;
    if(interp->bin[i][1]==coord->n[i]) --interp->bin[i][1];
    if(interp->bin[i][1]<0) ++interp->bin[i][1];

    /* Everything went well, so store the weights we found */
    for(j=0;j<L2_INTERP_BINS;++j){
      if(j&type)      /* For high bit, store adjecent bin weight */
	  interp->f[j]*=high;
	else            /* For low bit, store center bin weight */
	  interp->f[j]*=low;
    }
  }

  return true;
}

/* Routine that reads single level2 file */
int I3PhotonicsL2Reader::read_level2_file(const char *file,Level2_header_type *h,
					  Level2_data_type *data,unsigned long *memused){
  FILE *fh;
  unsigned int size;
  int j;

  *memused=0;

  if((fh=fopen(file,"rb"))==NULL){
    log_error("Photonics Level2: Unable to open the input file %s.",file);
    return false;
  }
  log_debug("Photonics Level2: Loading table file: '%s'",file);

  if(fread(h,sizeof(Level2_header_type),1,fh)!=1){
    log_error("Photonics Level2: Error while reading the level2 data header in file %s.",file);
    return false;
  }

  if (!checkMetaHead(&(h->MetaHead)))
    return false;

  if(h->MetaHead.level!=2){
    log_error("Photonics Level2: Error, I can't read level %d tables %s.",h->MetaHead.level,file);
    return false;
  }

  // byte swapping of header could be done here...
  if(!isLittleEndian()){
    // this is not tested. /J
    if(checkMetaHeadLittle(&(h->MetaHead))){
      byteswap32_l2header(h);
    }
  }

  /* Calculate table offsets */
  if(h->type==ABS){
    data->offset[3]=0;
    data->offset[2]=1;
  }else{
    data->offset[3]=1;
    data->offset[2]=data->offset[3]*h->n[3];
  }
  data->offset[1]=data->offset[2]*h->n[2];
  data->offset[0]=data->offset[1]*h->n[1];
  
  /* Allocate memory and read it */
  size=data->offset[0]*h->n[0];
  if((data->cont=(float *)malloc(size*sizeof(float)))==NULL){
    log_error("Photonics Level2: Unable to allocate memory for data in file %s.",file);
    return false;
  }
  if(fread(data->cont,sizeof(float),size,fh)!=size){
    log_error("Photonics Level2: Error while reading the level2 data in file %s.",file);
    return false;
  }
  if( (j=fclose(fh)) != 0){
    log_error("Photonics Level2: fclose(%s) returned %d, system error is: %s",file,j,strerror(errno));
  }

  // byte swapping of data done here (Power-PC support)...
  if(!isLittleEndian()){
    // this is not tested. /J
    log_error("Photonics: level2_reader.c: Experimental byte swapping: little endian to Big-endian (Power-PC)");
    if(checkMetaHeadLittle(&(h->MetaHead))){
      {
	unsigned long k;
	for(k=0;k<size;++k){
	  px_swap32(&(data->cont[k]));
	}
      }
    }
  }

  //  mytablesets_level2.tableset[mytablesets_level2.current].memused+=(unsigned long) (size*sizeof(float));

  *memused= (unsigned long) (size*sizeof(float));
  return true;
}

// performs byte swapping on relevant members of level2 headers for Power-PC etc.
void I3PhotonicsL2Reader::byteswap32_l2header(Level2_header_type * h)
{
  log_warn("Photonics: level2_reader.c: NOT VERIFIED!");
  log_warn("Photonics: level2_reader.c: byte swapping (Power-PC support) of level2 header.");

  int i,j;

  // Swapping inspired by C++ variant by Alex Olivas. /J
  
  // We'll read the values into these//  // THIS COLUME IS CUT IN HERE FROM level2_photonics.h
                                     //  MetaHead_type MetaHead;
                                     //  char dummy_padding[28]; /* not used */
  px_swap32((void*)&(h->gausconv));  //  float gausconv;         /*convolved with gausian of this width */
  px_swap32((void*)&(h->ref_np));    //  float ref_np;           /*phase refractive index used for true <--> residual calc*/
  px_swap32((void*)&(h->ref_ng));    //  float ref_ng;           /*phase refractive index used for true <--> residual calc*/
				     //  bool dummy_a      ;     /*                    NOT USED                     */
  px_swap32((void*)&(h->source));    //  source_type source;     /* Emission source type                            */
				     //  bool dummy_b;           /*                    NOT USED                     */
                                     //  float dummy_c1;         /* Step for photon tracking (m)                    */
  px_swap32((void*)&(h->e));	     //  float e;                /* Shower energy for shower sources in (GeV)       */
                                     //  float dummy_c2;         /*                    NOT USED                     */
  px_swap32((void*)&(h->theta));     //  float theta;            /* Source axis zenith angle                        */
				     //  float dummy_c3[2];      /*                    NOT USED                     */
 				     //  geo_type dummy_d;       /*                    NOT USED                     */
  // n : looped below...	     //  ph_int n[VARS];         /* Number of bins in each dimension                */
  // range : looped below...         //  float range[L2VARS][2]; /* Recordable limits for each dimension            */
 				     //  float dummy_range[VARS-L2VARS][2]; /*         NOT USED                     */
				     //  float dummy_e[VARS][2]; /*                    NOT USED                     */
  px_swap32((void*)&(h->z0));	     //  float z0;               /* depth of active volume center wrt ice table     */
  px_swap32((void*)&(h->d_scale));   //  scale_type d_scale;     /* distance scaling parameter                      */
  px_swap32((void*)&(h->t_scale));   //  scale_type t_scale;     /* time scaling parameter                          */
  px_swap32((void*)&(h->lambda));    //  float lambda;           /* light wavelength, or spectral "index"           */
  px_swap32((void*)&(h->efficiency));//  ph_int efficiency;      /* Efficinency calibrations applied                */
                                     //  ph_int dummy_e2;        /* NOT USED                                        */
  px_swap32((void*)&(h->type));	     //  photonics_l2_table_flavor type;        /* Table type                       */
                                     //  ph_long n_dummy_f1;     /*                    NOT USED                     */
  px_swap32((void*)&(h->l_scale));   //  scale_type l_scale;     /* lengthwize scaling parameter                    */
  px_swap32((void*)&(h->tref));	     //  time_ref_type tref;     /* Timing reference                                */
					             					         
  for(i=0; i<L2VARS; i++)
    px_swap32((void*)&(h->n[i]));
					   
  for(i=0; i<L2VARS; i++)
    for(j=0; j<2; j++)
      px_swap32((void*)&(h->range[i][j]));

}

/* Table comparator for sorting of data tables */
int I3PhotonicsL2Reader::level2_table_comp(const void *a,const void *b)
{
  const Amasim_Level2_Table_type *table_a=(const Amasim_Level2_Table_type*)a;
  const Amasim_Level2_Table_type *table_b=(const Amasim_Level2_Table_type*)b;

  if(table_a->zLow<table_b->zLow)
    return -1;
  else if(table_a->zLow>table_b->zLow)
    return 1;
  else{
   if(table_a->aLow<table_b->aLow)
     return -1;
   else if(table_a->aLow>table_b->aLow)
     return 1;
   else{
     log_debug("Photonics Level2: Comparator found two tables with same zLow and aLow");
     return 0;
   }
  }
}
