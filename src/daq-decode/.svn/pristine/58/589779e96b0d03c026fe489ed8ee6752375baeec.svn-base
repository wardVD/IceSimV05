/**
 * copyright  (C) 2004
 * the icecube collaboration
 * $Id$
 *
 * @file I3DAQFile.h
 * @brief definition of the I3DAQFile class
 * @version $Revision$
 * @date $Date$
 * @author tschmidt
 */
#ifndef I3DAQFILE_H_INCLUDED
#define I3DAQFILE_H_INCLUDED

// forward declarations


// header files

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <icetray/I3Logging.h>
#include <icetray/I3FrameObject.h>

// definitions


/**
 * @brief Wrapper class to read data from binary I3 DAQ files.
 */
class I3DAQFile
{
 public:
  /**
   * @brief Default constructor.
   */
  I3DAQFile();
  /**
   * @brief Constructor.
   * 
   * It will open a binary I3 DAQ file.
   * @param name Name of the binary I3 DAQ file.
   */
  explicit I3DAQFile(const std::string& name);
  /**
   * @brief Destructor.
   */
  virtual ~I3DAQFile();
  /**
   * @brief Opens a binary I3 DAQ file.
   * 
   * @param name Name of the binary I3 DAQ file.
   */  
  void Open(const std::string& name);
  /**
   * @brief Returns the name of the current binary I3 DAQ file.
   * 
   * @return The name.
   */
  const std::string& GetName() const;
  /**
   * @brief Returns the header data of the current binary I3 DAQ file.
   * 
   * @return The header data.
   */
  const std::vector<char>& GetHeader() const;
  const std::vector<char>& GetHeader();
  /**
   * @brief Returns the next event data.
   * 
   * @param buffer The event data.
   * @return False, if there is no event left in the current binary I3 DAQ file.
   */
  bool GetEvent(std::vector<char>& buffer);

 private:
  std::string name_;
  boost::shared_ptr<boost::iostreams::filtering_istream> ifs_;
  bool valid_;
  std::vector<char> dummy_;
  
  
  void Open();

  // private copy constructors and assignment  
  I3DAQFile(const I3DAQFile&);
  I3DAQFile& operator=(const I3DAQFile&);


  // logging
  SET_LOGGER ("I3DAQFile");
};

/**
 * pointer type to insulate users from memory management
 */
I3_POINTER_TYPEDEFS(I3DAQFile);

#endif // I3DAQFILE_H_INCLUDED
