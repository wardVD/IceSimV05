#!/usr/bin/env python

"""
The Honda tables are not exactly
easy to parse, so I'm going to change 
their format for easier readability.

D. Jason Koskinen
Michael Larson
Feb. 25, 2014
"""

import os, numpy

FluxFileName = "honda2012_spl_solmin.d"

def textParsing(text, FluxFileName):

    rawText = open("../data/%s" % FluxFileName)


    outfile = open("honda_tmp.txt", 'w')

    nline = 0
    
    for line in rawText.readlines():
        if ( nline > 130700):
            return
        # end if()
        nline += 1
        if len(line) > 56 and len(line) < 70:
            for i in line.split():
                i = i.replace(",", "")
                i = i.replace("]", "")
                i = i.replace("=", "")
                if len(i) > 0 and i[0].isdigit() or (len(i) > 0 and i[1].isdigit()):
                    outfile.write("%s " % i)
                # end if()
            # end for()
            outfile.write("\n")
        elif len(line) is 56:
            outfile.write(line)
            continue
        # end if()
    ## end for()

    return
# end def()


textParsing("ijij", FluxFileName)

azimuth = 0
zenith  = 0

azimuth_list = []
zenith_list  = []

lines   = open("honda_tmp.txt", "r").readlines()
current = []
total   = {}

for line in lines:
    splits = line.split()
    if len(splits) == 4:
        if not (azimuth == 0 and zenith == 0):
            total[(azimuth, zenith)] = current
        # end if()
        
        azimuth = (splits[2], splits[3])
        zenith  = (splits[0], splits[1])

        if azimuth not in azimuth_list:
            azimuth_list.append(azimuth)
        # end if()
        if zenith not in zenith_list:
            zenith_list.append(zenith)
        # end if()
        current = [line]

        continue
    # end if()
    current.append(line)
# end for()

total[(azimuth, zenith)] = current

zenith_list.reverse()

outfile = open( ("%s_easier.txt" % FluxFileName.split(".")[0]), "w")

for azimuth in azimuth_list:
    for zenith in zenith_list:
        key = (azimuth, zenith)
        outfile.writelines( total[key] )
    # end for()
# end for()

os.remove("honda_tmp.txt")

