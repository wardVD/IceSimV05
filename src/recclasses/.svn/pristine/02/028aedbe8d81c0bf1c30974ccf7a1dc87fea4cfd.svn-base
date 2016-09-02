/**
 * copyright (C) 2005
 * the IceCube collaboration
 * $Id$
 *
 * @version $Revision:$
 * @date $Date:$
 * @author Aya Ishihara <aya @ icecube.wisc.edu>
 */

#ifndef RECCLASSES_I3PORTIAPULSE_H
#define RECCLASSES_I3PORTIAPULSE_H

#include <icetray/I3FrameObject.h>
#include <dataclasses/physics/I3RecoPulse.h>
#include <dataclasses/I3Map.h>

/**
 * @brief Stores charge-based pulse information (see project portia). 
 */
class I3PortiaPulse : public I3FrameObject 
{

 public:
 /**
  *  @brief Null constructor
  */
  I3PortiaPulse() {
    BaseLine_ = 0.0;
    LaunchTime_ = 0.0; 
    StartTime_ = 0.0; 
    PeakBinTime_ = 0.0; 
    Time50_ = 0.0; 
    Time80_ = 0.0; 
    TOT_ = 0.0; 
    BinNumber_ =0; 
    BinSize_ =0.0; 
    LETime_ = 0.0; 
    NPE_ = 0.0; 
    PMTGain_ = 0.0; 
    IntegratedCharge_ = 0.0; 
    Amplitude_ = 0.0; 
    LCBit_ = false; 
    PositionX_ = 0.0; 
    PositionY_=0.0; 
    PositionZ_=0.0;}
  
 /**
  *  @brief Function to get I3RecoPulse
  */
  I3RecoPulse& GetRecoPulse(){ return recoPulse_; }
 /**
  *  @brief Function to get const I3RecoPulse
  */
  const I3RecoPulse& GetRecoPulse() const { return recoPulse_; }
 /**
  *  @brief Function to set I3RecoPulse
  */
  void   SetRecoPulse(I3RecoPulse& p){ recoPulse_=p; }
 /**
  *  @brief Function to set x position of the pulse
  */
  void   SetPositionX(double x){PositionX_=x;};
 /**
  *  @brief Function to set y position of the pulse
  */
  void   SetPositionY(double y){PositionY_=y;};
 /**
  *  @brief Function to set z position of the pulse
  */
  void   SetPositionZ(double z){PositionZ_=z;};
 /**
  *  @brief Function to get x position of the pulse
  */
  double GetPositionX() const {return PositionX_;};
 /**
  *  @brief Function to get y position of the pulse
  */
  double GetPositionY() const {return PositionY_;};
 /**
  *  @brief Function to get z position of the pulse
  */
  double GetPositionZ() const {return PositionZ_;};
 /**
  *  @brief Function to get integrated charge of the pulse
  */
  double GetIntegratedCharge() const {return IntegratedCharge_;}
  void   SetIntegratedCharge(double charge){IntegratedCharge_ = charge;}
 /**
  *  @brief Function to get baseline used to obtain the pulse information
  */
  double GetBaseLine() const {return BaseLine_;}
  void   SetBaseLine(double base){BaseLine_ = base;}
  
 /**
  *  @brief Function to get the time of the maximum entry of the waveform bins
  */
  double GetPeakBinTime() const {return PeakBinTime_;}
  void   SetPeakBinTime(double ptime){PeakBinTime_ = ptime;}

  /**
   * @brief Position of the peak can be obtained StartTime+PeakBinPosition*BinSize
   */
  double GetEstimatedNPE() const {return NPE_;}
  void   SetEstimatedNPE(double npe){NPE_ = npe;}

  /**
   * @brief DOM Launch Time
   */
  double GetLaunchTime() const {return LaunchTime_;}
  void   SetLaunchTime(double ltime){LaunchTime_ = ltime;}

  /**
   * @brief Waveform Start Time
   */
  double GetStartTime() const {return StartTime_;}
  void   SetStartTime(double stime){StartTime_ = stime;}

  /**
   * @brief LE Time -- generally this very close to the Launch time within a few ns
   */
  double GetLETime() const {return LETime_;}
  void   SetLETime(double ltime){LETime_ = ltime;}

  /**
   * @brief T50
   */
  double GetTime50() const {return Time50_;}
  void   SetTime50(double time){Time50_ = time;}

  /**
   * @brief T80
   */
  double GetTime80() const {return Time80_;}
  void   SetTime80(double time){Time80_ = time;}

  /**
   * @brief TOT
   */
  double GetTOT() const {return TOT_;}
  void   SetTOT(double time){TOT_ = time;}

  /**
   * @brief BinNumber
   */
  int  GetBinNumber() const {return BinNumber_;}
  void SetBinNumber(int num){BinNumber_ = num;}

  /**
   * @brief BinSize
   */
  double GetBinSize() const {return BinSize_;}
  void   SetBinSize(double size){BinSize_ = size;}


  /**
   * @brief Amplitude used to be in recopulse but not anymore
   */
  double GetAmplitude() const {return Amplitude_;}
  void   SetAmplitude(double amp){Amplitude_ = amp;}

  /**
   * @brief LC bit
   */
  bool GetLCBit() const {return LCBit_;}
  void SetLCBit(bool lc){LCBit_ = lc;}

  /**
   * @brief PMT Gain
   */

  double GetPMTGain() const {return PMTGain_;}
  void   SetPMTGain(double pm){PMTGain_ = pm;}
  
 private:
  /**
   * This pulse has 3 additional members to I3RecoPulse which has member functions
   * which has Time/Amplitude/Width and an unique HitID.
   * double 	GetTime ()
   * int 	GetHitID ()
   * double 	GetWidth ()
   *
   */

  I3RecoPulse recoPulse_;

  double BaseLine_;
  double LaunchTime_;
  double StartTime_;
  double PeakBinTime_;
  double Time50_;
  double Time80_;
  double TOT_;
  int BinNumber_;
  double BinSize_;
  double LETime_;
  double NPE_;
  double PMTGain_;
  double IntegratedCharge_;
  double Amplitude_;
  bool   LCBit_;

  double  PositionX_;
  double  PositionY_;
  double  PositionZ_;
  
  friend class boost::serialization::access;
  template <typename Archive>
    void serialize(Archive& ar, unsigned version);

};

I3_POINTER_TYPEDEFS(I3PortiaPulse);

class OMKey; //forward declare
typedef I3Map<OMKey, I3PortiaPulse> I3PortiaPulseMap;
I3_POINTER_TYPEDEFS(I3PortiaPulseMap);

#endif
