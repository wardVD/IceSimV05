/**
 * Copyright (C) 2009
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3InjectorFactory.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */


#ifndef _TOPSIMULATOR_I3INJECTORFACTORY_H_
#define _TOPSIMULATOR_I3INJECTORFACTORY_H_

#include <icetray/I3SingleServiceFactory.h>
#include <topsimulator/interface/I3InjectorService.h>

/**
 * \brief This is the injector service factory which adds the 
 * injector service to the contexts of all modules/services. This
 * class is here only for backward compatibility with older scripts since
 * now everything is handled by I3SingleServiceFactory.
 */

template <class T>
class I3InjectorFactory : public I3SingleServiceFactory<T, I3InjectorService>
{
 public:
  I3InjectorFactory(const I3Context& context): I3SingleServiceFactory<T, I3InjectorService>(context)
  {}
};

#endif
