#ifndef FILTERMASKFILTER_H_INCLUDED
#define FILTERMASKFILTER_H_INCLUDED
/**
 * @author Kevin Meagher
 *
 * @brief This is an I3IcePick module that filters events based 
 * on the contents of an I3FilterResultsMap
 */

class I3Context;
class I3Frame;

#include <string>
#include <vector>

#include <icetray/I3IcePick.h>

class FilterMaskFilter : public I3IcePick
{
 public:
  explicit FilterMaskFilter(const I3Context& context) ;

  void Configure();

  bool SelectFrame(I3Frame &frame);
  
private:
  ///Pass Frames where at least one of these filters is true
  std::vector<std::string> filter_name_list_;

  ///Name of I3FilterResultMap object in frame
  std::string filter_result_name_;
};

#endif  // FILTERMASKFILTER_H_INCLUDED
