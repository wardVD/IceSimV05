/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3RandomFilter.h
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3RANDOMFILTER_H_INCLUDED
#define I3RANDOMFILTER_H_INCLUDED

// forward declarations

class I3Context;
class I3Frame;

// header files

#include <icetray/I3IcePick.h>
#include <icetray/I3Logging.h>
#include <phys-services/I3RandomService.h>

// definitions


/** A filter that randomly selects 1/<em>Prescale</em> <em>events</em> (physics frames).
 * 
 * I3RandomFilter supports three parameter:
 * <ul>
 * <li><VAR>Prescale</VAR> (determines the fraction of <em>selected</em>
 * events (default value is 1 (select every event))),
 * <li><VAR>Seed</VAR> (seed for a local random function used for prescaling
 * (default value is 1)) and
 * <li><VAR>UseRandomService</VAR> (use random service for prescaling
 * instead of a local random function (default value is false)).
 * </ul>
 */
class I3RandomFilter : public I3IcePick
{
 public:
  /** Constructor.
   *
   * @param context the I3Context object containing this objects services etc.
   */
  I3RandomFilter(const I3Context& context);
  /** Destructor.
   */
  virtual ~I3RandomFilter();
  void Configure();
  bool SelectFrame(I3Frame& frame);

 private:
  unsigned int prescale_;
  unsigned int seed_;
  bool useRandomService_;
  I3RandomServicePtr randomService_;


  // private copy constructors and assignment
  I3RandomFilter(const I3RandomFilter&);
  I3RandomFilter& operator=(const I3RandomFilter&);


  // logging
  SET_LOGGER ("I3RandomFilter");
};

#endif /*I3RANDOMFILTER_H_INCLUDED*/
