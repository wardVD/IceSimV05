/**
 * @file I3PhotonicsReader.cxx
 * @brief This file for code for reading both L1 and L2 photonics tables
 *
 * @author Kevin Meagher
 * @author Johan Lundberg
 * @author P. Miocinovic
 * @author RCB
 * @author KWW
 * @version $Id$
 * @date $Date$
 * 
 * The following is a list of files from photonic which I 
 * copied verbatium to create this file:
 *   \li photonics-1.73/lib/photonics.h
 *   \li photonics-1.73/lib/geometry.c
 *   \li photonics-1.73/lib/io.c
 *   \li photonics-1.73/lib/misc.c
 *
 * Non functioning code was removed, comments cleaned up
 *
 * For documentation of photonics files see
 * http://photonics.sourceforge.net/index.php
 * 
 * @copyright Copyright (c) The IceCube Collaboration
 */

#include <cmath>
#include <cstring>
#include <stdint.h>
#include <sys/mman.h> 
#include "dataclasses/I3Constants.h"
#include "icetray/I3Logging.h"
#include "photonics-service/I3PhotonicsReader.h"

/*******************************************************************/
/* photonics.h: Data type definitions for photonics library.       */
/*******************************************************************/
#define Max(x,y) ((x)>(y)?(x):(y))
#define Min(x,y) ((x)<(y)?(x):(y))
#define FILE_FORMAT_LABEL     "tabl" 
#define FILE_FORMAT_REVISION  0 /**< file format 0 is valid from version 1.4.1 */

namespace photonics{
  /*******************************************************************/
  /*  geometry.c : Photonics library routines for generating geometry*/
  /*******************************************************************/
  double next_boundary_sqr(const double base,const double step){
    double bound,root;
    
    if(base>=0)
      bound=sqr(sqrt(base)+step);
    else{
      root=-sqrt(-base)+step;
      bound=(root<0?-sqr(root):sqr(root));
    }
    return fabs(bound-(int)bound)>EPS?bound:(int)bound;
  }
  
  /*******************************************************************/
  /*  io.c : Photonics library routines for I/O of photon tables     */
  /*******************************************************************/
  
  /* copy_header:  Copies header info                             */
  void copy_header(const Header_type *h,Header_type *h_out){
    
    if(!h_out || !h) {
      log_fatal("photonics lib/io.c (copy_header): Trying to copy to or from uninitialized header.");
    }
    
    /* This should be quicker and more abstract: /J
       since all is copied, and there is no pointers, this should be just fine. */
    
    memcpy(h_out,h,sizeof(Header_type));
    
    return;
  }
  
  /* true if the examined meta head is little endian */
  bool checkMetaHeadLittle(MetaHead_type* MetaHead)
  {
    return (MetaHead->endian==1);
  }
  
  /**
   * @brief true if the examined meta head is written on 32 bit machine 
   */
  bool checkMetaHead32bit(MetaHead_type* MetaHead)
  {
    return (MetaHead->bitsys==0u);
  }
  
  bool checkMetaHead_back(MetaHead_type* MetaHead,int verbose){
    /* if we don't understand the meta header label. This may
       mean obsolete photonics table format or just a random file. We'll try to find
       out. This is just magic based on comparing with old tables.
       old level2 is not recognized at all.
       
       this should be big/little endian independent...
    */
    if(0!=strcmp(MetaHead->File_Format_Label , FILE_FORMAT_LABEL)){
      log_error("photonics table has unsupported format.");
      
      int i,j,k;
      float *tmp = (float*)MetaHead;
      

      /* all of this is black magic to give the user good warnings
	 for some old, old table types */

      j=0;
      k=HEADER_TYPE_LENGTH/sizeof(float);

      for(i=0;i<k;i++){
	if((fabs(tmp[i])<2000)&&(fabs(tmp[i])>0.01)){
	  j++;
	}
	if(fabs(tmp[i])>100000)
	  j--;
      }
      
      if((j>5)&&(j<25)){
	j=1;
      } else{
	j=j<-22?-3:0;
      }
      
      if ((tmp[43]==-1)||((tmp[43]>=200)&&(tmp[43]<700))){
	j=j+1;
      }
      if((tmp[16]<tmp[17])&&(tmp[18]<tmp[19])&&(tmp[20]<tmp[21])){
	j=j+1;
      }
      
      k=3;
      j=Min(j,k);
      j=Max(j,0);
      if(j>0){
	log_error("This looks like an old [unversioned] table. Try photonics v_1_40b_19082005");
      }
      
      return false;

    } else if (verbose) {
      log_info("Valid table meta head");
    }

    if(MetaHead->File_Format_Revision!=FILE_FORMAT_REVISION){
      if(MetaHead->File_Format_Revision>FILE_FORMAT_REVISION){
	log_error("Table has newer format than you copy of photonics.");
      } else {
	log_error("Table has older format than you copy of photonics.");       
      }
    }
    log_debug("Table created with Version %s",MetaHead->Photonics_Version);
    log_debug(" ... file format rev: %d, %s bit, %s endian.",MetaHead->File_Format_Revision,
	      checkMetaHead32bit(MetaHead)?"32":"64",
	      checkMetaHeadLittle(MetaHead)?"little":"big");
    log_debug("table level: %d",MetaHead->level);
    
    return true;
  }
  

  bool checkMetaHead(MetaHead_type* MetaHead){
    return checkMetaHead_back( MetaHead,0);
  }
  
  /* convert a residual time from one group index of refraction to another */
  /* returns residual time, as if it would have been calculated            */
  /* with group refractive ind_new.                                        */
  double residual_convert(double ind_old, /* refractive index for input timedelay */
			  double ind_new, /* refractive index to convert timedelay into */
			  double time_old, /* residual time in ind_old sens */
			  double rho,      /* perpendicular distance track origin--OM */
			  double l,        /* longitudinal distance track origin--OM */
			  double rev_sin_ang, /* 1/(sin(cherenkov_angle)) */
			  time_ref_type REFTYPE) /* type of residual calculation */
  {
    static const double invspeed=1.0/I3Constants::c;
    double time_out;
    
    if(ind_old==ind_new){
      return time_old;
    }

    // direct hit, for muon type time reference. we only use rho here.
    if (REFTYPE == DIRECT){
      time_out = time_old + invspeed * rho*rev_sin_ang*(ind_old - ind_new );
    } else {
      // straight line from origin
      if (REFTYPE == ORIGIN) {
	time_out = time_old + invspeed * sqrt(rho*rho+l*l)*(ind_old - ind_new );
      }
      else {
	if (REFTYPE == POCA) {
	  // point of closest aproach.
	  log_fatal("Photonics io.c(residual_convert): 'POCA' Refractive index conversion is not tested. verify it and remove this error.");
	} else {
	  log_fatal("Photonics io.c(residual_convert): internal error, ref: %d ",(int)REFTYPE);
	}
      }
    }
    
    return time_out;
  }
  
  /*******************************************************************/
  /*  misc.c : Photonics library routines that don't fit anywhere    */
  /*           else                                                  */
  /*******************************************************************/
  
  /* binary:   Binary search of an integrated probability         */
  /*           distribution.                                      */
  int binary(const float *item,int count,double key){
    int low,high,mid;
    
    if(key<item[0]) return -1;           /* Account for first bin assymetry */
    if(key>item[count-1]) return count;  /* Account for last bin assymetry */
    if(std::isnan(item[count-1])) {
      log_warn("warning 'nan' in 'binary'");
      return (count); /* this should not happen */
    }
    low=0;
    high=count-1;
    while(low<=high){
      mid=(low+high) >> 1;
      if(key<item[mid] && key<=item[mid+1]) high=mid-1;
      else if(key>item[mid] && key>=item[mid+1]) low=mid+1;
      else return mid;
    }
    log_error("Binary search problem: n=%d",count);
    log_error("key=%e low=%d high=%d",key,low,high);
    return -1;
  }
  
  /* find:  Modified hunt routine from NR, for seeded binary search*/
  /* IMPORTANT: If array is sorted from low to high, bins are defined
     as [low_edge,high_edge). If sorted from high to low, then bins
     are (low_edge,high_edge]. E.g.
     array={-1,0,1}  find with key=0 returns 1, i.e. 0 is in [0,1)
     array={1,0,-1}  find with key=0 returns 0, i.e. 0 is in (-1,0]
     
     note: n is the highest allowed index. that is, (nbins-1)
     this function does not work when n=0;
  */
  int find(const double *array,int n,double key,int seed){
    int jlo,jm,jhi,inc,dir;
    
    if(n<=0){
      log_fatal("photonics misc.c: can't search vector of length %d",n);
    }
  
    jlo = seed;
    dir=(array[n]>array[0]);
    
    /* Initial bound check */
    if(dir){
      if(key > array[n]) return n;
      if(key < array[0]) return -1;
    }else{
      if(key < array[n]) return n;
      if(key > array[0]) return -1;
    }
    
    if (jlo < 0 || jlo >= n) { /* Input guess not useful. */
      jlo=0;
      jhi=n;
    }else{
      inc=1;   /* Set the hunting increment. */
      if((key >= array[jlo]) == dir) { /* Hunt up */
	if(jlo == n-1){
	  if((key > array[n]) == dir) return n;
	  if(key == array[n]) return jlo;
	}
	jhi=jlo+1;
	while((key >= array[jhi]) == dir){
	  /* Not done hunting, so double the increment */
	  jlo=jhi;
	  inc <<= 1;
	  jhi=jlo+inc;
	  if(jhi >= n){  /* Done hunting, since off end of table. */
	    jhi=n;
	    break;
	  }
	}       /* Done hunting, value bracketed. */
      }else{ /* Hunt down */
	if(jlo == 0) return -1; /* Since (key < array[0]) is true */
	jhi=jlo--;
	while((key < array[jlo]) == dir){
	  /* Not done hunting, so double the increment */
	  jhi=jlo;
	  inc <<= 1;
	  if(inc >= jhi){   /* Done hunting, since off end of table. */
	    jlo=0;
	    break;
	  }else jlo=jhi-inc;
	}
      }   /* Done hunting, value bracketed. */
    }     /* Begin the final bisection phase */
    
    /* optimized: */
    if(dir){
      while (jhi-jlo != 1) {
	jm=(jhi+jlo) >> 1;
	if((key < array[jm]))
	  jhi=jm;
	else
	  jlo=jm;
      }
    } else {
      while (jhi-jlo != 1) {
	jm=(jhi+jlo) >> 1;
	if((key < array[jm]))
	  jlo=jm;
	else
	  jhi=jm;
      }
    }
    
    return jlo;
  }
  
  /* Functions for determination of supported environment */
  bool isLittleEndian() {
    
    union {
      int i;
      unsigned char s[sizeof(int)] ;
    } x ; // i&s share memory
    
    x.i = 0;         // zero all of x
    x.s[0] = 1;      // set first byte in structure to 1
    return x.i == 1; // check if that means the integer is 1
  }

  int memlength() { 
    return sizeof(void*);
  }

  int ph_long_length() {
    return sizeof(ph_long);
  }

  bool is32bitMemory() {
    return ( 4 == memlength() );
  }

  bool is64bitMemory() {
    return ( 8 == memlength() );
  }

  bool isSupportedEnvironment() {
    bool isok = true;
    if (!isLittleEndian()) {
      log_warn("WARNING! Little endian system NOT detected Big endian is experimental. Good luck!");
      
      if(!is32bitMemory()){
	isok=false;
	log_warn("non-32bit and Big endian is NOT supported at all.");
      }
    } else {
      log_debug("Little endian system detected");
    }
    
    if (is32bitMemory()){
      log_debug("Detected 32 bit memory");
    } else if (is64bitMemory()){
      log_debug("Detected 64 bit memory");
    } else {
      log_fatal("Detected unknown architecture: Unknown memory length: %d",memlength());
    }
    
    if(ph_long_length()!=8) {
      log_fatal("Length of 'ph_long' is not 8, its: %d : Fatal error",ph_long_length());
    }
    
    if(sizeof(Header_type) != HEADER_TYPE_LENGTH ){
      log_fatal("Length of 'header_type' of wrong size");
    }
    
    return isok;
  }
  
  // 32 bit byte swapping. bytes ABCD -> DCBA. Operates directly on p_swapit_
  void px_swap32 (void * p_swapit_)
  {
    
    uint32_t s = *(uint32_t *)p_swapit_;
    
    s = ((s & 0x000000FF) << 24) +
        ((s & 0x0000FF00) << 8 ) +
        ((s & 0x00FF0000) >> 8 ) +
        ((s & 0xFF000000) >> 24);
    
    *((uint32_t *)p_swapit_) = s;

  }
  
  // 64 bit byte swapping. bytes ABCD EFGH -> DCBA HGFE. Operates directly on p_swapit_
  void px_swap64 (void * p_swapit_)
  {
    
    long long s = *(long long *)p_swapit_;
    
    s = ((s & 0x00000000000000FFll) << 24) +
        ((s & 0x000000000000FF00ll) << 8) +
        ((s & 0x0000000000FF0000ll) >> 8) +
        ((s & 0x00000000FF000000ll) >> 24 ) +
        ((s & 0x000000FF00000000ll) << 24 ) +
        ((s & 0x0000FF0000000000ll) << 8) +
        ((s & 0x00FF000000000000ll) >> 8) +
        ((s & 0xFF00000000000000ll) >> 24);

    *((long long *)p_swapit_) = s;

  }
    
}  //namespace photonics
