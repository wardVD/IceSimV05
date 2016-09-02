#ifndef CRAMERRAOPARAMSCONVERTER_H_INCLUDED
#define CRAMERRAOPARAMSCONVERTER_H_INCLUDED
/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3CramerRaoParamsConverter.h 136731 2015-08-21 21:52:17Z nega $
 *
 * @version $Revision: 136731 $
 * @date $LastChangedDate: 2015-08-21 23:52:17 +0200 (Fri, 21 Aug 2015) $
 * @author Fabian Kislat <fabian.kislat@desy.de> $LastChangedBy: nega $
 */

#include "tableio/I3Converter.h"
#include "recclasses/CramerRaoParams.h"

/// Converter for CramerRaoParams
class CramerRaoParamsConverter : public I3ConverterImplementation<CramerRaoParams> {
private:
	I3TableRowDescriptionPtr CreateDescription(const CramerRaoParams &p);
	size_t FillRows(const CramerRaoParams &p, I3TableRowPtr rows);
};

#endif  // I3CRAMERRAOPARAMSCONVERTER_H_INCLUDED
