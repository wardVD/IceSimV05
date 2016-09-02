#ifndef I3CSCDLLHFITPARAMSCONVERTER_H_INCLUDED
#define I3CSCDLLHFITPARAMSCONVERTER_H_INCLUDED
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
#include "recclasses/I3CscdLlhFitParams.h"

/// Converter for I3CscdLlhFitParams
class I3CscdLlhFitParamsConverter : public I3ConverterImplementation<I3CscdLlhFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3CscdLlhFitParams & params); 
    size_t FillRows(const I3CscdLlhFitParams& params, I3TableRowPtr rows);
};
    

#endif  // I3CSCDLLHFITPARAMSCONVERTER_H_INCLUDED
