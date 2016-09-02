/**
 * copyright  (C) 2011
 * The Icecube Collaboration
 *
 * $Id: I3CLastFitParamsConverter.h 2011-11-25 $
 *
 * @version $Revision: $
 * @date $ $
 * @author Lars Mohrmann <lars.mohrmann@desy.de> $LastChangedBy: $
 */
#ifndef I3CLASTFITPARAMSCONVERTER_H_INCLUDED
#define I3CLASTFITPARAMSCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include "recclasses/I3CLastFitParams.h"

/// Converter for I3CLastFitParams
class I3CLastFitParamsConverter : public I3ConverterImplementation<I3CLastFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3CLastFitParams &params); 
    size_t FillRows(const I3CLastFitParams &params, I3TableRowPtr rows);
};

#endif  // I3CLASTFITPARAMSCONVERTER_H_INCLUDED
