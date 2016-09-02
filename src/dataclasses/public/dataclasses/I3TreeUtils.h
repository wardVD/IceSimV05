/**
    copyright  (C) 2004
    the icecube collaboration
    @file I3MCTreeUtils.h
*/

#ifndef I3TREEUTILS_H_INCLUDED
#define I3TREEUTILS_H_INCLUDED

#include "dataclasses/I3Tree.h"
#include "icetray/I3DefaultName.h"

namespace I3TreeUtils
{
  /**
   * Adds a top-level node to the tree.
   * The top level object has no parent.
   * A tree can have multiple top level objects.
   */
  template<class Type>
    void AddTopLevel(I3Tree<Type>& t, const Type& p)
    {
      typename I3Tree<Type>::iterator si;
      si = t.end(t.begin());
      t.insert(si,p);
    }

  template<class Type>
    void AddTopLevel(I3Tree<Type>& t, const I3Tree<Type>& subt)
    {
      typename I3Tree<Type>::iterator si;
      si = t.end(t.begin());
      t.insert_subtree(si,subt.begin());
    }

  /**
   * Alternative way to add top level object
   */
  template<class Type>
    void AddTopLevel(boost::shared_ptr<I3Tree<Type> > t, const Type& p)
    {
      AddTopLevel<Type>(*t, p);
    }

  /**
   * Appends a child to an object
   */
  template<class Type>
    void AppendChild(I3Tree<Type>& t, const Type& parent, const Type& child)
    {
      //Need to find the iterator of the parent
      //do it bonehead-style first
      typename I3Tree<Type>::iterator i;
      typename I3Tree<Type>::iterator p_iter;
      for (i=t.begin(); i!=t.end(); ++i) {
	if (i->GetMajorID() == parent.GetMajorID() &&
	    i->GetMinorID() == parent.GetMinorID() ) {
	  p_iter = i;
	  break;
	}
      }
      if (p_iter != t.end()) {
	t.append_child(p_iter,child);
      } else {
	log_error("attempt to add a child to a non-existant parent.");
      }
    }

  /**
   * Alternative to append a child
   */
  template<class Type>
    void AppendChild(boost::shared_ptr<I3Tree<Type> > t, 
		     const Type& parent, const Type& child)
    {
      AppendChild<Type>(*t, parent, child);
    }

  /**
   * Random access to the tree objects
   */
  template<class Type>
    Type& GetObject(const I3Tree<Type>& t, uint64_t major_id, int minor_id)
    {
      typename I3Tree<Type>::iterator i;
      for (i=t.begin(); i!=t.end(); ++i) {
	if ((i->GetMajorID() == major_id) &&
	    (i->GetMinorID() == minor_id)) {
	  return *i;
	}
      }
      log_fatal("There is no object with ID = (%lli,%i) ",
		(long long)major_id,
		minor_id);
      return *i;
    }

  /**
   * Alternative way to get object from tree
   */
  template<class Type>
    Type& GetObject(boost::shared_ptr<const I3Tree<Type> > t, uint64_t major_id, int minor_id)
    {
      return GetObject<Type>(*t, major_id, minor_id);
    }


  /**
   * Gets a list of top level objects from the tree
   */
  template<class Type>
    const std::vector<Type> GetTopLevelList(const I3Tree<Type>& t)
    {
      std::vector<Type> topLevelList;
      typename I3Tree<Type>::sibling_iterator i;
      for (i=t.begin(); i!=t.end(); ++i)
	topLevelList.push_back(*i);
      return topLevelList;
    }

  /**
   * Alternative way to get top level objects
   */
  template<class Type>
    const std::vector<Type> GetTopLevelList(boost::shared_ptr<const I3Tree<Type> > t)
    {
      return GetTopLevelList<Type>(*t);
    }

  /**
   * Gets a list of daughters of the parent object.
   */
  template<class Type>
    const std::vector<Type> GetDaughters(const I3Tree<Type>& t, const Type& p)
    {
      std::vector<Type> daughterList;
      typename I3Tree<Type>::iterator i;
      for (i=t.begin(); i != t.end(); i++) {
	if (i->GetMajorID() == p.GetMajorID() &&
	    i->GetMinorID() == p.GetMinorID()) {
	  typename I3Tree<Type>::sibling_iterator j(i);
	  for (j=t.begin(i); j!=t.end(i); ++j)
	    daughterList.push_back(*j);
	  return daughterList;
	}
      }
      log_warn("No daughters found for this object.");
      return daughterList;
    }

  /**
   * Alternative way to get list of daugters
   */
  template<class Type>
    const std::vector<Type> GetDaughters(boost::shared_ptr<const I3Tree<Type> > t, 
					 const Type& p)
    {
      return GetDaughters<Type>(*t, p);
    }

  /**
   * Returns true if the object is a top level object
   */
  template<class Type>
    bool IsTopLevel(const I3Tree<Type>& t, const Type& p)
    {
      typename I3Tree<Type>::sibling_iterator s;
      for (s=t.begin(); s!=t.end(); s++)
	if (s->GetMajorID() == p.GetMajorID() &&
	    s->GetMinorID() == p.GetMinorID() ) 
	  return true;
      return false;
    }

  /**
   * Alternate way to check if object is a top level object
   */
  template<class Type>
    bool IsTopLevel(boost::shared_ptr<const I3Tree<Type> > t, const Type& p)
    {
      return IsTopLevel<Type>(*t, p);
    }

  /**
   * Returns true if the object has a parent
   */
  template<class Type>
    bool HasParent(const I3Tree<Type>& t, const Type& child)
    {
      typename I3Tree<Type>::iterator i = t.begin();
      for( ; i != t.end(); i++){
	if(i->GetMajorID() == child.GetMajorID() && 
	   i->GetMinorID() == child.GetMinorID() && 
	   !IsTopLevel<Type>(t,child))
	  if(t.parent(i) != t.end()) return true;
      }
      return false;
    }

  /**
   * Alternative way to check if object has a parent
   */
  template<class Type>
    bool HasParent(boost::shared_ptr<const I3Tree<Type> > t, const Type& child)
    {
      return HasParent<Type>(*t, child);
    }

  /**
   * Gets the parent of an object.  log_fatals if parent does not exist.
   */
  template<class Type>
    const Type& GetParent(const I3Tree<Type>& t, const Type& child)
    {
      typename I3Tree<Type>::iterator i = t.begin();
      for( ; i != t.end(); i++){
	if(i->GetMajorID() == child.GetMajorID() && 
	   i->GetMinorID() == child.GetMinorID() && 
	   !IsTopLevel<Type>(t,child))
	  return *(t.parent(i));
      }
      log_fatal("No parent found for this object.");
    }

  /**
   * Alternative way to get parent
   */
  template<class Type>
    const Type& GetParent(boost::shared_ptr<const I3Tree<Type> > t,
				const Type& child)
    {
      return GetParent<Type>(*t, child);
    }

}

#endif 


