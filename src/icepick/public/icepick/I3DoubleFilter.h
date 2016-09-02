#ifndef I3DOUBLEFILTER_H
#define I3DOUBLEFILTER_H

#include "icetray/I3IcePick.h"
#include "dataclasses/I3Double.h"

/**
 * @brief An I3IcePick which takes a "I3Double" (bool) from the frame and 
 *        accepts the event (by return true) if its is within an user
 *        prescribed range.  To be specific about this process, the code
 *        first checks if the I3Double exists in the frame.  If not, the
 *        icepick will be rejected the event.  Otherwise, the code checks
 *        to see if the value of the I3Double is within the user specified
 *        range and, if not, rejects the event.  By default, the range is
 *        [-1.0e10,+1.0e10], i.e., a large range that should pass most,
 *        if not all, quantities.
 *
 * @todo  change the default values for the range to [-inf,+inf] after 
 *        learning how to use the inf stuff in math.h properly. 
 *
 * @author fox (bfox@phys.psu.edu)
 *
 */

class I3DoubleFilter : public I3IcePick
{

  public:

    I3DoubleFilter(const I3Context& context) : 
         I3IcePick(context)        ,
         input_I3Double_Name_("")  ,
         Minimum_Value_( -1.0e10 ) ,  // my version of -Inf
         Maximum_Value_( +1.0e10 )    // my version of +Inf
    {
    
         AddParameter( "I3DoubleObject"                        ,
                       "The name of the input I3Double object" ,
                       input_I3Double_Name_ );
         AddParameter( "MinimumValue" ,
                       "Minimum value of I3Double for event to be kept [default=-1.0e10]" ,
                       Minimum_Value_ );
         AddParameter( "MaximumValue" ,
                       "Maximum value of I3Double for event to be kept [default=1.0e10]"  ,
                       Maximum_Value_ );
    
         return;
    
    } // end of public constructor function for I3DoubleFilter object
    
    void Configure()
    {
    
         GetParameter( "I3DoubleObject" , input_I3Double_Name_ );
         GetParameter( "MinimumValue"   , Minimum_Value_       );
         GetParameter( "MaximumValue"   , Maximum_Value_       );
    
         log_info(" (%s) -  I3DoubleObject = %s"                       ,
                  GetName().c_str()                                  ,
                  input_I3Double_Name_.c_str()                         );
         log_info(" (%s) -    MinimumValue = %f (for retaining event)" ,
                  GetName().c_str()                                  ,
                  Minimum_Value_                                       );
         log_info(" (%s) -    MaximumValue = %f (for retaining event)" ,
                  GetName().c_str()                                  ,
                  Maximum_Value_                                       );
    
         return;
    
    } // end of public member function I3DoubleFilter::Configure()
    
    bool SelectFrame(I3Frame& frame) 
    {
    
         if ( frame.Has(input_I3Double_Name_) ) {
    
    	    double Input_I3Double_value = frame.Get<I3Double>(input_I3Double_Name_).value;
            if ( ( Input_I3Double_value >= Minimum_Value_ ) &&
                 ( Input_I3Double_value <= Maximum_Value_ ) ) {
               log_info("(%s) - KEEP - %s - value = %f - return true",
                         GetName().c_str()          ,
                         input_I3Double_Name_.c_str() ,
                         Input_I3Double_value         );
               return true;
    	    } else {
               log_info("(%s) - DROP - %s - value = %f - return false",
    		         GetName().c_str()            ,
                         input_I3Double_Name_.c_str() ,
                         Input_I3Double_value         );
               return false;
            }
    
         } // end of IF-block for input_I3Double_Name_ frame object exists in the frame
         else {
    
            log_info("(%s) - DROP - %s - object missing - return false",
                      GetName().c_str()          ,
                      input_I3Double_Name_.c_str() );
            return false;
    
         } // end of IF-block for input_I3Double_Name_ frame object exists in the frame
    
    } // end of public member function I3DoubleFilter::SelectFrame()

  private:

    std::string input_I3Double_Name_;
    double      Minimum_Value_;
    double      Maximum_Value_;

    SET_LOGGER("I3DoubleFilter"); // log4cplus output tag

}; // end of definition for I3DoubleFilter class

#endif // I3DOUBLEFILTER_H

