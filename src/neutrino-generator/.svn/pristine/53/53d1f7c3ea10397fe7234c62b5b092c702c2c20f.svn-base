/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Kotoyo Hoshina <hoshina@icecube.wisc.edu>
 *
 *  @brief   Utils IMPLEMENTATION FILE
 */

#include <neutrino-generator/utils/EnumTypeDefs.h>
#include <icetray/I3Logging.h>

namespace nugen {

//_________________________________________________________
std::string GetSimModeString(SimMode t)
{
     std::string type_string="no_entry";
     if ( t == FULL )           type_string="Full";
     else if ( t == INEARTH )   type_string="InEarth";
     else if ( t == FINALONLY ) type_string="FinalOnly";
     return type_string;
}

//_________________________________________________________
std::string GetVTXGenModeString(VTXGenMode t)
{
     std::string type_string="no_entry";
     if ( t == NUGEN )       type_string="NuGen";
     else if ( t == GENIE )  type_string="Genie";
     else if ( t == HYBRID ) type_string="Hybrid";
     return type_string;
}

//_________________________________________________________
std::string GetBenchModeString(BenchMode t)
{
     std::string type_string="no_entry";
     if ( t == OFF )            type_string="Off";
     else if ( t == DETEDGE )   type_string="DetEdge";
     else if ( t == INDET )     type_string="InDet";
     else if ( t == ALLBENCH )  type_string="AllBench";
     return type_string;
}

//_________________________________________________________
std::string GetInjectionModeString(InjectionMode t)
{
     std::string type_string="no_entry";
     if ( t == SURFACE)         type_string="Surface";
     else if ( t == CIRCLE )    type_string="Circle";
     else if ( t == DYNAMIC )   type_string="Dynamic";
     return type_string;
}

//_________________________________________________________
std::string GetInteractionTypeString(InteractionType t)
{
     std::string type_string="no_entry";
     if ( t == CC )                 type_string="NuGenCC";
     else if ( t == NC )            type_string="NuGenNC";
     else if ( t == GR )            type_string="NuGenGR";
     else if ( t == GN)             type_string="GenieAll";
     else if ( t == DECAY )         type_string="NuGenDECAY";
     else if ( t == NOINTERACTION ) type_string="NoInteraction";
     else if ( t == UNDEFINED )     type_string="Undefined";
     return type_string;
}

//_________________________________________________________
std::string GetPropagationModeString(PropagationMode t)
{
     std::string type_string="no_entry";
     if ( t == LEGACY )             type_string="Legacy";
     else if ( t == NOPROPWEIGHT )  type_string="NoPropWeight";
     else if ( t == NCGRWEIGHTED )  type_string="NCGRWeighted";
     else if ( t == AUTODETECT )    type_string="AutoDetect";
     return type_string;
}

//_________________________________________________________
std::string GetAngleSamplingModeString(AngleSamplingMode t)
{
     std::string type_string="no_entry";
     if ( t == COS)        type_string="Cos";
     else if ( t == ANG )  type_string="Ang";
     return type_string;
}

//_________________________________________________________
SimMode ToSimMode(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     SimMode t = FULL;
     if ( s == "FULL")            t = FULL;
     else if( s == "INEARTH")     t = INEARTH;
     else if ( s == "FINALONLY")  t = FINALONLY;
     else {
        log_fatal("SimMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
VTXGenMode ToVTXGenMode(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     VTXGenMode t = NUGEN;
     if ( s == "NUGEN")        t = NUGEN;
     else if ( s == "GENIE")   t = GENIE;
     else if ( s == "HYBRID")  t = HYBRID;
     else {
        log_fatal("VTXGenMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
BenchMode ToBenchMode(const std::string &ss) 
{
     std::string s = ToUpperCase(ss);
     BenchMode t = OFF;
     if ( s == "OFF")            t = OFF;
     else if ( s == "DETEDGE")   t = DETEDGE;
     else if ( s == "INDET")     t = INDET;
     else if ( s == "ALLBENCH")  t = ALLBENCH;
     else {
        log_fatal("BenchMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
InjectionMode ToInjectionMode(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     InjectionMode t = SURFACE;
     if ( s == "SURFACE")        t = SURFACE;
     else if ( s == "CYLINDER")  t = SURFACE; // cylinder is alias
     else if ( s == "CIRCLE")    t = CIRCLE;
     else if ( s == "DYNAMIC")   t = DYNAMIC;
     else {
        log_fatal("InjectionMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
InteractionType ToInteractionType(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     InteractionType t = CC;
     if ( s == "CC" || s == "NUGENCC")            t = CC;
     else if ( s == "NC" || s == "NUGENNC")       t = NC;
     else if ( s == "GR" || s == "NUGENGR")       t = GR;
     else if ( s == "GN" || s == "GENIEALL")      t = GN;
     else if ( s == "DECAY" || s == "NUGENDECAY") t = DECAY;
     else if ( s == "NOINTERACTION"     )         t = NOINTERACTION;
     else if ( s == "UNDEFINED")                  t = UNDEFINED;
     else {
        log_fatal("InteractionType %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
PropagationMode ToPropagationMode(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     PropagationMode t = AUTODETECT;
     if ( s == "LEGACY")               t = LEGACY;
     else if ( s == "NOPROPWEIGHT")    t = NOPROPWEIGHT;
     else if ( s == "NCGRWEIGHTED")    t = NCGRWEIGHTED;
     else if ( s == "AUTODETECT")      t = AUTODETECT;
     else if ( s == "NOWEIGHT") { 
        log_error("option NoWeight has been changed to NoPropWeight. Please update your script. For now I set NoPropWeight.");
        t = NOPROPWEIGHT;
     } else {
        log_fatal("PropagationMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
AngleSamplingMode ToAngleSamplingMode(const std::string &ss)
{
     std::string s = ToUpperCase(ss);
     AngleSamplingMode t = COS;
     if ( s == "COS")         t = COS;
     else if ( s == "ANG")    t = ANG;
     else {
        log_fatal("AngleSamplingMode %s does not exist", s.c_str());        
     }
     return t;
}

//_________________________________________________________
std::string ToUpperCase(std::string s)
{
     transform(s.begin(), s.end(), s.begin(), toupper); 
     return s;
}

}

