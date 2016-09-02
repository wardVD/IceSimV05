/**
 * Copyright (C) 2010
 * The IceCube collaboration
 * ID: $Id: $
 *
 * @file I3IceTopResponseFactory.h
 * @version $Rev: $
 * @date $Date: $
 * @author Tilo Waldenmaier
 */

#ifndef _TOPSIMULATOR_I3ICETOPRESPONSEFACTORY_H_
#define _TOPSIMULATOR_I3ICETOPRESPONSEFACTORY_H_

#include <icetray/I3SingleServiceFactory.h>
#include <topsimulator/interface/I3IceTopResponseTemplate.h>

/**
 * \brief This is the IceTop response service factory which adds the 
 * IceTop response service to the contexts of all modules/services. This
 * class is here only for backward compatibility with older scripts since
 * everything is handled by I3SingleServiceFactory.
 */

template <class T>
class I3IceTopResponseFactory : public I3SingleServiceFactory<I3IceTopResponseTemplate<T>, I3IceTopResponseService>
{
 public:
 I3IceTopResponseFactory(const I3Context& context):
   I3SingleServiceFactory<I3IceTopResponseTemplate<T>, I3IceTopResponseService>(context)
   {};
};

#endif
