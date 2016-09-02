#include <I3Test.h>

#include <boost/optional.hpp>
#include <trigger-sim/utilities/DetectorStatusUtils.h>
#include <dataclasses/status/I3TriggerStatus.h>

using DetectorStatusUtils::tk_ts_pair_t;

TEST_GROUP(DetectorStatusUtilsTests);

TEST(GetTriggerStatusUsage)
{
  I3DetectorStatusConstPtr ds;
  // for these test it doesn't matter that
  // we're passing a NULL pointer
  // we just want to make the compiler happy
  {
    boost::optional<tk_ts_pair_t> ts = DetectorStatusUtils::GetTriggerStatus(ds);
    ENSURE( !ts );
  }

  {
    TriggerKey::TypeID typeID = TriggerKey::SIMPLE_MULTIPLICITY;
    boost::optional<tk_ts_pair_t> ts = DetectorStatusUtils::GetTriggerStatus
      (ds,DetectorStatusUtils::_typeID = typeID);
    ENSURE( !ts );
  }

  {
    TriggerKey::SourceID sourceID = TriggerKey::IN_ICE;
    TriggerKey::TypeID typeID = TriggerKey::SIMPLE_MULTIPLICITY;
    TriggerKey::SubtypeID subtypeID = TriggerKey::LASER;
    int configID(42);
    boost::optional<tk_ts_pair_t> ts = DetectorStatusUtils::GetTriggerStatus
      (ds,DetectorStatusUtils::_subtypeID = subtypeID,
       DetectorStatusUtils::_typeID = typeID,
       DetectorStatusUtils::_sourceID = sourceID ,
       DetectorStatusUtils::_configID = configID );
    ENSURE( !ts );
  }

  {
    // if you don't want to use optional parameters
    // the old-style works just fine.  gotta give it
    // to those boost guys sometimes.
    TriggerKey::SourceID sourceID = TriggerKey::IN_ICE;
    TriggerKey::TypeID typeID = TriggerKey::SIMPLE_MULTIPLICITY;
    TriggerKey::SubtypeID subtypeID = TriggerKey::LASER;
    int configID(42);
    boost::optional<tk_ts_pair_t> ts = DetectorStatusUtils::GetTriggerStatus
      (ds,sourceID,typeID,configID,subtypeID);
    ENSURE( !ts );

    ts = DetectorStatusUtils::GetTriggerStatus
      (ds,sourceID,typeID,configID);
    ENSURE( !ts );
    
    ts = DetectorStatusUtils::GetTriggerStatus
      (ds,sourceID,typeID);
    ENSURE( !ts );

     ts = DetectorStatusUtils::GetTriggerStatus
      (ds,sourceID);
    ENSURE( !ts );
  }
}

I3DetectorStatusPtr GenerateDetectorStatus(){
  I3DetectorStatusPtr ds(new I3DetectorStatus);
  
  std::vector<TriggerKey> keys;
  keys.push_back(TriggerKey());
  keys.push_back(TriggerKey(TriggerKey::ICE_TOP,TriggerKey::SIMPLE_MULTIPLICITY));
  keys.push_back(TriggerKey(TriggerKey::ICE_TOP,TriggerKey::MIN_BIAS));
  keys.push_back(TriggerKey(TriggerKey::ICE_TOP,TriggerKey::CALIBRATION));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::MIN_BIAS));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::CALIBRATION));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::SLOW_PARTICLE));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::VOLUME));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::STRING));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::SIMPLE_MULTIPLICITY,999));
  keys.push_back(TriggerKey(TriggerKey::IN_ICE,TriggerKey::SIMPLE_MULTIPLICITY,666));
  keys.push_back(TriggerKey(TriggerKey::SPASE,TriggerKey::SPASE_2,777));

  typedef std::vector<TriggerKey> tk_vector_t;
  BOOST_FOREACH(tk_vector_t::value_type& tk,keys)
    ds->triggerStatus[tk] = I3TriggerStatus();
  return ds;
}

TEST(GetTriggerStatusTest)
{
  I3DetectorStatusConstPtr ds = GenerateDetectorStatus();
  
  // should be able to find these with just this information
  typedef std::pair<TriggerKey::SourceID,TriggerKey::TypeID> st_pairs_t;
  std::vector<st_pairs_t> good_pairs;
  good_pairs.push_back(st_pairs_t(TriggerKey::ICE_TOP,TriggerKey::MIN_BIAS));
  good_pairs.push_back(st_pairs_t(TriggerKey::IN_ICE,TriggerKey::CALIBRATION));
  good_pairs.push_back(st_pairs_t(TriggerKey::IN_ICE,TriggerKey::SLOW_PARTICLE));
  good_pairs.push_back(st_pairs_t(TriggerKey::IN_ICE,TriggerKey::VOLUME));
  good_pairs.push_back(st_pairs_t(TriggerKey::IN_ICE,TriggerKey::STRING));
  good_pairs.push_back(st_pairs_t(TriggerKey::ICE_TOP,TriggerKey::MIN_BIAS));
  good_pairs.push_back(st_pairs_t(TriggerKey::ICE_TOP,TriggerKey::CALIBRATION));
  
  boost::optional<tk_ts_pair_t> ts;
  typedef std::vector<st_pairs_t> pair_list_t;
  BOOST_FOREACH(pair_list_t::value_type& st_pair, good_pairs){
    ts = DetectorStatusUtils::GetTriggerStatus
      (ds,DetectorStatusUtils::_sourceID = st_pair.first,
       DetectorStatusUtils::_typeID = st_pair.second);
    ENSURE( (bool)ts );
  }

  ENSURE( !DetectorStatusUtils::GetTriggerStatus(ds) );

  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::SPASE) );

  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_typeID = TriggerKey::SPASE_2) );

  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_configID = 777 ) );

  ENSURE( !DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE) );

  ENSURE( !DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE,
	   DetectorStatusUtils::_typeID = TriggerKey::SIMPLE_MULTIPLICITY) );
						
  ENSURE( !DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::ICE_TOP) );
						
  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE,
	   DetectorStatusUtils::_typeID = TriggerKey::SIMPLE_MULTIPLICITY,
	   DetectorStatusUtils::_configID = 999) );
						
  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
	  (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE,
	   DetectorStatusUtils::_typeID = TriggerKey::SIMPLE_MULTIPLICITY,
	   DetectorStatusUtils::_configID = 666) );

  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
  	  (ds,DetectorStatusUtils::_configID = 999) );

  ENSURE( (bool)DetectorStatusUtils::GetTriggerStatus
  	  (ds,DetectorStatusUtils::_configID = 666) );
	   						
}

TEST(GetBestMatchesTest)
{
  I3DetectorStatusConstPtr ds = GenerateDetectorStatus();

  std::vector<TriggerKey> best_matches;

  // this should generate no matches
  // in the old buggy implementation this would return
  // a list of all InIce triggers.
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE,
     DetectorStatusUtils::_typeID = TriggerKey::TWO_COINCIDENCE);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 0 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 7 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_sourceID = TriggerKey::ICE_TOP);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 3 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_typeID = TriggerKey::MIN_BIAS);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 2 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_sourceID = TriggerKey::IN_ICE,
     DetectorStatusUtils::_typeID = TriggerKey::MIN_BIAS);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 1 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_sourceID = TriggerKey::SPASE,
     DetectorStatusUtils::_typeID = TriggerKey::SPASE_2,
     DetectorStatusUtils::_configID = 777);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 1 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_typeID = TriggerKey::SIMPLE_MULTIPLICITY,
     DetectorStatusUtils::_configID = 666);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 1 );

  best_matches.clear();
  best_matches = DetectorStatusUtils::GetBestMatches
    (ds,DetectorStatusUtils::_typeID = TriggerKey::SIMPLE_MULTIPLICITY);
  std::cerr<<""<<best_matches.size()<<std::endl;
  ENSURE( best_matches.size() == 3 );
}
