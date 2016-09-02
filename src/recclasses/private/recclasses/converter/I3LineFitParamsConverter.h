#ifndef I3LINEFITPARAMSCONVERTER_H_INCLUDED
#define I3LINEFITPARAMSCONVERTER_H_INCLUDED
/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy$
 */

#include <tableio/I3Converter.h>
#include "recclasses/I3LineFitParams.h"

/// Converter for I3LineFitParams
class I3LineFitParamsConverter : public I3ConverterImplementation<I3LineFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3LineFitParams &params); 
    size_t FillRows(const I3LineFitParams &params, I3TableRowPtr rows);
};

#endif  // I3LINEFITPARAMSCONVERTER_H_INCLUDED
