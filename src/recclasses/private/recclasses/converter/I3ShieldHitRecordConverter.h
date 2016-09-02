#ifndef I3SHIELDHITRECORDCONVERTER_H_INCLUDED
#define I3SHIELDHITRECORDCONVERTER_H_INCLUDED

#include <tableio/I3Converter.h>
#include <recclasses/I3ShieldHitRecord.h>
#include <tableio/converter/I3VectorConverter.h>

/// Converter for I3ShieldHitRecord
class I3ShieldHitRecordConverter : public I3ConverterImplementation<I3ShieldHitRecord> {
private:
	virtual I3TableRowDescriptionPtr CreateDescription(const I3ShieldHitRecord &r);
	virtual size_t FillRows(const I3ShieldHitRecord &r, I3TableRowPtr rows);
};
#endif  // I3SHIELDHITRECORDCONVERTER_H_INCLUDED
