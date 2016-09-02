/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file I3WimpSimReader.cxx
 * @date $Date: 2012-11-18$
 * @author mzoll <marcel.zoll@fysik.su.se>
 * @brief Reads WimpSim data for Sun and Earth Events of teh New WimpSim-v3.01 format
 * This Module takes a WIMPSim Files, reads them event by event and puts Varibales into the frame: I3EventHeader, I#MCTree, WIMP_params
 */

// code progeniators
#include "wimpsim-reader/I3WimpSimReader.h"
// definitions and conversions
#include "icetray/I3Units.h"
#include "dataclasses/I3Constants.h"
// icetray processing
#include "icetray/I3Frame.h"
#include "icetray/I3Module.h"
#include "phys-services/I3RandomService.h"
// I3DataTypes
#include "icetray/I3Bool.h"
#include "icetray/I3Int.h"
#include "dataclasses/I3Double.h"
#include "dataclasses/I3String.h"
#include "dataclasses/I3Time.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/physics/I3EventHeader.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/physics/I3MCList.h"
#include "dataclasses/physics/I3MCTreeUtils.h"

#include <boost/assign/list_of.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assert.hpp>

#include <list>
#include <fstream>
#include <sstream>
#include <math.h>

#include <iostream>

using namespace std;
using namespace WimpSim;
using namespace WimpSimTools;
//_______________________________________________________________________
I3WimpSimReader::~I3WimpSimReader() {}
//_______________________________________________________________________
I3WimpSimReader::I3WimpSimReader(const I3Context &context):I3Module(context),
  //options
  randomService_(),
  randomServiceName_("I3RandomService"),
  filenames_(),
  over_(0),
  number_events_(0),
  position_limits_(8, NAN),
  radius_(NAN),
  zenith_min_(0*I3Units::degree),
  zenith_max_(180*I3Units::degree),
  electrons_(false),
  muons_(true),
  taus_(false),
  ncs_(false),
  sensHeight_(NAN),
  sensRadius_(NAN),
  startmjd_(NAN),
  endmjd_(NAN),
  writeInfoOpt_(true),
  infoFileName_(""),
  writeDrivingTimeOpt_(false),//TODO kill every mention of this variable
  pFrameOpt_(false),
  flatMapOpt_(false),
  fixGenLength_(100),
  //helpers/tracers
  file_index_(-1),
  wimpfile_(),
  issued_files_(0),
  issued_events_(0),
  processed_events_(0),
  //bookkeeping
  col_weight_(),
  col_events_(),
  tot_weight_(),
  tot_events_()
{
  log_debug("Entering Constructor");

  std::list<double> laber = boost::assign::list_of(-800.)(800.)(-800.)(800.)(-800.)(800.);
  std::vector<double> poslim (laber.begin(), laber.end());
  position_limits_ = poslim;
  
  AddOutBox("OutBox");
  AddParameter("RandomService","The RandomService in the context", randomService_);
  AddParameter("RandomServiceName", "Name of the RandomService to be used", randomServiceName_);
  AddParameter("FileNameList", "The WimpSim file to read from", filenames_);
  AddParameter("NEvents", "Number of events to issue, if unconfigured read everything", number_events_);
  AddParameter("PositionLimits", "Array of xmin,xmax,ymin,ymax,zmin,zmax", position_limits_);
  AddParameter("InjectionRadius", "If >0, events will be injected in cylinder with zmin, zmax height", radius_);
  AddParameter("LowerZenCut", "optional lower Zenith Cut in [rad]", zenith_min_);
  AddParameter("UpperZenCut","optional upper Zenith Cut in [rad]", zenith_max_);
  AddParameter("UseElectrons", "Read and simulate electron vertices", electrons_);
  AddParameter("UseMuons", "Read and simulate muon vertices",  muons_);
  AddParameter("UseTaus", "Read and simulate tau vertices", taus_);
  AddParameter("UseNC","Read and simulate NeutralCurrent vertices", ncs_);
  AddParameter("Oversampling", "N oversamplings", over_);
  AddParameter("SensitiveHeight", "Muon box activated height", sensHeight_);
  AddParameter("SensitiveRadius", "Muon box activated radius", sensRadius_);
  AddParameter("StartMJD", "MJD to start simulation; if unspecified: read everything", startmjd_);
  AddParameter("EndMJD", "MJD to end simulation; if unspecified: read everything", endmjd_);
  AddParameter("InfoFileName", "write Information to this textfile", infoFileName_);
}
//_______________________________________________________________________
void I3WimpSimReader::Configure() {
  log_debug("Entering Configure()");

  GetParameter("RandomService", randomService_);
  GetParameter("RandomServiceName", randomServiceName_);
  GetParameter("FileNameList", filenames_);
  GetParameter("NEvents", number_events_);
  GetParameter("PositionLimits", position_limits_);
  GetParameter("InjectionRadius", radius_);
  GetParameter("LowerZenCut", zenith_min_);
  GetParameter("UpperZenCut", zenith_max_);
  GetParameter("UseElectrons", electrons_);
  GetParameter("UseMuons", muons_);
  GetParameter("UseTaus", taus_);
  GetParameter("UseNC", ncs_);
  GetParameter("Oversampling", over_);
  GetParameter("SensitiveHeight", sensHeight_);
  GetParameter("SensitiveRadius", sensRadius_);
  GetParameter("StartMJD", startmjd_);
  GetParameter("EndMJD", endmjd_);
  GetParameter("InfoFileName", infoFileName_);

  //DANGER hardcodings
  writeInfoOpt_ = true; // this is right now hardcoded, but it is a true option
  writeDrivingTimeOpt_ = false; // this is right now hardcoded, but it is a true option
  pFrameOpt_ = false; // this is right now hardcoded, but it is a true option
  flatMapOpt_ = false; // this is right now hardcoded, but it is a true option
  fixGenLength_ = 100; // this is right now hardcoded, but it is a true option

  // Look if everything is configured right
  if (!randomService_){
    randomService_ = context_.Get<I3RandomServicePtr>(randomServiceName_);
    if (!randomService_)
      log_fatal("No Random Service configured!");
  }

  if (filenames_.empty())
    log_fatal("Specify a Filename with Parameter 'FileNameList'");
  else{
    for(uint filenames_index=0; filenames_index<filenames_.size(); filenames_index++) {
      ifstream thisfile(filenames_[filenames_index].c_str(), ios_base::in);
      if (! thisfile.good())
        log_fatal("The specified file %s could not be accessed",filenames_[filenames_index].c_str());
    }
  }
  if (radius_!=0 && radius_ > (-position_limits_[0]+ position_limits_[1])/2)
    log_warn("Radius exceeds Box limits in x");
  if (radius_!=0 && radius_ > (-position_limits_[2]+ position_limits_[3])/2)
    log_warn("Radius exceeds Box limits in y");
  if ((position_limits_[0]+ position_limits_[1])/2 || (position_limits_[2]+ position_limits_[3])/2 || (position_limits_[4]+ position_limits_[5])/2)
    log_warn("Beware, you shift away the box-generator center from the detector center");

  if (position_limits_.size()!=6)
    log_fatal("Specify the right amount of Position Limits which is 6: [xmin,xmax,ymin,ymax,zmin,zmax]");
  if ((position_limits_[0] > position_limits_[1]) || (position_limits_[2] > position_limits_[3]) || (position_limits_[4] > position_limits_[5]))
    log_fatal("PositionLimits should be a ordered list of [xlow, xhigh, ylow, yhigh, zlow, z high]");
  if ((position_limits_[0] == position_limits_[1]) || (position_limits_[2] == position_limits_[3]) || (position_limits_[4] == position_limits_[5]))
    log_error("Box-Limits are equal for x, y or z; this will most likely lead to errors");

  if ((zenith_min_<0. || 180.<zenith_min_) || (zenith_max_<0. || 180.<zenith_max_))
    log_fatal("Zenith cut values must be within [0, 180] degrees");
  if (zenith_min_>zenith_max_)
    log_fatal("LowerZenCut exceeds UpperZenCut");

  if (radius_<=0.)
    log_fatal("Injection radius must be positive and not zero");

  if ((! std::isnan(sensHeight_) || ! std::isnan(sensRadius_)) && (electrons_))
    log_warn("No Implementation for generated Length of this ParticleType: electrons. Will use RandomPosition in InjectionVolume");
  if ((! std::isnan(sensHeight_) || ! std::isnan(sensRadius_)) && (taus_))
    log_warn("No Implementation for generated Length of this ParticleType: taus. Will use RandomPosition in InjectionVolume");
  if ((! std::isnan(sensHeight_) || ! std::isnan(sensRadius_)) && (ncs_ ))
    log_warn("No Implementation for generated Length of this ParticleType: neutralcurent. Will use RandomPosition in InjectionVolume");

  if ( (! std::isnan(sensRadius_) && std::isnan(sensHeight_)) || (std::isnan(sensRadius_) && ! std::isnan(sensHeight_)))
    log_fatal("Configure both 'SensitiveRadius' and 'SensitiveHeight' or none");

  if (! std::isnan(radius_) && ! std::isnan(sensRadius_)  && sensRadius_ > radius_)
    log_warn("SensitiveRadius exceeds InjectionRadius");

  if (! std::isnan(sensHeight_) && sensHeight_ > (-position_limits_[2]+ position_limits_[3]))
    log_warn("SensitiveHeight exceeds Box limits in z");

  if (endmjd_ < startmjd_)
    log_fatal("'EndMJD' is lesser 'StartMJD'");
}
//_______________________________________________________________________
void I3WimpSimReader::Process() {
  log_debug("Entering Process()");
  static bool wimpfileIsOpen;
  static bool fileHeaderIsRead;

  static boost::shared_ptr<WimpSimBaseHeader> WimpHeaderPtr;
  static boost::shared_ptr<WimpSimBaseEvent> WimpEventPtr;

  do {
    if (number_events_ != 0 && issued_events_ >= number_events_) {
      log_info("Issued the requested number of events: Exiting IceTray");
      wimpfile_.close(); // close the file
      RequestSuspension();
      return;
    }
    if (!wimpfileIsOpen) { // If no file is open at the time, open the next file.
      wimpfileIsOpen = OpenNextFile(); // try open the next file in line
      if (wimpfileIsOpen){
        WimpHeaderPtr = boost::shared_ptr<WimpSimBaseHeader>(new WimpSimBaseHeader); //renew the object
        fileHeaderIsRead = ReadFileHeader(wimpfile_, WimpHeaderPtr); // try to read the header from the newly opened file
        if (!fileHeaderIsRead){
          wimpfile_.close();
          wimpfileIsOpen = false;
          log_fatal("ERROR while reading FileHeader;");
          continue;
        }
        else {
          issued_files_++;
          if (writeInfoOpt_){
            I3FramePtr head_info_frame = WriteInfoFrame(WimpHeaderPtr);
            PushFrame(head_info_frame);
          }
          static const std::string source_ = WimpHeaderPtr->source_;
          if (WimpHeaderPtr->source_ != source_) {
            log_warn("It seems files are containing Earth and Sun Events."
              "This is supported but not recommended to do.");
          }
        }
      }
      else{ // If no file could be opened
        log_debug("Reached end of available data stream: Exiting Tray");
        RequestSuspension();
        return;
      }
    }

    exit_codes event_exit_code=SUCCESS; //tracer for error-codes

    // read an event or use the oversampled
    static unsigned int doOver; // tracer of the current oversample
    if (over_ == 0 || doOver == 0) { // Read new Event from file, move the pointer in the file along
      doOver = over_;

      if (WimpHeaderPtr->source_== "Sun") {
        boost::shared_ptr<WimpSimSunEvent> SunEventPtr = boost::shared_ptr<WimpSimSunEvent>(new WimpSimSunEvent);
        event_exit_code = ReadSunEvent(wimpfile_, SunEventPtr);
        WimpEventPtr = SunEventPtr;
      }
      else if (WimpHeaderPtr->source_== "Earth") {
        boost::shared_ptr<WimpSimEarthEvent> EarthEventPtr = boost::shared_ptr<WimpSimEarthEvent>(new WimpSimEarthEvent);
        event_exit_code = ReadEarthEvent(wimpfile_, EarthEventPtr);
        WimpEventPtr = EarthEventPtr;
      }
      else {
        event_exit_code = ERROR;
      }
    }
    else if (over_ > 0 && doOver > 0) // treat oversampling and decrease the oversampling iterator by 1
      doOver--;

    //special MJD treatment
    double mjd= NAN;
    if (WimpHeaderPtr->source_== "Sun")
      mjd = WimpEventPtr->GetMJD();
    else if (WimpHeaderPtr->source_== "Earth") {
      if (startmjd_ == endmjd_)
        mjd = startmjd_;
      else{
        mjd = RandomMJDTime().GetModJulianDayDouble();
        log_trace("Randomized mjd : %f", mjd);
      }
    }

    //apply Cuts and add Weights to Bookkeeping
    if (!event_exit_code)
      event_exit_code = CutsNWeights(WimpEventPtr, mjd);

    processed_events_++;
    switch (event_exit_code) {
      // ALL was WELL
      case SUCCESS: // event was sucessfully read and lies inside timewindow
        log_debug("Event was fully processed");
        PushFrame(WriteEvent(WimpHeaderPtr, WimpEventPtr, mjd));
        issued_events_++;
        log_debug("Delivered one Event to the OutBox");
        return;
      //CUTS and EXITS
      case FAILED_TIMECUTS: // event was sucessfully read but lies outside timewindow
        processed_events_++;
        log_debug("Event failed Time Cuts");
        break;
      case FAILED_ZENITHCUTS: // event was sucessfully read but lies outside zenith cuts
        log_debug("Event failed Zenith Cuts");
        processed_events_++;
        break;
      case FAILED_LEPTONCUTS: // event was sucessfully read but lies outside lepton cuts
        log_debug("Event failed Lepton Cuts");
        processed_events_++;
        break;
      case FAILED_ENERGYCUTS: // event was sucessfully read but lies outside energy cuts
        log_debug("Event failed Energy Cuts");
        processed_events_++;
        break;
      case END_EVENTSTREAM: // regular end of datafile was reached while reading
        wimpfile_.close();
        wimpfileIsOpen = false;
        fileHeaderIsRead = false;
        log_debug("End of event stream from files; Closed the file");
        break;
      case ERROR_EVENTSTREAM: // something unforeseen happened
        log_error("ERROR while reading next event; trying the next file");
        wimpfile_.close();
        wimpfileIsOpen = false;
        fileHeaderIsRead = false;
        break;
      //UNFORESEEN
      default:
        log_debug("Unrecognized Error Code");
        processed_events_++;
        break;
    }
  } while (true);
}
//_______________________________________________________________________
void I3WimpSimReader::Finish() {
  log_debug("Entering Finish()");
  if (wimpfile_.is_open()) // if forgotten to close
    wimpfile_.close();
  log_notice("Delivered %i events from %i processed in %i files", issued_events_, processed_events_, issued_files_);
  if (issued_files_ != filenames_.size())
    log_warn("There were corrupt or wrongly read files");
  if (writeInfoOpt_)
    PushFrame(WriteFinishFrame());
  Report();
  if (infoFileName_ != "")
    WriteInfoToFile(infoFileName_);
}
//_______________________________________________________________________
std::string I3WimpSimReader::ReportString() const {
  log_debug("Entering Report()");

  std::ostringstream report; //the report from my module
  report << "*************************************" << endl;;
  report << "delivered / total weight  (delivered / total particles):" << endl;
  report << "nu_weight     : " << col_weight_.nu_ << " / " << tot_weight_.nu_ << " ( " << col_events_.nu_ << " / " << tot_events_.nu_ << " )" << endl;
  report << "  lowe_weight : " << col_weight_.lowe_ << " / " << tot_weight_.lowe_ << " ( " << col_events_.lowe_ << " / " << tot_events_.lowe_ << " )" << endl;
  report << "lep_weight    : " << col_weight_.lep_ << " / " << tot_weight_.lep_ << " ( " << col_events_.lep_ << " / " << tot_events_.lep_ << " )" << endl;
  report << "  e_weight    : " << col_weight_.e_ << " / " << tot_weight_.e_ << " ( " << col_events_.e_ << " / " << tot_events_.e_ << " )" << endl;
  report << "  mu_weight   : " << col_weight_.mu_ << " / " << tot_weight_.mu_ << " ( " << col_events_.mu_ << " / " << tot_events_.mu_ << " )" << endl;
  report << "  tau_weight  : " << col_weight_.tau_ << " / " << tot_weight_.tau_ << " ( " << col_events_.tau_ << " / " << tot_events_.tau_ << " )" << endl;
  report << "  nc_weight   : " << col_weight_.nc_ << " / " << tot_weight_.nc_ << " ( " << col_events_.nc_ << " / " << tot_events_.nc_ << " )" << endl;
  report << "had_weight    : " << col_weight_.had_ << " / " << tot_weight_.had_ << " ( " << col_events_.had_ << " / " << tot_events_.had_ << " )" << endl;
  report << "=============== " << endl;
  report << "Files read    : " << issued_files_ << endl;
  
  return report.str();
}

void I3WimpSimReader::Report() const {
  log_debug("Entering Report()");

  log_notice("%s", ReportString().c_str());
}

//______________________________________________________________________
I3FramePtr I3WimpSimReader::WriteFinishFrame() const{
  log_debug("Entering WriteFinishFrame()");

  I3FramePtr frame(new I3Frame(I3Frame::Stream('W')));

  I3MapStringDoublePtr tot_weight_map (new I3MapStringDouble);
  (*tot_weight_map)["nu_weight"] = tot_weight_.nu_;
  (*tot_weight_map)["lep_weight"] = tot_weight_.lep_;
  (*tot_weight_map)["e_weight"] = tot_weight_.e_;
  (*tot_weight_map)["mu_weight"] = tot_weight_.mu_;
  (*tot_weight_map)["tau_weight"] = tot_weight_.tau_;
  (*tot_weight_map)["nc_weight"] = tot_weight_.nc_;
  (*tot_weight_map)["had_weight"] = tot_weight_.had_;
  (*tot_weight_map)["lowe_weight"] = tot_weight_.lowe_;

  I3MapStringDoublePtr col_weight_map (new I3MapStringDouble);
  (*col_weight_map)["nu_weight"] = col_weight_.nu_;
  (*col_weight_map)["lep_weight"] = col_weight_.lep_;
  (*col_weight_map)["e_weight"] = col_weight_.e_;
  (*col_weight_map)["mu_weight"] = col_weight_.mu_;
  (*col_weight_map)["tau_weight"] = col_weight_.tau_;
  (*col_weight_map)["nc_weight"] = col_weight_.nc_;
  (*col_weight_map)["had_weight"] = col_weight_.had_;
  (*col_weight_map)["lowe_weight"] = col_weight_.lowe_;

  frame->Put("DeliveredWeightMap", col_weight_map);
  frame->Put("TotalWeightMap", tot_weight_map);

  return frame;
}
//_______________________________________________________________________
bool I3WimpSimReader::OpenNextFile(){
  log_debug("Entering OpenNextFile()");
  file_index_+=1;
  if((uint)file_index_>=filenames_.size()){
    log_debug("No file left. Ending the run");
    return false;
  }
  else{
    wimpfile_.open(filenames_[file_index_].c_str(), ios_base::in); //open as read-only
    if (!wimpfile_.good())
      log_fatal("File %s is inaccessable", filenames_[file_index_].c_str());
    log_debug("Opened the file %s ",filenames_[file_index_].c_str());
    return true;
  }
}
//_______________________________________________________________________
bool I3WimpSimReader::ReadFileHeader(ifstream &wimpfile, boost::shared_ptr<WimpSimBaseHeader> &WimpHeaderPtr) {
  log_debug("Entering ReadFileHeader()");
  // the Fileheader foramtion is a bit irradic so that readin must be done by a find-and-match;
  bool prog_reading = true; //progressive_reading : the file-header is templated, every error in reading will reset this and terminate this methode
  string datastring; // container storing whole lines
  string tmpstring=""; // a container which stores the next word read
  string hashstore; // a container that stores the hash charakter(s)
  string equalsstore; // a container that stores the equalsign
  string doublestore; // a container to store read doubles
  bool IsEOF=false; // End of file/End of line readed while reading
  while (datastring != "### This is the last line of the wimpevent header") { // until end of header is found
    if (IsEOF) {
      prog_reading = false;
      break;
    }
    if (wimpfile.eof()) {
      IsEOF = true;
      //no break; will happen next iteration
    }
    getline(wimpfile_,datastring);
    istringstream line(datastring.c_str());
    line >> hashstore;
    log_trace("These should be hash: %s",hashstore.c_str());
    if (!(hashstore == "#" || hashstore == "###")) {
      log_fatal("An expected header line did not start with hash");
      break;
    }
    bool IsEOS =false;
    tmpstring="";
    do { // 'find and match' of certain strings in this line
      if (IsEOS) //takes care not to try to read out of scope
        break;
      if (line.eof()) {
        IsEOS=true;
        //no break; will happen next iteration
      }
      //WimpEvent run Parameters
      if (tmpstring == "Detector") {
        line >> tmpstring;
        if (tmpstring=="latitude")
          line >> equalsstore >> WimpHeaderPtr->latitude_;
        if (tmpstring=="longitude")
          line >> equalsstore >> WimpHeaderPtr->longitude_;
      } else if (tmpstring == "mjd1")
        line >> equalsstore >> WimpHeaderPtr->mjd1_;
      else if (tmpstring == "mjd2")
        line >> equalsstore >> WimpHeaderPtr->mjd2_;
      //WimpAnn run parameters
      else if (tmpstring == "Annihilation") {
        line >> tmpstring;
        if (tmpstring=="channel")
          line >> equalsstore >> WimpHeaderPtr->channel_;
      } else if (tmpstring == "WIMP") {
        line >> tmpstring;
        if (tmpstring=="mass") {
          line >> equalsstore >> WimpHeaderPtr->mass_;
        }
      } else if (tmpstring == "Number") {
        line >> tmpstring;
        if (tmpstring=="of") {
          line >> tmpstring;
          if (tmpstring=="annihilations") {
            line >> equalsstore >> WimpHeaderPtr->number_content_;
          }
        }
      } else if (tmpstring == "Source")
        line >> equalsstore >> WimpHeaderPtr->source_;
      //Neutrino Oscillation Parameters
      else if (tmpstring == "theta_12")
        line >> equalsstore >> WimpHeaderPtr->theta_12_;
      else if (tmpstring == "theta_13")
        line >> equalsstore >> WimpHeaderPtr->theta_13_;
      else if (tmpstring == "theta_23")
        line >> equalsstore >> WimpHeaderPtr->theta_23_;
      else if (tmpstring == "delta")
        line >> equalsstore >> WimpHeaderPtr->delta_;
      else if (tmpstring == "delta_m^2_21")
        line >> equalsstore >> WimpHeaderPtr->delta_m2_21_;
      else if (tmpstring == "delta_m^2_31")
        line >> equalsstore >> WimpHeaderPtr->delta_m2_31_;
      //splitting information this file
      else if (tmpstring == "Total") {
        line >> tmpstring;
        if (tmpstring=="number") {
          line >> tmpstring; // word == "of"
          line >> tmpstring;
          if (tmpstring=="annihilations") {
            line >> tmpstring; // word == "in"
            line >> tmpstring; // word == "this"
            line >> tmpstring;
            if (tmpstring=="run") {
              line >> equalsstore >> WimpHeaderPtr->number_total_;
            }
          }
        }
      } else if (tmpstring == "File") {
        line >> tmpstring;
        if (tmpstring=="number") {
          line >> WimpHeaderPtr->files_index_;
          line >> tmpstring; // word == "out"
          line >> tmpstring; // word == "of"
          line >> WimpHeaderPtr->files_total_;
        }
      } else if (tmpstring == "Seed") {
        line >> tmpstring; //  word == for
        line >> tmpstring;
        if (tmpstring=="random") {
          line >> tmpstring; //  word == number
          line >> tmpstring; //  word == generator
          line >> tmpstring; //  word == (for
          line >> tmpstring; //  word == beginning
          line >> tmpstring; //  word == of
          line >> tmpstring; //  word == run)
          line >> equalsstore >> WimpHeaderPtr->seed_;
        }
      }
      //always try to read the next word
      line >> tmpstring;
      log_trace("next word: %s", tmpstring.c_str());
    } while (true);
  }//end while(); Last Line of the FileHeader was found
  if (prog_reading==true) {
    log_info("WimpSim file Header successfully read");
    return true;
  }
  else {
    log_fatal("No events read, because no line found saying: "
    "'# EE (end of event)'. This is a comment line that should be just before your first event. "
    "It is the last of the comment lines automatically put on top of your .dat file by WIMPSIM. "
    "Make sure there are no whitespace after this line.");
    return false;
  }
}
//_______________________________________________________________________
I3Position I3WimpSimReader::RandomPosition() const {
  I3Position pos;

  if (std::isnan(radius_)) { // carthesian isotropic
    pos.SetX( randomService_->Uniform(position_limits_[0], position_limits_[1]) );
    pos.SetY( randomService_->Uniform(position_limits_[2], position_limits_[3]) );
    pos.SetZ( randomService_->Uniform(position_limits_[4], position_limits_[5]) );
  }
  else { // cylindirical isotropic
    double x;
    double y;
    do {
      x = randomService_->Uniform(-radius_, radius_);
      y = randomService_->Uniform(-radius_, radius_);
    } while ((x*x+y*y) > (radius_*radius_)); // not inside radius
    //add shift of cylinder centre
    pos.SetX( x + (position_limits_[0]+ position_limits_[1])/2 );
    pos.SetY( y + (position_limits_[2]+ position_limits_[3])/2 );
    //randomize z position
    pos.SetZ( randomService_->Uniform(position_limits_[4], position_limits_[5]) );
  }
  return pos;
}
//_______________________________________________________________________
double I3WimpSimReader::VolumeBoxOrCylinder() const {
  static double volume;
  if (std::isnan(radius_)) { // carthesian isotropic
    volume=std::abs(position_limits_[1]- position_limits_[0])*std::abs(position_limits_[3]- position_limits_[2])*std::abs(position_limits_[5]- position_limits_[4]);
  }
  else { // cylindirical isotropic
    volume=(I3Constants::pi * (radius_*radius_) *std::abs(position_limits_[5]- position_limits_[4]));
  }
  return volume;
}
//_______________________________________________________________________
double I3WimpSimReader::RandomPositionVolume() const {
  static const double volume = VolumeBoxOrCylinder();
  return volume;
}
//_______________________________________________________________________
I3WimpSimReader::GenBox I3WimpSimReader::GenerateBox (const I3Particle& injected_neutrino, const I3Particle& injected_lepton) const {
  log_debug("Entering GenerateBox()");

  GenBox gen_box;

  gen_box.box_vector = injected_lepton.GetDir();

  const double zen = injected_lepton.GetDir().GetZenith();

  const double coszen = cos(zen);
  const double sinzen = sin(zen);
  //Find sides of muon box in meters
  gen_box.box_vertical = sensHeight_*fabs(sinzen) + 2*sensRadius_*fabs(coszen);
  gen_box.box_horizontal = 2 * sensRadius_;
  const double L_min = sensHeight_*fabs(coszen) + 2*sensRadius_*fabs(sinzen);

  // find the generated_length argument by the correct particle type
  switch (injected_lepton.GetType()) {
    case I3Particle::MuPlus:
    case I3Particle::MuMinus:
      gen_box.box_length = Length_mu(injected_lepton.GetEnergy()) + L_min;
      break;
    case I3Particle::EPlus:
    case I3Particle::EMinus:
      log_fatal("No Implementation for generated Length of that ParticleType: %s", injected_lepton.GetTypeString().c_str());
      //gen_box.box_length = Length_elmag(injected_lepton.GetEnergy()) + L_min;
      break;
    case I3Particle::TauPlus:
    case I3Particle::TauMinus:
      log_fatal("No Implementation for generated Length of that ParticleType: %s", injected_lepton.GetTypeString().c_str());
      //gen_box.box_length = Length_tau(injected_lepton.GetEnergy()) + L_min;
      break;
    case I3Particle::Hadrons:
      log_fatal("No Implementation for generated Length of that ParticleType: %s", injected_lepton.GetTypeString().c_str());
      //gen_box.box_length = Length_had(injected_lepton.GetEnergy()) + L_min;
      break;
    default:
      log_fatal("No Implementation for generated Length of that ParticleType: %s", injected_lepton.GetTypeString().c_str());
      break;
  }
  //Randomize point inside box
  gen_box.box_pos.SetX(gen_box.box_vertical*(randomService_->Uniform(-0.5,0.5)));
  gen_box.box_pos.SetY(gen_box.box_horizontal*(randomService_->Uniform(-0.5,0.5)));
  gen_box.box_pos.SetZ(gen_box.box_length*(randomService_->Uniform(0,1)) - L_min/2);

  //Set and calculate effective volume output parameters
  gen_box.vgen = gen_box.box_horizontal*gen_box.box_vertical*gen_box.box_length;
  //set and calculate the effective area for the neutrino as projection of the faceside
  gen_box.aproj = ( cos(fabs(injected_neutrino.GetDir().GetZenith()-injected_lepton.GetDir().GetZenith())) * gen_box.box_horizontal )
    * ( cos(fabs(injected_neutrino.GetDir().GetAzimuth()-injected_lepton.GetDir().GetAzimuth())) * gen_box.box_vertical);
  //Tilt the box to detector system
  gen_box.particle_pos= RotateZenAzi(gen_box.box_pos, gen_box.box_vector);

  //Add the shift of coordinates from centers of position_limits_
  gen_box.particle_pos.SetX(gen_box.particle_pos.GetX()+(position_limits_[0]+position_limits_[1])/2); // NOTE changed this 2012-09-18 to take center of PositionLimits as center
  gen_box.particle_pos.SetY(gen_box.particle_pos.GetY()+(position_limits_[2]+position_limits_[3])/2); // NOTE changed this 2012-09-18 to take center of PositionLimits as center
  gen_box.particle_pos.SetZ(gen_box.particle_pos.GetZ()+(position_limits_[4]+position_limits_[5])/2); // NOTE changed this 2012-09-18 to take center of PositionLimits as center

  return gen_box;
}
//_______________________________________________________________________
I3Position I3WimpSimReader::RotateZenAzi (const I3Position& pos, const I3Direction& dir) const {
  //Precalculate sines and coses
  const double zenith = dir.GetZenith();
  const double azimuth = dir.GetAzimuth();

  const double px = pos.GetX();
  const double py = pos.GetY();
  const double pz = pos.GetZ();

  const double coszenith = cos(zenith);
  const double sinzenith = sin(zenith);
  const double cosazimuth = cos(azimuth);
  const double sinazimuth = sin(azimuth);

  I3Position return_position;

  //Perform rotation
  return_position.SetX( px*coszenith*cosazimuth - py*sinazimuth + pz*sinzenith*cosazimuth );
  return_position.SetY( px*coszenith*sinazimuth + py*cosazimuth + pz*sinzenith*sinazimuth );
  return_position.SetZ( -px*sinzenith                           + pz*coszenith            );

  return return_position;
}
//_______________________________________________________________________
double I3WimpSimReader::Length_mu (const double energy) const {
  // TODO reference needed for these values; (here taken from old 2007 implementation)
  static const double rho=0.917;  // density [1/rho_water]
  static const double a=0.212;    // [GeV/mWe]
  static const double b=0.251E-3; // [1/mWe]
  static const double S=1.2;      // scale

  return (S/(b*rho))*log(1.0 + energy*b/a);
}
//_______________________________________________________________________
double I3WimpSimReader::Length_elmag (const double energy) const {
  //reference: Bernhard Voigt, Sensitivity of the IceCube Detector for Ultra-High Energy Electron-Neutrino Events, PhD thesis, 20080430, 2008, page 32
  //reference: M. Kowalski. On the Cherenkov light emission of hadronic and electromagnetic cascades. AMANDA Internal Report 20020803, 2002, page 20
  // DANGER this is the total track length: the sum of of the length of all subtracks; it is usefull fr the total light yeald, but not as an estimate of the toltal length of a  track in the detector
  const double factor=5.2; // [m/GeV]
  const double S=1.2;      // scale

  return (S*factor*energy);
}
//_______________________________________________________________________
double I3WimpSimReader::Length_tau (const double energy) const { // TODO implement this in the distant furture
  log_warn("There is currently no implementation for the Generated Length for taus; will fall back to default value of %f meters", fixGenLength_);
  return fixGenLength_;
}
//_______________________________________________________________________
double I3WimpSimReader::Length_had (const double energy) const {
  //reference: Bernhard Voigt, Sensitivity of the IceCube Detector for Ultra-High Energy Electron-Neutrino Events, PhD thesis, 20080430, 2008, page 36
  //reference: M. Kowalski. Search for neutrino induced cascades with the AMANDA-II detector. PhD thesis, Humboldt-Universität zu Berlin, 2004. page 21
  //reference: M. Kowalski. On the Cherenkov light emission of hadronic and electromagnetic cascades. AMANDA Internal Report 20020803, 2002, page 6
  // TODO check this implememtation
  // DANGER this is the total track length: the sum of of the length of all subtracks; it is usefull fr the total light yeald, but not as an estimate of the toltal length of a  track in the detector
  static const double E_0 = 0.399;
  static const double m = 0.130;
  static const double f_0 = 0.467;
  static const double RMS_0 = 0.379;
  static const double gamma = 1.160;
  const double F_em = 1 -pow(energy/E_0, -m); // fraction of the em part in the cascade
  const double F= F_em + (1 - F_em)*f_0; // scaling factor for hadronic cascades
  const double RMS = RMS_0 * pow(log(energy),-gamma);

  //return (F*Length_elmag(energy));
  return (randomService_->Gaus(F, RMS));
}
//_______________________________________________________________________
I3FramePtr I3WimpSimReader::WriteInfoFrame(const boost::shared_ptr<WimpSimBaseHeader> WimpHeaderPtr) const{
  log_debug("Entering WriteInfoFrame()");

  I3FramePtr frame(new I3Frame(I3Frame::Stream('W')));//I3Frame::TrayInfo)); //I3Frame::Stream('M')

  I3MapStringDoublePtr osc_params(new I3MapStringDouble());
  (*osc_params)["theta_12"] = WimpHeaderPtr->theta_12_;
  (*osc_params)["theta_13"] = WimpHeaderPtr->theta_13_;
  (*osc_params)["theta_23"] = WimpHeaderPtr->theta_23_;
  (*osc_params)["delta"] = WimpHeaderPtr->delta_;
  (*osc_params)["delta_m^2_21"] = WimpHeaderPtr->delta_m2_21_;
  (*osc_params)["delta_m^2_31"] = WimpHeaderPtr->delta_m2_31_;

  I3MapStringDoublePtr WimpSim_params(new I3MapStringDouble());
  (*WimpSim_params)["WIMP_mass"]=WimpHeaderPtr->mass_;
  (*WimpSim_params)["Annihilation_Channel"]=WimpHeaderPtr->channel_;
  (*WimpSim_params)["Number_of_Annihilations"]=WimpHeaderPtr->number_content_;
  (*WimpSim_params)["Total_of Annihilations"]=WimpHeaderPtr->number_total_;
  (*WimpSim_params)["Files_index"]=WimpHeaderPtr->files_index_;
  (*WimpSim_params)["Files_total"]=WimpHeaderPtr->files_total_;
  (*WimpSim_params)["Seed"]=WimpHeaderPtr->seed_;
  if (WimpHeaderPtr->source_=="Sun") {
    (*WimpSim_params)["Source"]=1;
    (*WimpSim_params)["Detector_Longitude"]=WimpHeaderPtr->longitude_;
    (*WimpSim_params)["Detector_Latitude"]=WimpHeaderPtr->latitude_;
    (*WimpSim_params)["mjd1"]=WimpHeaderPtr->mjd1_;
    (*WimpSim_params)["mjd2"]=WimpHeaderPtr->mjd2_;
  }
  else if (WimpHeaderPtr->source_=="Earth")
    (*WimpSim_params)["Source"]=2;

  I3MapStringDoublePtr WimpSimReader_params(new I3MapStringDouble());
  (*WimpSimReader_params)["PositionLimitsXmin"]=position_limits_[0];
  (*WimpSimReader_params)["PositionLimitsXmax"]=position_limits_[1];
  (*WimpSimReader_params)["PositionLimitsYmin"]=position_limits_[2];
  (*WimpSimReader_params)["PositionLimitsYmax"]=position_limits_[3];
  (*WimpSimReader_params)["PositionLimitsZmin"]=position_limits_[4];
  (*WimpSimReader_params)["PositionLimitsZmax"]=position_limits_[5];
  (*WimpSimReader_params)["InjectionRadius"]=radius_;
  (*WimpSimReader_params)["LowZenCut"]=zenith_min_;
  (*WimpSimReader_params)["UpperZenCut"]=zenith_max_;
  (*WimpSimReader_params)["UseElectrons"]=electrons_;
  (*WimpSimReader_params)["UseMuons"]=muons_;
  (*WimpSimReader_params)["UseTaus"]=taus_;
  (*WimpSimReader_params)["UseNC"]=ncs_;
  (*WimpSimReader_params)["Oversampling"]=over_;
  (*WimpSimReader_params)["SensitiveHeight"]=sensHeight_;
  (*WimpSimReader_params)["SensitiveRadius"]=sensRadius_;
  (*WimpSimReader_params)["NEvents"]=number_events_;
  if (WimpHeaderPtr->source_ == "Sun") {
    if (std::isnan(startmjd_)) //this is the read everything configuration
      (*WimpSimReader_params)["StartMJD"]=WimpHeaderPtr->mjd1_;
    else  // this is startmjd_ has been set
      (*WimpSimReader_params)["StartMJD"]=startmjd_;
    if (std::isnan(endmjd_)) //this is the read everything configuration
      (*WimpSimReader_params)["EndMJD"]=WimpHeaderPtr->mjd2_;
    else // this is endmjd_ has been set
      (*WimpSimReader_params)["EndMJD"]=endmjd_;
  } else {
    (*WimpSimReader_params)["StartMJD"]=startmjd_;
    (*WimpSimReader_params)["EndMJD"]=endmjd_;
  }
  frame->Put("File", I3StringPtr(new I3String(filenames_[file_index_])) ); //index-1 steering is not shifted from OpenNextFile()
  frame->Put("Source", I3StringPtr(new I3String(WimpHeaderPtr->source_))); // Put this also in the infoframe as a string

  frame->Put("Neutrino_Osc_Params", osc_params);
  frame->Put("WimpSim_Params", WimpSim_params);
  frame->Put("WimpSimReader_Params", WimpSimReader_params);

  return frame;
}
//_______________________________________________________________________
std::string I3WimpSimReader::HeaderString(const boost::shared_ptr<WimpSimBaseHeader> WimpHeaderPtr) const{
  log_debug("Entering WriteInfo()");

  std::ostringstream outstream;

  outstream << "File::" << filenames_[file_index_]<<endl; //index-1 steering is not shifted from OpenNextFile()

  outstream << "Osciallation Parameters::" << endl;
  outstream << "theta_12 = "<< WimpHeaderPtr->theta_12_<<endl;
  outstream << "theta_13 = "<< WimpHeaderPtr->theta_13_<<endl;
  outstream << "theta_23 = "<< WimpHeaderPtr->theta_23_<<endl;
  outstream << "delta = "<< WimpHeaderPtr->delta_<<endl;
  outstream << "delta_m^2_21 = "<< WimpHeaderPtr->delta_m2_21_<<endl;
  outstream << "delta_m^2_31 = "<< WimpHeaderPtr->delta_m2_31_<<endl;
  outstream << endl;
  outstream << "WimpSim_params::"<<endl;
  outstream << "WIMP_mass = "<<WimpHeaderPtr->mass_<<endl;
  outstream << "Annihilation_Channel = "<<WimpHeaderPtr->channel_<<endl;
  outstream << "Number_of_Annihilations = "<<WimpHeaderPtr->number_content_<<endl;
  outstream << "Total_of Annihilations = "<<WimpHeaderPtr->number_total_<<endl;
  outstream << "Files_index = "<<WimpHeaderPtr->files_index_<<endl;
  outstream << "Files_total = "<<WimpHeaderPtr->files_total_<<endl;
  outstream << "Seed = "<<WimpHeaderPtr->seed_<<endl;
  if (WimpHeaderPtr->source_=="Sun") {
    outstream << "Source = "<<1<<endl;
    outstream << "Detector_Longitude = "<<WimpHeaderPtr->longitude_<<endl;
    outstream << "Detector_Latitude = "<<WimpHeaderPtr->latitude_<<endl;
    outstream << "mjd1 = "<<WimpHeaderPtr->mjd1_<<endl;
    outstream << "mjd2 = "<<WimpHeaderPtr->mjd2_<<endl;
  }
  else if (WimpHeaderPtr->source_=="Earth")
    outstream << "Source = "<<2<<endl;
  outstream << endl;
  outstream << "WimpSimReader_params::"<<endl;
  outstream << "PositionLimitsXmin = "<<position_limits_[0]<<endl;
  outstream << "PositionLimitsXmax = "<<position_limits_[1]<<endl;
  outstream << "PositionLimitsYmin = "<<position_limits_[2]<<endl;
  outstream << "PositionLimitsYmax = "<<position_limits_[3]<<endl;
  outstream << "PositionLimitsZmin = "<<position_limits_[4]<<endl;
  outstream << "PositionLimitsZmax = "<<position_limits_[5]<<endl;
  outstream << "InjectionRadius = "<<radius_<<endl;
  outstream << "LowZenCut = "<<zenith_min_<<endl;
  outstream << "UpperZenCut = "<<zenith_max_<<endl;
  outstream << "UseElectrons = "<<electrons_<<endl;
  outstream << "UseMuons = "<<muons_<<endl;
  outstream << "UseTaus = "<<taus_<<endl;
  outstream << "UseNC = "<<ncs_<<endl;
  outstream << "Oversampling = "<<over_<<endl;
  outstream << "SensitiveHeight = "<<sensHeight_<<endl;
  outstream << "SensitiveRadius = "<<sensRadius_<<endl;
  outstream << "NEvents = "<<number_events_<<endl;
  if (WimpHeaderPtr->source_ == "Sun") {
    if (std::isnan(startmjd_)) //this is the read everything configuration
      outstream << "StartMJD = "<<WimpHeaderPtr->mjd1_<<endl;
    else  // this is startmjd_ has been set
      outstream << "StartMJD = "<<startmjd_<<endl;
    if (std::isnan(endmjd_)) //this is the read everything configuration
      outstream << "EndMJD = "<<WimpHeaderPtr->mjd2_<<endl;
    else // this is endmjd_ has been set
      outstream << "EndMJD = "<<endmjd_<<endl;
  } else {
    outstream << "StartMJD = "<<startmjd_<<endl;
    outstream << "EndMJD = "<<endmjd_<<endl;
  }
  outstream << endl;

  return outstream.str();
}
//_______________________________________________________________________
double I3WimpSimReader::RandomMJD() const {
  return randomService_->Uniform(startmjd_,endmjd_);
}
//_______________________________________________________________________
I3Time I3WimpSimReader::RandomMJDTime() const {
  const double mjd_ = randomService_->Uniform(startmjd_,endmjd_);
  return I3Time(mjd_);
}

//_______________________________________________________________________
I3FramePtr I3WimpSimReader::WriteEvent(const boost::shared_ptr<WimpSimBaseHeader> WimpHeaderPtr,
                                       const boost::shared_ptr<WimpSimBaseEvent> WimpEventPtr,
                                       const double mjd) const
{
  I3FramePtr frame(new I3Frame(pFrameOpt_ ? I3Frame::Physics : I3Frame::DAQ));

  I3Particle primary;
  I3Particle lepton;
  I3Particle hadron;

  I3EventHeaderPtr event_header(new I3EventHeader());
  event_header->SetStartTime(I3Time(mjd)); //TODO constructor
  event_header->SetRunID(file_index_); // NOTE this should in future be corsika conventions: 4 digits dataset + 5 digits filenumber
  event_header->SetSubRunID(0);
  event_header->SetEventID(WimpEventPtr->eventnbr_);
  event_header->SetSubEventID(0);
  event_header->SetState(I3EventHeader::OK);

  // write the Wimp_Params
  I3WimpParamsPtr wimp_params(new I3WimpParams());
  wimp_params->SetSource(WimpHeaderPtr->source_);
  wimp_params->SetMass(WimpHeaderPtr->mass_);
  wimp_params->SetChannel(ConvertToDecayChannel(WimpHeaderPtr->channel_));
  wimp_params->SetNuWeight(WimpEventPtr->nu_weight_);
  wimp_params->SetLepWeight(WimpEventPtr->lep_weight_);
  wimp_params->SetHadWeight(WimpEventPtr->had_weight_);
  wimp_params->SetVgen(NAN);
  std::isnan(WimpEventPtr->GetMJD()) ? wimp_params->SetTime(I3Time()) : wimp_params->SetTime(I3Time(WimpEventPtr->GetMJD()));
  //NOTE Implementation if WimpParams should be written as a I3Map
  I3MapStringDoublePtr wimp_params_map(new I3MapStringDouble());
  (*wimp_params_map)["source"] = SourceStringToInt(WimpHeaderPtr->source_);
  (*wimp_params_map)["mass"] = WimpHeaderPtr->mass_;
  (*wimp_params_map)["channel"] = WimpHeaderPtr->channel_;
  (*wimp_params_map)["nu_weight_"] = WimpEventPtr->nu_weight_;
  (*wimp_params_map)["lep_weight_"] = WimpEventPtr->lep_weight_;
  (*wimp_params_map)["had_weight_"] = WimpEventPtr->had_weight_;
  (*wimp_params_map)["vgen"] = NAN;
  (*wimp_params_map)["time_mjd_"] = WimpEventPtr->GetMJD();

  I3Position vertex = RandomPosition();
  primary.SetPos(vertex);
  lepton.SetPos(vertex);
  hadron.SetPos(vertex);

  primary.SetType(WimpSimToI3ParticleType(WimpEventPtr->nu_type_));
  primary.SetShape(I3Particle::Primary);
  primary.SetLocationType(I3Particle::InIce);
  primary.SetEnergy(WimpEventPtr->nu_energy_*I3Units::GeV); //no correction for restmass
  primary.SetTime(0.*I3Units::ns); //vertex at time t=0
  primary.SetDir(I3Direction(WimpEventPtr->GetIceCubeNu_Zenith(), WimpEventPtr->GetIceCubeNu_Azimuth()));
  primary.SetFitStatus(I3Particle::OK);

  lepton.SetType(WimpSimToI3ParticleType(WimpEventPtr->lep_type_));
  lepton.SetLocationType(I3Particle::InIce);
  lepton.SetEnergy(TotEnergyToKinEnergy(WimpEventPtr->lep_energy_, lepton.GetType()));
  lepton.SetTime(0.*I3Units::ns);
  lepton.SetDir(I3Direction(WimpEventPtr->GetIceCubeLep_Zenith(),WimpEventPtr->GetIceCubeLep_Azimuth()));
  lepton.SetFitStatus(I3Particle::OK);

  hadron.SetType(WimpSimToI3ParticleType(WimpEventPtr->had_type_));
  hadron.SetShape(I3Particle::Cascade);
  hadron.SetLocationType(I3Particle::InIce);
  hadron.SetEnergy(TotEnergyToKinEnergy(WimpEventPtr->had_energy_, hadron.GetType()));
  hadron.SetTime(0.*I3Units::ns);
  hadron.SetDir(I3Direction(WimpEventPtr->GetIceCubeHad_Zenith(), WimpEventPtr->GetIceCubeHad_Azimuth()));
  hadron.SetFitStatus(I3Particle::OK);

  switch (lepton.GetType()) {
    case (I3Particle::EPlus):
    case (I3Particle::EMinus):
      lepton.SetShape(I3Particle::Cascade);
      break;
    case (I3Particle::MuPlus):
    case (I3Particle::MuMinus):
      lepton.SetShape(I3Particle::StartingTrack);
      break;
    case (I3Particle::TauPlus):
    case (I3Particle::TauMinus):
      lepton.SetShape(I3Particle::StartingTrack);
      break;
    case (I3Particle::NuE):
    case (I3Particle::NuEBar):
    case (I3Particle::NuMu):
    case (I3Particle::NuMuBar):
    case (I3Particle::NuTau):
    case (I3Particle::NuTauBar):
      lepton.SetShape(I3Particle::StartingTrack);
      break;
    default:
      log_debug("lepton type which is recognized");
      break;
  }
  log_debug("This was a %s with %f GeV", WimpEventPtr->lep_type_.c_str(), WimpEventPtr->lep_energy_);

  log_debug("Entering the dolepton subroutine");
  // compute the GeneratedBox and overwrite properties
  if (! std::isnan(sensHeight_) && ! std::isnan(sensRadius_)) { //if sensitive hight and radius are configure
    switch (lepton.GetType()) {
      case I3Particle::MuPlus:
      case I3Particle::MuMinus:
      {
        GenBox gen_box;
        if (lepton.IsNeutrino()) // it's a nc event: compute for the hadronic-cascade part
          gen_box = GenerateBox(primary, hadron);
        else // it's a regular lepton event: compute for the leptonic part
          gen_box = GenerateBox(primary, lepton);
        vertex = gen_box.particle_pos;

        if (!flatMapOpt_) {
          wimp_params->SetVgen(gen_box.vgen);
          wimp_params->SetAproj(gen_box.aproj);
        }
        else {
          (*wimp_params_map)["vgen"] = gen_box.vgen;
          (*wimp_params_map)["aproj"] = gen_box.aproj;
        }
        log_debug("dolepton subroutine with vgen %f", gen_box.vgen);
        break;
      }
      case I3Particle::EPlus:
      case I3Particle::EMinus:
      case I3Particle::TauPlus:
      case I3Particle::TauMinus:
      case I3Particle::Hadrons:
        log_debug("No Implementation for generated Length of this ParticleType: %s", lepton.GetTypeString().c_str());
      default:
      {
        log_debug("Will use RandomPosition in InjectionVolume");
        vertex = RandomPosition();
        if (!flatMapOpt_) {
          wimp_params->SetVgen(RandomPositionVolume());
          wimp_params->SetAproj(NAN);
        }
        else {
          (*wimp_params_map)["vgen"] = RandomPositionVolume();
          (*wimp_params_map)["aproj"] = NAN;
        }
        log_debug("dolepton subroutine with vgen %f", RandomPositionVolume());
        break;
      }
    }
  }
  else { //if sensitive hight and radius have not been configured
    vertex = RandomPosition();
    if (!flatMapOpt_) {
      wimp_params->SetVgen(RandomPositionVolume());
      wimp_params->SetAproj(NAN);
    }
    else {
      (*wimp_params_map)["vgen"] = RandomPositionVolume();
      (*wimp_params_map)["aproj"] = NAN;
    }
    log_debug("dolepton subroutine with vgen %f", RandomPositionVolume());
  }
  
  primary.SetPos(vertex);
  lepton.SetPos(vertex);
  hadron.SetPos(vertex);

  //aftermath, so that neutrino is emmited at the sun/earth and propagates to the vertex; Observe that is the Sun's eveident position
  double source_distance;
  if (WimpHeaderPtr->source_ == "SUN") //I3WimpParams::SUN)
    source_distance = 1.496E11*I3Units::m;
  else //Source == "EARTH" //I3WimpParams::EARTH)
    source_distance = 6356752.314*I3Units::m; //Earth Ellipsoid modell GRS 80, WGS84 (1974) Polar-radius
  
  primary.SetPos(primary.GetPos() - source_distance*primary.GetDir() );
  primary.SetTime(primary.GetTime() - source_distance/I3Constants::c); //vertex at time t=0
  primary.SetLength(source_distance);
  primary.SetSpeed(I3Constants::c); //nutrinos travel at lightspeed
    
  
  // --- register all Objects with the frame ---
  log_debug("Put all objects into the frame");

  I3MCTreePtr WIMP_mctree(new I3MCTree);
  I3MCTreeUtils::AddPrimary(*WIMP_mctree, primary);
  I3MCTreeUtils::AppendChild(*WIMP_mctree, primary, lepton);
  I3MCTreeUtils::AppendChild(*WIMP_mctree, primary, hadron);

  frame->Put("I3EventHeader", event_header);
  frame->Put("I3MCTree", WIMP_mctree);
  if (!flatMapOpt_)
    frame->Put("WIMP_params", wimp_params);
  else
    frame->Put("WIMP_params", wimp_params_map);
  if (writeDrivingTimeOpt_) {
    I3TimePtr driving_time(new I3Time(event_header->GetStartTime()));
    frame->Put("DrivingTime", driving_time);
  }
  return frame;
}
//_______________________________________________________________________
I3WimpSimReader::exit_codes I3WimpSimReader::ReadSunEvent(ifstream &wimpfile, boost::shared_ptr<WimpSimSunEvent> &SunEventPtr){
  log_debug("Entering ReadSunEvent()");

  // fileio helpers
  string datastring;
  string tempstring;

  // container variables
  string event; // container for signal flags: EV(new event), #END(end of eventstream)
  string inout; // container for signal flags: I(incomming), O(outgoing)
  string endevent; // container for signal flag: EE(end of event)

  static unsigned int lasteventnbr=0; //last event number read

  //Read data:
  //------------------------------------------------------------------------------
  if (!getline(wimpfile,datastring)) {
    log_error("could not read next line from file: corrupt or EOF");
    return ERROR_EVENTSTREAM;
  }
  istringstream line1(datastring.c_str());
  line1 >> event;

  if (event == "EV") //Good Event
    log_debug("Good event");
  else if (event == "#END") { //Regular end of Datafile reached
    log_debug("End Event stream");
    lasteventnbr=0;
    return END_EVENTSTREAM;
  } else {
    log_error("Bad event; last good read event was number %i", lasteventnbr);
    return ERROR_EVENTSTREAM;
  }
  // read in event information
  line1 >> SunEventPtr->eventnbr_ >> SunEventPtr->mjd_ >> SunEventPtr->sun_az_ >> SunEventPtr->sun_el_ >> SunEventPtr->sun_ra_ >> SunEventPtr->sun_dec_;

  if (SunEventPtr->eventnbr_!=(lasteventnbr+1)) {
    log_error("Event Numbers read are not successive");
    return ERROR_EVENTSTREAM;
  }
  else
    lasteventnbr=SunEventPtr->eventnbr_;

  // read in primary particle
  getline(wimpfile,datastring);
  istringstream line2(datastring.c_str());
  line2 >> inout >> SunEventPtr->nu_type_ >> SunEventPtr->nu_energy_ >> SunEventPtr->nu_angle_ >> SunEventPtr->nu_weight_ >> SunEventPtr->nu_az_ >> SunEventPtr->nu_el_;
  if (inout != "I")
      log_fatal("An Incomming particle was not marked as such");
  if (! IsNeutrino(StringToParticleType(SunEventPtr->nu_type_)))
    log_fatal("neutrino particle has wrong type");


  // read in the first daughter particle
  getline(wimpfile,datastring);
  istringstream line3(datastring.c_str());
  line3 >> inout; // peak into the first word
  // Read the Particle
  if (inout == "EE") {
    if (SunEventPtr->nu_energy_<=1*I3Units::GeV) {
      log_debug("This event is lesser-equal 1GeV and has no daughters");
      return SUCCESS;
    } else {
      log_error("Bad event; tried to read EV,I,EE event; last good read event was number %i", lasteventnbr);
      return ERROR_EVENTSTREAM;
    }
  } else if (inout == "O") { // tried to read EV,I,O,O,EE event
    line3 >> SunEventPtr->lep_type_ >> SunEventPtr->lep_energy_ >> SunEventPtr->lep_angle_ >> SunEventPtr->lep_weight_ >> SunEventPtr->lep_az_ >> SunEventPtr->lep_el_; // read  the remainder of second daughter properties
    if (! IsLepton(StringToParticleType(SunEventPtr->lep_type_)))
      log_fatal("leptonic particle has wrong type");
    // read the second daughter particle
    getline(wimpfile,datastring);
    istringstream line4(datastring.c_str());
    line4 >> inout >> SunEventPtr->had_type_ >> SunEventPtr->had_energy_ >> SunEventPtr->had_angle_ >> SunEventPtr->had_weight_ >> SunEventPtr->had_az_ >> SunEventPtr->had_el_;
    if (inout != "O")
      log_fatal("An Outgoing particle was not marked as such");
    if (StringToParticleType(SunEventPtr->had_type_) != hshow)
      log_fatal("hadronic particle has wrong type");
    getline(wimpfile, datastring);
    istringstream line5(datastring.c_str());
    line5 >> endevent;
    if (endevent != "EE") { // event EV,I,O,O,EE event did not end in EE
      log_error("Bad event; tried to read EV,I,O,O,EE event; last good read event was number %i",lasteventnbr);
      return ERROR_EVENTSTREAM;
    }
  } else {
    log_error("Bad event; tried to read EV,I,EE or EV,I,O,O,EE event; last good read event was number %i", lasteventnbr);
  }
  log_debug("Event %i successfully read", lasteventnbr);
  return SUCCESS;
}
//_______________________________________________________________________
I3WimpSimReader::exit_codes I3WimpSimReader::ReadEarthEvent(ifstream &wimpfile, boost::shared_ptr<WimpSimEarthEvent> &EarthEventPtr) {
  log_debug("Entering ReadEarthEvent()");

  // fileio helpers
  string datastring;
  string tempstring;

  // container variables
  string event; // container for signal flags: EV(new event), #END(end of eventstream)
  string inout; // container for signal flags: I(incomming), O(outgoing)
  string endevent; // container for signal flag: EE(end of event)

  static unsigned int lasteventnbr = 0; //last event number read; incremented each call

  //Read data:
  //------------------------------------------------------------------------------
  if (!getline(wimpfile,datastring)) {
    log_error("could not read next line from file: corrupt or EOF");
    return ERROR_EVENTSTREAM;
  }
  istringstream line1(datastring.c_str());
  line1 >> event;

  if (event == "EV") // Good Event
    log_debug("Good event");
  else if (event == "#END") { //Regular end of Datafile reached
    log_debug("End Event stream");
    lasteventnbr = 0;
    return END_EVENTSTREAM;
  }
  else {
    log_debug("Bad event; last good read event was %i",  lasteventnbr);
    return ERROR_EVENTSTREAM;
  }
  // read in event information
  line1 >> EarthEventPtr->eventnbr_;

  if (EarthEventPtr->eventnbr_!=(lasteventnbr+1)) {
    log_error("Event Numbers read are not successive");
    return ERROR_EVENTSTREAM;
  }
  else
    lasteventnbr=EarthEventPtr->eventnbr_;

    // read in primary particle
  getline(wimpfile,datastring);
  istringstream line2(datastring.c_str());
  line2 >> inout >> EarthEventPtr->nu_type_ >> EarthEventPtr->nu_energy_ >> EarthEventPtr->nu_nadir_ >> EarthEventPtr->nu_az_ >> EarthEventPtr->nu_weight_;
  if (! IsNeutrino(StringToParticleType(EarthEventPtr->nu_type_)))
    log_fatal("neutrino particle has wrong type");

  // read in the first daughter particle
  getline(wimpfile,datastring);
  istringstream line3(datastring.c_str());
  line3 >> inout;
  // Read the Particle
  if (inout == "EE") {
    if (EarthEventPtr->nu_energy_<=1*I3Units::GeV) {
      log_trace("This event is under 1GeV and has no daughters");
      return SUCCESS;
    }
    else {
      log_error("Bad event; tried to read EV,I,EE event; last good read event was number %i", lasteventnbr);
      return ERROR_EVENTSTREAM;
    }
  } else if (inout == "O") { // tried to read EV,I,O,O,EE event
    line3 >> EarthEventPtr->lep_type_ >> EarthEventPtr->lep_energy_ >> EarthEventPtr->lep_nadir_ >> EarthEventPtr->lep_az_ >> EarthEventPtr->lep_weight_;
    if (! IsLepton(StringToParticleType(EarthEventPtr->lep_type_)))
      log_fatal("leptonic particle has wrong type");
    getline(wimpfile, datastring);
    istringstream line4(datastring.c_str());
    line4 >> inout >> EarthEventPtr->had_type_ >> EarthEventPtr->had_energy_ >> EarthEventPtr->had_nadir_ >> EarthEventPtr->had_az_ >> EarthEventPtr->had_weight_;
    if (StringToParticleType(EarthEventPtr->had_type_) != hshow)
      log_fatal("hadronic particle has wrong type");
    getline(wimpfile, datastring);
    istringstream line5(datastring.c_str());
    line5 >> endevent;

    if (endevent != "EE") { // event EV,I,O,O,EE event did not end in EE
      log_error("Bad event; tried to read EV,I,O,O,EE event; last good read event was number %i", lasteventnbr);
      return ERROR_EVENTSTREAM;
    }
  } else {
    log_debug("Bad event; tried to read EV,I,EE or EV,I,O,O,EE event; last good read event was number %i", lasteventnbr);
  }
  log_debug("Event %i successfully read", lasteventnbr);
  return SUCCESS;
}
//_______________________________________________________________________
I3WimpSimReader::exit_codes I3WimpSimReader::CutsNWeights(const boost::shared_ptr<WimpSimBaseEvent> WimpEventPtr, const double mjd) {
  log_debug("Entering CutsNWeights()");
  // === Sum up the weight and prepare Variables ===
  tot_weight_.nu_+=WimpEventPtr->nu_weight_;
  tot_events_.nu_++;
  
  // === Apply cuts ===
  if (WimpEventPtr->nu_energy_<=1.*I3Units::GeV) {
    log_debug("This event is lesser-equal 1GeV and has no daughters");
    tot_weight_.lowe_+=WimpEventPtr->nu_weight_;
    tot_events_.lowe_+=1; //over_;
    return FAILED_ENERGYCUTS;
  }
  
  tot_weight_.lep_+=WimpEventPtr->lep_weight_;
  tot_events_.lep_++;
  tot_weight_.had_+=WimpEventPtr->had_weight_;
  tot_events_.had_++;

  bool dolepton = false;
  switch (StringToParticleType(WimpEventPtr->lep_type_)) {
    case (e_plus):
    case (e_minus): {
      tot_weight_.e_+=WimpEventPtr->lep_weight_;
      tot_events_.e_++;
      if (electrons_)
        dolepton = true;
      break;
    }
    case (mu_plus):
    case (mu_minus):{
      tot_weight_.mu_+=WimpEventPtr->lep_weight_;
      tot_events_.mu_++;
      if (muons_)
        dolepton = true;
      break;
    }
    case (tau_plus):
    case (tau_minus):{
      tot_weight_.tau_+=WimpEventPtr->lep_weight_;
      tot_events_.tau_++;
      if (taus_)
        dolepton = true;
      break;
    }
    case (nu_e):
    case (nu_e_bar):
    case (nu_mu):
    case (nu_mu_bar):
    case (nu_tau):
    case (nu_tau_bar):{
      tot_weight_.nc_+=WimpEventPtr->lep_weight_;
      tot_events_.nc_++;
      if (ncs_)
        dolepton = true;
      break;
    }
    default: {
      log_debug("no lepton that was recognized; something went wrong or field is empty");
      dolepton = false;
      break;
    }
  }

  // === Apply cuts ===
  if (! dolepton) {
    log_debug("This type of event was not specified : (secondary lepton) %s", WimpEventPtr->lep_type_.c_str());
    return FAILED_LEPTONCUTS;
  }

  if (! std::isnan(mjd)) {
    if (mjd < startmjd_ || endmjd_ < mjd) {
      log_debug("Event is outside specified timewindow");
      return FAILED_TIMECUTS;
    }
  }

  const double sun_zenith = WimpEventPtr->GetIceCubeOrigin_Zenith();
  if ((sun_zenith*I3Units::radian > zenith_max_*I3Units::radian) || (sun_zenith*I3Units::radian < zenith_min_*I3Units::radian)) {
    log_debug("Event is outside specified zenith cuts");
    return FAILED_ZENITHCUTS;
  }

  // === Passed all Cuts, sum up the weight
  col_weight_.nu_+=WimpEventPtr->nu_weight_;
  col_events_.nu_++;
  col_weight_.lep_+=WimpEventPtr->lep_weight_;
  col_events_.lep_++;
  col_weight_.had_+=WimpEventPtr->had_weight_;
  col_events_.had_++;

  switch (StringToParticleType(WimpEventPtr->lep_type_)) {
    case e_plus:
    case (e_minus): {
      col_weight_.e_+=WimpEventPtr->lep_weight_;
      col_events_.e_++;
      break;
    }
    case (mu_plus):
    case (mu_minus):{
      col_weight_.mu_+=WimpEventPtr->lep_weight_;
      col_events_.mu_++;
      break;
    }
    case (tau_plus):
    case (tau_minus):{
      col_weight_.tau_+=WimpEventPtr->lep_weight_;
      col_events_.tau_++;
      break;
    }
    case (nu_e):
    case (nu_e_bar):
    case (nu_mu):
    case (nu_mu_bar):
    case (nu_tau):
    case (nu_tau_bar):{
      col_weight_.nc_+=WimpEventPtr->lep_weight_;
      col_events_.nc_++;
      break;
    }
    default: {
      log_error("no lepton that was recognized; something went wrong");
      break;
    }
  }
  // everythin went fine
  return SUCCESS;
}

//_______________________________________________________________________
void I3WimpSimReader::WriteInfoToFile (const std::string &filename) const {
  std::ofstream ofs(filename.c_str());
  ofs << ReportString();
  // TODO include also the Header info somehow : HeaderString()
  ofs.close();
}
