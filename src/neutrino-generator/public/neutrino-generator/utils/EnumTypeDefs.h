#ifndef NuGEnumTypeDefs_H
#define NuGEnumTypeDefs_H
//--------------------------------------
// nugen enums 

#include <map>
#include <string>
#include "dataclasses/I3Vector.h"
#include "dataclasses/I3Map.h"
#include "dataclasses/physics/I3Particle.h"
#include <algorithm>

namespace nugen {

  /**
   * @brief simulation mode flag
   * FULL - all simulation (same as old nugen)
   * INEARTH - simulate InEarth propagation only
   * FINALONLY - simulate final interaction only
   */
  enum SimMode    {FULL, INEARTH, FINALONLY};

  /**
   * @brief vertex generator mode flag
   * NUGEN - use NUGEN cross section and interaction classes
   * GENIE - use GENIE cross section and interaction modules
   * HYBRID - use NUGEN and GENIE, swith them with energy thresthold
   */
  enum VTXGenMode {NUGEN, GENIE, HYBRID};


  enum BenchMode  {OFF=0, DETEDGE=1, INDET=2, ALLBENCH=100};

  /**
   * @brief define injection shape.
   * SURFACE  - inject primary neutrinos only if the neutrino has
   *            interactions with the MuonGun::Surface object
   *            the object around IceCube does not rotate.
   *            Used to called as CYLINDER option.
   * CIRCLE   - inject primary neutrinos with the circle area. 
   *            same as old nugen.
   * DYNAMIC  - not implemented yet, the SURFACE option with
   *            energy dependent scale parameter(not yet implemented)
   */
  enum InjectionMode {SURFACE, CIRCLE, DYNAMIC};

  /**
   * @brief type flag of interactions
   * CC -- NuGen charged current
   * NC -- NuGen neutral current
   * GR -- NuGen grashow
   * GN -- Genie
   */
  enum InteractionType {CC=1, NC=2, GR=3, GN=4, DECAY=11,
                        NOINTERACTION=0, UNDEFINED= -1};

  /**
   * @brief define propagation mode.
   *
   * LEGACY       - this option is obsolate, used only for emulating
   *                icesim version < 2.6. Some neutrinos may be lost to
   *                absorption in CC interactions.
   * NOPROPWEIGHT - allowing absorption in CC interactions.
   *                For NuTau simulation, this option must be NoPropWeight.
   * NCGRWEIGHTED - prohibiting CC interactions. All neutrinos 
   *                (unless NuE->Grashow W->hadron interaction happens) will 
   *                survive propagation, but will have a propagation 
   *                weight that must be multiplied into the interaction 
   *                weight to obtain correct rates.
   * AUTODETECT   - use NCGRWEIGHTED for NuE and NuMu, NOPROPWEIGHT for
   *                NuTau
   */
  enum PropagationMode {LEGACY=0, NOPROPWEIGHT=1, NCGRWEIGHTED=2, 
                        AUTODETECT=10};

  /**
   * @brief flag for angle sampling
   * COS -- sample zenith in Cos(theta)
   * ANG -- sample zenith in theta
   */
  enum AngleSamplingMode {COS=0, ANG=1};

  std::string GetSimModeString(SimMode t);
  std::string GetVTXGenModeString(VTXGenMode t);
  std::string GetBenchModeString(BenchMode t);
  std::string GetInjectionModeString(InjectionMode t);
  std::string GetInteractionTypeString(InteractionType t);
  std::string GetPropagationModeString(PropagationMode t);
  std::string GetAngleSamplingModeString(AngleSamplingMode t);

  SimMode            ToSimMode(const std::string &s);
  VTXGenMode         ToVTXGenMode(const std::string &s);
  BenchMode          ToBenchMode(const std::string &s);
  InjectionMode      ToInjectionMode(const std::string &s);
  InteractionType    ToInteractionType(const std::string &s);
  PropagationMode    ToPropagationMode(const std::string &s);
  AngleSamplingMode ToAngleSamplingMode(const std::string &s);

  typedef std::pair<double, double> DoubleDoublePair;
  typedef std::map<int, double> IntDoubleMap;
  typedef std::map<double, double> DoubleDoubleMap;
  typedef std::map<InteractionType, double> InteractionTypeDoubleMap;

  std::string ToUpperCase(std::string s);

}


#endif 


