#ifndef I3TENSOR_OF_INERTIA_FIT_PARAMS_CONVERTER_H
#define I3TENSOR_OF_INERTIA_FIT_PARAMS_CONVERTER_H
/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy$
 */

#include "tableio/I3Converter.h"
#include "recclasses/I3TensorOfInertiaFitParams.h"

/// Converter for I3TensorOfInertiaFitParams
class I3TensorOfInertiaFitParamsConverter : public I3ConverterImplementation<I3TensorOfInertiaFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TensorOfInertiaFitParams & params); 
    size_t FillRows(const I3TensorOfInertiaFitParams& params, I3TableRowPtr rows);
};
    
#endif
