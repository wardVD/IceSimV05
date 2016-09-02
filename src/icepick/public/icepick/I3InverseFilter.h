#ifndef I3INVERSEFILTER_H_INCLUDED
#define I3INVERSEFILTER_H_INCLUDED

#include <string>

#include <icetray/I3Configuration.h>
#include <icetray/I3Context.h>
#include <icetray/I3IcePick.h>
#include <icetray/I3Logging.h>
#include <icetray/I3Bool.h>

/**
 * @brief An I3IcePick which takes a previouslly run IcePick
 * decision and filters with the "reverse" logic
 */
class I3InverseFilter : public I3IcePick
{
 public:
  I3InverseFilter(const I3Context& context) : I3IcePick(context),
    inputDecision_(""), defaultInputDecision_(true)
    {
      AddParameter("InputDecisionName","The name of the decision "
                   "in the frame for which you want the inverse.", inputDecision_);
      AddParameter("DefaultInputDecision","The boolean value that "
                   "is used if \"InputDecisionName\" does not exist in frame",
                   defaultInputDecision_);
    }

  void Configure()
    {
      GetParameter("InputDecisionName", inputDecision_);
      log_info( "(%s) Will invert the filter decision from \"%s\"",
                GetName().c_str(), inputDecision_.c_str() );
      GetParameter("DefaultInputDecision", defaultInputDecision_);
    }

  bool SelectFrame(I3Frame& frame) {

    if( frame.Has(inputDecision_) )
      {
        bool inputbool = frame.Get<I3Bool>(inputDecision_).value;

        if ( !inputbool )
          {
            log_debug("(%s) %s rejects event, so inverse decision keeps it.",
                      GetName().c_str(), inputDecision_.c_str() );
            return true;
          } else {
            log_debug("(%s) %s keeps event, so inverse decision rejects it.",
                      GetName().c_str(), inputDecision_.c_str() );
            return false;
          }

      }

    if( defaultInputDecision_ )
      {
        log_debug("(%s) Input decision missing in the frame. Rejecting event.",
                  GetName().c_str());
      } else {
        log_debug("(%s) Input decision missing in the frame. Selecting event.",
                  GetName().c_str());
      }
    return !defaultInputDecision_;
  }

 private:
  std::string inputDecision_;
  bool defaultInputDecision_;

  SET_LOGGER("I3InverseFilter");
};

#endif
