import numpy

from icecube.photospline import spglam as glam
from icecube.photospline.glam.bspline import bspline
from icecube.photospline.splinetable import SplineTable
from icecube.photospline import splinefitstable
import os, numpy


def CreateSplineFits(fluxname, flavorname):
    type = 0

    if flavorname.lower() == "numu":
        flavorname = "numu"
        type = 1
    elif flavorname.lower() == "numubar":
        flavorname = "numubar"
        type = 2
    elif flavorname.lower() == "nue":
        flavorname = "nue"
        type = 3
    elif flavorname.lower() == "nuebar":
        flavorname = "nuebar"
        type = 4
    else:
        print "not an accurate flavor type"
        return 
    # end if()
        
    hondaFile = open("%s_easier.txt" % fluxname, "r").readlines()

    # ##############################
    # This is the number of bins to prepend to the arrays
    # in order to have a 'smooth' ramp up to the initial flux
    # values. Otherwise the spline functions go to zero at the
    # extrapolation edges.
    #
    # The step sizes in cosZenith, azimuth, and energy are
    # set from the original knots in the honda calculations.
    # ##############################

    nbuffer = 5

    nEnergy  = 0
    nAzimuth = 0
    nZenith  = 0

    energy_values  = numpy.linspace( 0, 0.05, nbuffer).tolist()
    azimuth_values = numpy.linspace( -15-(nbuffer*30), -15, nbuffer).tolist()
    zenith_values  = numpy.linspace( -1.05-(nbuffer*0.1), -1.05, nbuffer).tolist()

    for line in hondaFile:
        splitLine = line.split()

        # Check to see if this is a new grouping of azimuth/zenith
        if len(splitLine) == 4:
            zenith  = (float(splitLine[1]) + float(splitLine[0]))/2.0
            azimuth = (float(splitLine[3]) + float(splitLine[2]))/2.0

            if zenith not in zenith_values:
                zenith_values.append(zenith)
                nZenith = nZenith + 1
            # end if()

            if azimuth not in azimuth_values:
                azimuth_values.append(azimuth)
                nAzimuth = nAzimuth + 1
            # end if()
            continue
        # end if()

        energy = float(splitLine[0])

        if energy not in energy_values:
            energy_values.append(energy)
            nEnergy = nEnergy + 1
        # end if()
    # end for()

    energy_values.extend(numpy.linspace(energy_values[-1]*1.1, energy_values[-1]*2, nbuffer).tolist())
    azimuth_values.extend(numpy.linspace(375, 375+(nbuffer*30), nbuffer).tolist())
    zenith_values.extend(numpy.linspace(1.05, 1.05+(nbuffer*0.1), nbuffer).tolist())

    # ##############################
    # Now start looping over a second time after we create the 3d flux table
    # to fill the 3d flux table with the flux values
    # ##############################

    flux_3d = numpy.zeros( (len(zenith_values), len(azimuth_values), len(energy_values)))

    azimuth = 0
    zenith  = 0

    for line in hondaFile:
        splitLine = line.split()
        if len(splitLine) == 4:
            zenith  = (float(splitLine[1]) + float(splitLine[0]))/2.0
            azimuth = (float(splitLine[3]) + float(splitLine[2]))/2.0
            continue
        # end if()

        energy = float(splitLine[0])
        flux   = float(splitLine[type])

        # 3D values, which need to be converted to cm^-2 from m^-2 in order to match convention
        flux_3d[ zenith_values.index(zenith) ][ azimuth_values.index(azimuth) ][ energy_values.index(energy) ] = flux * 1e-4

    # end for()

    # Reflect the zenith bins
    for azi in range(0, len(azimuth_values)):
        for en in range(0, nEnergy):
            for i in range (0, nbuffer+1):
                #flux_3d[nbuffer-i][azi][en] = flux_3d[nbuffer+i][azi][en]
                flux_3d[nbuffer-i][azi][en] = flux_3d[nbuffer][azi][en]
            # end for()

            for i in range (0, nbuffer+1):
                #flux_3d[nZenith+nbuffer+i-1][azi][en] = flux_3d[nZenith+nbuffer-i-1][azi][en]
                flux_3d[nZenith+nbuffer+i-1][azi][en] = flux_3d[nZenith+nbuffer-1][azi][en]
            # end for()
        # end for()
    # end for()

    # Wrap the azimuth bins
    for zen in range(0, len(zenith_values)):
        for i in range(0, nbuffer):
            flux_3d[zen][i] = flux_3d[zen][i+nAzimuth-nbuffer][:]

            flux_3d[zen][nbuffer+nAzimuth+i] = flux_3d[zen][i+nbuffer][:]
        # end for()
    # end for()

    # Pad the energy bins
    for zen in range(0, len(zenith_values)):
        for azi in range(0, len(azimuth_values)):
            # lower buffer
            for i in range(0, nbuffer):
                flux_3d[zen][azi][i] = flux_3d[zen][azi][nbuffer]
            # end for()
            # upper buffer
            for i in range(nbuffer+nEnergy, nEnergy + 2*nbuffer):
                flux_3d[zen][azi][i] = flux_3d[zen][azi][nEnergy+nbuffer]
            # end for()
        # end for()
    # end for()

    #****************************************************
    # and now splining stuff
    #
    # From the photospline documentation, spglam runs 
    # much, much quicker and reduces the spline time for
    # the HONDA 2012 flux from 8-10 minutes to < 15 seconds.
    #****************************************************


    # spline parameters, where smoothness is the degree 
    # to which the spline should be forced to go through
    # the knots.

    order  = 1
    smooth = 0

    array_energy  = numpy.array(energy_values)
    array_azimuth = numpy.array(azimuth_values)
    array_zenith  = numpy.array(zenith_values)

    # knot selection
    # 1 is all knots, 2 = half the knots, 3 = third of the knots, etc

    step = 1

    knots_energy = numpy.array(numpy.array(energy_values[:nbuffer]))
    knots_energy = numpy.append(knots_energy,
                                [numpy.array([energy_values[i] for i in range(nbuffer, len(energy_values)-nbuffer, step)]),
                                 ])
    knots_energy = numpy.append(knots_energy, 
                                numpy.array(energy_values[-nbuffer:])
                                )

    knots_azimuth = numpy.array(azimuth_values[:nbuffer])
    knots_azimuth = numpy.append(knots_azimuth,
                                [numpy.array([azimuth_values[i] for i in range(nbuffer, len(azimuth_values)-nbuffer, step)]),
                                 ])
    knots_azimuth = numpy.append(knots_azimuth, 
                                numpy.array(azimuth_values[-nbuffer:])
                                )

    knots_zenith = numpy.array(zenith_values[:nbuffer])
    knots_zenith = numpy.append(knots_zenith,
                                [numpy.array([zenith_values[i] for i in range(nbuffer, len(zenith_values)-nbuffer, step)]),
                                 ])
    knots_zenith = numpy.append(knots_zenith, 
                                numpy.array(zenith_values[-nbuffer:])
                                )

    # run the spline function

    result_3d = glam.fit( flux_3d, 
                          numpy.ones(flux_3d.shape), 
                          [array_zenith, array_azimuth, array_energy], 
                          [knots_zenith, knots_azimuth, knots_energy], 
                          order,
                          smooth,
        )

    # ##############################
    # Write out the values of the interpolation fits
    # to a file to be read in by the flux module
    # ##############################

    if os.path.isfile("../data/%s_%s.fits" % (fluxname, flavorname)):
        os.remove("../data/%s_%s.fits" % (fluxname, flavorname))
    # end if()
    splinefitstable.write( result_3d, "../data/%s_%s.fits" % (fluxname, flavorname))

# end def()


CreateSplineFits("honda2012_spl_solmin", "numu")
CreateSplineFits("honda2012_spl_solmin", "numubar")
CreateSplineFits("honda2012_spl_solmin", "nue")
CreateSplineFits("honda2012_spl_solmin", "nuebar")
