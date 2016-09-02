/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TopLateralFitParamsConverter.h 144008 2016-03-30 21:24:16Z hdembinski $
 *
 * @version $Revision: 144008 $
 * @date $LastChangedDate: 2016-03-30 16:24:16 -0500 (Wed, 30 Mar 2016) $
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy: hdembinski $
 */

#include <tableio/I3Converter.h>
#include <recclasses/I3TopLateralFitParams.h>

/// Converter for I3TopLateralFitParams
class I3TopLateralFitParamsConverter : public I3ConverterImplementation<I3TopLateralFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopLateralFitParams& params);
    size_t FillRows(const I3TopLateralFitParams& params, I3TableRowPtr rows);
};

