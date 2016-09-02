#ifndef PARTICLECHECK_H_INCLUDED
#define PARTICLECHECK_H_INCLUDED
/** 
 * @author Kevin Meagher
 *
 * @brief This Module goes through the frame checking the list of particles
 *  against a list of the same particles with a prefix
 */

#include <icetray/I3Module.h>

class I3Direction;
class I3Position;
class I3Particle;

class ParticleCheck : public I3Module
{
public:
  ///Constructor
  ParticleCheck(const I3Context& context) ;
  void Configure();
  void Physics(I3FramePtr frame);
  void Finish();
  
  ///compare two doubles that might be nan return true if they agree
  static bool Compare( const double& l, const double &r);
  ///compare two I3Directions return true if they match
  static bool Compare(const I3Direction& lhs, const I3Direction& rhs);
  ///compare two I3Positions return true if they match
  static bool Compare(const I3Position& lhs, const I3Position& rhs);
  ///compare two I3Particles return true if they match
  static bool Compare(const I3Particle& lhs, const I3Particle& rhs);
  
private:
  ///map of particle name to pair where first is number of matchs second is total comparisons
  std::map <std::string, std::pair <int, int> > particle_matches_;
  ///prefex to prepend to particles to get pole version
  std::string prefix_;
};

#endif  // PARTICLECHECK_H_INCLUDED
