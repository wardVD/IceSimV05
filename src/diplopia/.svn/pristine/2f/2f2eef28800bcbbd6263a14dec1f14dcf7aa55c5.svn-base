#include "diplopia/DiplopiaUtils.h"


  DiplopiaUtils::FileReader::~FileReader()
  {
    close();
  }

  DiplopiaUtils::FileReader::FileReader(const std::string& filename)
    : path_(filename) 
  {
    open_file(filename);
  }

  void
  DiplopiaUtils::FileReader::close()
  {
    ifs_.reset();
  }

  int
  DiplopiaUtils::FileReader::open_file(const std::string& filename)
  {
    log_info("Opening file %s", filename.c_str());
    I3::dataio::open(ifs_, filename);
    cache_.clear();
    return 0;
  }

  I3FramePtr
  DiplopiaUtils::FileReader::pop_frame(I3Frame::Stream s)
  {
    if(ifs_.peek() == EOF)
      return I3FramePtr();
    I3FramePtr f(new I3Frame);
    while(more()) {
      try {
        f->load(ifs_);
        cache_.purge(f->GetStop());
        f->purge();
        f->merge(cache_);
        // Act like I3Module: merge all keys except TrayInfo and Physics
        if (f->GetStop() != I3Frame::TrayInfo &&
            f->GetStop() != I3Frame::Physics)
                cache_.merge(*f);

        nframe_++;
        if (s == I3Frame::None || f->GetStop() == s)
          return f;
        else
          f->clear();
      } catch (const std::exception& e) {
        log_fatal("caught exception %s", e.what());
      }
    }
    return I3FramePtr();
  }

  bool
  DiplopiaUtils::FileReader::more()
  {
    return ifs_.peek() != EOF;
  }

