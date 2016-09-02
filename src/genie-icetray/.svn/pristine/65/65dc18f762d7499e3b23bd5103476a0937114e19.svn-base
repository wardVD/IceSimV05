//____________________________________________________________________________
/*!

\program gspladd

\brief   Merges XML files containing GENIE cross section splines

         Syntax :
           gspladd -f file_list -d directory_list -o output.xml

         Options :
           -f A list of input xml cross-section files. If more than one then 
              separate using commas.
           -d A list of input directories where to look for xml cross section
              files. If more than one then separate using commas.
           -o output xml file
 
         Notes :
           There must be at least 2 files for the merges to work

         Examples :

           1) shell% gspladd -f xsec_Fe56.xml,xsec_O16.xml -o xsec.xml

              will merge xsec_Fe56.xml and xsec_O16.xml into a single file
              named xsec_all.xml

           2) shell% gspladd -f xsec_Fe56.xml -d /path,/other_path  -o xsec.xml

              will merge xsec_Fe56.xml with all the xml cross-section files that
              can be found in the /path and /other_path directories and write-out
              a single file named xsec_all.xml

\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         Rutherford Appleton Laboratory

\created July 05, 2007

\cpright Copyright (c) 2003-2010, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include <TSystem.h>

#include "Base/XSecAlgorithmI.h"
#include "Conventions/XmlParserStatus.h"
#include "Messenger/Messenger.h"
#include "Utils/XSecSplineList.h"
#include "Utils/StringUtils.h"
#include "Utils/SystemUtils.h"
#include "Utils/CmdLnArgParser.h"

using std::string;
using std::vector;
using std::ostringstream;

using namespace genie;

vector<string> GetAllInputFiles   (void);
void           GetCommandLineArgs (int argc, char ** argv);
void           PrintSyntax        (void);

//User-specified options:
string         gOutFile;   ///< output XML file
vector<string> gInpFiles;  ///< list of input XML files
vector<string> gInpDirs;   ///< list of input dirs (to look for XML files)
vector<string> gAllFiles;  ///< list of all input files

//____________________________________________________________________________
int main(int argc, char ** argv)
{
    
  // START MODIFICATIONS -------- 

  // Currently the only modifications are the ones seen here.
  // they auto-set all environment variables expected by GENIE.
    
  // Genie uses the same stupid system of locating its installation
  // path as ROOT: an environment variable. This one's called "GENIE".
  // We know exactly where GENIE is installed (cmake already checked),
  // so we are just going to set/override the GENIE variable here. -ck
  setenv("GENIE", GENIE_PATH, 1); // 1 means: *do* overwrite
    
  // just un-set the most important environment variables to make sure nothing
  // gets messed up by settings the user might have configured.
  // If anything in GENIE depends on one of those variables,
  // please add command line parameters to this tool and set the variable
  // using the parameter!
  unsetenv("GSEED");
  unsetenv("GXMLPATH"); // do NOT use any splines (we are going to build them right now)
  unsetenv("GXMLPATHS"); // do NOT use any existing splines (we are going to build them right now)
  unsetenv("GALGCONF"); // do NOT use any existing splines (we are going to build them right now)
  unsetenv("GPRODMODE"); // always use Messenger.xml, not Messenger_production.xml

  char *envLHAPDF = getenv("LHAPATH");
  if (!envLHAPDF)
  {
    char *dummy = getenv("I3_PORTS");
    if (!dummy) LOG("gmkspl", pFATAL) << "$I3_PORTS not set, LHAPATH environment variable cannot be auto-configured.";
    const std::string I3_PORTS = dummy;
    std::string LHAPDFPath_ = I3_PORTS + "/share/lhapdf/PDFsets";
    setenv("LHAPATH", LHAPDFPath_.c_str(), 1); // 1 means: *do* overwrite
  }
  else
  {
    LOG("gmkspl", pWARN) << "Using LHAPATH from environment: LHAPATH=" << envLHAPDF;
  }

  // END MODIFICATIONS --------
      
  GetCommandLineArgs(argc,argv);
  
  XSecSplineList * xspl = XSecSplineList::Instance();

  vector<string>::const_iterator file_iter = gAllFiles.begin();
  for( ; file_iter != gAllFiles.end(); ++file_iter) {
    string filename = *file_iter;
    LOG("gspladd", pNOTICE) << " ---- >> Loading file : " << filename;
    XmlParserStatus_t ist = xspl->LoadFromXml(filename, true);
    assert(ist==kXmlOK);
  }

  LOG("gspladd", pNOTICE) 
     << " ****** Saving all loaded splines into : " << gOutFile;
  xspl->SaveAsXml(gOutFile);

  return 0;
}
//____________________________________________________________________________
vector<string> GetAllInputFiles(void)
{
  vector<string> files;

  vector<string>::const_iterator file_iter;
  vector<string>::const_iterator dir_iter;

  // add all files that were input explictly
  file_iter = gInpFiles.begin();
  for( ; file_iter != gInpFiles.end(); ++file_iter) {
    string filename = *file_iter;
    files.push_back(filename);
  } // file_iter

  // loop over input directories
  dir_iter = gInpDirs.begin();
  for( ; dir_iter != gInpDirs.end(); ++dir_iter) {
    string path = *dir_iter;
    // get all XML files in this dir
    vector<string> path_files = utils::system::GetAllFilesInPath(path,"xml");
    // add these files too
    file_iter = path_files.begin();
    for( ; file_iter != path_files.end(); ++file_iter) {
       string filename = *file_iter;
       files.push_back(filename);
    }//file_iter
  }//dir_iter

  return files;
}
//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("gspladd", pNOTICE) << "Parsing command line arguments";

  CmdLnArgParser parser(argc,argv);

  if( parser.OptionExists('f') ) {
    LOG("gspladd", pINFO) << "Reading input files";
    string inpfiles = parser.ArgAsString('f');
    if(inpfiles.find(",") != string::npos) {
       // split the comma separated list
       gInpFiles = utils::str::Split(inpfiles, ",");
    } else {
       // there is just one file
       gInpFiles.push_back(inpfiles);
    }
  }

  if( parser.OptionExists('d') ) {
    LOG("gspladd", pINFO) << "Reading input directories";
    string inpdirs = parser.ArgAsString('d');
    if(inpdirs.find(",") != string::npos) {
       // split the comma separated list
       gInpDirs = utils::str::Split(inpdirs, ",");
    } else {
       // there is just one directory
       gInpDirs.push_back(inpdirs);
    }
  }

  if( parser.OptionExists('o') ) {
    LOG("gspladd", pINFO) << "Reading output file name";
    gOutFile = parser.ArgAsString('o');
  } else {
    LOG("gspladd", pFATAL) << "You must specify an output file name";
    PrintSyntax();
    exit(1);
  }

  gAllFiles = GetAllInputFiles();
  if(gAllFiles.size() <= 1) {
    LOG("gspladd", pFATAL) << "There must be at least 2 input files";
    PrintSyntax();
    exit(1);
  }
}
//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gspladd", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "   gspladd  -f file_list -d directory_list  -o output.xml\n";
}
//____________________________________________________________________________
