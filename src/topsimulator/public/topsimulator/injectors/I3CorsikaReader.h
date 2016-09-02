/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3CorsikaReader.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3CORSIKAREADER_H_
#define _TOPSIMULATOR_I3CORSIKAREADER_H_


#include <dataclasses/physics/I3Particle.h>
#include <topsimulator/injectors/CorsikaSubBlock.h>

#include <simclasses/I3CorsikaShowerInfo.h>
#include <simclasses/CorsikaLongStep.h>

#include <string>
#include <fstream>


/**
 * \brief CORSIKA reader based on the original CORSIKA reader in the 
 * corsikaXX project written by Ralf Ulrich and Peter Niessen.
 */

class I3CorsikaReader
{
public:
    I3CorsikaReader();
    ~I3CorsikaReader();
    
    void SetFileList(const std::vector<std::string>& fileNameList);
    
    bool NextRun(int& runID);
    
    bool NextEvent(I3Particle& primary, int& evtID);
    
    bool NextParticle(I3Particle& particle, double& weight, const std::vector<int>& ignoretypes = std::vector<int>());
    bool NextParticle(I3Particle& particle, double& weight, double& hadgen, const std::vector<int>& ignoretypes = std::vector<int>());
    
    I3CorsikaShowerInfoPtr GetShowerInfo() {return showerInfo_;};
    
    void Rewind();
    
    bool IsThinned() {return isThinned_;};
    
    // KR adds this so that, after the observation level is set, it can be "hacked" upwards.
    // Not to be used for anything other than fixing problems after-the-fact!
    // Call this immediately after I3CorsikaReader::NextRun() (before reading in any particles!)
    void HackObsLevelUpwards(double extraz=0) { 
      zObsLevel_ += extraz; 
      if (extraz!=0) log_info("...raising the obs. level by %f meters (to %f)", extraz, zObsLevel_);
    }
    
private:
    
    int32_t CorsikaToPDG(int corsika_id);
    bool OpenFile(const std::string& filename);
    void CloseFile();
    
    bool NextSubBlock();
    
    void FillLongProfile(const CorsikaSubBlock& longBlock);
    
    
    bool isThinned_;
    bool isCurved_;
    int particleDataSize_;
    
    int currentRunNumber_;
    int currentEvtNumber_;
    int currentPartIndex_;
    
    std::ios::pos_type lastEvtBlockPtr_;
    int lastEvtSubBlockCounter_;
    
    int eventCounter_;
    int64_t particleCounter_;
#ifndef NDEBUG
    time_t startTime_;
    time_t endTime_;
#endif
    int obsLevel_;
    double zObsLevel_;
    
    
    std::vector<std::string> fileNameList_;
    int fileIndex_;
    
    
    std::ifstream corsFile_;
    CorsikaSubBlock subBlock_;
    
    I3CorsikaShowerInfoPtr showerInfo_;
    
    SET_LOGGER("I3CorsikaReader")
};

#endif
