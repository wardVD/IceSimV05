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

#ifndef FILLRATIO_I3FILLRATIOINFOCONVERTER_H_INCLUDED
#define FILLRATIO_I3FILLRATIOINFOCONVERTER_H_INCLUDED

#include "tableio/I3Converter.h"
#include "recclasses/I3FillRatioInfo.h"

/// Converter for I3FillRatioInfo
class I3FillRatioInfoConverter : public I3ConverterImplementation<I3FillRatioInfo > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3FillRatioInfo & params); 
    size_t FillRows(const I3FillRatioInfo& params, I3TableRowPtr rows);
};

#endif // FILLRATIO_I3FILLRATIOINFOCONVERTER_H_INCLUDED
