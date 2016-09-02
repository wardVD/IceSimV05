/*
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3ROOTBranchWrapperEnum.h 139374 2015-11-09 14:05:38Z gmaggi $
 *
 * @version $Revision: 139374 $
 * @date $LastChangedDate: 2015-11-09 08:05:38 -0600 (Mon, 09 Nov 2015) $
 * @author Fabian Kislat <fabian.kislat@desy.de> Last changed by: $LastChangedBy: gmaggi $
 */

#ifndef ROOTWRITER_I3ROOTBRANCHWRAPPERENUM_H_INCLUDED
#define ROOTWRITER_I3ROOTBRANCHWRAPPERENUM_H_INCLUDED

#include "I3ROOTBranchWrapper.h"
#include <icetray/I3PointerTypedefs.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

I3_FORWARD_DECLARATION(I3TableRow);

struct I3Datatype;
class TBranch;
class TTree;

class I3ROOTBranchWrapperEnum : public I3ROOTBranchWrapper {
  friend class OnlyForTest;

 public:
  I3ROOTBranchWrapperEnum();
  I3ROOTBranchWrapperEnum(TTree *tree, const I3Datatype &type, const std::string &branchname,
			  const std::string &docstring, unsigned int index, size_t arrayLength = 1,
			  const I3ROOTBranchWrapperConstPtr &counter = I3ROOTBranchWrapperConstPtr());
  I3ROOTBranchWrapperEnum(const I3ROOTBranchWrapperEnum &rhs);
  virtual ~I3ROOTBranchWrapperEnum();

  void Fill(const I3TableRowConstPtr &data);

  typedef void (I3ROOTBranchWrapperEnum::*FillFn)(const I3TableRowConstPtr&);

 private:
  boost::shared_ptr<std::string> data_;
  std::map<long, std::string> mapping_;
  
  template <typename T>
    void FillImp(const I3TableRowConstPtr &data);
  
  FillFn selectFillImp(const I3Datatype &type);
  
  FillFn fillImp_;
};

I3_POINTER_TYPEDEFS(I3ROOTBranchWrapperEnum);

#endif // ROOTWRITER_I3ROOTBRANCHWRAPPER_H_INCLUDED
