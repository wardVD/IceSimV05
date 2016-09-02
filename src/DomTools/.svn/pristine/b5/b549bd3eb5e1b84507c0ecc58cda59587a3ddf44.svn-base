/**
 * copyright  (C) 2005
 * the IceCube collaboration
 * Version $Id$
 *
 * @file I3OMSelection.h
 * @version $Revision:$
 * @date $Date$
 */

#ifndef DOMTOOLS_I3OMSELECTION_H_INCLUDED
#define DOMTOOLS_I3OMSELECTION_H_INCLUDED

#include <icetray/I3ConditionalModule.h>
#include <icetray/I3Frame.h>
#include <icetray/OMKey.h>
#include <dataclasses/I3Map.h>
#include <dataclasses/I3Vector.h>
#include <dataclasses/geometry/I3Geometry.h>
#include <dataclasses/physics/I3DOMLaunch.h>
#include <vector>
#include <set>
#include <fstream>
#include <iostream>

template <class Selected>
class I3OMSelection : public I3ConditionalModule
{
 public:
  inline void DefaultStream(I3Frame::Stream &v) { v = I3Frame::Physics; }

  I3OMSelection(const I3Context& context) : I3ConditionalModule(context),
      inputName_("")                               ,
      outputName_("")                              ,
      fileName_("")                                ,
      inputResponse_("input")                      ,
      outputResponse_("output")                    ,
      selectInverse_(false)                        ,
      frame_counter_(0)                            ,
      frame_with_missing_inputResponse_counter_(0)
  {
      AddOutBox("OutBox");

      inputName_ = "";  // optional
      AddParameter("InputOMSelection",
                   "Name of a previously-existing OMSelection that you'd like to append to", 
                   inputName_); 

      outputName_ = "BadOMSelection";  // default
      AddParameter("OutputOMSelection",
                   "Name of the new OMSelection that will be created", 
                   outputName_); 

      fileName_ = "";    //optional
      AddParameter("BadOMFileName",
                   "Name of a file containing bad OMKeys", 
                   fileName_); 

      omittedKeysVect_.clear();
      AddParameter("OmittedKeys",
                   "OMKeys for OMs that we want to clean out", 
                   omittedKeysVect_); 

      omittedStringsVect_.clear();
      AddParameter("OmittedStrings",
                   "Strings for OMs that we want to clean out", 
                   omittedStringsVect_); 

      AddParameter("InputResponse",
                   "Name of the input response ",
                   inputResponse_);

      AddParameter("OutputResponse",
                   "Name of the output response ",
                   outputResponse_);

      AddParameter("SelectInverse",
                   "Selects the Inverse of the specified selection ",
                   selectInverse_);

     I3Frame::Stream stream;
     DefaultStream(stream);
     AddParameter("Stream", "Stream on which to run", stream);
  } // end of templated constructor for I3OMSelection object

  void Configure()
  {
      GetParameter("InputOMSelection",
                    inputName_); 

      GetParameter("OutputOMSelection",
                    outputName_); 

      GetParameter("BadOMFileName",
                    fileName_); 

      GetParameter("OmittedKeys",
                   omittedKeysVect_);

      GetParameter("OmittedStrings",
                    omittedStringsVect_); 
      
      GetParameter("InputResponse",
                   inputResponse_);

      GetParameter("OutputResponse",
                   outputResponse_);

      GetParameter("SelectInverse",
                   selectInverse_);

      I3Frame::Stream stream;
      GetParameter("Stream", stream);
      Register(stream, &I3OMSelection<Selected>::HitCleaning);

  } // end of template member function ::Configure()

  // This function was largely "re-stolen" from the old I3BadOMFileReader code.
  // It handles the reading of a bad-om file (it the user specifies one).
  // This needs to only be done once, so it's in geometry.
  void Geometry(I3FramePtr frame)
  {
      // Geometry will be necessary to translate an entire string
      // into a list of OMKeys
      const I3Geometry &geo = frame->template Get<I3Geometry>("I3Geometry");
      I3OMGeoMap geo_inice = geo.omgeo;
      
      // --------  FIRST, DEAL WITH THE INPUT FILE (IF ONE EXISTS) -----------
      omittedKeysByFile_.clear();
      
      if (fileName_ != "") {
        // Open the file, if it exists, and read in all the Bad OM's
	std::ifstream BadOMFile;
        BadOMFile.open(fileName_.c_str(), std::ifstream::in);
        if(BadOMFile.fail()) log_fatal("The file '%s' was not opened successfully",
                                       fileName_.c_str());
        
        std::string sbuffer;
        int str, om;
        std::string tag;
        while(getline(BadOMFile,sbuffer))
          {
	    std::istringstream buffer(sbuffer);
            
            // Read a (string,OM) pair
            buffer>>str>>om;
            if(!buffer.fail())
              {
                OMKey key(str,om);
                omittedKeysByFile_.insert(key);
                log_trace("From file: will ignore str %d om %d", str, om);
                continue;
              }
                        
            // Read an entire string
            buffer.clear();
            buffer.seekg(0, std::ios::beg);
            buffer>>tag>>str;
            if(!buffer.fail())
              {
                if(tag=="String")
                  {
                    log_trace("From file: will ignore all of str %d",str);
                    // Convert the string to a big list of OMKeys
                    I3OMGeoMap::iterator iter_geo;
                    for(iter_geo=geo_inice.begin(); iter_geo!=geo_inice.end(); iter_geo++)
                      {
                        if(iter_geo->first.GetString()==str) {
                          omittedKeysByFile_.insert(iter_geo->first);
                          log_trace("I am adding (from a string) a Bad OM: str %d om %d",
                                    iter_geo->first.GetString(), iter_geo->first.GetOM());
                        }
                      }
                    continue;
                  }
              }
            
            // End-of-line cleanup
            buffer.clear();
            buffer.ignore(255,'\n');
          }
        
      } // end if filename exists
      
      // ----- SECOND, DEAL WITH ADDITIONAL BAD OM's / STRINGS FROM USER PARAMETERS -----
      
      // copy these into a set since the set has the wonderful
      // 'find' method.
      // Convert the vector to a set:
      omittedKeysByUser_ = std::set<OMKey>(omittedKeysVect_.begin(),omittedKeysVect_.end());

      // Get the bad strings from the user
      for(std::vector<int>::iterator iter = omittedStringsVect_.begin() ; 
          iter != omittedStringsVect_.end() ; 
          iter++)
        {
          // Convert the string to a big list of OMKeys
          I3OMGeoMap::iterator iter_geo;
          for(iter_geo=geo_inice.begin(); iter_geo!=geo_inice.end(); iter_geo++)
            {
              if(iter_geo->first.GetString()==*iter) {
                omittedKeysByUser_.insert(iter_geo->first);
                log_trace("I am adding (from a string) a Bad OM: str %d om %d",
                          iter_geo->first.GetString(), iter_geo->first.GetOM());
              }
            }
          //omittedStringsByUser_.insert(*iter);
        }
      PushFrame(frame,"OutBox");
      
  } // end of template member function ::Geometry()

  void HitCleaning(I3FramePtr frame)
  {
    if (!frame->Has(inputResponse_)) {
      PushFrame(frame);
      return;
    }

    //If user wants to omit strings but didn't give a GCD file:
    //Deal with the requirement on L.123
    if(omittedStringsVect_.size() > 0){
      if(!frame->Has("I3Geometry")){
	log_fatal("Geometry needed to translate an omitted string to a list of OMKeys");
	PushFrame(frame);
	return;
      }
    }

    frame_counter_++;
    // hacksies to get this to work on gcc 3.2
    if ( ! frame->Has(inputResponse_) ) { // if inputResponse is not in the frame, put in empty output objects
      if ( frame_with_missing_inputResponse_counter_ == 0 ) {
	log_warn("1ST TIME - Frame %d does not have inputResponse_ (expected object name \"%s\") ... putting in empty output objects",
                      frame_counter_,
                      inputResponse_.c_str() );
	 } else {
            log_debug("%d TIME - Frame %d does not have inputResponse_ (expected object name \"%s\") ... putting in empty output objects",
                      frame_with_missing_inputResponse_counter_,
                      frame_counter_,
                      inputResponse_.c_str() );
	 }
         frame_with_missing_inputResponse_counter_++;
         boost::shared_ptr< I3Map<OMKey,Selected> > output( new I3Map<OMKey,Selected>() );
         boost::shared_ptr< I3Vector<OMKey> > cleanedKeys( new I3Vector<OMKey> );
         frame->Put( outputResponse_ , output );
         frame->Put( std::string(outputResponse_+"CleanedKeys") , cleanedKeys );
         if ( outputName_ == "" ) log_fatal("You must specify a name for the output Bad-OM list");
         I3VectorOMKeyPtr PutADummyInTheFrame( new I3VectorOMKey() );
         frame->Put( outputName_ , PutADummyInTheFrame );
      }
      else {

      // hacksies to get this to work on gcc 3.2
         const I3Map<OMKey,Selected>& input = frame->template Get< I3Map<OMKey,Selected> >(inputResponse_);

      // Copy the old map into a new one
         boost::shared_ptr< I3Map<OMKey,Selected> > output( new I3Map<OMKey,Selected>(input) );

      // Make an empty vector for storing the list of removed OMs
         boost::shared_ptr< I3Vector<OMKey> > cleanedKeys( new I3Vector<OMKey> );

      // Copy the previously-existing list, if it exists, and turn it into a set
         omittedKeysPrevious_.clear();
         if ( inputName_ != "" ) {
            if ( frame->Has(inputName_) ) {
               const I3VectorOMKey oldlist = frame->template Get<I3VectorOMKey>(inputName_);
      // - Convert the vector into a set:
               omittedKeysPrevious_ = std::set<OMKey>(oldlist.begin(),oldlist.end());
            } else {
               log_fatal("You've asked for a previous OM list %s which doesn't exist.",
                         inputName_.c_str());
            }
         } else {
            log_debug("You have not asked for a previous OM list.");
         }
          

      // Construct the new "master" bad-OM list, containing bad OM's 
      // 1) from the previous list,
      // 2) from the bad-OM file, and
      // 3) from the user parameters.
      // What we want is the UNION of these three sets ... which is then stored in omittedAll_

         std::set<OMKey> temp12;
         std::set_union( omittedKeysByFile_.begin(), omittedKeysByFile_.end(),
			 omittedKeysByUser_.begin(), omittedKeysByUser_.end(),
			 std::insert_iterator< std::set<OMKey> >(temp12,temp12.begin()) );
	 std::set_union( temp12.begin(), temp12.end(),
			 omittedKeysPrevious_.begin(), omittedKeysPrevious_.end(),
			 std::insert_iterator< std::set<OMKey> >(omittedAll_,omittedAll_.begin()) );

      //---------- PERFORM THE HIT CLEANING -----------------
      // loop over the original, removing all the bad keys and
      // bad strings
         typename I3Map<OMKey,Selected>::const_iterator iter;
         for( iter  = input.begin() ; 
              iter != input.end()   ; 
              iter++ ) 
         {
            log_trace("checking OM (%d,%d)",
                      iter->first.GetString(),
                      iter->first.GetOM());
            bool clean = false;
            if ( omittedAll_.find(iter->first) != omittedAll_.end() )
            {
               log_trace("scheduling response on om (%d,%d) for cleaning because "
                         "it was in the omitted keys list",
                         iter->first.GetString(),
                         iter->first.GetOM());
               clean = true;
            }
          // These bad strings have already been converted into OMKeys and
          // added to the OMKey list, so this bit has been commented out.
          //if(omittedStringsByUser_.find(iter->first.GetString()) != 
          //  omittedStringsByUser_.end())
          // {
          //   log_trace("scheduling response on om om (%d,%d) for cleaning "
          //             "because it was in the omitted strings list",
          //                iter->first.GetString(),
          //                iter->first.GetOM());
          //   clean = true;
          // }
            if ( selectInverse_ ) 
            {
               clean = !clean; // take inverse of selection if parameter "selectInverse_" is true
               log_trace("take the inverse of the selection as selectInverse_ is true");
            }
            if ( clean )
            {
               output->erase(iter->first);
               log_trace("cleaning out response on om (%d,%d)",
                         iter->first.GetString(),
                         iter->first.GetOM());
               cleanedKeys->push_back(iter->first);
            }
            else
            {
               log_trace("response on om (%d,%d) is not cleaned",
                         iter->first.GetString(),
                         iter->first.GetOM());
            }
          
         } // end of FOR-loop over the OMs wherein the "bad" OMs are removed from the output series

         log_debug("Original number of OMs hit: %zd",input.size());
         log_debug("Cleaned number of OMs hit: %zd",output->size());
         frame->Put(outputResponse_,output);
         frame->Put(std::string(outputResponse_ + "CleanedKeys"),cleanedKeys);

      // Create the I3Vector of the *complete* bad-om list and also
      // put this in the frame.

         if (outputName_ == "") log_fatal("You must specify a name for the output Bad-OM list");

      // the set->vector shortcut should work now
         I3VectorOMKeyPtr putMeInTheFrame( new I3VectorOMKey(omittedAll_.begin(),omittedAll_.end()) );
         frame->Put( outputName_ , putMeInTheFrame );

      } // end of IF-block for this event has the desired InputResponse in it

      PushFrame(frame,"OutBox");

      return;

  } // end of templated member function ::Physics()

 private:

  std::string inputName_;     // previously-existing bad-OM list (optional)
  std::string outputName_;    // name of the new bad-OM list (required)
  std::string fileName_;      // name of a bad-OM file (optional)

  std::vector<OMKey> omittedKeysVect_;
  std::vector<int> omittedStringsVect_;

  std::set<OMKey> omittedKeysByFile_; 
  std::set<OMKey> omittedKeysByUser_;       
  std::set<OMKey> omittedKeysPrevious_;

  std::set<OMKey> omittedAll_;      // Union set of all omitted OMKeys (file or user or previous)
  //I3VectorOMKey putMeInTheFrame_;   //  I3Vector version of the same thing which will go in the frame

  std::string inputResponse_;                            // PYTHON PARAMETER: name of the input launch/hit/pulse/etc. map
  std::string outputResponse_;                           // PYTHON PARAMETER: name of the output launch/hit/pulse/etc. map
  bool   selectInverse_;                            // PYTHON PARAMETER: set to False to get Pretz-logic behavior / set to True to get normal behavior
  int    frame_counter_;                            // counter to keep track of how many frames pass through the module (useful for debugging output)
  int    frame_with_missing_inputResponse_counter_; // counter of the frames which lacked the specified inputResponse_ (useful for debugging output)

  //  void LookUpOMKeysForEntireString(int str, I3OMGeoMap geo_inice) {
  //  vector<OMKey> result;
  // I3OMGeoMap::iterator iter_geo;
  // for(iter_geo=geo_inice.begin(); iter_geo!=geo_inice.end(); iter_geo++)
  //   {
  //        if(iter_geo->first.GetString()==str) {
  //          omittedKeysByFile_.push_back(iter_geo->first);
  //          log_debug("I am adding (from a string) a Bad OM: str %d om %d",
  //                    iter_geo->first.GetString(), iter_geo->first.GetOM());
  //        }
  //   }
  //}

  SET_LOGGER("DomTools");

};

template <>
inline void I3OMSelection<I3DOMLaunchSeries>::DefaultStream(I3Frame::Stream &v) { v = I3Frame::DAQ; }
#endif
