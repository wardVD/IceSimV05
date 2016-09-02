/**
 * copyright  (C) 2012
 * the IceCube Collaboration
 * $Id: $
 *
 * @file I3WimpSimReader.h
 * @date $Date: 2012-12-20$
 * @author mzoll <marcel.zoll@fysik.su.se>
 */

#ifndef I3WIMPSIMREADER_H
#define I3WIMPSIMREADER_H

#include "simclasses/I3WimpParams.h"
#include "wimpsim-reader/WimpSimTools.h"

#include <string>
#include <vector>
#include <fstream>
#include "icetray/I3Module.h"
#include "icetray/I3Frame.h"
//#include "dataclasses/physics/I3EventHeader.h"
#include "dataclasses/physics/I3Particle.h"
#include "dataclasses/I3Position.h"
#include "dataclasses/I3Direction.h"
#include "dataclasses/I3Time.h"
#include "phys-services/I3RandomService.h"

#include "boost/shared_ptr.hpp"

/**
 * A Generator Module for that reads WimpSimFiles
 */
class I3WimpSimReader: public I3Module{
public: //interaction with the tray
  /// Constructor with arguments passed
  I3WimpSimReader(const I3Context& context);
  /// std Destructor
  ~I3WimpSimReader();
  /// std Configure (configure from context, check correct params)
  void Configure();
  /// std Process (the action goes down here)
  void Process();
  /// std Finish (close files, report)
  void Finish();

private: //shield private instances
  /// turn your logging on: 'I3WimpSimReader'
  SET_LOGGER("I3WimpSimReader");
  /// The RandomService installed in the tray: can also be passed as an OPTION
  I3RandomServicePtr randomService_;

protected: // should be exposed for interfacing to unit-test modules
  // Options passed
  /// OPTION: the name of the random-number generator
  std::string randomServiceName_;
  /// OPTION: the filename mask with wildcards and stuff
  std::vector< std::string > filenames_;
  /// OPTION: N oversampling
  uint over_;
  /// OPTION: number of events to process
  uint number_events_;
  /// OPTION: list of position limits
  std::vector<double> position_limits_;
  /// OPTION: radius of effVol
  double radius_;
  /// OPTION: optional lower zenith cut
  double zenith_min_;
  /// OPTION: optional upper zenith cut
  double zenith_max_;
  /// OPTION: Read and simulate electron vertices
  bool electrons_;
  /// OPTION: Read and simulate muon vertices
  bool muons_;
  /// OPTION: Read and simulate tau vertices
  bool taus_;
  /// OPTION: Read and simulate tau vertices
  bool ncs_;
  /// OPTION: Muon box activated height
  double sensHeight_;
  /// OPTION: Muon box activated Radius
  double sensRadius_;
  /// OPTION: MJD from which to start simulating
  double startmjd_;
  /// OPTION: MJD towards which to cease simulating
  double endmjd_;
  // hardcoded arguments
  /// OPTION: write Info about weights into an IceTray.Info frame
  bool writeInfoOpt_;
  /// OPTION: Write Information to this text-file
  std::string infoFileName_;
  /// OPTION: write DrivingTime to the frame (needed for correct processing 2011)
  bool writeDrivingTimeOpt_;
  /// OPTION: write P-Frames instead of Q-Frames, this is for backwards-compatibility
  bool pFrameOpt_;
  /// OPTION: write a flat I3StringDoubleMap for the WIMP
  bool flatMapOpt_;
  /// OPTION: fall back value if a dynamic length can be calculated
  double fixGenLength_;

protected: // helpers/tracers
  /// the current index in file_vector_
  int file_index_;
  /// the currently opened file
  std::ifstream wimpfile_;

protected: //bookkeeping
  /// number of files that have been successfully read
  uint issued_files_;
  /// number of events that have been generated
  uint issued_events_;
  /// number of processed events
  uint processed_events_;

  /// a Bookkepping class to encapsulate different cases
  struct BookKeeper{
    /// total/collected neutrino weight/events in the read file
    double nu_;
    /// total/collected lepton weight/events in the read file
    double lep_;
    /// total/collected electron weight/events in the read file
    double e_;
    /// total/collected muon weight/events in the read file
    double mu_;
    /// total/collected tau weight/events in the read file
    double tau_;
    /// total/collected neutral current weight/events in the read file
    double nc_;
    /// total/collected hadronic weight/events in the read file
    double had_;
    /// total/collected low-energetic weight/events in the read file
    double lowe_;
    ///constructor
    BookKeeper(): nu_(0), lep_(0), e_(0), mu_(0), tau_(0), nc_(0), had_(0), lowe_(0) {};
  };

  /// collected weights
  BookKeeper col_weight_;
  /// collected events
  BookKeeper col_events_;
  ///total weights
  BookKeeper tot_weight_;
  ///total events
  BookKeeper tot_events_;

protected: //internals
  ///exit codes
  enum exit_codes{
    ERROR = -1, //general errors
    SUCCESS = 0, // everthing went fine
    FAILED_ENERGYCUTS = 1,
    FAILED_LEPTONCUTS = 2,
    FAILED_TIMECUTS = 3,
    FAILED_ZENITHCUTS = 4,
    END_EVENTSTREAM = 10, //regular end
    ERROR_EVENTSTREAM = 20, //complications
  };

private: // methods
  /** @brief Move through the vector of file names and open the next file in line
    * @return true upon success,
    *	      false if not
    */
  bool OpenNextFile();

  /** @brief Reads Headers from the WimpSim-file
    * @param wimpfile : A already opened file that contains WIMP data
    * @param WimpHeaderPtr pointer to the object to which to write the FileHeader
    * @return true if header was successfully read,
    *	      false if not
    */
  bool ReadFileHeader(std::ifstream &wimpfile, boost::shared_ptr<WimpSimTools::WimpSimBaseHeader> &WimpHeaderPtr);

  /** @brief Calculate a position inside position_limits (with ranomization)
    * @return the randomized position.
    */
  I3Position RandomPosition() const;

  /** @brief Calculate the constant Injection Volume as a Box or a Cylinder
    * @return a static return of the volume of either a box or a cylinder
    */
  double VolumeBoxOrCylinder() const;

  /** @brief Calculate the Volume of the Injection Volume
    * @return the volume in [m3].
    */
  double RandomPositionVolume() const;

  /// a struct to contain a about the GeneratedBox and the 'Generated Volume'.
  struct GenBox {
    /// position of particle in real IceCube coordinate system
    I3Position particle_pos;
    /// relative position of particle inside the box eigensystem
    I3Position box_pos;
    /// direction of the long-side of the box
    I3Direction box_vector;
    /// length of the long-side of the box
    double box_length;
    /// length of the horizontal-side of the box
    double box_horizontal;
    /// length of the vertical-side of the box
    double box_vertical;
    /// generated volume enclosed by the box: V_eff_mu
    double vgen;
    /// projected faceside area for the neutrino: A_eff_nu
    double aproj;
  };

  /** @brief Calculate sensitive box around muon track and effective Volume
    * @param neutrino the neutrino to calculate the effective area for
    * @param lepton the lepton to calculate the muonbox for
    * @return MuonBox containing the calculated variables
    */
  GenBox GenerateBox (const I3Particle& neutrino, const I3Particle& lepton) const;

  /** @brief Calculate Length of mu track in ice from second order parametrization
    * @param energy : muon energy
    * @return length of muon track (in meter)
    */
  double Length_mu (const double energy) const;

  /** @brief Calculate Length of electron cascade in ice
    * @param energy : electron energy
    * @return length of electron cascade (in meter)
    */
  double Length_elmag (const double energy) const;

  /** @brief Calculate Length of tau track in ice
    * @param energy : tau energy
    * @return length of tau track (contained) (in meter)
    */
  double Length_tau (const double energy) const;

  /** @brief Calculate Length of hadronic cascades in ice
    * @param energy : hadronic energy
    * @return length of hadronic cascades (contained) (in meter)
    */
  double Length_had (const double energy) const;

  /** @brief Writes Info-Frames containing basic steering info from the .dat file
    * @param WimpHeaderPtr pointer to the container with the information from the file header
    * @return pointer to the heads info frame
    */
  I3FramePtr WriteInfoFrame(const boost::shared_ptr<WimpSimTools::WimpSimBaseHeader> WimpHeaderPtr) const;

  /** @brief A function that report on the processed events and weights collected and writes them to a Info-frame and pushes it to the Outbox
    * @return pointer to the last info frame
    */
  I3FramePtr WriteFinishFrame() const;

  /** @brief A function that report on the processed events and weights collected
    */
  void Report() const;

  /** @brief Compute a random MJD between starmjd_ and endmjd_
    * @return MJD randomized
    */
  double RandomMJD () const;

  /** @brief Compute a random MJD between starmjd_ and endmjd_
    * @return MJD randomized
    */
  I3Time RandomMJDTime () const;

  /** @brief rotate the box
    * which was previously defined in the Cartesian-coordinate eigenvectors to the eigenvectors of the particle
    * @param pos : the position
    * @param dir : the rotation expressed in 3 space
    * @return position in IceCube
    */
  I3Position RotateZenAzi (const I3Position& pos, const I3Direction& dir) const;

  /** @brief write the event to a frame
    * @param WimpHeaderPtr pointer to the FileHeader Information Container
    * @param WimpEventPtr pointer to the Event Information container
    * @param mjd time which to write to I3EventHeader and DrivingTime
    * @return the frame to which the WimpEvent has been written
    */
  I3FramePtr WriteEvent(const boost::shared_ptr<WimpSimTools::WimpSimBaseHeader> WimpHeaderPtr, const boost::shared_ptr<WimpSimTools::WimpSimBaseEvent> WimpEventPtr, const double mjd) const;

  /** @brief write the Sun type event to the container
    * @param wimpfile the file-object to read event from (progressive-reading)
    * @param SunEventPtr pointer to the object to which to write the event
    * @return WimpSimReader internal exit_codes
    */
  exit_codes ReadSunEvent(std::ifstream &wimpfile, boost::shared_ptr<WimpSimTools::WimpSimSunEvent> &SunEventPtr);

  /** @brief write the Earth type event to the container
    * @param wimpfile the file-object to read event from (progressive-reading)
    * @param EarthEventPtr pointer to the object to which to write the event
    * @return WimpSimReader internal exit_codes
    */
  exit_codes ReadEarthEvent(std::ifstream &wimpfile, boost::shared_ptr<WimpSimTools::WimpSimEarthEvent> &EarthEventPtr);

  /** @brief Evaluate Cuts and Weights of this event
    * @param WimpEventPtr pointer to the object from which the decision and values should be taken
    * @param mjd the modjulian day that should be used
    * @return WimpSimReader internal exit_codes
    */
  exit_codes CutsNWeights(const boost::shared_ptr<WimpSimTools::WimpSimBaseEvent> WimpEventPtr, const double mjd);

  //====================== *** NEW *** ==================
  ///write file header info to a string
  std::string HeaderString(const boost::shared_ptr<WimpSimTools::WimpSimBaseHeader> WimpHeaderPtr) const;
  ///write module state information
  std::string ReportString() const;
  ///write all info to a file
  void WriteInfoToFile (const std::string &filename) const;
};

//Register the Module with the tray
I3_MODULE(I3WimpSimReader);

#endif
