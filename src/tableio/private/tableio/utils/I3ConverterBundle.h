/**
 * copyright  (C) 2010
 * The Icecube Collaboration
 *
 * $Id: I3ConverterBundle.h 143491 2016-03-19 06:28:17Z cweaver $
 *
 * @version $Revision: 143491 $
 * @date $LastChangedDate: 2016-03-19 01:28:17 -0500 (Sat, 19 Mar 2016) $
 * @author Jakob van Santen <vansanten@wisc.edu> $LastChangedBy: cweaver $
 */

#ifndef TABLEIO_UTILS_I3CONVERTERBUNDLE_H_INCLUDED
#define TABLEIO_UTILS_I3CONVERTERBUNDLE_H_INCLUDED

#include <tableio/I3Converter.h>

// I3ConverterBundlerepresents a bundle of converters all acting on the same frame object
// this can be used to extend the functionality of compiled bookers in a 
// more user-friendly way than with multiple inheritance
class I3ConverterBundle : public I3Converter {
    public:
        I3ConverterBundle(std::vector<I3ConverterPtr>& converters);
        // virtual ~I3ConverterBundle();
        virtual size_t GetNumberOfRows(I3FrameObjectConstPtr object);
        virtual size_t GetNumberOfRows(const I3FrameObject& object);
        virtual I3TableRowDescriptionConstPtr GetDescription(I3FrameObjectConstPtr object);
        virtual I3TableRowDescriptionConstPtr GetDescription(const I3FrameObject& object);
        virtual size_t Convert(I3FrameObjectConstPtr object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr());
        
        virtual size_t Convert(const I3FrameObject& object, 
                                     I3TableRowPtr rows, 
                                     I3FramePtr frame=I3FramePtr());

        virtual ConvertState CanConvert(I3FrameObjectConstPtr object);
    private:
        I3ConverterBundle();
        std::vector<I3ConverterPtr> converters_;
};

#endif // TABLEIO_UTILS_I3CONVERTERBUNDLE_H_INCLUDED
