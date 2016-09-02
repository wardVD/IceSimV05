#ifndef DIPLOPIA_COINCIDENT_I3READER_SERVICE_H_INLCUDED
#define DIPLOPIA_COINCIDENT_I3READER_SERVICE_H_INLCUDED

#include <icetray/I3TrayHeaders.h>
#include <icetray/I3Logging.h>
#include <sim-services/I3GeneratorService.h>
#include <diplopia/DiplopiaUtils.h>
#include "dataclasses/physics/I3MCTree.h"
#include "dataclasses/physics/I3MCTreeUtils.h"
#include "dataclasses/I3Double.h"
#include "dataclasses/I3String.h"

#include <icetray/I3Frame.h>
#include <icetray/open.h>
#include <fstream>
#include <boost/iostreams/filtering_stream.hpp>

#include <cmath>
#include <assert.h>

/**
 * copyright  (C) 2006
 * The IceCube Collaboration
 *
 * @date $Date:$
 * @author Juan Carlos Diaz Velez
 *                                                                       
 * @brief Utilities for merging MCTrees MCInfo and MCHitSeriesMaps in separate 
 *	events to produce coincident in-ice events.
 *
 */

class CoincidentI3ReaderService: public I3GeneratorService
{
  public:
  
    CoincidentI3ReaderService() { };
    CoincidentI3ReaderService(const CoincidentI3ReaderService&);
    CoincidentI3ReaderService(const std::string& filename);
    ~CoincidentI3ReaderService();

    virtual I3MCTreePtr GetNextEvent();
    virtual I3FramePtr GetNextFrame();

    virtual double GetRate();

    int Init();

    bool Open(std::string path);

  private:
  
    DiplopiaUtils::FileReader reader_;
    std::string path_;
    double rate_;
    bool configured_;

};

I3_POINTER_TYPEDEFS(CoincidentI3ReaderService);

#endif //DIPLOPIA_COINCIDENT_I3READER_SERVICE_H_INLCUDED

