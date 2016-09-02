/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3TopRecoPlaneFitParamsConverter.h 144008 2016-03-30 21:24:16Z hdembinski $
 *
 * @version $Revision: 144008 $
 * @date $LastChangedDate: 2016-03-30 16:24:16 -0500 (Wed, 30 Mar 2016) $
 * @author Fabian Kislat <fabian.kislat@desy.de>, last changed by $LastChangedBy: hdembinski $
 */

#include <tableio/I3Converter.h>
#include <recclasses/I3TopRecoPlaneFitParams.h>

/// Converter for I3TopRecoPlaneFitParams
class I3TopRecoPlaneFitParamsConverter : public I3ConverterImplementation<I3TopRecoPlaneFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopRecoPlaneFitParams& params);
    size_t FillRows(const I3TopRecoPlaneFitParams& params, I3TableRowPtr rows);
};

