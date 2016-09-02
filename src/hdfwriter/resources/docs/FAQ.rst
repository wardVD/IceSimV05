.. _hdfwriter-faq:

FAQ
===

Can I use hdfwriter to write from files with only Q frames, or would I have to create P frames on my own with those objects that I want to have in the hdf5 file?
-----------------------------------------------------------------------------------------------------------------------------------------------------------------

The later. Put an I3NullSplitter in front of I3HDFWriter.

I get errors like FATAL (tableio): trying to get the address of unknown field OFUFilter_14_BadGCD (I3TableRow.h:221 in T *I3TableRow::GetPointer(const std::string &, size_t) [T = bool])
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

I3FilterResultMapConverter assumes that the set of filter names is fixed for a
run. Either book runs with different filter configurations separately, or
remove the new keys from each FilterMask if you do not need them.
