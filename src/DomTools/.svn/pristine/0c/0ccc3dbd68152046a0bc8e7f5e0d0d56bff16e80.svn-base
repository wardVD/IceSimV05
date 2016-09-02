/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file I3ResponseIter.h
 * @version $Revision:$
 * @date $Date$
 */

#ifndef DOMTOOLS_I3RESPONSEITER_H_INCLUDED
#define DOMTOOLS_I3RESPONSEITER_H_INCLUDED

#include "DomTools/Utility.h"
#include "icetray/OMKey.h"
#include "dataclasses/I3Map.h"
#include "icetray/I3Logging.h"
#include <string>
#include <map>
#include <vector>

template <class responseClass>
class I3ResponseIter
{
  typename I3Map<OMKey, std::vector<responseClass> >::const_iterator   response_series_map_iter_;
  typename I3Map<OMKey, std::vector<responseClass> >::const_iterator   response_series_map_end_;
  typename std::vector<responseClass>::const_iterator                  response_series_iter_;
  bool                                                            valid_response_;
  int                                                             sequence_number_;

  I3ResponseIter();

  void Check(){
    if(!valid_response_) 
      log_fatal("Tried to read response beyond the end of the iterator");
  }

  public:

  I3ResponseIter(const I3Map<OMKey, std::vector<responseClass> > &rsm)
  {
    response_series_map_iter_=rsm.begin();
    response_series_map_end_ =rsm.end();

    valid_response_=false;
    sequence_number_=0;
    for(; response_series_map_iter_!=response_series_map_end_; response_series_map_iter_++)
    {
      for(response_series_iter_=response_series_map_iter_->second.begin();
          response_series_iter_!=response_series_map_iter_->second.end();
          response_series_iter_++)
      {
        valid_response_=true;
        break;
      }
      if(valid_response_) break;
    }
   
    if(!valid_response_) log_info("found empty response_series_map");
  }

  void operator++(int)
  {
    ++(*this);
  }
 
  void operator++()
  {
    Check();

    valid_response_=false;
    bool need_to_restart_series=false;
    for(; response_series_map_iter_!=response_series_map_end_; response_series_map_iter_++)
    {
      if(need_to_restart_series)
      {
        response_series_iter_=response_series_map_iter_->second.begin();
        sequence_number_=0;
      }
      else 
      {
        response_series_iter_++;
        sequence_number_++;
      }
      if(response_series_iter_!=response_series_map_iter_->second.end()) 
      {
        valid_response_=true;
        break;
      }
      need_to_restart_series=true;
    }
  }

  bool End() {return(valid_response_?false:true);}


  responseClass Get()      {Check(); return(*response_series_iter_);}
  int GetSequencenumber()   {Check(); return(sequence_number_);}

  const OMKey &GetOMKey()  {Check(); return(response_series_map_iter_->first);}
  double GetTime()         {Check(); return(domtools::GetTime(*response_series_iter_));}
  double GetAmplitude()    {Check(); return(domtools::GetAmplitude(*response_series_iter_));}
  double GetWidth()        {Check(); return(domtools::GetWidth(*response_series_iter_));}

  SET_LOGGER("I3ResponseIter");
};

#endif
