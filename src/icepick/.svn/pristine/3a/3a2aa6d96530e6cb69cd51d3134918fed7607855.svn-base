/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3FrameObjectFilter.cxx
 * @version $Revision$
 * @date $Date$
 * @author kjmeagher
 */
 
// class header file

#include <icepick/I3FrameObjectFilter.h>

#include <boost/algorithm/string.hpp>

#include <icetray/I3Context.h>
#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3EventHeader.h>

// namespace declarations


// implementation

I3FrameObjectFilter::I3FrameObjectFilter(const I3Context& context) : 
  I3IcePick(context),
  frame_object_key_(""),
  frame_object_key_starts_with_(""),
  display_warning_(false)
{
  AddParameter("FrameObjectKey",
               "I3FrameObjectFilter will accept the frame if "
               "an object with this name is present or...",
	       frame_object_key_);
  AddParameter("FrameObjectKeyStartsWith",
               "...if an object name starts with this string",
               frame_object_key_starts_with_);
  AddParameter("DisplayWarning",
	       "Should I3FrameObjectFilter display a warning "
               "for each rejected frame",
	       display_warning_);
}

void I3FrameObjectFilter::Configure() 
{
  GetParameter("FrameObjectKey",frame_object_key_);
  GetParameter("FrameObjectKeyStartsWith",frame_object_key_starts_with_);
  GetParameter("DisplayWarning",display_warning_);
}

bool I3FrameObjectFilter::SelectFrame(I3Frame& frame) 
{
  if(!frame_object_key_.empty() && frame.Has(frame_object_key_))
    {
      return true;
    }
  else if(!frame_object_key_starts_with_.empty())
    {
      for(I3Frame::typename_iterator iter = frame.typename_begin();
          iter!=frame.typename_end();
          ++iter)
        {
          if(boost::starts_with(iter->first, frame_object_key_starts_with_))
            {
              return true;
            }
        }
    }
    
  if (display_warning_)
    {
      if(frame.Has("I3EventHeader"))
        {
          if(!frame_object_key_.empty() &&
             !frame_object_key_starts_with_.empty())
            {
              log_warn("Event %u is missing "
                       "\"%s\" and \"%s...\": rejecting frame",
                       frame.Get<I3EventHeader>().GetEventID(),
                       frame_object_key_.c_str(),
                       frame_object_key_starts_with_.c_str());
            }
          else if(!frame_object_key_.empty())
            {
              log_warn("Event %u is missing "
                       "\"%s\": rejecting frame",
                       frame.Get<I3EventHeader>().GetEventID(),
                       frame_object_key_.c_str());
            }
          else if(!frame_object_key_starts_with_.empty())
            {
              log_warn("Event %u is missing "
                       "\"%s...\": rejecting frame",
                       frame.Get<I3EventHeader>().GetEventID(),
                       frame_object_key_starts_with_.c_str());
            }
          else
            {
              log_warn("Event %u: rejecting frame",
                       frame.Get<I3EventHeader>().GetEventID());
            }
        }
      else
        {
          if(!frame_object_key_.empty() &&
             !frame_object_key_starts_with_.empty())
            {
              log_warn("Event with no event header is missing "
                       "\"%s\" and \"%s...\": rejecting frame",
                       frame_object_key_.c_str(),
                       frame_object_key_starts_with_.c_str());
            }
          else if(!frame_object_key_.empty())
            {
              log_warn("Event with no event header is missing "
                       "\"%s\": rejecting frame",
                       frame_object_key_.c_str());
            }
          else if(!frame_object_key_starts_with_.empty())
            {
              log_warn("Event with no event header is missing "
                       "\"%s...\": rejecting frame",
                       frame_object_key_starts_with_.c_str());
            }
          else
            {
              log_warn("Event with no event header: rejecting frame");
            }      
        }
    }

  return false;
}


#include <interfaces/I3IcePickModule.h>
#include <interfaces/I3IceForkModule.h>
#include <icetray/I3IcePickInstaller.h>

I3_MODULE(I3IcePickModule<I3FrameObjectFilter>);
I3_MODULE(I3IceForkModule<I3FrameObjectFilter>);
I3_SERVICE_FACTORY(I3IcePickInstaller<I3FrameObjectFilter>);

