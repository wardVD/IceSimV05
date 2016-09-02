#ifndef INSTALL_H
#define INSTALL_H
/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: install.h 2219 2015-08-14 18:44:32Z olivas $
 *
 * @version $Revision: 2219 $
 * @date $LastChangedDate: 2015-08-14 13:44:32 -0500 (Fri, 14 Aug 2015) $
 * @author Jakob van Santen <vansanten@wisc.edu> Last changed by: $LastChangedBy: olivas $
 */

#include <icetray/I3Context.h>

template <typename T>
struct I3InstallService {
	static bool
	func(boost::shared_ptr<T> what, I3ContextPtr ctx, const std::string& where)
	{
		return ctx->Put(where, what);
	}	
};

#endif
