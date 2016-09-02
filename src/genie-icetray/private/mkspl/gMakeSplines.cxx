//____________________________________________________________________________
/*!

\program gmkspl

\brief   GENIE utility program building XML cross section splines that can
         be loaded into GENIE to speed-up event generation.
         The list of neutrino PDG codes is passed from the command line.
         The list of nuclear target PDG codes is either passed from the 
         command line or extracted from the input ROOT/GEANT geometry.

         Syntax :
           gmkspl -p nupdg <-t tgtpdg, -f geomfile> [-o output_xml_file]
                  [-n nknots] [-e max_energy]
         Note :
           [] marks optional arguments.
           <> marks a list of arguments out of which only one can be 
              selected at any given time.

         Options :
           -p  A comma separated list of nu PDG codes.
           -t  A comma separated list of tgt PDG codes. 
               PDG code format: 10LZZZAAAI
           -f  A ROOT file containing a ROOT/GEANT geometry description.
           -o  Output XML filename.
               Default: `xsec_splines.xml'.
           -n  Number of knots per spline.
               Default: 15 knots per decade of energy range with a minimum 
               of 30 knots totally.
           -e  Maximum energy in spline.
               Default: The max energy in the validity range of the spline 
               generating thread.


        ***  See the User Manual for more details and examples. ***


\author  Costas Andreopoulos <costas.andreopoulos \at stfc.ac.uk>
         STFC, Rutherford Appleton Laboratory

\created September 27, 2005

\cpright Copyright (c) 2003-2010, GENIE Neutrino MC Generator Collaboration
         For the full text of the license visit http://copyright.genie-mc.org
         or see $GENIE/LICENSE
*/
//____________________________________________________________________________

#include <cassert>
#include <cstdlib>
#include <string>
#include <vector>

#include <TSystem.h>

#include "Conventions/GBuild.h"
#include "EVGDrivers/GEVGDriver.h"
#include "Interaction/Interaction.h"
#include "Messenger/Messenger.h"
#include "PDG/PDGCodeList.h"
#include "Utils/StringUtils.h"
#include "Utils/XSecSplineList.h"
#include "Utils/CmdLnArgParser.h"

#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
#include "Geo/ROOTGeomAnalyzer.h"
#endif

using std::string;
using std::vector;

using namespace genie;

#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
using namespace genie::geometry;
#endif

//Prototypes:
void          GetCommandLineArgs (int argc, char ** argv);
void          PrintSyntax        (void);
PDGCodeList * GetNeutrinoCodes   (void);
PDGCodeList * GetTargetCodes     (void);

//Defaults for optional options:
string kDefOptXMLFilename = "xsec_splines.xml";

//User-specified options:
string gOptNuPdgCodeList  = "";
string gOptTgtPdgCodeList = "";
string gOptGeomFilename   = "";
string gOptXMLFilename    = "";
int    gOptNKnots         = -1;
double gOptMaxE           = -1.;

//____________________________________________________________________________
int main(int argc, char ** argv)
{

  // START MODIFICATIONS -------- 

  // This tool is based on gMakeSplines.cxx from GENIE 2.6.4.
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

  //-- parse command line arguments
  GetCommandLineArgs(argc,argv);

  //-- get spline list 
  //  (& autoload splines specified in $GSPLOAD in case free nucleon cross
  //   sections are recycled for nuclear targets))
  XSecSplineList * spline_list = XSecSplineList::Instance();
  spline_list->AutoLoad();

  PDGCodeList * neutrinos = GetNeutrinoCodes();
  PDGCodeList * targets   = GetTargetCodes();

  if(!neutrinos || neutrinos->size() == 0 ) {
     LOG("gmkspl", pFATAL) << "Empty neutrino PDG code list";
     PrintSyntax();
     exit(2);
  }
  if(!targets || targets->size() == 0 ) {
     LOG("gmkspl", pFATAL) << "Empty target PDG code list";
     PrintSyntax();
     exit(3);
  }

  LOG("gmkspl", pINFO) << "Neutrinos: " << *neutrinos;
  LOG("gmkspl", pINFO) << "Targets: "   << *targets;

  //-- loop over all possible input init states and ask the GEVGDriver
  //   to build splines for all the interactions that its loaded list
  //   of event generators can generate.

  PDGCodeList::const_iterator nuiter;
  PDGCodeList::const_iterator tgtiter;

  for(nuiter = neutrinos->begin(); nuiter != neutrinos->end(); ++nuiter) {
    for(tgtiter = targets->begin(); tgtiter != targets->end(); ++tgtiter) {

      int nupdgc  = *nuiter;
      int tgtpdgc = *tgtiter;
      InitialState init_state(tgtpdgc, nupdgc);

      GEVGDriver driver;
      driver.Configure(init_state);
      driver.CreateSplines(gOptNKnots, gOptMaxE);
    }
  }

  //-- save the splines at the requested XML file
  spline_list->SaveAsXml(gOptXMLFilename);

  delete neutrinos;
  delete targets;

  return 0;
}
//____________________________________________________________________________
void GetCommandLineArgs(int argc, char ** argv)
{
  LOG("gmkspl", pINFO) << "Parsing command line arguments";

  CmdLnArgParser parser(argc,argv);

  //-- Optional arguments

  // output XML file name:
  if( parser.OptionExists('o') ) {
    LOG("gmkspl", pINFO) << "Reading output filename";
    gOptXMLFilename = parser.ArgAsString('o');
  } else {
    LOG("gmkspl", pINFO) << "Unspecified filename - Using default";
    gOptXMLFilename = kDefOptXMLFilename;
  }

  // number of knots:
  if( parser.OptionExists('n') ) {
    LOG("gmkspl", pINFO) << "Reading number of knots/spline";
    gOptNKnots = parser.ArgAsInt('n');
  } else {
    LOG("gmkspl", pINFO)
      << "Unspecified number of knots - Using default";
    gOptNKnots = -1;
  }

  // max spline energy (if < max of validity range)
  if( parser.OptionExists('e') ) {
    LOG("gmkspl", pINFO) << "Reading maximum spline energy";
    gOptMaxE = parser.ArgAsDouble('e');
  } else {
    LOG("gmkspl", pINFO) 
       << "Unspecified maximum spline energy - Using default";
    gOptMaxE = -1;
  }

  //-- Required arguments

  // comma-separated neutrino PDG code list:
  if( parser.OptionExists('p') ) {
    LOG("gmkspl", pINFO) << "Reading neutrino PDG codes";
    gOptNuPdgCodeList = parser.ArgAsString('p');
  } else {
    LOG("gmkspl", pFATAL) 
       << "Unspecified neutrino PDG code list - Exiting";
    PrintSyntax();
    exit(1);
  }

  // comma-separated target PDG code list or input geometry file:
  bool tgt_cmd = true;
  if( parser.OptionExists('t') ) {
    LOG("gmkspl", pINFO) << "Reading target nuclei PDG codes";
    gOptTgtPdgCodeList = parser.ArgAsString('t');
  } else {
    LOG("gmkspl", pINFO) << "No code list specified from the command line";
    tgt_cmd = false;
  }

  bool tgt_geom = true;
  if( parser.OptionExists('f') ) {
    LOG("gmkspl", pINFO) << "Reading ROOT geometry filename";
    gOptGeomFilename = parser.ArgAsString('f');
  } else {
    LOG("gmkspl", pINFO) << "No geometry file was specified";
    tgt_cmd = false;
  }

  bool both =  tgt_geom &&  tgt_cmd;
  bool none = !tgt_geom && !tgt_cmd;

  if(none) {
    LOG("gmkspl", pFATAL) 
          << "No geom file or cmd line target list was specified - Exiting";
    PrintSyntax();
    exit(1);
  }

  if(both) {
    LOG("gmkspl", pFATAL) 
       << "You specified both a geom file and a cmd line target list "
         << "- Exiting confused";
    PrintSyntax();
    exit(1);
  }

  //-- print the options you got from command line arguments
  LOG("gmkspl", pINFO) << "Neutrino PDG codes  = " << gOptNuPdgCodeList;
  LOG("gmkspl", pINFO) << "Target PDG codes    = " << gOptTgtPdgCodeList;
  LOG("gmkspl", pINFO) << "Input ROOT geometry = " << gOptGeomFilename;
  LOG("gmkspl", pINFO) << "Output XML file     = " << gOptXMLFilename;
}
//____________________________________________________________________________
void PrintSyntax(void)
{
  LOG("gmkspl", pNOTICE)
    << "\n\n" << "Syntax:" << "\n"
    << "   gmkspl -p nupdg <-t tgtpdg, -f geomfile> [-o output_xml]"
    << " [-n nknots] [-e max_energy]";
}
//____________________________________________________________________________
PDGCodeList * GetNeutrinoCodes(void)
{
  // split the comma separated list
  vector<string> nuvec = utils::str::Split(gOptNuPdgCodeList,  ",");

  // fill in the PDG code list
  PDGCodeList * list = new PDGCodeList;
  vector<string>::const_iterator iter;
  for(iter = nuvec.begin(); iter != nuvec.end(); ++iter) {
    list->push_back( atoi(iter->c_str()) );
  }
  return list;
}
//____________________________________________________________________________
PDGCodeList * GetTargetCodes(void)
{
  bool from_geom_file = ( gOptGeomFilename.size()   > 0 );
  bool from_cmd_line  = ( gOptTgtPdgCodeList.size() > 0 );

  if (from_cmd_line) {
     // split the comma separated list
     vector<string> tgtvec = utils::str::Split(gOptTgtPdgCodeList, ",");

     // fill in the PDG code list
     PDGCodeList * list = new PDGCodeList;
     vector<string>::const_iterator iter;
     for(iter = tgtvec.begin(); iter != tgtvec.end(); ++iter) {
        list->push_back( atoi(iter->c_str()) );
     }
     return list;
  }

  if (from_geom_file) {
#ifdef __GENIE_GEOM_DRIVERS_ENABLED__
     // create/configure a geometry driver
     LOG("gmkspl", pINFO) << "Creating/configuring a ROOT geom. driver";
     ROOTGeomAnalyzer * geom = new ROOTGeomAnalyzer(gOptGeomFilename);

     PDGCodeList * list = new PDGCodeList(geom->ListOfTargetNuclei());

     delete geom;
     return list;
#else
     LOG("gmkspl", pFATAL) 
      << "To read-in a ROOT geometry you need to enable the geometry drivers!";
     gAbortingInErr = true;
     exit(1);
     return 0;
#endif

  }
  return 0;
}
//____________________________________________________________________________

