/**
 * copyright  (C) 2007
 * the icecube collaboration
 * $Id$
 *
 * @version $Revision$
 * @date $Date$
 * @author juancarlos@icecube.wisc.edu
 *
 */

#ifndef I3XMLSUMMARYSERVICE_H
#define I3XMLSUMMARYSERVICE_H

#include <phys-services/I3SummaryService.h>

#include <dataclasses/I3Map.h>
#include <iosfwd>
#include <sys/time.h>
#include <sys/resource.h>

/**
 * @brief This class provides an XML implementation of the I3SummaryService. 
 * It allows collection of job statistics that can be written to xml for other programs 
 * to parse and collect (e.g. sim-prod).
 *
 */

class I3XMLSummaryService: public I3SummaryService {
 public:
  /**
   * constructors
   */
  I3XMLSummaryService();

  I3XMLSummaryService(std::string filename);

  /**
   * destructor
   */ 
  virtual ~I3XMLSummaryService();


  /**
   * Set filename to save serialized service to
   */
  void SetPath(std::string filename);

  /**
   * Access dictionary entry
   */
  double& operator[] (const std::string& key);

  /**
   * Add a value to the value summary map
   */
  void SetValue(std::string name,double value);

  /**
   * Get value from the value summary map
   */
  double GetValue(std::string name);

  /**
   * Write an I3XMLSummaryService map to a stream
   */
  void WriteXML(std::ostream& oss);

  /**
   * Write an I3XMLSummaryService map to a file
   */
  void WriteXML(std::string outfile);

  /**
   * Write an I3XMLSummaryService map to file
   */
  void WriteXML();

  /**
   * Load an I3XMLSummaryService map from an iostream
   */
  void ReadXML(std::istream& iss);

  /**
   * Load an I3XMLSummaryService map from a file
   */
  void ReadXML(std::string infile);

 private:

  I3Map<std::string,double> valuemap_;
  std::string filename_;

  time_t start_real_,stop_real_;
  struct rusage stop_, start_;
  bool fail_;

  // copy and assignment private
  I3XMLSummaryService(const I3XMLSummaryService&);
  const I3XMLSummaryService& operator=(const I3XMLSummaryService&);

  SET_LOGGER("I3XMLSummaryService");

};

I3_DEFAULT_NAME(I3XMLSummaryService);
I3_POINTER_TYPEDEFS(I3XMLSummaryService);

#endif //I3XMLSUMMARYSERVICE_H
