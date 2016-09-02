/**
 * @file I3PhotonicsL1Reader.cxx
 * @brief This file for code for reading level1 photonics tables
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @author RCB
 * @author KWW
 * @version $Id: I3PhotonicsL1Reader.cxx 142407 2016-02-24 12:46:49Z kjmeagher $
 * @date $Date: 2016-02-24 06:46:49 -0600 (Wed, 24 Feb 2016) $
 * 
 * The following is a list of files from photonic which I 
 * copied verbatium to create this file:
 *   \li photonics-1.73/amasim/photoamasim.h 
 *   \li photonics-1.73/lib/reader.h
 *   \li photonics-1.73/lib/reader.c
 *   \li photonics-1.73/lib/io.c
 *   \li photonics-1.73/amasim/table_io.c
 *
 * Non functioning code was removed, comments cleaned up
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
#include <cassert>
#include <sys/user.h> 
#include <sys/mman.h>
#include "icetray/I3Logging.h"
#include "dataclasses/I3Constants.h"
#include "photonics-service/I3PhotonicsL1Reader.h"

/*******************************************************************/
/* photoamasim.h: Data type definitions for interface between      */
/*                photonics and amasim                             */
/*******************************************************************/

#define ABS_EXT     ".abs"
#define PROB_EXT    ".prob"

#define MAX_TABLES  10000

#define MUON 0 
#define EMS  1 
#define HADS 2 

#define  INTERP_BINS     8    /// Number of bins for interpolation

/*******************************************************************/
/*  reader.h : Photonics library definitions for reading of photon */
/*             tables. (Higher level I/O)                          */
/*******************************************************************/

#define  INTERP_NONE      0x00 
#define  INTERP_PRIMARY   0x01
#define  INTERP_SECONDARY 0x02 
#define  INTERP_TERTIARY  0x04 
#define  INTERP_EMISSION  0x08 
#define  INTERP_ARRIVAL   0x10 
#define  INTERP_TIME      0x20

using namespace photonics;

I3PhotonicsL1Reader::I3PhotonicsL1Reader() :
  in(NULL),
  data_chunk(NULL),err_chunk(NULL),
  table(NULL),n_tables(NULL),
  geo(NULL)
{
  io.h_offset=0;
  for ( unsigned i =0; i< VARS ; i++) {
    io.offset[i]=0;
  }
  io.n_chunk=0;
  io.h=NULL;

  pref_level1.zedgelimiting=1;
  pref_level1.greetingdone=0;
  pref_level1.external_ref_ng=I3Constants::n_ice_group;
  pref_level1.z_load_offset=0.0;
     
  mytablesets_level1.current=0;
  mytablesets_level1.n_tablesets=0;
  mytablesets_level1.request_angle_limit_low =-photonics_reader_InF;
  mytablesets_level1.request_angle_limit_high=photonics_reader_InF;
  mytablesets_level1.request_depth_limit_low =-photonics_reader_InF;
  mytablesets_level1.request_depth_limit_high= photonics_reader_InF;
  mytablesets_level1.tableset=NULL;
}

I3PhotonicsL1Reader::~I3PhotonicsL1Reader() 
{
  free_tablesets_level1();
}

/*******************************************************************/
/* table_io.c: Load and read photonics tables for amasim use       */
/*******************************************************************/

#define MESSAGE_CYCLE 10000

#define L1_INTERP_DEPTH 0x10
#define L1_INTERP_ANGLE 0x20

/** @brief Table comparator for sorting of data tables */
int I3PhotonicsL1Reader::level1_table_comp(const void *a,const void *b){
  const Amasim_Table_Type *table_a=(const Amasim_Table_Type*)a;
  const Amasim_Table_Type *table_b=(const Amasim_Table_Type*)b;

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
     log_debug("Photonics Level1: Comparator found two tables with same zLow and aLow");
     return 0;
   }
  }
}

int I3PhotonicsL1Reader::init_tableset_level1()
{
    int index; /* New level1 table set index */
    unsigned long mm=0,mthis=0;

    /* Make memory for level1 tableset */
    if ( mytablesets_level1.n_tablesets ==  0 ) {
	/* First level1 tableset needs malloc */
	mytablesets_level1.tableset =
	  (Tableset_level1_type *) malloc ( sizeof( Tableset_level1_type ) );
	index = 0;
	mytablesets_level1.n_tablesets = 1;
    } else {
	/* Following table sets needs realloc */
	mytablesets_level1.tableset =
	  (Tableset_level1_type *)realloc ( mytablesets_level1.tableset,
		      (mytablesets_level1.n_tablesets+1)*
		      sizeof( Tableset_level1_type ) );
	index = mytablesets_level1.n_tablesets;
	mytablesets_level1.n_tablesets++;
    }

    /*Make memory for tables and info in tableset*/

    mytablesets_level1.tableset[index].n_tables =
      (int *)malloc( mthis=sizeof ( int ) );
    (*mytablesets_level1.tableset[index].n_tables) = 0;
    mm+=mthis;

    mytablesets_level1.tableset[index].geo =
      (Geo_type *)malloc( mthis=sizeof ( Geo_type ) );
    mm+=mthis;

    mytablesets_level1.tableset[index].header =
      (Header_type *)malloc( mthis=sizeof (Header_type) );
    mm+=mthis;

    mytablesets_level1.tableset[index].table =
      (Amasim_Table_Type *)malloc( mthis=sizeof (Amasim_Table_Type)*MAX_TABLES );
    mm+=mthis;

    mytablesets_level1.tableset[index].memused=0;    // the data
    mytablesets_level1.tableset[index].memforset=mm; // the set, no data

    /* initiate variables */
    mytablesets_level1.tableset[index].z_src_max=0;
    mytablesets_level1.tableset[index].z_src_min=0;
    mytablesets_level1.tableset[index].a_src_max=0;
    mytablesets_level1.tableset[index].a_src_min=0;

    /*return tableset index*/
    return index;
}

int I3PhotonicsL1Reader::swap_tableset_level1( int index )
{
    /* Check index */
  if ( (unsigned int)index >= mytablesets_level1.n_tablesets ) {
	return -1;
    }
    
    /* Change current index to new index */
    mytablesets_level1.current = index;
    
    /* Repoint static global pointers */
    geo      =
	mytablesets_level1.tableset[mytablesets_level1.current].geo;
    table    =
	mytablesets_level1.tableset[mytablesets_level1.current].table;
    n_tables =
	mytablesets_level1.tableset[mytablesets_level1.current].n_tables;
    
    return index;
}

geo_type I3PhotonicsL1Reader::get_level1_geometry()
{
    Header_type *header;
    if (mytablesets_level1.n_tablesets == 0)
    	return (CYLINDRICAL);
    header = mytablesets_level1.tableset[mytablesets_level1.current].header;
    return (header->geo);
}

void I3PhotonicsL1Reader::free_tablesets_level1( )
{
  int i,j,k;

    /* Loop over all tablesets in array */
    for ( i = 0; (unsigned int)i < mytablesets_level1.n_tablesets; i++ ) {
	/* Swap to ensure n_tables is up to date */
	swap_tableset_level1( i );

	/* Free memory used by each table */
	for ( j = 0; j < (*mytablesets_level1.tableset[i].n_tables); j++ ) {
	    data_free(&mytablesets_level1.tableset[i].table[j].abs);
	    data_free(&mytablesets_level1.tableset[i].table[j].prob);
	}

	/* Free tableset members */
	free( mytablesets_level1.tableset[i].n_tables );
	for(k=0;k<3;k++){
	  free( (*mytablesets_level1.tableset[i].geo).dim[k] );
	  free( (*mytablesets_level1.tableset[i].geo).bracket[k] );
	  free( (*mytablesets_level1.tableset[i].geo).middle[k] );
	}
	free( (*mytablesets_level1.tableset[i].geo).timing );
	free( (*mytablesets_level1.tableset[i].geo).timing_middle );
	free( mytablesets_level1.tableset[i].geo );
	free( mytablesets_level1.tableset[i].header );
	free( mytablesets_level1.tableset[i].table );
	free( mytablesets_level1.tableset[i].z_layer_index );
	free( mytablesets_level1.tableset[i].z_layer_n_thetas );
	free( mytablesets_level1.tableset[i].z_layer_centers );
	

	/* trust that we are now done */
	mytablesets_level1.tableset[i].memused=0;
	mytablesets_level1.tableset[i].memforset=0;
    }
    
    if ( mytablesets_level1.n_tablesets > 0 ) {
      /* Free memory used by tableset array */
      free( mytablesets_level1.tableset );
    }

    /* Reset current tableset index */
    mytablesets_level1.current = 0;

    /* Reset number of tablesets counter */
    mytablesets_level1.n_tablesets = 0;
}

void I3PhotonicsL1Reader::set_level1_angular_interval(float alow,float ahigh) 
{
  ahigh=(ahigh<alow)?alow:ahigh;

  mytablesets_level1.request_angle_limit_low = alow;
  mytablesets_level1.request_angle_limit_high = ahigh;
}

void I3PhotonicsL1Reader::set_level1_depth_interval(float zlow,float zhigh)
{
  zhigh=(zhigh<zlow)?zlow:zhigh;

  mytablesets_level1.request_depth_limit_low = zlow;
  mytablesets_level1.request_depth_limit_high = zhigh;
}

/** @brief Routine that reads in all tables specified in driver file */
int I3PhotonicsL1Reader::load_tables(const char *driver_file){
  FILE *driver_fh;
  int i=0,ii=0,k,j;
  int index_tableset=-1;
  char line[MAXLINE];
  char tmpline[MAXLINE];

  char tmp_name[MAXLINE];
  //  char in_file_name[MAXLINE];

  double low_ang_tab=-photonics_reader_InF,high_ang_tab=photonics_reader_InF;
  double low_dep_tab=-photonics_reader_InF,high_dep_tab=photonics_reader_InF;
  float tmp1,tmp2,tmp3,tmp4;
  int tmpint;
  int q,r;

  float tmpdepth;

  Opt_type opt;
  Header_type *h;

  if(pref_level1.greetingdone<1){
    pref_level1.greetingdone++;
    log_info("Photonics Level1: Version '%s'",PHOTONICS_VERSION);

    if(!isSupportedEnvironment()){
      log_warn("Photonics is not supported in this environment -- continuing anyway.");
    }
  }



  /* Open driver file */
  if((driver_fh=fopen(driver_file,"r"))==NULL){
    log_error("Photonics Level1: Unable to open table input driver file '%s'.",driver_file);
    return false;
  }
  log_debug("Photonics Level1: Loading driver file '%s'",driver_file);

  float a_lowest=photonics_reader_InF;
  float a_highest=-photonics_reader_InF;
  float z_lowest=photonics_reader_InF;
  float z_highest=-photonics_reader_InF;

  /* preparsing of the file to find the angles */
  {
    i=0;
    while(!feof(driver_fh)){
      if(i>=MAX_TABLES){
	log_error("Photonics Level1: Maximum number of tables read without reaching EOF.");
	return false;
      }
      
      /* Read in a line, skipping comments */
      if(fgets(line,MAXLINE,driver_fh) && line[0]!='#' && line[0]!=' ' && strlen(line)>4){

	/* if the last number is not set, it's probably a level2 table */
	sscanf(line,"%s %d %f %f %f %f",tmpline,&tmpint,&tmp1,&tmp2,&tmp3,&tmp4);
	k=tmp4;      // so we save it and
	tmp4=tmp4+1; // check if it changes...

	/* the real scan */
	sscanf(line,"%s %d %f %f %f %f",tmpline,&tmpint,&tmp1,&tmp2,&tmp3,&tmp4);

	if(tmp4!=k){
	    log_fatal("Photonics Level1: Error: This seems to be a level2 driver file: '%s'",driver_file);
	}

	/* Log_Fatal if we find a table with '@' mark. then it's a level2 table... */
	if (0==strncmp("@",line,strlen("@"))) {
	  log_fatal("Photonics Level1: Error: found level2 style '@' mark in driver file: '%s' bad line: '%s'",driver_file,line);
	}

       
	if( tmp1 >= tmp2 ){
	  log_error("Photonics Level1: Error. The driver file has layers of undefined size");
	  return -1;
	}

	a_lowest=(tmp3<a_lowest)?tmp3:a_lowest;
	a_highest=(tmp3>a_highest)?tmp3:a_highest;
	
	tmpdepth=(tmp1+tmp2)*0.5;

	z_lowest=(tmpdepth<z_lowest)?tmpdepth:z_lowest;
	z_highest=(tmpdepth>z_highest)?tmpdepth:z_highest;


	if (tmpdepth<=mytablesets_level1.request_depth_limit_low)
	  /* find the highest low bound allowed */
	  if(tmpdepth>low_dep_tab){
	    low_dep_tab=tmpdepth;
	  }

	if(tmpdepth>=mytablesets_level1.request_depth_limit_high)
	  /* find the lowest high bound allowed */
	  if(tmpdepth<high_dep_tab){
	    high_dep_tab=tmpdepth;
	  }
	
	
	if(tmp3<=mytablesets_level1.request_angle_limit_low)
  	  /* find the highest low bound allowed */
	  if(tmp3>low_ang_tab){
	    low_ang_tab=tmp3;
	  }

	if(tmp3>=mytablesets_level1.request_angle_limit_high)
	  /* find the lowest high bound allowed */
	  if(tmp3<high_ang_tab){
	    high_ang_tab=tmp3;
	  }

      }
      i++;
    }  /* end of preparsing */
    
    if((low_ang_tab>high_ang_tab)||(low_dep_tab>high_dep_tab)){
      log_error("Photonics Level1: no tables matched");
      return false;
    }
    
  }

  /* Note that low_ang_tab and high_ang_tab is used later in this file */
  /* rewind the file */
  rewind(driver_fh);
  

  /* Loop over all entries */
  i=0;
  while(!feof(driver_fh) && i<MAX_TABLES){
  /* Read in a line, skipping comments */
    if(fgets(line,MAXLINE,driver_fh) && line[0]!='#' && line[0]!=' '){

      /* Parse the line */
      sscanf(line,"%s %d %f %f %f %f",tmpline,&tmpint,&tmp1,&tmp2,&tmp3,&tmp4);

      tmpdepth=(tmp1+tmp2)*0.5;

      if(!((tmp3>=low_ang_tab)&&
	   (tmp3<=high_ang_tab)&&
	   (tmpdepth>=low_dep_tab)&&
	   (tmpdepth<=high_dep_tab))){
	log_debug("Photonics Level1: skipping %s",tmpline);     
	continue;
      }

      /******************************************/
      /* We assume we need to have a table set here */
      if (index_tableset==-1) {
	index_tableset = init_tableset_level1();
      }
      swap_tableset_level1( index_tableset );
      /* set the pointer to the header of the table set: */
      h=     mytablesets_level1.tableset[index_tableset].header;
      /******************************************/

      sscanf(line,"%s %d %f %f %f %f",tmp_name,
	     &table[i].type,&table[i].zLow,&table[i].zHigh,
	     &table[i].aLow,&table[i].aHigh);

      /* Read in the tables */
      log_debug("Photonics Level1: Loading table(%d) %s",i,tmp_name);

      sprintf(opt.in_file_name,"%s%s",tmp_name,ABS_EXT);
      if(!read_file_without_errors(&opt,&table[i].abs)){
	log_error("Photonics Level1: Unable to read photonics file %s.",opt.in_file_name);
	return false;
      }
      copy_header(get_header(),&table[i].h_abs);
      mytablesets_level1.tableset[index_tableset].memused+=(unsigned long) data_allocated(&table[i].abs,&table[i].h_abs);
      close_input(NULL);
      
      sprintf(opt.in_file_name,"%s%s",tmp_name,PROB_EXT);
      if(!read_file_without_errors(&opt,&table[i].prob)){
	log_error("Photonics Level1: Unable to read photonics file %s.",opt.in_file_name);
	return false;
      }
      copy_header(get_header(),&table[i].h_prob);
      mytablesets_level1.tableset[index_tableset].memused+=(unsigned long) data_allocated(&table[i].prob,&table[i].h_prob);
      close_input(NULL);

      // We only allow level1 tables
      if((table[i].h_prob.MetaHead.level!=1)||(table[i].h_abs.MetaHead.level!=1)){
	log_fatal("Photonics Level1: Error: I can't handle level (abs:%d,prob:%d) tables.",table[i].h_abs.MetaHead.level,table[i].h_prob.MetaHead.level);
      }

      if(i==0){ /* Calculate geometry on the first pass */
	copy_header(&table[i].h_prob,&(*h));
	for(ii=0;ii<VARS;++ii){
	  (*h).maxes[ii][0]=(*h).limits[ii][0];
	  (*h).maxes[ii][1]=(*h).limits[ii][1];
	}
	if(!set_up_geometry(&(*h),&(*geo))){
	  log_error("Photonics Level1: Unable to obtain geometry info for photon tables.");
	  return false;
	}
      }
      
      ++i;  /* Count up */
      
    }
  }
  
  if(!feof(driver_fh)){
    log_error("Photonics Level1: Maximum number of tables read without reaching EOF.");
  }

  fclose(driver_fh);

  /* calculate some usefull information */
  /* let's only consider the active table set. */
  (*n_tables) = i;

  /* sort tables for faster and simpler look up */

  qsort(table,(*n_tables),sizeof(Amasim_Table_Type),level1_table_comp);

  log_info("Photonics Level1: loaded %4d tables, setid %3d",*n_tables,index_tableset);

  j=mytablesets_level1.current ;

  /* this is directly cut from level1_table_io.c ****************** */

  /* count the number of z layers */
  mytablesets_level1.tableset[j].n_z_layers=1;
  for (k=0; k<(*n_tables-1) ; k++) {
    if ( table[k].zHigh != table[k+1].zHigh ) {
      mytablesets_level1.tableset[j].n_z_layers++;
    }
  }

  /* allocate memory for lookup tables */
  mytablesets_level1.tableset[j].z_layer_index =
    (int *)malloc( sizeof ( int )*mytablesets_level1.tableset[j].n_z_layers );

  mytablesets_level1.tableset[j].z_layer_n_thetas =
    (int *)malloc( sizeof ( int )*mytablesets_level1.tableset[j].n_z_layers );
      
  mytablesets_level1.tableset[j].z_layer_centers=
    (double *)malloc( sizeof ( double )*mytablesets_level1.tableset[j].n_z_layers );


  /* calculate and store  the index to the first bin in each z layer */

  q=0;
  assert(*n_tables>0);
  for (k=0; k<(*n_tables) ; k++) {
    if (k==0 || table[k].zHigh != table[k-1].zHigh ){
      mytablesets_level1.tableset[j].z_layer_index[q]=k;
      q++;
    }
    if ((k==0)||(mytablesets_level1.tableset[j].a_src_max<table[k].aLow))
      mytablesets_level1.tableset[j].a_src_max=table[k].aLow;
    if ((k==0)||(mytablesets_level1.tableset[j].a_src_min >table[k].aLow))
      mytablesets_level1.tableset[j].a_src_min =table[k].aLow;
  }

  /* store max and min valid z locations, and central z values */
  for (k=0; k<(mytablesets_level1.tableset[j].n_z_layers) ; k++) {

    r=mytablesets_level1.tableset[j].z_layer_index[k];
    for(q=0;(r+q)<(*n_tables);q++){
      if (table[r].zHigh!=table[r+q].zHigh)
	break;
      mytablesets_level1.tableset[j].z_layer_n_thetas[k]=q+1;
    }
		

    tmpdepth=(table[r].zHigh+table[r].zLow)*0.5;
    mytablesets_level1.tableset[j].z_layer_centers[k]=tmpdepth;

    if ((k==0)||(mytablesets_level1.tableset[j].z_src_max<tmpdepth))
      mytablesets_level1.tableset[j].z_src_max=tmpdepth;
    if ((k==0)||(mytablesets_level1.tableset[j].z_src_min>tmpdepth))
      mytablesets_level1.tableset[j].z_src_min=tmpdepth;
	
  }

  /* verify that all z layers have the same number of angles */
  for (k=0; k<(-1 + mytablesets_level1.tableset[j].n_z_layers) ; k++) {
    if(mytablesets_level1.tableset[j].z_layer_n_thetas[k]!=
       mytablesets_level1.tableset[j].z_layer_n_thetas[k+1]){
      log_fatal("Photonics Level1: Error: Inhomogenous table set loaded");
    }
  }

  /* if there is only one angle in the driver file: */
  if ( a_highest == a_lowest ){
    mytablesets_level1.tableset[j].a_src_max=table[0].aHigh;
  }

  /* ************************************************************** */

  for (k=0; k<(*n_tables) ; k++) {
    /* Note that only .aLow (table center angle) is used here. aHigh is
       only a dummy indication the location of the next angle. */
    if ((k==0)||(mytablesets_level1.tableset[j].a_src_max<table[k].aLow))
        mytablesets_level1.tableset[j].a_src_max=table[k].aLow;
    if ((k==0)||(mytablesets_level1.tableset[j].a_src_min >table[k].aLow))
        mytablesets_level1.tableset[j].a_src_min =table[k].aLow;
  }

  /* TODO: we could check that the loaded tables have the same
     binning for all layers */

  /* if there is only one angle in the driver file: */
  if ( a_highest == a_lowest ){
    mytablesets_level1.tableset[j].a_src_max=table[0].aHigh;
  }


  log_info("Photonics Level1: Active interval: %4g<=th<=%4g  %4g<=z<=%4g",
	   mytablesets_level1.tableset[j].a_src_min,
	   mytablesets_level1.tableset[j].a_src_max,
	   mytablesets_level1.tableset[j].z_src_min,
	   mytablesets_level1.tableset[j].z_src_max);

  return i;
}

/// @brief Find the angle number [upper and lower] for a requsted angle.
///
/// The returned indecis are offsets from the lowest angle. Add the
/// index from find_z_table to acces the right table.
inline int I3PhotonicsL1Reader::find_l1theta_table(float a_source, int *TidA, int *TidB, double *wA, double *wB)const{

  int my_current=mytablesets_level1.current;

  // assuming all layers have the same number of angles.
  int n_thetas=mytablesets_level1.tableset[my_current].z_layer_n_thetas[0];

  double a_other_weight;

  int id0=-1;
  int id1;

  if(n_thetas<2){
    id0=0;
    id1=0;
    a_other_weight=0;
  }else{
    if(a_source>=mytablesets_level1.tableset[my_current].a_src_max){
      id0=n_thetas-1;
      id1=id0;
      a_other_weight=0;
    }else{
      if(a_source<=mytablesets_level1.tableset[my_current].a_src_min){
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
      log_fatal("Photonics Level1: Error: (a) Internal interpolation error nthetas: %d (a_src %f) high/low: [%f %f]",n_thetas,a_source,table[0].aLow,table[n_thetas-1].aLow);
    }

    a_other_weight=
      (a_source-table[id0].aLow)/
      (table[id1].aLow-table[id0].aLow);
  }

  if((id0<0)||(id0>=n_thetas)){
    log_fatal("Photonics Level1: Error: (b) Internal interpolation error (a_src)");
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

/// @brief find the index to the tables [upper and lower] for a requested source depth.
inline int I3PhotonicsL1Reader::find_l1z_table(float z_source, int *idA, int *idB, 
					       double *wA, double *wB)const{

  static int last_id0=-1;

  int my_current=mytablesets_level1.current;
  
  int id0=-1;
  int id1;
  
  double z_other_weight;
  
  if(mytablesets_level1.tableset[my_current].n_z_layers<2){
    id0=0;
    id1=0;
    z_other_weight=0;
  }else{
    /* Use 1000*EPS (1mm) since precision does not allow adding 1E-6 to
     something of the order of 1000 */
    if(z_source>=mytablesets_level1.tableset[my_current].z_src_max-1000.*EPS){
      id0=mytablesets_level1.tableset[my_current].n_z_layers-1;
      id1=id0;
      z_other_weight=0;
    }else{
      if(z_source<=mytablesets_level1.tableset[my_current].z_src_min+1000.*EPS){
	id0=0;
	id1=0;
	z_other_weight=0;
      }
    }
  }

  if(id0==-1){
    id0=find(mytablesets_level1.tableset[my_current].z_layer_centers,
	     mytablesets_level1.tableset[my_current].n_z_layers -1,
	     z_source ,
	     last_id0);
    id1=id0+1;

    z_other_weight=
      (z_source-mytablesets_level1.tableset[my_current].z_layer_centers[id0  ])/
      (mytablesets_level1.tableset[my_current].z_layer_centers[id1]-
       mytablesets_level1.tableset[my_current].z_layer_centers[id0])  ;
  }

  if(id0<0){
    log_fatal("Photonics Level1: Error: (c) internal interp error. (z_src)");
  }

  if(z_other_weight>0.5){
    *idB=mytablesets_level1.tableset[my_current].z_layer_index[id0];
    *idA=mytablesets_level1.tableset[my_current].z_layer_index[id1];
    *wB=1-z_other_weight;
    *wA=z_other_weight;
  }else{
    *idA=mytablesets_level1.tableset[my_current].z_layer_index[id0];
    *idB=mytablesets_level1.tableset[my_current].z_layer_index[id1];
    *wA=1-z_other_weight;
    *wB=z_other_weight;
  }

  last_id0=id0;
  return 0;
}

/** @brief Returns mean photon density for specified source depth, angle, and type 
 *
 * @returns -1 for no table found (z_source/angle out of bounds) */
double I3PhotonicsL1Reader::get_photon_density(float z_source,
			  float angle_track,
			  int type,
			  float l,
			  float rho,
			  float phi,
			  int interp_mask,
			  int *table_id,
			  int *l_bin,
			  int *rho_bin,
			  int *phi_bin)const{
  int i;

  int interp_mask_stripped;
  double value_amp;

  /* At this point, we do not support depth interpolation in this function.
     Angular (theta) interpolation is done here explicitly, and we use the interpolate()
     function only to interpolate the 3 first (intrinsic) dimensions;
     z,rho,phi.
     For maximal supported inerpolation, give mask 255.
     Note; theta interpolation is always done if mask is not 0.
  
     Hence, for the a_other_weight of this function, we clear all bits higher than bit 3 */

  interp_mask_stripped=interp_mask&7;

  /* Variables needed for table reading and interpolation */
  Read_type coord;
  Interp_type interp;
  size_t pos=0;

  *table_id=*l_bin=*rho_bin=*phi_bin=-1;

  /* z_src outside valid regions is simply set to the closest edge,     */
  /* without touching the relative relation between observer and source.*/

  if(*n_tables==0){
    log_fatal("Photonics Level1: [1] No table loaded in the active table set");
    *table_id=-1;
    return -1;
  }

  if(std::isnan(z_source)){
    *table_id=-1;
    return -1;
  }

  if(std::isnan(angle_track)){
    *table_id=-1;
    return -1;
  }

  /* Fill in request vector */
  coord.val[0]=rho;
  coord.val[1]=Rad(fabs(phi));
  coord.val[2]=l;
  coord.val[3]=coord.val[4]=1;
  coord.val[5]=0;
  /* Remove possible ambiguity in secondary coordinate, phi */
  while(coord.val[1]>M_PI) coord.val[1]=2*M_PI-coord.val[1];
  /* Set interpolation mask */
  interp.mask = coord.interp_mask = interp_mask_stripped;

  /* Get bin coordinates of requested values */
  // note! this assumes all tables have the same structure!
  if(!get_coord(&table[0].h_abs,&(*geo),&coord)){
    log_debug("Photonics Level1: Photonics table lookup failed.");
    return -1;
  }
  /* Get requested value */

  /*********************************************************************/



  // Find the 4 best tables in angle_track,z_source space:

  double Rplane = rho*cos(Rad(phi));
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


  int TidA,TidB;
  double TwA,TwB;
  find_l1theta_table(angle_track, &TidA, &TidB, &TwA, &TwB);
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


  find_l1z_table(z_source+shiftA, &idA0, &idA1, &wA0, &wA1);
  find_l1z_table(z_source+shiftB, &idB0, &idB1, &wB0, &wB1);

  if((idA0<0)||(idB0<0)){
    *table_id=-1;
    return -1;
  }


  double WW[4]; // weight
  int ID[4];    // table id

  if(interp_mask&L1_INTERP_ANGLE){
    if(interp_mask&L1_INTERP_DEPTH){
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
    if(interp_mask&L1_INTERP_DEPTH){
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
      log_trace("Photonics Level1: note. ID %d , W: %f ",ID[tmpi],WW[tmpi]);
      wsum+=WW[tmpi];
    }
    if(fabs(wsum-1.0)>1e-6){
      log_error("Photonics Level1: error. Weight sum not 1. it is: %f ",wsum);
      *table_id=-1;
      return -1;
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
	log_debug("Photonics Level1: debug 1/3, center bin (amp>0) not true: %f",table[*table_id].abs.cont[test_pos]);
	log_debug("Photonics Level1: debug 2/3 l, rho, phi:: %d %d %d (bins)",coord.bin[0],coord.bin[1],coord.bin[2]);
	log_debug("Photonics Level1: debug 3/3 l, rho, phi:: %g %g %g (vals)",coord.val[0],coord.val[1],coord.val[2]);
	*table_id=-1;
	return -1;
      }
  }


  if(*table_id<0){
    log_error("this should not happen");
  }

 if((interp_mask!=0)&&(interpolate(&coord,&interp))){  /* Interpolate */
    
    /* Set up intrinsic interpolation matrix (index to bin) */
    unsigned int Vapos[INTERP_BINS];

    int i;
    for(i=0;i<INTERP_BINS;++i){
      Vapos[i]=
	table[0].abs.offset[0]*interp.bin[0][i&INTERP_PRIMARY?1:0]+
	table[0].abs.offset[1]*interp.bin[1][i&INTERP_SECONDARY?1:0]+
	table[0].abs.offset[2]*interp.bin[2][i&INTERP_TERTIARY?1:0];
    }


    value_amp=0.0;

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

      tmp_amp=0;

      for(i=0;i<INTERP_BINS;++i){
	if(interp.f[i]<0.0) continue;
	tmp_A=interp.f[i]*table[tmpI].abs.cont[Vapos[i]];
	tmp_amp+=tmp_A;
      }
      value_amp+=tmp_amp*tmpW;
    }

  }else{  /* Non-interpolated value */
    /* Calculate position for readout */
    for(i=0;i<3;++i) pos+= table[*table_id].abs.offset[i]*coord.bin[i];
    value_amp=table[*table_id].abs.cont[pos];
  }


  /* Copy l, rho, and phi bins for future reference */
  *rho_bin = coord.bin[0];
  *phi_bin = coord.bin[1];
  *l_bin = coord.bin[2];

  return value_amp;
}

/** @brief Returns time delay based on input random number for selected table and bin*/
double I3PhotonicsL1Reader::get_photon_delay(double r,int id,int z_bin,
					     int rho_bin,int phi_bin)const{
  int bin;
  double low,high;
  double low_time,high_time;
  double f;
  size_t pos;
  double outtime;

  if(id<0){
    log_error("Photonics Level1: error (bug?). User request with id<0, (id=%d)",id);
    return -1;
  }
  if((id)>=(*n_tables)){
    log_error("Photonics Level1: error (bug?). User request with id>=n_tables (id=%d,n_tables=%d)",id,*n_tables);
    return -1;
  }

  /* Calculate position for readout */
  pos=table[id].prob.offset[0]*rho_bin+table[id].prob.offset[1]*phi_bin+
    table[id].prob.offset[2]*z_bin;

  /* Find which bin random number landed in */
  bin=binary(&table[id].prob.cont[pos],table[id].h_prob.n[5],r);

  /* Algoritham for right edged bins */
  if(bin==table[id].h_prob.n[5]) return -1; // Illegal return; r>1
  ++bin; /* Need to move bin up by one since binary returns in
	    the range [-1,table[id].h_prob.n[5]-1] */
  low = bin>0?table[id].prob.cont[pos+bin-1]:0;
  high = table[id].prob.cont[pos+bin];

  f=(r-low)/(high-low);
  
  /* Find bin edges */
  low_time = bin<table[id].h_prob.n[5]?(*geo).timing[bin]:(*geo).timing[bin-1];
  high_time =  bin<table[id].h_prob.n[5]?(*geo).timing[bin+1]:(*geo).timing[bin];

  /* Interpolate and return random delay time */
  outtime = low_time+f*(high_time-low_time);

  /* convert to interface residual time format */
  outtime = residual_convert(
      mytablesets_level1.tableset[mytablesets_level1.current].header->ref_ng,
      pref_level1.external_ref_ng,
      outtime,
      (*geo).middle[0][rho_bin]  ,
      (*geo).middle[2][  z_bin] ,
      0, /* Note: this is not used for 'ORIGIN' */
      ORIGIN);

  // sorry that we actually use magic numbers, but for now we have to
  // protect -1. if we really want -1, set it to -1+eps
  if(outtime==-1)
    outtime=outtime+EPS;

  return outtime;
}

int I3PhotonicsL1Reader::get_photon_delays(double (*uniform_random)(const void *r, double a, double b), 
					   void (*multinomial_random)(const void *r, size_t k, unsigned int N, const double p[], unsigned int n[]), 
					   const void *r, int N, double *delays, 
					   int id,int z_bin,int rho_bin,int phi_bin)const{
  size_t pos;
  int i,j,bin;

  if(id<0){
    log_error("Photonics Level1: error (bug?). User request with id<0, (id=%d)",id);
    return -1;
  }
  if((id)>=(*n_tables)){
    log_error("Photonics Level1: error (bug?). User request with id>=n_tables (id=%d,n_tables=%d)",id,*n_tables);
    return -1;
  }

  /* Calculate position for readout */
  pos=table[id].prob.offset[0]*rho_bin+table[id].prob.offset[1]*phi_bin+
    table[id].prob.offset[2]*z_bin;

  /* Sample number of photons in each CDF bin */
  assert(table[id].h_prob.n[5]>0);
  unsigned int n[table[id].h_prob.n[5]];
  double cdf[table[id].h_prob.n[5]];
  for (i = 0; i < table[id].h_prob.n[5]; i++)
    cdf[i] = table[id].prob.cont[pos+i];
  for (i = table[id].h_prob.n[5] - 1; i > 0; i--)
    cdf[i] -= cdf[i-1];

  multinomial_random(r, table[id].h_prob.n[5], N, cdf, n);

  for (bin = 0, j = 0; bin < table[id].h_prob.n[5]; bin++) {
     double low, high;
     if (n[bin] == 0)
        continue;

     low = bin<table[id].h_prob.n[5]?(*geo).timing[bin]:(*geo).timing[bin-1];
     high = bin<table[id].h_prob.n[5]?(*geo).timing[bin+1]:(*geo).timing[bin];

     for (i = 0; (unsigned int)i < n[bin]; i++, j++) {
         delays[j] = uniform_random(r, low, high);

         /* convert to interface residual time format */
         delays[j] = residual_convert(
          mytablesets_level1.tableset[mytablesets_level1.current].header->ref_ng,
          pref_level1.external_ref_ng,
          delays[j],
          (*geo).middle[0][rho_bin]  ,
          (*geo).middle[2][  z_bin] ,
          0, /* Note: this is not used for 'ORIGIN' */
          ORIGIN);
     }
  }

  return 0;
}

/* Returns probability of given time delay based for selected table and bin*/
double I3PhotonicsL1Reader::get_delay_prob(double delay,int id,int z_bin,
					   int rho_bin,int phi_bin)const{
  int bin;
  double low,high;
  double low_time,high_time;
  size_t pos;

  /* convert to internal residual time format */
  delay = residual_convert(
      pref_level1.external_ref_ng,
      mytablesets_level1.tableset[mytablesets_level1.current].header->ref_ng,
      delay,
      (*geo).middle[0][rho_bin]  ,
      (*geo).middle[2][  z_bin] ,
      0 , /* Note: this is not used for 'ORIGIN' */
      ORIGIN);


  if(id<0){
    log_error("Photonics Level1: error (bug?). User request with id<0, (id=%d)",id);
    return -1;
  }
  if((id)>=(*n_tables)){
    log_error("Photonics Level1: error (bug?). User request with id>=n_tables (id=%d,n_tables=%d)",id,*n_tables);
    return -1;
  }

  /* Calculate position for readout */
  pos=table[id].prob.offset[0]*rho_bin+table[id].prob.offset[1]*phi_bin+
    table[id].prob.offset[2]*z_bin;

  /* Find which bin random number landed in */
  static int lastbin=-1;
  bin=find((*geo).timing,table[id].h_prob.n[5],delay,lastbin);
  lastbin=bin;

  /* Check validity of bin found */
  if(bin<0 || bin>=table[id].h_prob.n[5]) return -1;

  /* Find bin edges */
  low_time = bin<table[id].h_prob.n[5]?(*geo).timing[bin]:(*geo).timing[bin-1];
  high_time =  bin<table[id].h_prob.n[5]?(*geo).timing[bin+1]:(*geo).timing[bin];

  /* Algoritham for right edged bins */
  low = bin>0?table[id].prob.cont[pos+bin-1]:0;
  high = table[id].prob.cont[pos+bin];

  /* Return dP/dt in nanoseconds */
  return (high-low)/(high_time-low_time);
}

double I3PhotonicsL1Reader::get_level1_lmax()const
{
  return (*geo).bracket[2][table[0].h_abs.n[2]];
}

/*******************************************************************/
/*  io.c : Photonics library routines for I/O of photon tables     */
/*******************************************************************/

bool I3PhotonicsL1Reader::open_binary_input(const char *file,FILE **fh){
  if((*fh=fopen(file,"rb"))==NULL){
    log_error("Unable to open the input file %s.",file);
    return false;
  }
  log_debug("Photonics Level1: Loading table file '%s'",file);
  return true;
}

void I3PhotonicsL1Reader::close_input(FILE *fh){
  if(fh){
    fclose(fh);
    if(data_chunk) free(data_chunk);
    data_chunk=NULL;
    if(err_chunk) free(err_chunk);
    err_chunk=NULL;
  }else if(in){
    fclose(in);
    in=NULL;
    io.h=NULL;
    if(data_chunk) free(data_chunk);
    data_chunk=NULL;
    if(err_chunk) free(err_chunk);
    err_chunk=NULL;
  }
}

bool I3PhotonicsL1Reader::read_header(Header_type *h,FILE *fh)const{
  if(!fh) fh=in;
  if(fh){
    if(fseek(fh,0L,SEEK_SET))
      log_fatal("photonics lib/io.c (read_header):Unable to relocate file stream position.");
    if(fread(h,sizeof(Header_type),1,fh)!=1){
      log_fatal("photonics lib/io.c (read_header):Abort. Error while reading the data header.");
      /* in principal, we should first try read _only_ the metahead... */
    }

    if (!checkMetaHead(&(h->MetaHead)))
      return false;
    
    return true;
  }else{
    return false;
  }
}

/** @brief Read chunk from current stream position  */
float *I3PhotonicsL1Reader::read_current_chunk(FILE *fh){
  if(!fh) fh=in;
  if(!fh) log_fatal("photonics lib/io.c (read_current_chunk):Input not initialized.");
  if(!data_chunk) data_chunk= (float *)calloc(io.n_chunk,io.offset[VARS-1]);
  if(fread(data_chunk,io.offset[VARS-1],io.n_chunk,fh)!=(size_t)io.n_chunk)
    log_fatal("photonics lib/io.c (read_current_chunk): Error while reading data.");

  return data_chunk;
}

/** @brief Verify that it's possible to read data and setup 
   variables for input                             */
FILE *I3PhotonicsL1Reader::test_input(const Opt_type *opt){
  int i;

  if(!open_binary_input(opt->in_file_name,&in)){
    sleep(1);
    log_warn("Trying again.");
    if(!open_binary_input(opt->in_file_name,&in)){
      log_fatal("photonics lib/io.c (test_input):Abort!");
    }
  }

  if(io.h) free(io.h);
  if((io.h=(Header_type *)malloc(sizeof(Header_type)))==NULL)
    log_fatal("photonics lib/io.c (test_input):Unable to allocate input header buffer");

  if(read_header(io.h,NULL)){
    if(!isLittleEndian()){
      if(checkMetaHeadLittle(&(io.h->MetaHead))){
	byteswap32_l1header(io.h);
      }
    }
    io.h_offset = sizeof(Header_type);
    io.offset[VARS-1] = sizeof(float);
    for(i=VARS-1;i>0;--i)
      io.offset[i-1]=io.offset[i]*io.h->n[i];
    io.n_chunk=io.h->n[VARS-1];
  }else
    log_fatal("photonics lib/io.c (test_input):Unable to read data header. Aborting!");

  return in;
}

/** @brief Returns a pointer to header of currently opened  
    input file, or NULL if none available.           */
Header_type *I3PhotonicsL1Reader::get_header()const{
  if(in==NULL){
    return NULL;
  } else {
    return io.h;
  }
}

bool I3PhotonicsL1Reader::mmap_file(const Opt_type *opt,Data_type *data,bool errs)const{
  int fd = 0;
  int i = 0;
  char *ptr = NULL;  //char pointer to match void* arithmatic in original photonics file :-(

  size_t size = io.h->n[0]*io.h->n[1]*io.h->n[2]*io.h->n[3]*io.h->n[4]*io.h->n[5]
    * sizeof(float);

  if (isLittleEndian() != checkMetaHeadLittle(&(io.h->MetaHead))){
	  return false;
  } else {

	  off_t offset, remainder;

	  data->offset[VARS-1] = 1;
	  for(i=VARS-1;i>0;--i) data->offset[i-1]=data->offset[i]*io.h->n[i];
	
	  fd = fileno(in);

	  offset = ftell(in);
	  remainder = offset % PAGE_SIZE;
	  offset -= remainder;
	  ptr = (char *)mmap(NULL,size + remainder,PROT_READ,
		     MAP_PRIVATE, fd, offset);
	  if (ptr == MAP_FAILED){
	    log_error("mmap() failed with %s", strerror(errno));
	    return false;
	  }
          if (ptr == NULL) {
            log_error("whoa something wierd happend with mmap()");
            return false;
          }
	  fseek(in, size, SEEK_CUR);
	  ptr += remainder;
	  data->cont = (float *)ptr;

	  if (errs) {
	       offset = ftell(in);
	       remainder = offset % PAGE_SIZE;
	       offset -= remainder;
	       ptr = (char *)mmap(NULL,size + remainder,PROT_READ,
		     MAP_PRIVATE, fd, offset);
	       fseek(in, size, SEEK_CUR);
	       ptr += remainder;
	       data->err = (float *)ptr;
	  }
	  data->mmaped = size + remainder;
  }

  return true;
}

/** @brief Entire photon table file is readout
    but without errors                 */
bool I3PhotonicsL1Reader::read_file_without_errors(const Opt_type *opt,Data_type *data){
  test_input(opt);
  
  data->mmaped = 0;
  if (mmap_file(opt, data, false))
	return true;

  if(io.h->record_errors){  /* We need a bit of hack in this case */
    Header_type tmp_h;
    copy_header(io.h,&tmp_h);
    tmp_h.record_errors=false;
    data_alloc(data,&tmp_h);
  }else
    data_alloc(data,io.h);
  return read_table(data->cont,NULL);
}

/* read_table: Entire photon array is readout out               */
bool I3PhotonicsL1Reader::read_table(float *array,FILE *fh){
  int i,j,k,l,m,n;
  size_t pos=0;
  float *data;
  
  if(!fh) fh=in;
  if(!fh){
    log_error("No input file opened.");
    return false;
  }

  for(i=0;i<io.h->n[0];++i)            /* Record array */
    for(j=0;j<io.h->n[1];++j)
      for(k=0;k<io.h->n[2];++k)
	for(l=0;l<io.h->n[3];++l)
	  for(m=0;m<io.h->n[4];++m){
	    data=read_current_chunk(NULL);
	    for(n=0;n<io.h->n[5];++n) array[pos++]=data[n];
	  }

  if(!isLittleEndian()){
    unsigned long size = io.h->n[0]*io.h->n[1]*io.h->n[2]*io.h->n[3]*io.h->n[4]*io.h->n[5];
    if(checkMetaHeadLittle(&(io.h->MetaHead))){
      {
	unsigned long k;
	for(k=0;k<size;++k){
	  px_swap32(&(array[k]));
	}
      }
    }
  }

  return true;
}

void I3PhotonicsL1Reader::byteswap32_l1header(Header_type * h){
  int i,j;

  // Swapping inspired by C++ variant by Alex Olivas. /J
  
  // We'll read the values into these//  // THIS COLUME IS CUT IN HERE FROM photonics.h

                                            //  MetaHead_type MetaHead;
                                            //  char dummy_padding[20]; /* not used */
  px_swap32((void*)&(h->impsampl_Lf));      //  float impsampl_Lf  ;    /* Importance sampling factor for exp() steps length */
  px_swap32((void*)&(h->impsampl_Tf));      //  float impsampl_Tf  ;    /* Importance sampling factor for HG scattering      */
  px_swap32((void*)&(h->dummy_l2padd));     //  float dummy_l2padd;     /* not used [in level1, gauss factor in level2]      */
  px_swap32((void*)&(h->ref_np));           //  float ref_np;     /*phase refractive index used for true <--> residual calc*/
  px_swap32((void*)&(h->ref_ng));           //  float ref_ng;     /*phase refractive index used for true <--> residual calc*/
  px_swap32((void*)&(h->record_errors));    //  bool record_errors; /* Statistical errors are tracked */
  px_swap32((void*)&(h->source));           //  source_type source; /* Emission source type */
  px_swap32((void*)&(h->extended_source));  //  bool extended_source; /* Simulaton of extended shower sources */
  px_swap32((void*)&(h->step));             //  float step;         /* Step for photon tracking (m) */
  px_swap32((void*)&(h->e));                //  float e;            /* Shower energy for shower sources in (GeV) */
  px_swap32((void*)&(h->volume));           //  float volume;       /* Radial size for volume source (m) */
  px_swap32((void*)&(h->angle));            //  float angle;        /* Source axis zenith angle */
  //   DONT FORGET TO LOOP          //  float source_rz[2]; /* Source location wrt active volume center */
  px_swap32((void*)&(h->geo));              //  geo_type geo;      /* Geometry type */
  //   DONT FORGET THIS EITHER      //  ph_int n[VARS];       /* Number of bins in each dimension */
  //     OR THIS                    //  float limits[VARS][2]; /* Recordable limits for each dimension */
  //       THIS LOOKS LIKE THE LAST //  float maxes[VARS][2];  /* Active limits for each dimension */
  px_swap32((void*)&(h->depth));            //  float depth;       /* depth of active volume center wrt ice table */
  px_swap32((void*)&(h->d_scale));          //  scale_type d_scale;  /* distance scaling parameter */
  px_swap32((void*)&(h->t_scale));          //  scale_type t_scale;  /* time scaling parameter */
  px_swap32((void*)&(h->lambda));           //  float lambda;      /* light wavelength, or spectral "index" */
  px_swap32((void*)&(h->efficiency));       //  ph_int efficiency;    /* Efficinency calibrations applied */
  px_swap64((void*)&(h->n_photon));         //  ph_long n_photon;      /* number of photons to generate */
  px_swap64((void*)&(h->n_entries));        //  ph_long n_entries;   /* total number of MC entries                */
  px_swap32((void*)&(h->refraction_mode));  //  ph_int refraction_mode;  /* refraction mode */
  px_swap32((void*)&(h->User_ieC));         //  ph_int User_ieC;    /* not used */
  for(i=0; i<2; i++)
    px_swap32((void*)&(h->source_rz[i]));
					   
  for(i=0; i<VARS; i++)
    px_swap32((void*)&(h->n[i]));
					   
  for(i=0; i<VARS; i++)
    for(j=0; j<2; j++)
      px_swap32((void*)&(h->limits[i][j]));

  for(i=0; i<VARS; i++)
    for(j=0; j<2; j++)
      px_swap32((void*)&(h->maxes[i][j]));

}

/*******************************************************************/
/*  geometry.c : Photonics library routines for generating geometry*/
/*******************************************************************/

/* Functions calculating next boundry value based in scaling used. */
double I3PhotonicsL1Reader::next_boundary_lin(const double base,const double step){
  double bound;
  bound=base+step;
  return fabs(bound-(int)bound)>EPS?bound:(int)bound;
}

/** @brief Set up azimuthal regions based on user input   
 *  NB: Values kept as cosines for faster computation down the line */
bool I3PhotonicsL1Reader::evaluate_slices(const Header_type *h,Geo_type *geo){
  int i;

  /* Step is kept in radians */
  geo->d[1]=(Rad(h->limits[1][1])-Rad(h->limits[1][0]))/h->n[1];

  /* Setup azimuthal slices (linear in radians, kept in cosines) */
  assert(geo->n[1]>0);
  for (i=0; i<geo->n[1]; ++i){
    if(i==0){            /* Lowest slice */
      if(geo->buffer[1][0]){
	geo->dim[1][i].low.val=Rad(h->maxes[1][0]);
	geo->dim[1][i].high.val=Rad(h->limits[1][0]);
      }else{
	geo->dim[1][i].low.val=Rad(h->limits[1][0]);
	geo->dim[1][i].high.val=next_boundary_lin(geo->dim[1][i].low.val,
						  geo->d[1]);
      }
    }else if(i==geo->n[1]-1){   /* Highest slice */
      geo->dim[1][i].low.val=geo->dim[1][i-1].high.val;
      if(geo->buffer[1][1])
	geo->dim[1][i].high.val=Rad(h->maxes[1][1]);
      else
	geo->dim[1][i].high.val=Rad(h->limits[1][1]);

      /* Check bounds for round off errors */
    }else{  /* Other slices */
      geo->dim[1][i].low.val=geo->dim[1][i-1].high.val;
      geo->dim[1][i].high.val=next_boundary_lin(geo->dim[1][i].low.val,
						geo->d[1]);
    }
  }
  
  /* Convert to cosines and evaluate mid point values */
  for (i=0; i<geo->n[1]; ++i){
    double mid;
    mid = (geo->dim[1][i].low.val+geo->dim[1][i].high.val)/2;

    geo->dim[1][i].low.val=cos(geo->dim[1][i].low.val);
    geo->dim[1][i].high.val=cos(geo->dim[1][i].high.val);
    geo->dim[1][i].low.sine=sqrt(1-sqr(geo->dim[1][i].low.val));
    geo->dim[1][i].high.sine=sqrt(1-sqr(geo->dim[1][i].high.val));
    
    geo->dim[1][i].mid.cosine=cos(mid);
    geo->dim[1][i].mid.sine=sqrt(1-sqr(geo->dim[1][i].mid.cosine));

    /* Check round off errors */
    if(fabs(geo->dim[1][i].low.val)<EPS) geo->dim[1][i].low.val=0;
    if(fabs(geo->dim[1][i].high.val)<EPS) geo->dim[1][i].high.val=0;
    
    log_debug("Slice %2d  ----> %6.2f - %6.2f deg",i+1,
	      Deg(acos(geo->dim[1][i].low.val)),
	      Deg(acos(geo->dim[1][i].high.val)));
  }

  /* Check bounds for round off errors */
  if(fabs(geo->dim[1][0].low.val-1.0)<EPS) geo->dim[1][0].low.val=1.0;
  if(fabs(geo->dim[1][geo->n[1]-1].high.val+1.0)<EPS)
    geo->dim[1][geo->n[1]-1].high.val=-1.0;

  return true;
}

/** @brief  Set up cylindrical active volume based   
    on user input                            */
bool I3PhotonicsL1Reader::cylindrical_geometry(const Header_type *h,Geo_type *geo){
  double (*next_boundary)(const double,const double);
  int i;

  switch(h->d_scale){ /* Select scaling requested */
  case LINEAR :
    geo->d[0]=(h->limits[0][1]-h->limits[0][0])/h->n[0];
    geo->d[2]=(h->limits[2][1]-h->limits[2][0])/h->n[2];
    next_boundary=next_boundary_lin;
    break;
  case QUADRATIC :
    geo->d[0]=(sqrt(h->limits[0][1])-sqrt(h->limits[0][0]))/h->n[0];

    if(h->limits[2][1]>=0 && h->limits[2][0]<=0) /* Around center */
      geo->d[2]=(sqrt(h->limits[2][1])+sqrt(-h->limits[2][0]))/h->n[2];
    else             /* Offset to one side */
      geo->d[2]=fabs(sqrt(fabs(h->limits[2][1]))-sqrt(fabs(h->limits[2][0])))/
	h->n[2];
    
    next_boundary=next_boundary_sqr;
    break;
  default:
    log_error("Unknown distance scaling requested.");
    return false;
  }

  /* Set up cylindrical shells */
  for (i=0; i<geo->n[0]; ++i){
    if(i==0){       /* Innermost shell */
      if(geo->buffer[0][0]){
	geo->dim[0][i].low.val=h->maxes[0][0];
	geo->dim[0][i].high.val=h->limits[0][0];
      }else{
	geo->dim[0][i].low.val=h->limits[0][0];
	geo->dim[0][i].high.val=
	  next_boundary(geo->dim[0][i].low.val,geo->d[0]);
      }
    }else if(i==geo->n[0]-1){   /* Outermost shell */
      geo->dim[0][i].low.val=geo->dim[0][i-1].high.val;
      if(geo->buffer[0][1])
	geo->dim[0][i].high.val=h->maxes[0][1];
      else
	geo->dim[0][i].high.val=h->limits[0][1];
    }else{  /* Other shells */
      geo->dim[0][i].low.val=geo->dim[0][i-1].high.val;
      geo->dim[0][i].high.val=next_boundary(geo->dim[0][i].low.val,geo->d[0]);
    }
    log_debug("Cylinder %2d  ---->  %6.2f - %6.2f m",i+1,
	      geo->dim[0][i].low.val,
	      geo->dim[0][i].high.val);
  }

  /* Set up vertical regions */
  for(i=0;i<geo->n[2];++i){
    if(i==0){       /* Bottommost level */
      if(geo->buffer[2][0]){
	geo->dim[2][i].low.val=h->maxes[2][0];
	geo->dim[2][i].high.val=h->limits[2][0];
      }else{
	geo->dim[2][i].low.val=h->limits[2][0];
	geo->dim[2][i].high.val=next_boundary(geo->dim[2][i].low.val,
					      geo->d[2]);
      }
    }else if(i==geo->n[2]-1){   /* Topmost level */
      geo->dim[2][i].low.val=geo->dim[2][i-1].high.val;
      if(geo->buffer[2][1])
	geo->dim[2][i].high.val=h->maxes[2][1];
      else
	geo->dim[2][i].high.val=h->limits[2][1];
    }else{  /* Other levels */
      geo->dim[2][i].low.val=geo->dim[2][i-1].high.val;
      geo->dim[2][i].high.val=next_boundary(geo->dim[2][i].low.val,geo->d[2]);
    }
    log_debug("Level %2d  ---->  %7.2f m - %7.2f m",i+1,
	      geo->dim[2][i].low.val,
	      geo->dim[2][i].high.val);
  }
    
  return evaluate_slices(h,geo) && cylindrical_areas(h,geo);
}

/** @brief Evaluate area of each surface edge        */
bool I3PhotonicsL1Reader::cylindrical_areas(const Header_type *h,Geo_type *geo){
  int i,j,k;

  log_debug("Calculating surface areas.");

  /*************   Cylinder areas   **********/

  /* Allocate memory we need */
  for(i=0;i<geo->n[0];++i){
    if(!(geo->dim[0][i].low.area = (double **)malloc(sizeof(double*)*geo->n[1])))
      return false;
    for(j=0;j<geo->n[1];++j)
      if(!(geo->dim[0][i].low.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;
    
    if(!(geo->dim[0][i].high.area = (double **)malloc(sizeof(double*)*geo->n[1])))
      return false;
    for(j=0;j<geo->n[1];++j)
      if(!(geo->dim[0][i].high.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;

    for(j=0;j<geo->n[1];++j)
      for(k=0;k<geo->n[2];++k){
	geo->dim[0][i].low.area[j][k]=2*M_PI*geo->dim[0][i].low.val*
	  (geo->dim[2][k].high.val-geo->dim[2][k].low.val)*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][j].high.val)-acos(geo->dim[1][j].low.val))/(2*M_PI):
	   1/M_PI); /* Changed back to 1/PI (from 1 set by JL) to account for
		     fix in source.c; PM Dec 20, 2004
		     Changed Oct 1 2004 by Johan Lundberg, was:1/PI);
		     Ped's comment: Projected area is at most 2rz */
	geo->dim[0][i].high.area[j][k]=2*M_PI*geo->dim[0][i].high.val*
	  (geo->dim[2][k].high.val-geo->dim[2][k].low.val)*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][j].high.val)-acos(geo->dim[1][j].low.val))/(2*M_PI):
	   1/M_PI); /* Changed back to 1/PI (from 1 set by JL) to account for
		     fix in source.c; PM Dec 20, 2004
		     Changed Oct 1 2004 by Johan Lundberg */
      }
  }

  /*************    Slice areas   **********/

  /* Allocate memory we need */
  for(i=0;i<geo->n[1];++i){
    if(!(geo->dim[1][i].low.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[1][i].low.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;
    
    if(!(geo->dim[1][i].high.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[1][i].high.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;

    for(j=0;j<geo->n[0];++j)
      for(k=0;k<geo->n[2];++k){
	if(geo->n[1]>1)
	  geo->dim[1][i].low.area[j][k]=geo->dim[1][i].high.area[j][k]=
	    (geo->dim[0][j].high.val-geo->dim[0][j].low.val)*
	    (geo->dim[2][k].high.val-geo->dim[2][k].low.val);
	else
	  geo->dim[1][i].low.area[j][k]=geo->dim[1][i].high.area[j][k]=0;
      }
  }

  /*************  Level areas   **********/

  /* Allocate memory we need */
  for(i=0;i<geo->n[2];++i){
    if(!(geo->dim[2][i].low.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[2][i].low.area[j] = (double *)calloc(geo->n[1],sizeof(double))))
	return false;
  
    if(!(geo->dim[2][i].high.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[2][i].high.area[j] = (double *)calloc(geo->n[1],sizeof(double))))
	return false;
    
    for(j=0;j<geo->n[0];++j)
      for(k=0;k<geo->n[1];++k){
	geo->dim[2][i].low.area[j][k]=geo->dim[2][i].high.area[j][k]=
	  M_PI*(sqr(geo->dim[0][j].high.val)-sqr(geo->dim[0][j].low.val))*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][k].high.val)-acos(geo->dim[1][k].low.val))/(2*M_PI):
	   1);
      }
  }

  return true;
}

/** @brief  Set up spherical active volume based on    
    user input                                 */
bool I3PhotonicsL1Reader::spherical_geometry(const Header_type *h,Geo_type *geo){
  double (*next_boundary)(const double,const double);
  int i;
  bool tmp_bool;

  switch(h->d_scale){ /* Select scaling requested */
  case LINEAR :
    geo->d[0]=(h->limits[0][1]-h->limits[0][0])/h->n[0];
    next_boundary=next_boundary_lin;
    break;
  case QUADRATIC :
    geo->d[0] = (sqrt(h->limits[0][1])-sqrt(h->limits[0][0]))/h->n[0];
    next_boundary=next_boundary_sqr;
    break;
  default:
    log_error("Unknown distance scaling requested.");
    return false;
  }

  /* Theta always binned in cosine scaling */
  geo->d[2]=(cos(Rad(h->limits[2][0]))-cos(Rad(h->limits[2][1])))/h->n[2];

  /* Set up radial shells */
  for (i=0; i<geo->n[0]; ++i){
    if(i==0){       /* Innermost shell */
      if(geo->buffer[0][0]){
	geo->dim[0][i].low.val=h->maxes[0][0];
	geo->dim[0][i].high.val=h->limits[0][0];
      }else{
	geo->dim[0][i].low.val=h->limits[0][0];
	geo->dim[0][i].high.val=next_boundary(geo->dim[0][i].low.val,
					      geo->d[0]);
      }
    }else if(i==geo->n[0]-1){   /* Outermost shell */
      geo->dim[0][i].low.val=geo->dim[0][i-1].high.val;
      if(geo->buffer[0][1])
	geo->dim[0][i].high.val=h->maxes[0][1];
      else
	geo->dim[0][i].high.val=h->limits[0][1];
    }else{  /* Other shells */
      geo->dim[0][i].low.val=geo->dim[0][i-1].high.val;
      geo->dim[0][i].high.val=next_boundary(geo->dim[0][i].low.val,geo->d[0]);
    }
    log_debug("Shell %2d  ----> %6.2f - %6.2f m",i+1,geo->dim[0][i].low.val,
	      geo->dim[0][i].high.val);
  }

  /* Setup theta cones */
  for (i=0; i<geo->n[2]; ++i){
    if(i==0){            /* Lowest cosine cone */
      if(geo->buffer[2][1]){
	geo->dim[2][i].low.val=cos(Rad(h->maxes[2][1]));
	geo->dim[2][i].high.val=cos(Rad(h->limits[2][1]));
      }else{
	geo->dim[2][i].low.val=cos(Rad(h->limits[2][1]));
	geo->dim[2][i].high.val=next_boundary_lin(geo->dim[2][i].low.val,
						  geo->d[2]);
      }
      /* Check bounds for round off errors */
      if(geo->dim[2][i].low.val<-1) geo->dim[2][i].low.val=-1;
    }else if(i==geo->n[2]-1){   /* Highest cosine cone */
      geo->dim[2][i].low.val=geo->dim[2][i-1].high.val;
      if(geo->buffer[2][0])
	geo->dim[2][i].high.val=cos(Rad(h->maxes[2][0]));
      else
	geo->dim[2][i].high.val=cos(Rad(h->limits[2][0]));
      /* Check bounds for round off errors */
      if(geo->dim[2][i].high.val>1) geo->dim[2][i].high.val=1;
    }else{  /* Other cones */
      geo->dim[2][i].low.val=geo->dim[2][i-1].high.val;
      geo->dim[2][i].high.val=next_boundary_lin(geo->dim[2][i].low.val,
						geo->d[2]);
    }
    log_debug("Cone %2d  ----> %6.2f - %6.2f deg",i+1,
	      Deg(acos(geo->dim[2][i].low.val)),
	      Deg(acos(geo->dim[2][i].high.val)));
  }

  /* Evaluate conical mid points and edge sines */
  for (i=0; i<geo->n[2]; ++i){
    double mid=(acos(geo->dim[2][i].low.val)+acos(geo->dim[2][i].high.val))/2;
    geo->dim[2][i].mid.cosine = cos(mid);
    geo->dim[2][i].mid.sine = sin(mid);
    geo->dim[2][i].low.sine = sqrt(1-sqr(geo->dim[2][i].low.val));
    geo->dim[2][i].high.sine = sqrt(1-sqr(geo->dim[2][i].high.val));
  }

  /* For consistency with other coordinates, we invert geo->buffer[2] */
  tmp_bool = geo->buffer[2][1];
  geo->buffer[2][1] = geo->buffer[2][0];
  geo->buffer[2][0] = tmp_bool;

  return evaluate_slices(h,geo) && spherical_areas(h,geo);
}

/** @brief Evaluate area of each surface edge          */
bool I3PhotonicsL1Reader::spherical_areas(const Header_type *h,Geo_type *geo){
  int i,j,k;

  log_debug("Calculating surface areas.");

  /*************    Shell areas   **********/

  for(i=0;i<geo->n[0];++i){
    /* Allocate memory we need */
    if(!(geo->dim[0][i].low.area = (double **)malloc(sizeof(double*)*geo->n[1])))
      return false;
    for(j=0;j<geo->n[1];++j)
      if(!(geo->dim[0][i].low.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;
  
    if(!(geo->dim[0][i].high.area = (double **)malloc(sizeof(double*)*geo->n[1])))
      return false;
    for(j=0;j<geo->n[1];++j)
      if(!(geo->dim[0][i].high.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;

    for(j=0;j<geo->n[1];++j)
      for(k=0;k<geo->n[2];++k){
	geo->dim[0][i].low.area[j][k]=4*M_PI*sqr(geo->dim[0][i].low.val)*
	  (geo->dim[2][k].high.val-geo->dim[2][k].low.val)/2*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][j].high.val)-acos(geo->dim[1][j].low.val))/(2*M_PI):
	   4); /* Max area is Pi r^2 */
	geo->dim[0][i].high.area[j][k]=4*M_PI*sqr(geo->dim[0][i].high.val)*
	  (geo->dim[2][k].high.val-geo->dim[2][k].low.val)/2*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][j].high.val)-acos(geo->dim[1][j].low.val))/(2*M_PI):
	   4); /* Max area is Pi r^2 */
      }
  }

  /*************    Slice areas   **********/

  for(i=0;i<geo->n[1];++i){
    /* Allocate memory we need */
    if(!(geo->dim[1][i].low.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[1][i].low.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;
    
    if(!(geo->dim[1][i].high.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[1][i].high.area[j] = (double *)calloc(geo->n[2],sizeof(double))))
	return false;

    for(j=0;j<geo->n[0];++j)
      for(k=0;k<geo->n[2];++k){
	if(geo->n[1]>1)
	  geo->dim[1][i].low.area[j][k]=geo->dim[1][i].high.area[j][k]=
	    (sqr(geo->dim[0][j].high.val)-sqr(geo->dim[0][j].low.val))/2*
	    (acos(geo->dim[2][k].low.val)-acos(geo->dim[2][k].high.val));
	else
	  geo->dim[1][i].low.area[j][k]=geo->dim[1][i].high.area[j][k]=0;
      }
  }

  /*************    Cone areas   **********/

  for(i=0;i<geo->n[2];++i){
    /* Allocate memory we need */
    if(!(geo->dim[2][i].low.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[2][i].low.area[j] = (double *)calloc(geo->n[1],sizeof(double))))
	return false;
    
    if(!(geo->dim[2][i].high.area = (double **)malloc(sizeof(double*)*geo->n[0])))
      return false;
    for(j=0;j<geo->n[0];++j)
      if(!(geo->dim[2][i].high.area[j] = (double *)calloc(geo->n[1],sizeof(double))))
	return false;

    for(j=0;j<geo->n[0];++j)
      for(k=0;k<geo->n[1];++k){
	geo->dim[2][i].low.area[j][k]=
	  M_PI*(sqr(geo->dim[0][j].high.val)-sqr(geo->dim[0][j].low.val))*
	  sqrt(1-sqr(geo->dim[2][i].low.val))*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][k].high.val)-acos(geo->dim[1][k].low.val))/(2*M_PI):
	   1);
	geo->dim[2][i].high.area[j][k]=
	  M_PI*(sqr(geo->dim[0][j].high.val)-sqr(geo->dim[0][j].low.val))*
	  sqrt(1-sqr(geo->dim[2][i].high.val))*
	  (geo->n[1]>1?
	   (acos(geo->dim[1][k].high.val)-acos(geo->dim[1][k].low.val))/(2*M_PI):
	   1);
      }
  }

  return true;
}


/** @brief Set up cubic active volume based on user input */
bool I3PhotonicsL1Reader::cubic_geometry(const Header_type *h,Geo_type *geo){
  double (*next_boundary)(const double,const double);
  int i,n;

  switch(h->d_scale){ /* Select scaling requested */
  case LINEAR :
    for(n=0;n<3;++n) geo->d[n]=(h->limits[n][1]-h->limits[n][0])/h->n[n];
    next_boundary=next_boundary_lin;
    break;
  case QUADRATIC :
    for(n=0;n<3;++n)
      if(h->limits[n][0]<=0 && h->limits[n][1]>=0)/* Around center */
	geo->d[n]=(sqrt(h->limits[n][1])+sqrt(-h->limits[n][0]))/h->n[n];
      else             /* Offset to one side */
	geo->d[n]=fabs(sqrt(fabs(h->limits[n][1]))-
		       sqrt(fabs(h->limits[n][0])))/h->n[n];
    next_boundary=next_boundary_sqr;
    break;
  default:
    log_error("Unknown distance scaling requested.");
    return false;
  }

  /* Set up cubic regions */
  for(n=0;n<3;++n){
    for(i=0;i<geo->n[n];++i){
      if(i==0){       /* Minimum level */
	if(geo->buffer[n][0]){
	  geo->dim[n][i].low.val=h->maxes[n][0];
	  geo->dim[n][i].high.val=h->limits[n][0];
	}else{
	  geo->dim[n][i].low.val=h->limits[n][0];
	  geo->dim[n][i].high.val=next_boundary(geo->dim[n][i].low.val,
						geo->d[n]);
	}
      }else if(i==geo->n[n]-1){   /* Maximum level */
	geo->dim[n][i].low.val=geo->dim[n][i-1].high.val;
	if(geo->buffer[n][1])
	  geo->dim[n][i].high.val=h->maxes[n][1];
	else
	  geo->dim[n][i].high.val=h->limits[n][1];
      }else{  /* Other levels */
	geo->dim[n][i].low.val=geo->dim[n][i-1].high.val;
	geo->dim[n][i].high.val=next_boundary(geo->dim[n][i].low.val,
					      geo->d[n]);
      }
      log_debug("Dim %1d level %2d  ---->  %7.2f m - %7.2f m",n+1,i+1,
		geo->dim[n][i].low.val,
		geo->dim[n][i].high.val);
    }
  }
    
  return cubic_areas(h,geo);
}


/** @brief Evaluate area of each surface edge              */
bool I3PhotonicsL1Reader::cubic_areas(const Header_type *h,Geo_type *geo){
  int i,j,k,n,nn,nnn;

  log_debug("Calculating surface areas.");

  /*************   Boundry areas   **********/
  for(n=0;n<3;++n){
    nn=(n==0);
    nnn=2-(n==2);

    /* Allocate memory we need */
    for(i=0;i<geo->n[n];++i){
      if(!(geo->dim[n][i].low.area = (double **)malloc(sizeof(double*)*geo->n[nn])))
	return false;
      for(j=0;j<geo->n[nn];++j)
	if(!(geo->dim[n][i].low.area[j] = (double *)calloc(geo->n[nnn],sizeof(double))))
	  return false;
      
      if(!(geo->dim[n][i].high.area = (double **)malloc(sizeof(double*)*geo->n[nn])))
	return false;
      for(j=0;j<geo->n[nn];++j)
	if(!(geo->dim[n][i].high.area[j] = (double *)calloc(geo->n[nnn],sizeof(double))))
	  return false;

      /* Calculate areas */
      for(j=0;j<geo->n[nn];++j)
	for(k=0;k<geo->n[nnn];++k)
	  geo->dim[n][i].low.area[j][k]=geo->dim[n][i].high.area[j][k]=
	    (geo->dim[nn][j].high.val-geo->dim[nn][j].low.val)*
	    (geo->dim[nnn][k].high.val-geo->dim[nnn][k].low.val);
      
    }
  }

  return true;
}


/* @brief Calculates timing bin coordinates based on  
   user input                                  */
bool I3PhotonicsL1Reader::timing_geometry(const Header_type *h,Geo_type *geo){
  double (*next_boundary)(const double,const double);
  int i;

  /* Allocate memory */
  if(!(geo->timing = (double *)malloc(sizeof(double)*(h->n[5]+1))))
     return false;
  if(!(geo->timing_middle = (double *)malloc(sizeof(double)*(h->n[5]))))
    return false;
     
  switch(h->t_scale){
  case LINEAR :
    geo->d[5]=(h->limits[5][1]-h->limits[5][0])/h->n[5];
    next_boundary=next_boundary_lin;
    break;
  case QUADRATIC :
    geo->d[5]=(sqrt(h->limits[5][1])-sqrt(h->limits[5][0]))/h->n[5];
    next_boundary=next_boundary_sqr;
    break;
  default:
    log_error("Unknown time scaling requested.");
    return false;
  }

  /* Set up timing bins */
  for(i=0;i<=h->n[5];++i){
    if(i==0)                       /* First bin */
      geo->timing[i]=h->limits[5][0];
    else if(i==h->n[5])          /* Last bin upper edge */
      geo->timing[i]=h->limits[5][1];
    else                           /* Other bins */
      geo->timing[i]=next_boundary(geo->timing[i-1],geo->d[5]);
  }

  /* Calculate middles of bins */
  for(i=0;i<h->n[5];++i){
    geo->timing_middle[i]=(geo->timing[i]+geo->timing[i+1])/2;
    log_debug("Timing bin %3d  ---->  %7.1f ns - %7.1f ns (%7.1f)",i+1,
	      geo->timing[i],geo->timing[i+1],geo->timing_middle[i]);
  }

  return true;
}


/*  set_up_geometry:   set up requested geometry                */
bool I3PhotonicsL1Reader::set_up_geometry(const Header_type *h,Geo_type *geo){
  int i,j;
  bool status;
  
  /* Decide how many bins we need for each dimension and allocate memory */
  for(i=0;i<3;++i){
    geo->n[i]=h->n[i];
    if(h->maxes[i][0]<h->limits[i][0]){
      ++geo->n[i];
      geo->buffer[i][0]=true;
    }else
      geo->buffer[i][0]=false;
    if(h->maxes[i][1]>h->limits[i][1]){
      ++geo->n[i];
      geo->buffer[i][1]=true;
    }else
      geo->buffer[i][1]=false;

    if(!(geo->bracket[i] = (double *)malloc(sizeof(double)*(geo->n[i]+1))))
      return false;
    if(!(geo->middle[i] = (double *)malloc(sizeof(double)*(geo->n[i]))))
      return false;
    if(!(geo->dim[i] = (Region_type *)malloc(sizeof(Region_type)*geo->n[i])))
      return false;
#ifdef DEBUG
    if(!(debug.cross[i] = calloc(geo->n[i],sizeof(int))))
      return false;
#endif
  }

  /* Evaluate active volume region edges */
  switch(h->geo){
  case SPHERICAL :
    log_fatal("Support for Spherical Geometry has been discontinued");
    break;
  case CYLINDRICAL :
    status = cylindrical_geometry(h,geo);
    break;
  case CUBIC :
    log_fatal("Support for Cubic Geometry has been discontinued");
    break;
  default:
    log_error("Unknown geometry type %d requested.",h->geo);
    return false;
  }

  /* Return if there was a problem */
  if(!status) return false;
  
  /* Copy edges into bracket array for fast search */
  //assert(geo->n[i]>0);
  for(i=0;i<3;++i){
    for(j=0;j<geo->n[i];++j)
      geo->bracket[i][j] = geo->dim[i][j].low.val;
    geo->bracket[i][geo->n[i]]= geo->dim[i][geo->n[i]-1].high.val;

    for(j=0;j<geo->n[i];++j){
      if(i==1 && h->geo!=CUBIC) // Treat azimuth values differently
	geo->middle[i][j] = (acos(geo->bracket[i][j])+acos(geo->bracket[i][j+1]))/2;
      else
	geo->middle[i][j] = (geo->bracket[i][j]+geo->bracket[i][j+1])/2;
      log_debug("Dimension %1d, bin %3d  ----> %6.2f - %6.2f (%6.2f)",i+1,j+1,
		geo->bracket[i][j],geo->bracket[i][j+1],geo->middle[i][j]);
    }
  }

  /* Calculate some zenith steps we'll need later */
  geo->d[3]=(cos(Rad(h->limits[3][0]))-cos(Rad(h->limits[3][1])))/h->n[3];
  geo->d[4]=(cos(Rad(h->limits[4][0]))-cos(Rad(h->limits[4][1])))/h->n[4];

  /* Finally create timing bin search array and return */
  return timing_geometry(h,geo);
}

/*******************************************************************/
/*  Reader.c : Photonics library routines for reading of photon    */
/*             tables. (Higher level I/O)                          */
/*******************************************************************/

bool I3PhotonicsL1Reader::get_coord(const Header_type *h,const Geo_type *geo,
				    Read_type *coord){
  int i,j;
  int type;

  static double tmplimcos4=1000;
  static double tmplim4=0;

  static double tmpd4=1000;
  static double tmpdinv4=0;

  // so that we don't have to recompute at each call
  if(tmplimcos4==1000 || (tmplim4!=h->limits[4][1])){
    tmplimcos4=cos(Rad(h->limits[4][1]));
    tmplim4=h->limits[4][1];
  }

  // so that we don't have to recompute at each call
  if(tmpd4==1000 || (tmpd4!=geo->d[4])){
    tmpdinv4=1.0/(geo->d[4]+EPS);
    tmpd4=geo->d[4];
  }

  for(i=0;i<VARS;++i){
    coord->n[i]=h->n[i];
    type = 1 << i;
    switch(i){
    case 1:
      /* Azimuhtal coordinate is treated different since it's stored as a cosine */
      if(h->geo!=CUBIC){
	j = find(geo->bracket[i],coord->n[i],cos(coord->val[i]),-1);
	coord->bin[i]=j;
	if(coord->interp_mask&type && j>=0 && j<coord->n[i]){
	  coord->binCenter[i]=geo->middle[i][j];  /* Middle's are stored in radians */
	  if(coord->val[i]>coord->binCenter[i] && j<coord->n[i]-1)
	    coord->binSpacing[i]=geo->middle[i][j+1]-coord->binCenter[i];
	  else if(coord->val[i]<coord->binCenter[i] && j>0)
	    coord->binSpacing[i]=coord->binCenter[i]-geo->middle[i][j-1];
	  else
	    coord->binSpacing[i]=0;
	}
	break;
      }
      /* Let it fall through for CUBIC geometry */
    case 0:
    case 2:
      j = find(geo->bracket[i],coord->n[i],coord->val[i],-1);
      coord->bin[i]=j;
      if(coord->interp_mask&type && j>=0 && j<coord->n[i]){
	coord->binCenter[i]=geo->middle[i][j];
	if(coord->val[i]>coord->binCenter[i] && j<coord->n[i]-1)
	  coord->binSpacing[i]=geo->middle[i][j+1]-coord->binCenter[i];
	else if(coord->val[i]<coord->binCenter[i] && j>0)
	  coord->binSpacing[i]=coord->binCenter[i]-geo->middle[i][j-1];
	else
	  coord->binSpacing[i]=0;
      }
      break;
    case 3: /* These values are always in cosine */
    case 4:
      j=(int)((coord->val[i]-tmplimcos4)*tmpdinv4);
      coord->bin[i]=j;
      if(coord->interp_mask&type && j>=0 && j<coord->n[i]){
	coord->binCenter[i]=(j+0.5)*geo->d[i]+tmplimcos4;
	/* This is bit redundant, but letter on we check whether to do interpolation
	   by coord->binSpacing[i]!=0 */
	if(coord->val[i]>coord->binCenter[i] && j<coord->n[i]-1)
	  coord->binSpacing[i]=geo->d[i];
	else if(coord->val[i]<coord->binCenter[i] && j>0)
	  coord->binSpacing[i]=geo->d[i];
	else
	  coord->binSpacing[i]=0;
      }
      break;
    case 5:
      j = find(geo->timing,coord->n[i],coord->val[i],-1);
      coord->bin[i]=j;
      if(coord->interp_mask&type && j>=0 && j<coord->n[i]){
	coord->binCenter[i]=geo->timing_middle[j];
	if(coord->val[i]>coord->binCenter[i] && j<coord->n[i]-1)
	  coord->binSpacing[i]=geo->timing_middle[j+1]-coord->binCenter[i];
	else if(coord->val[i]<coord->binCenter[i] && j>0)
	  coord->binSpacing[i]=coord->binCenter[i]-geo->timing_middle[j-1];
	else
	  coord->binSpacing[i]=0;
      }
      break;
    default:
      break;
    }
    /* Check that we actually found requested coordinate */
    if(coord->bin[i]<0 || coord->bin[i]>=coord->n[i]) return false;
  }

  return true;
}

 
bool I3PhotonicsL1Reader::interpolate(const Read_type *coord,Interp_type *interp){
  int i,j,type;  /* Counter variables */
  int dir;       /* Adjecent bin selection variable */
  double a;      /* Used to calculate weights */
  double high,low;  /* Weight used for bit 'high' and bit 'low'
		       high==adjecent bin, low==center bin */
  /*double norm=0; /\* Sum for renormalizing *\/ */
  unsigned short not_mask;


  not_mask= ~interp->mask; /* Inverted mask for quick checks */

  /* First we need to zero the the return structure */
  for(i=0;i<INTERP_BINS;++i) interp->f[i]=(not_mask&i)==0?1.:0.;
  for(i=0;i<VARS;++i) interp->bin[i][0]=interp->bin[i][1]=coord->bin[i];

  /* Loop over possible interpolations and execute them */
  for(i=0;i<VARS;++i){
    type = 1 << i;
    if(!(interp->mask&type)) continue;
    if(coord->binSpacing[i]){ // We can have interpolation
      a=coord->val[i]-coord->binCenter[i];
      dir = a>0?1:-1;
      high=fabs(a)/coord->binSpacing[i];
      low=1-high;
    }else{  // Interpolation not necessary
      dir = 0;
      high = 0;
      low = 1;
    }
    /* Store adjecent bin */
    interp->bin[i][1]=interp->bin[i][0]+dir;

    /* Everything went well, so store the weights we found */
    for(j=0;j<INTERP_BINS;++j){
      if(j&type)      /* For high bit, store adjecent bin weight */
	  interp->f[j]*=high;
	else            /* For low bit, store center bin weight */
	  interp->f[j]*=low;
    }
  }

  return true;

}
/*******************************************************************/
/*  memalloc.c : Photonics library routines for memory management  */
/*******************************************************************/

/* Allocate memory for our main data array                         */
bool I3PhotonicsL1Reader::data_alloc(Data_type *data,const Header_type *h){
  int i;
  
  data->offset[VARS-1]=1;
  for(i=VARS-1;i>0;--i) data->offset[i-1]=data->offset[i]*h->n[i];

  if((data->cont=(float *)calloc(h->n[0]*data->offset[0],sizeof(float)))==NULL)
    return false;
  if(h->record_errors)
    if((data->err=(float *)calloc(h->n[0]*data->offset[0],sizeof(float)))==NULL)
      return false;
  
  return true;
}

/* Determine the size of memory that WAS allocated by identical
 * call to data_alloc */
unsigned long I3PhotonicsL1Reader::data_allocated(Data_type *data,const Header_type *h){
  unsigned long tot=0;
  
  tot= (h->n[0]*data->offset[0]) * sizeof(float);

  if(h->record_errors)
  tot*=2;
  
  return tot;
}

/* Free memory from our main data array                            */
void I3PhotonicsL1Reader::data_free(Data_type *data){
  if (data->mmaped > 0) {
    munmap(data->cont, data->mmaped);
    munmap(data->err, data->mmaped);
  } else {
    free(data->cont);
    if(data->err) free(data->err);
  }
}



