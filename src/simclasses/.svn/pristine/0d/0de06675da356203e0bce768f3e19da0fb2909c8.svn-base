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

#ifndef TOPSIMULATOR_I3CORSIKASHOWERINFOCONVERTER_H_INCLUDED
#define TOPSIMULATOR_I3CORSIKASHOWERINFOCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include "simclasses/I3CorsikaShowerInfo.h"

class I3CorsikaShowerInfoConverter : public I3ConverterImplementation< I3CorsikaShowerInfo > {
public:
    I3CorsikaShowerInfoConverter();

private:
    I3TableRowDescriptionPtr CreateDescription(const I3CorsikaShowerInfo& info);
    size_t FillRows(const I3CorsikaShowerInfo& info, I3TableRowPtr rows);

    size_t nLongSteps_;
};

#endif // TOPSIMULATOR_I3CORSIKASHOWERINFOCONVERTER_H_INCLUDED
