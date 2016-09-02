/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3LaputopParamsConverter.h 124034 2014-10-01 19:52:02Z jgonzalez $
 *
 * @version $Revision: 124034 $
 * @date $LastChangedDate: 2014-10-01 15:52:02 -0400 (Wed, 01 Oct 2014) $
 * @author Eike Middell <eike.middell@desy.de> $LastChangedBy: jgonzalez $
 */

#include <tableio/I3Converter.h>
#include <recclasses/I3LaputopParams.h>

/// Converter for I3LaputopParams
class I3LaputopParamsConverter : public I3ConverterImplementation<I3LaputopParams > {
private:
    I3TableRowDescriptionPtr CreateDescription(const I3LaputopParams& params);
    size_t FillRows(const I3LaputopParams& params, I3TableRowPtr rows);
};

