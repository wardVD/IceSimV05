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
#include <recclasses/I3TopLateralFitParams.h>

/// Converter for I3TopLateralFitParams
class I3TopLateralFitParamsConverter : public I3ConverterImplementation<I3TopLateralFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopLateralFitParams& params);
    size_t FillRows(const I3TopLateralFitParams& params, I3TableRowPtr rows);
};

