#ifndef DETECTOR_STATUS_UTILS_H
#define DETECTOR_STATUS_UTILS_H

/**
 * Contains utility functions for getting information
 * out of the DetectorStatus frame necessary to configure
 * the modules.
 */
#include <dataclasses/status/I3DetectorStatus.h>
#include <dataclasses/status/I3TriggerStatus.h>
#include <dataclasses/TriggerKey.h>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/name.hpp>
#include <boost/parameter/preprocessor.hpp>
#include <iostream>
#include <sstream>

namespace DetectorStatusUtils{
  // this is going to come in handy for anyone using this utility
  typedef std::pair<TriggerKey,I3TriggerStatus> tk_ts_pair_t;

  BOOST_PARAMETER_NAME(detStat)
  BOOST_PARAMETER_NAME(sourceID)
  BOOST_PARAMETER_NAME(typeID)
  BOOST_PARAMETER_NAME(configID)
  BOOST_PARAMETER_NAME(subtypeID)

  BOOST_PARAMETER_FUNCTION(
			   // return type 
			   (std::vector<TriggerKey>),  
			   // function template name
			   GetBestMatches,
			   // namespace that parameters live in
			   // generally the user doesn't have to worry about this at all
			   tag,                
			   // I3DetectorStatusConstPtr is required
			   (required (detStat,(I3DetectorStatusConstPtr))) 
			   // the first element in the tuple is the parameter name 
			   // and the second (in parentheses) is the type
			   (optional // the following are optional
			    // same as above
			    // first is the parameter
			    // second (is parentheses) is the type
			    // third is the default value
			    (sourceID,(TriggerKey::SourceID),TriggerKey::UNKNOWN_SOURCE)
			    (typeID,(TriggerKey::TypeID),TriggerKey::UNKNOWN_TYPE)
			    (configID,(boost::optional<int>),boost::optional<int>())
			    (subtypeID,(TriggerKey::SubtypeID),TriggerKey::UNKNOWN_SUBTYPE)
			    )
			   )
    {

      // first check that the user gave us something to work with
      if( sourceID == TriggerKey::UNKNOWN_SOURCE && typeID == TriggerKey::UNKNOWN_TYPE
	  && !configID && subtypeID == TriggerKey::UNKNOWN_SUBTYPE ){
	log_error("C'mon.  Too many unknowns.");
	return std::vector<TriggerKey>();
      }
      
      if( ! detStat ){
	log_error("Really?  You passed a NULL pointer?  What kind of return value "
		  "are you expecting?  I'm not a mind reader.");
	return std::vector<TriggerKey>();
      }

      unsigned n_required_matches(0);
      if(sourceID != TriggerKey::UNKNOWN_SOURCE) ++n_required_matches;
      if(typeID != TriggerKey::UNKNOWN_TYPE) ++n_required_matches;
      if(configID ) ++n_required_matches;
      if(subtypeID != TriggerKey::UNKNOWN_SUBTYPE) ++n_required_matches;	   

      std::vector<TriggerKey> key_matches;
      BOOST_FOREACH(const I3TriggerStatusMap::value_type& vtype, detStat->triggerStatus){
        unsigned nmatches(0);
	if(sourceID != TriggerKey::UNKNOWN_SOURCE 
	   && vtype.first.GetSource() == sourceID )
	  ++nmatches;

	if(typeID != TriggerKey::UNKNOWN_TYPE 
	   && vtype.first.GetType() == typeID )
	  ++nmatches;
	
	if(configID && vtype.first.CheckConfigID()	   
	   && vtype.first.GetConfigID() == configID )
	  ++nmatches;
	
	if(subtypeID != TriggerKey::UNKNOWN_SUBTYPE
	   && vtype.first.GetSubtype() == subtypeID )
	  ++nmatches;

        if( nmatches == n_required_matches ){
          key_matches.push_back( vtype.first );
        }
      }
     
      return key_matches;
    }

    // This version of GetTriggerStatus takes the parameters by name
    // ( like python ), where the order doesn't matter.
    // To write this it python-ese it would look something like
    // I3TriggerStatus GetTriggerStatus(detStat, 
    //                                  sourceID = TriggerKey::UNKNOWN_SOURCE
    //                                  typeID = TriggerKey::UNKNOWN_TYPE
    //                                  configID = boost::optional<int>()
    //                                  subtypeID = TriggerKey::UNKNOWN_SUBTYPE)
    // The point of this function is to allow the user to specify only
    // the minimal amount of information required to retrieve a unique I3TriggerStatus
    // from the DetectorStatus.  The whole key is no longer necessary.  It will choose
    // whatever is appropriate.
    //
    // this needs to return the triggerkey as well.
    // long story...
  BOOST_PARAMETER_FUNCTION(
			   // return type 
			   (boost::optional<std::pair<TriggerKey,I3TriggerStatus> >),  
			   // function template name
			   GetTriggerStatus,   
			   // namespace that parameters live in
			   // generally the user doesn't have to worry about this at all
			   tag,                
			   // the first parameter of type I3DetectorStatusConstPtr is required
			   (required (detStat,(I3DetectorStatusConstPtr))) 
			   // the first element in the tuple is the parameter name 
			   // and the second (in parentheses) is the type
			   (optional // the following are optional
			    // same as above
			    // first is the parameter
			    // second (is parentheses) is the type
			    // third is the default value
			    (sourceID,(TriggerKey::SourceID),TriggerKey::UNKNOWN_SOURCE)
			    (typeID,(TriggerKey::TypeID),TriggerKey::UNKNOWN_TYPE)
			    (configID,(boost::optional<int>),boost::optional<int>())
			    (subtypeID,(TriggerKey::SubtypeID),TriggerKey::UNKNOWN_SUBTYPE)
			    )
			   )
    {

      // first check that the user gave us something to work with
      if( sourceID == TriggerKey::UNKNOWN_SOURCE && typeID == TriggerKey::UNKNOWN_TYPE
	  && !configID && subtypeID == TriggerKey::UNKNOWN_SUBTYPE ){
	log_error("C'mon.  You have to give me something to work with.");
	log_error("The defaults aren't ever going to exist in any GCD.");
	return boost::optional<tk_ts_pair_t>();
      }
      
      if( ! detStat ){
	log_error("Really?  You passed a NULL pointer?  What kind of return value "
		  "are you expecting?  I'm not a mind reader.");
	return boost::optional<tk_ts_pair_t>();
      }

      std::vector<TriggerKey> key_matches = 
	GetBestMatches(detStat,
		       _sourceID = sourceID,
		       _typeID = typeID,
		       _configID = configID,
		       _subtypeID = subtypeID);		       
					            
      if(key_matches.size() != 1){
	std::stringstream sstr;

	if(sourceID != TriggerKey::UNKNOWN_SOURCE) 
	  sstr << "  SourceID = "<<sourceID<<std::endl;
	if(typeID != TriggerKey::UNKNOWN_TYPE)
	  sstr << "  TypeID = "<<typeID<<std::endl;
	if(configID)
	  sstr << "  ConfigID = "<<configID.get()<<std::endl;
	if(subtypeID != TriggerKey::UNKNOWN_SUBTYPE)
	  sstr << "  SubtypeID = "<<subtypeID<<std::endl;
	    
	log_error("Here's the user specified input : \n%s",
		  sstr.str().c_str());
	    
	sstr.str(std::string());
        sstr << "key_matches.size() == "<<key_matches.size()<<std::endl;
	typedef std::vector<TriggerKey> tkvector_t;
	BOOST_FOREACH(const TriggerKey& tk, key_matches){
	  sstr<<tk<<std::endl;
	}
	    
	log_error("Here are the keys in the I3TriggerStatus: \n%s",
		  sstr.str().c_str());

	if(key_matches.size() == 0 ) log_error("No match found");
	else log_error("Can't resolve ambiguity.");

	return boost::optional<tk_ts_pair_t>();
      }else{
	TriggerKey tkey(key_matches.front());
	std::stringstream sstr;
	sstr<<tkey;
	log_trace("Found trigger key %s.", sstr.str().c_str());
	tk_ts_pair_t rval(tkey,detStat->triggerStatus.find(tkey)->second);
	return rval;
      }	  
      return boost::optional<tk_ts_pair_t>();
    }

}

#endif
