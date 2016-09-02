#include <cmath>
#include <filter-tools/ParticleCheck.h>
#include <icetray/I3Frame.h>
#include <dataclasses/physics/I3Particle.h>

/** This Module goes through the frame checking the list of particles
 *  against a list of the same particles with a prefix
 */

ParticleCheck::ParticleCheck(const I3Context& context) : 
  I3Module(context),
  prefix_("Pole")  //Default from 2007 online filter
{
  std::vector <std::string> particles;

  particles.push_back("CascadeLinefit");  //defaults for  2007 online filter
  particles.push_back("IcLinefit");
  particles.push_back("MoonFit");
  particles.push_back("ToI");
      
  AddParameter("ParticleNames","Names of the I3Particles to check",particles);
  AddParameter("ParticlePrefix","Prefix on the I3Particles that will be checked against",prefix_);
   
  AddOutBox("OutBox");
}
  
void ParticleCheck::Configure()
{
  std::vector <std::string> particles;
  GetParameter("ParticleNames",particles);
  GetParameter("ParticlePrefix",prefix_);

  // for each entry in the vector of particles initilize the map that will store the results
  for (std::vector<std::string>::iterator itr=particles.begin();
       itr!=particles.end();
       itr++)
    {
      particle_matches_[*itr]=std::pair<int,int>(0,0);
    }

}

void ParticleCheck::Physics(I3FramePtr frame)
{
  for (std::map <std::string, std::pair <int, int> >::iterator itr=particle_matches_.begin();
       itr!=particle_matches_.end();
       itr++)
    {
      I3ParticleConstPtr north_particle=frame->Get<I3ParticleConstPtr>(itr->first);
      I3ParticleConstPtr south_particle=frame->Get<I3ParticleConstPtr>(prefix_+itr->first);

      if (north_particle && south_particle && *north_particle==*south_particle)
	{
	  log_info("particles match for %s and %s",itr->first.c_str(),(prefix_+itr->first).c_str());
	  particle_matches_[itr->first].first+=1;
	}
      else
	{
	  log_warn("particles don't match for %s and %s",itr->first.c_str(),(prefix_+itr->first).c_str());
	}
      particle_matches_[itr->first].second+=1;
    }
  PushFrame(frame,"OutBox");
}

  
void ParticleCheck::Finish()
{
  for ( std::map<std::string, std::pair<int,int> >::iterator itr=particle_matches_.begin();itr!=particle_matches_.end();itr++)
    {
      printf("%30s matched %6d/%6d  %3.2f%%\n",itr->first.c_str(),itr->second.first,itr->second.second,100.*itr->second.first/itr->second.second);
    }
}
  
I3_MODULE(ParticleCheck);
