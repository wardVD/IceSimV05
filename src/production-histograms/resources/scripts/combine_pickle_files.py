#!/usr/bin/env python

'''
This script is a utility which combines histograms from multiple pickle
files into a single pickle file.  All pickle files should have the
same structure.  The purpose of this script is to combine the results
of a production run.  The 'first' random pickle is used as a template
for the rest, so they do really need to be the same.
'''

from optparse import OptionParser

parser = OptionParser()

parser.add_option("-i","--inpath",
                  dest="INPATH", 
                  help="Path to input pickle files.")

parser.add_option("-o","--output_pickle_filename",
                  dest="OUTFILENAME", 
                  help="Name of output file with combined histograms.")

(options, args) = parser.parse_args()

import os
import cPickle as pickle
import glob

file_list = glob.glob(options.INPATH)

from copy import deepcopy
def add(h1, h2):

    # Make sure the two histograms are of the same 'type.'
    # They have to have the same name, ranges, and number of bins.
    if((h1.xmin == h2.xmin) and\
       (h1.xmax == h2.xmax) and\
       (h1.name == h2.name) and\
       (len(h1.bin_values) == len(h2.bin_values))):

        h = deepcopy(h1)
        for bin_index, bin_value in enumerate(h2.bin_values):
            h.bin_values[bin_index] += bin_value 
        return h
    else:
       print("ERROR : not able to combine %s and %s" % (h1.name, h2.name))

histograms = None
for fn in file_list :
    f = open(fn)
    if not histograms :
        histograms = pickle.load(f)
        f.close()
        continue

    new_histograms = pickle.load(f)
    for key in histograms.keys() :
        histograms[key] = add(histograms[key], new_histograms[key])
    f.close()

f = open(options.OUTFILENAME, "w")
pickle.dump(histograms,f)
f.close()
