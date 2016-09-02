/**
 * copyright (C) 2005
 * the IceCube collaboration
 * $Id$
 *
 * @file I3PortiaEvent.h
 * @version $Revision:$
 * @date $Date:$
 * @author Aya Ishihara <aya @ icecube.wisc.edu>
 */

#ifndef RECCLASSES_I3PORTIAEVENT_H
#define RECCLASSES_I3PORTIAEVENT_H

#include <boost/serialization/version.hpp>
#include <boost/serialization/access.hpp>
#include <icetray/I3FrameObject.h>
#include <icetray/OMKey.h>

static const unsigned i3portiaEvent_version_ = 2;


/**
 * @brief Stores event-wise information of the fast pulse analysis (see project portia).
 */
class I3PortiaEvent : public I3FrameObject 
{
  
 public:
  
  /**
   * Information of the first pulse omkey to get position and time
   */
  OMKey GetFirstPulseOMKey() const {return FirstOMKey_;}
  void  SetFirstPulseOMKey(OMKey omkey){FirstOMKey_ = omkey;}

  /**
   * Information of the first pulse omkey within the basetime window
   * to get position and time
   */
  OMKey GetFirstPulseOMKeybtw() const {return FirstOMKeybtw_;}
  void  SetFirstPulseOMKeybtw(OMKey omkey){FirstOMKeybtw_ = omkey;}
  
  /**
   * Information of the last pulse omkey to get position and time
   */
  OMKey GetLastPulseOMKey() const {return LastOMKey_;}
  void  SetLastPulseOMKey(OMKey omkey){LastOMKey_ = omkey;}

  /**
   * Information of the last pulse omkey within the basetime window
   * to get position and time
   */
  OMKey GetLastPulseOMKeybtw() const {return LastOMKeybtw_;}
  void  SetLastPulseOMKeybtw(OMKey omkey){LastOMKeybtw_ = omkey;}

  /**
   * Information of the omkey recording the largest NPE to get position and time
   */
  OMKey GetLargestNPEOMKey() const {return largestOMKey_;}
  void  SetLargestNPEOMKey(OMKey omkey){largestOMKey_ = omkey;}

  /**
   * Get event-wise ATWD based NPE sum 
   */
  double GetTotalAtwdNPE() const {return AtwdNPE_;}
  void   SetTotalAtwdNPE(double npe){AtwdNPE_ = npe;}

  /**
   * Get event-wise ATWD based NPEs within the basetime window
   */
  double GetTotalAtwdNPEbtw() const {return AtwdNPEbtw_;}
  void   SetTotalAtwdNPEbtw(double npe){AtwdNPEbtw_ = npe;}

  /**
   * Get event-wise FADC based NPE sum 
   */
  double GetTotalFadcNPE() const {return FadcNPE_;}
  void   SetTotalFadcNPE(double npe){FadcNPE_ = npe;}

  /**
   * Get event-wise FADC based NPEs within the basetime window
   */
  double GetTotalFadcNPEbtw() const {return FadcNPEbtw_;}
  void   SetTotalFadcNPEbtw(double npe){FadcNPEbtw_ = npe;}

  /**
   * @brief Get event-wise Best NPE sum
   *
   * Best means either ATWD or FADC whichever gives a larger value or 
   * information obtained by combining ATWD and FADC pulse information.
   */
  double GetTotalBestNPE() const {return BestNPE_;}
  void   SetTotalBestNPE(double npe){BestNPE_ = npe;}

  /**
   * @brief Get event-wise Best NPEs within the basetime window
   *
   * Best means either ATWD or FADC whichever gives a larger value or 
   * information obtained by combining ATWD and FADC pulse information.
   */
  double GetTotalBestNPEbtw() const {return BestNPEbtw_;}
  void   SetTotalBestNPEbtw(double npe){BestNPEbtw_ = npe;}

  /**
   * Get total number of DOM launch
   */
  double GetTotalNch() const {return TotalNch_;}
  void   SetTotalNch(int nch){TotalNch_ = nch;}

  /**
   * Get total number of DOMs within the basetime window
   */
  double GetTotalNchbtw() const {return TotalNchbtw_;}
  void   SetTotalNchbtw(int nch){TotalNchbtw_ = nch;}

  /**
   * @brief Get total number of DOM which has ATWD portia pulse
   *
   * ATWD portia pulse is created when the integral of the waveform is 
   * larger than threshold charge.
   */
  double GetAtwdNch() const {return AtwdNch_;}
  void   SetAtwdNch(int nch){AtwdNch_ = nch;}

  /**
   * @brief Get total number of DOM which has ATWD portia pulse 
   * within the basetime window
   *
   * ATWD portia pulse is created when the integral of the waveform is 
   * larger than threshold charge.
   */
  double GetAtwdNchbtw() const {return AtwdNchbtw_;}
  void   SetAtwdNchbtw(int nch){AtwdNchbtw_ = nch;}

  /**
   * @brief Get total number of DOM which has FADC portia pulse
   *
   * FADC portia pulse is created when the integral of the waveform is 
   * larger than threshold charge.
   */
  double GetFadcNch() const {return FadcNch_;}
  void   SetFadcNch(int nch){FadcNch_ = nch;}

  /**
   * @brief Get total number of DOM which has FADC portia pulse
   * within the basetime window
   *
   * FADC portia pulse is created when the integral of the waveform is 
   * larger than threshold charge.
   */
  double GetFadcNchbtw() const {return FadcNchbtw_;}
  void   SetFadcNchbtw(int nch){FadcNchbtw_ = nch;}

 private:
  
  double BestNPE_;
  double AtwdNPE_;
  double FadcNPE_;
  int TotalNch_;
  int AtwdNch_;
  int FadcNch_;

  double BestNPEbtw_;
  double AtwdNPEbtw_;
  double FadcNPEbtw_;
  int TotalNchbtw_;
  int AtwdNchbtw_;
  int FadcNchbtw_;

  OMKey FirstOMKey_;
  OMKey LastOMKey_;

  OMKey largestOMKey_;
  OMKey FirstOMKeybtw_;
  OMKey LastOMKeybtw_;

  friend class boost::serialization::access;
  template <typename Archive>
    void serialize(Archive& ar, unsigned version);
};

BOOST_CLASS_VERSION(I3PortiaEvent, i3portiaEvent_version_);

I3_POINTER_TYPEDEFS(I3PortiaEvent);

#endif
