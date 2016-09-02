#ifndef FILTERCHECKMODULE_H_INCLUDED
#define FILTERCHECKMODULE_H_INCLUDED
/** 
 * @author Kevin Meagher
 *
 * @brief This module verifies the result of the pole filter
 *
 * If you re-run the reconstructions and filters in the north,
 * this module will check the results of the filters still in I3Bools
 * against the I3FilterResultMap that the pole filter generated.
 * It prints the result to standard out.
 */

#include <icetray/I3Module.h>

class FilterCheckModule : public I3Module
{
 public:
  FilterCheckModule(const I3Context& context);
  
  void Configure();

  void Physics(I3FramePtr frame);
  
  void Finish();

private:
  typedef std::map <std::string, std::pair <int, int> > FilterStatisticsMap;
  ///map of filter names to number of times the filter matches and the number of comparisons made
  FilterStatisticsMap filter_matches_;
  ///Name of I3FilterResultMap object in frame
  std::string filter_result_name_;

};

#endif  // FILTERCHECKMODULE_H_INCLUDED
