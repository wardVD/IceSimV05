/**
 *  copyright  (C) 2005
 *  the IceCube collaboration
 *  $Id: $
 *
 *  @version $Revision: $
 *  @date    $Date: $
 *  @author  Alex Olivas <olivas@icecube.umd.edu>
 *
 *  @brief   TreeUtils IMPLEMENTATION FILE
 */

#include <dataclasses/physics/I3MCTreeUtils.h>
#include <neutrino-generator/utils/TreeUtils.h>
#include <boost/foreach.hpp>
#include <inttypes.h>
#include <algorithm>

using namespace std;
using namespace nugen;

//#define __NUGDEBUG__

//_________________________________________________________
bool
CheckParticleDiff(const I3Particle & p1, const I3Particle &p2)
{
   bool pass = true;

#ifdef __NUGDEBUG__

   ///* for debug
   vector<string> errors;

   if (p1.GetMajorID() != p2.GetMajorID()) {
      char buf[2048];
      sprintf(buf, "MajorID %llu, %llu, diff %llu", (long long unsigned int)p1.GetMajorID(), (long long unsigned int)p2.GetMajorID(), (long long unsigned int)p1.GetMajorID() - p2.GetMajorID());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetMinorID() != p2.GetMinorID()) {
      char buf[2048];
      sprintf(buf, "MinorID %d, %d, diff %d", p1.GetMinorID(), p2.GetMinorID(), p1.GetMinorID() - p2.GetMinorID());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetZenith() != p2.GetZenith()) {
      char buf[2048];
      sprintf(buf, "Zenith %g, %g, diff %g", p1.GetZenith(), p2.GetZenith(), p1.GetZenith() - p2.GetZenith());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetAzimuth() != p2.GetAzimuth()) {
      char buf[2048];
      sprintf(buf, "Azimuth %g, %g, diff %g", p1.GetAzimuth(), p2.GetAzimuth(), p1.GetAzimuth() - p2.GetAzimuth());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetX() != p2.GetX()) {
      char buf[2048];
      sprintf(buf, "X %g, %g, diff %g", p1.GetX(), p2.GetX(), p1.GetX() - p2.GetX());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetY() != p2.GetY()) {
      char buf[2048];
      sprintf(buf, "Y %g, %g, diff %g", p1.GetY(), p2.GetY(), p1.GetY() - p2.GetY());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetZ() != p2.GetZ()) {
      char buf[2048];
      sprintf(buf, "Z %g, %g, diff %g", p1.GetZ(), p2.GetZ(), p1.GetZ() - p2.GetZ());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetTime() != p2.GetTime()) {
      char buf[2048];
      sprintf(buf, "Time %g, %g, diff %g", p1.GetTime(), p2.GetTime(), p1.GetTime() - p2.GetTime());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetEnergy() != p2.GetEnergy()) {
      char buf[2048];
      sprintf(buf, "Energy %g, %g, diff %g", p1.GetEnergy(), p2.GetEnergy(), p1.GetEnergy() - p2.GetEnergy());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetSpeed() != p2.GetSpeed()) {
      char buf[2048];
      sprintf(buf, "Speed %g, %g, diff %g", p1.GetSpeed(), p2.GetSpeed(), p1.GetSpeed() - p2.GetSpeed());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetLength() != p2.GetLength()) {
      if ((!std::isnan(p1.GetLength())) || (!std::isnan(p2.GetLength()))) {
         char buf[2048];
         sprintf(buf, "Length %g, %g, diff %g", p1.GetLength(), p2.GetLength(), p1.GetLength() - p2.GetLength());
         errors.push_back(string(buf));
         pass = false;
      }
   }

   if (p1.GetType() != p2.GetType()) {
      char buf[2048];
      sprintf(buf, "Type %d, %d, diff %d", p1.GetType(), p2.GetType(), p1.GetType() - p2.GetType());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetPdgEncoding() != p2.GetPdgEncoding()) {
      char buf[2048];
      sprintf(buf, "PdgEncoding %d, %d, diff %d", p1.GetPdgEncoding(), p2.GetPdgEncoding(), p1.GetPdgEncoding() - p2.GetPdgEncoding());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetShape() != p2.GetShape()) {
      char buf[2048];
      sprintf(buf, "Shape %d, %d, diff %d", p1.GetShape(), p2.GetShape(), p1.GetShape() - p2.GetShape());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetFitStatus() != p2.GetFitStatus()) {
      char buf[2048];
      sprintf(buf, "FitStatus %d, %d, diff %d", p1.GetFitStatus(), p2.GetFitStatus(), p1.GetFitStatus() - p2.GetFitStatus());
      errors.push_back(string(buf));
      pass = false;
   }

   if (p1.GetLocationType() != p2.GetLocationType()) {
      char buf[2048];
      sprintf(buf, "LocationType %d, %d, diff %d", p1.GetLocationType(), p2.GetLocationType(), p1.GetLocationType() - p2.GetLocationType());
      errors.push_back(string(buf));
      pass = false;
   }
   if (!pass) {
      cerr << "Particles don't match ! " << endl;
      for (unsigned int i = 0; i<errors.size(); ++i) {
         cerr << "  " <<  errors[i] << endl;
      }
   }

#else

   if (p1 != p2) {
      if ((!std::isnan(p1.GetLength())) || (!std::isnan(p2.GetLength()))) {
         pass = false;
      }
   }

#endif

   return pass;
}

//_________________________________________________________________
void TreeUtils::AddParticleToTree(I3MCTree & t,
                                  ParticlePtr particle,
                                  std::string indent)
{
   //cerr << "TreeUtils::AddParticleToTree is called" << endl;
   I3Particle p = particle->GetI3Particle();
   // I3ParticleID myID = p.GetID();

   // check if the particle exist in tree
   // old particle will be replaced if they are not identical

   I3MCTree::iterator iter(t,p);
   if (iter != t.end()) {

#ifdef __NUGDEBUG__
      fprintf(stderr, "particle exist in tree  %s, %llu, %d time %f location type %d\n",
              p.GetTypeString().c_str(),(long long unsigned int)p.GetMajorID(),
              p.GetMinorID(), p.GetTime(), p.GetLocationType());
#endif

      if (! CheckParticleDiff(*iter, p)) {
         (*iter) = p;
#ifdef __NUGDEBUG__
         cerr << "particle has been updated." << endl;
#endif
      }

   } else if (iter == t.end()) {

      // this particle doesn't exist in the tree yet.
      // check if it has a parent.
      I3ParticleID parentID = particle->GetParentID();  
      I3MCTree::iterator iter2(t, parentID);

      if (iter2 != t.end()) {
         // found a parent.
         // add the particle as a daughter
         I3MCTreeUtils::AppendChild(t, parentID, p);

#ifdef __NUGDEBUG__
         fprintf(stderr, "%s++ added child %s, %llu, %d to mother %llu, %d time %f location type %d\n", 
                indent.c_str(),
                p.GetTypeString().c_str(), 
                (long long unsigned int)p.GetMajorID(), p.GetMinorID(),
                (long long unsigned int)parentID.majorID, parentID.minorID,
                p.GetTime(), p.GetLocationType());
#endif

      } else {
         // add the particle as primary.
         I3MCTreeUtils::AddPrimary(t, p);

#ifdef __NUGDEBUG__
         fprintf(stderr, "added primary %s, %llu, %d time %f location type %d\n",
              p.GetTypeString().c_str(), (long long unsigned int)p.GetMajorID(),
              p.GetMinorID(), p.GetTime(), p.GetLocationType());
#endif

      }
   }

   // add daughters
   
   const vector<ParticlePtr>& dl = particle->GetDaughters();
   vector<ParticlePtr>::const_iterator j; 
   for(j = dl.begin(); j!=dl.end(); ++j){
      AddParticleToTree(t, *j, indent + "  ");
   }
}

//_________________________________________________________________
// private function that works only inside the file scope
bool TimeOrder(const ParticlePtr& rLeft, const ParticlePtr& rRight) 
{ 
   return rLeft->GetTime() < rRight->GetTime(); 
}

//_________________________________________________________________
void TreeUtils::ListToTree(I3MCTreePtr t,
                           ParticlePtrList list)
{
   // sort the list in time order
   std::sort(list.begin(), list.end(), TimeOrder);

   ParticlePtrList::const_iterator j;
   for (j = list.begin(); j!=list.end(); ++j) {
      //printf("Particle time %f\n", (*j)->GetTime());
      AddParticleToTree(*t, *j);
   }

   CheckForDuplicates(*t);
   CheckForInIces(*t);

   if (t->size() == 0) {
      log_fatal("tree size is zero!");
   }
}

//_________________________________________________________________
bool TreeUtils::CheckForDuplicates(I3MCTree & mctree){

  I3MCTree::iterator slow_iter;
  for(slow_iter = mctree.begin();
      slow_iter != mctree.end();
      slow_iter++){

    I3MCTree::iterator fast_iter(slow_iter);
    for(++fast_iter;//start one past the fast_iter
	fast_iter != mctree.end();
	fast_iter++){
      if(slow_iter->GetMinorID() == fast_iter->GetMinorID() &&
	 slow_iter->GetMajorID() == fast_iter->GetMajorID()){
	log_fatal("pathological tree. found a duplicate");
	return true;
      }
    }
  }
  return false;
}

//_________________________________________________________________
bool TreeUtils::CheckForInIces(I3MCTree & mctree){

  I3MCTree::iterator slow_iter;
  for(slow_iter = mctree.begin();
      slow_iter != mctree.end();
      slow_iter++){
     I3Particle parent = (*slow_iter);

     if (parent.GetLocationType() == I3Particle::InIce) {
        // for now NuGen does not generate grand childlen for 
        // inice nu. Check only direct children.
        std::vector<I3Particle> daughters = I3MCTreeUtils::GetDaughters(mctree, parent.GetID()); 
        
        for (unsigned int i=0; i<daughters.size(); ++i) {
           if (daughters[i].GetLocationType() != I3Particle::InIce) {
              log_fatal("LocationType mismatch! Mother %s, Daughter %s (location type %d)",
                         parent.GetTypeString().c_str(), 
                         daughters[i].GetTypeString().c_str(), 
                         daughters[i].GetLocationType()); 
           }
        }
     }
  }
  return true;
}


