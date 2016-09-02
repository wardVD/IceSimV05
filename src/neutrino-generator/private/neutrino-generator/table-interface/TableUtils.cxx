/**
 *   Copyright  (C) 2006
 *   The IceCube collaboration
 *   $Id: $
 *   @version $Revision: $
 *   @date    $Date:     $
 *   @author Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *   @brief reads cross section table list
 */

#include "neutrino-generator/table-interface/TableUtils.h"
#include <boost/filesystem.hpp>

using namespace std;
namespace fs = boost::filesystem;

namespace nugen {

std::string
guess_table_path(const std::string &tablepath)
{
    if (tablepath != "")
        return tablepath;

    std::string path;
    if (getenv("I3_DATA") != NULL) {
        path = std::string(getenv("I3_DATA"))
            + std::string("/neutrino-generator/cross_section_data/");
        if (fs::exists(path))
            return path;
    }
    if (getenv("I3_BUILD") != NULL) {
        path = std::string(getenv("I3_BUILD"))
            + std::string("/neutrino-generator/resources/cross_section_data/");
        if (fs::exists(path))
            return path;
    }
    
    path = ".";
    return path;
}

void TableUtils::ReadTableList(const string &tablepath,
                       const string &modelname, 
                       std::map<string, string> &xsecs)

{
   xsecs.clear();

   string path = guess_table_path(tablepath);

   string modelfile = modelname;
   string suffix = ".list";
   
   if (modelfile.find(suffix) == string::npos) modelfile += suffix;

   //
   // check file
   //
   string fname1 = path + modelfile;
   log_debug("open file %s", fname1.c_str());
   std::ifstream in(fname1.c_str());

   // if the earthmodel file doesn't exist, stop simulation
   if (in.fail()) {
      log_fatal("failed to open %s. Set correct EarthParamsPath.",
                 fname1.c_str());
   }

   //
   // read the file
   //

   const int bufsize = 8196;
   char buf[bufsize];
   string label, file;
   int nread = 0;

   while(!in.eof()) {

      in.getline(buf, bufsize);
      nread = in.gcount();

      if (nread == -1) {
         log_error("getline failed");

      } else if (nread == 1 || buf[0] == ' ' || buf[0] == '#') {
         // new line, start from white space, or comment line.
         continue;

      } else {
         // density data
         stringstream ss(buf);
         ss >> label >> file;

         xsecs[label] = path + file; 
         log_trace("label %s, filepath %s", 
                   label.c_str(), xsecs[label].c_str());

      }
   } // end of the while loop
   in.close();
} 

}
