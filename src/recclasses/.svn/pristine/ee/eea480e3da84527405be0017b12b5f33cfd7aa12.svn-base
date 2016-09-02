/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id$
 *
 * @version $Revision$
 * @date $LastChangedDate$
 * @author Fabian Kislat <fabian.kislat@desy.de>, last changed by $LastChangedBy$
 */

#include <tableio/I3Converter.h>
#include <recclasses/I3TopRecoPlaneFitParams.h>

/// Converter for I3TopRecoPlaneFitParams
class I3TopRecoPlaneFitParamsConverter : public I3ConverterImplementation<I3TopRecoPlaneFitParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3TopRecoPlaneFitParams& params);
    size_t FillRows(const I3TopRecoPlaneFitParams& params, I3TableRowPtr rows);
};

