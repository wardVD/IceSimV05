/**
 * copyright  (C) 2007
 * the icecube collaboration
 * $Id: I3XMLSummaryService.cxx 136718 2015-08-21 20:23:49Z nega $
 *
 * @version $Revision: 136718 $
 * @date $Date: 2015-08-21 15:23:49 -0500 (Fri, 21 Aug 2015) $
 * @author juancarlos
 *
 * @todo 
 */
//#include <iostream>

#include <fstream>

#include <phys-services/I3XMLSummaryService.h>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

/**
 * @brief This class provides an interface to write job statistics
 * by icetray modules.
 *
 */

using namespace boost::archive;

std::istream& operator >>(std::istream &is, I3MapStringDouble& i3obj)
{
	boost::shared_ptr<I3MapStringDouble> spt;
	try { 
			boost::archive::xml_iarchive xia(is);
			xia >> BOOST_SERIALIZATION_NVP(i3obj);
	} catch (const boost::archive::archive_exception& e) { 
			log_fatal("Archive error: %s ", e.what() ); 
	} 
	return is;
}

std::istream& operator >>(std::istream &is, I3MapStringDoublePtr i3objptr)
{
		is >> *i3objptr;
		return is;
}


std::ostream& operator <<(std::ostream &os, const I3MapStringDouble& i3obj)
{
	try { 
			const I3MapStringDouble *tobj_p = &i3obj;
			boost::archive::xml_oarchive xoa(os);
			xoa << make_nvp("I3XMLSummaryService", tobj_p); 
	} catch (const boost::archive::archive_exception& e) { 
			log_fatal("Archive error: %s ", e.what() ); 
	} 
	return os;
}

std::ostream& operator <<(std::ostream &os, const I3MapStringDoublePtr i3objptr)
{
	os << *i3objptr;
	return os;
}

I3XMLSummaryService::I3XMLSummaryService(std::string filename)
{
		filename_ = filename;
}

I3XMLSummaryService::I3XMLSummaryService():
		filename_("i3summary.xml")
{
    	start_real_ = time(NULL);
    	fail_ = (getrusage(RUSAGE_SELF, &start_) == -1);
}

void
I3XMLSummaryService::SetPath(std::string filename) {
		filename_= filename;
}

/**
 * Access dictionary entry
 */
double& I3XMLSummaryService::operator[] (const std::string& key)
{
	return valuemap_[key];
}


/**
 * Add a value to the value summary map
 */
void 
I3XMLSummaryService::SetValue(std::string name,double value) 
{
  valuemap_[name]= value;
}

/**
 * Get value from the value summary map
 */
double
I3XMLSummaryService::GetValue(std::string name)
{
  return valuemap_[name];
}


void
I3XMLSummaryService::WriteXML(std::ostream& oss)
{ 
	oss << valuemap_; 
}

void
I3XMLSummaryService::WriteXML(std::string outfile)
{ 
	std::ofstream oss(outfile.c_str(), std::ios::out);
	log_trace("Writting: map to %s", outfile.c_str() ); 
	oss << valuemap_; 
}

void
I3XMLSummaryService::WriteXML()
{ 
	WriteXML(filename_);
}

void
I3XMLSummaryService::ReadXML(std::istream& iss)
{ 
	iss >> valuemap_; 
}

void
I3XMLSummaryService::ReadXML(std::string infile)
{ 
	std::ifstream iss(infile.c_str(), std::ios::in);
	iss >> valuemap_; 
}

I3XMLSummaryService::~I3XMLSummaryService()
{
    // Compute CPU and real time
    if (getrusage(RUSAGE_SELF, &stop_) != -1 && !fail_) { 
		double user = (stop_.ru_utime.tv_sec - start_.ru_utime.tv_sec); 
		user += double(stop_.ru_utime.tv_usec - start_.ru_utime.tv_usec) / 10E+06; 
		valuemap_["user_time"] += user;
		
		double sys = (stop_.ru_stime.tv_sec - start_.ru_stime.tv_sec); 
		sys += double(stop_.ru_stime.tv_usec - start_.ru_stime.tv_usec) / 10E+06; 
		valuemap_["sys_time"] += sys;

        // integral shared text memory size 
        valuemap_["shared memory"]+=stop_.ru_ixrss;

        // ru_idrss integral unshared data size 
        // ru_isrss integral unshared stack size 
        valuemap_["unshared memory"]+=stop_.ru_idrss+stop_.ru_isrss;

        // swaps
        valuemap_["memory swaps"] += stop_.ru_nswap;

        // context switches
        valuemap_["context switches"] += stop_.ru_nvcsw + stop_.ru_nivcsw;
	}
   	stop_real_ = time(NULL);
	valuemap_["real_time"] += difftime(stop_real_, start_real_);

	// print the xml
	WriteXML();
}
