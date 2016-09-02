#ifndef I3DIPOLE_FIT_PARAMS_CONVERTER_H
#define I3DIPOLE_FIT_PARAMS_CONVERTER_H
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

#include <tableio/I3Converter.h>
#include "recclasses/I3DipoleFitParams.h"

/// Converter for I3DipoleFitParams
class I3DipoleFitParamsConverter : public I3ConverterImplementation<I3DipoleFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3DipoleFitParams & params); 
    size_t FillRows(const I3DipoleFitParams& params, I3TableRowPtr rows);
};
    
#endif
