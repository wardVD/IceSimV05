# ######################################################################
# D. Jason Koskinen
# March 25, 2014
#
# The following code checks the flux differences between the splines created
# from the honda tables and the values from the actual honda tables
# ######################################################################

from icecube.photospline.glam import glam
from icecube.photospline import splinefitstable
import os, math, numpy

import ROOT

from ROOT import TH1D, TH2D, TCanvas, TGraph, TStyle
from ROOT import gStyle, TColor

from array import array

def CheckSplineFlavor(flavorname):

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


    result_3d = splinefitstable.read("../data/honda2012_3d_spl_%s.fits" % flavorname)

    nEnergy  = 0
    nAzimuth = 0
    nZenith  = 0

    energy_values  = []
    azimuth_values = []
    zenith_values  = []

    # ##############################
    # Get the actual values from the
    # Honda tables for the flux at specific
    # values of azimuth, energy, and zenith.
    # The values of azi, en, and zen will 
    # be used as evaluation points for the 
    # spline in order to do a closure test.
    #
    # Known prior number of bins.
    # ##############################

    flux_3d = numpy.zeros( (30, 22, 111) )

    for line in open("honda_easier.txt", "r").readlines():
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
        flux   = float(splitLine[type])

        if energy not in energy_values:
            energy_values.append(energy)
            nEnergy = nEnergy + 1
        # end if()

        flux_3d[ zenith_values.index(zenith) ][ azimuth_values.index(azimuth) ][ energy_values.index(energy) ] = flux

    # end for()

    # ######################################################################
    # Create the histograms which will hold the differences between 
    # the spline and the Honda tables.
    # ######################################################################


    # ##############################
    # All the difference values from the
    # 3D Splines
    # ##############################

    hDiff_3D = TH1D("hist_diff_3d", "Difference 3D; flux ratio [(spline-Honda)/Honda];", 100, -0.002, 0.002)


    # ##############################
    # Creates 2D histograms of the difference in flux
    # between Honda and the splines in 1D
    # slices, e.g. the azimuth versus zenith flux
    # difference for a single energy.
    # ##############################

    hD_EnergyVAzi = []
    hD_EnergyVZen = []
    hD_AziVZen    = []

    for i in range( 0, len(zenith_values)):
        hD_EnergyVAzi.append( TH2D("EVAzi_Zen%f" % zenith_values[i], "Zenith=%f" % zenith_values[i], 50, 0, 360, 50, 0, 10))
    # end for()

    for i in range( 0, len(azimuth_values)):
        hD_EnergyVZen.append( TH2D("EVZen_Azi%f" % azimuth_values[i], "Azimuth=%f" % azimuth_values[i], 50, -1, 1, 50, 0, 10))
    # end for()

    for i in range( 0, len(energy_values)):
        hD_AziVZen.append( TH2D("AziVZen_En%f" % energy_values[i], "(Spline-Honda)/Honda  En=%0.2e" % energy_values[i], 50, 0, 360, 50, -1, 1))
    # end for()

    points_energy  = energy_values
    points_azimuth = azimuth_values
    points_zenith  = zenith_values

    values_3d = glam.grideval(result_3d, [points_zenith, points_azimuth, points_energy])

    print energy_values

    for n in range(0, len(points_energy)):
        for m in range(0, len(points_azimuth)):
            for k in range(0, len(points_zenith)):
                difference = (values_3d[k][m][n] - flux_3d[k][m][n])/flux_3d[k][m][n]
                hDiff_3D.Fill( difference)
                hD_AziVZen[n].Fill( azimuth_values[m], zenith_values[k], difference)
                #if abs(difference) > 0.05:
                #    print " 3D (%6.4e %6.4e %6.4e) %6.4e %6.4e     %4.d " % (energy_values[n], azimuth_values[m], zenith_values[k], values_3d[k][m][n], flux_3d[k][m][n], 100*(difference))
                # end if()
            # end for()
        #end for
    #end for

    # ##############################
    # Draw the histogram of the 
    # differences in flux between the 
    # splines and the calculated Honda values
    # for all knot values in azi, zen, and 
    # energy.
    # ##############################

    TCan1 = TCanvas("a", "a", 700, 700)

    TCan1.cd()
    hDiff_3D.Draw()

    # ##############################
    # Draw the 2D slices of azi versus zen
    # from 0.1 GeV to 10 GeV
    # of the differences in flux between the 
    # splines and the calculated Honda values
    # ##############################

    Number = 6;
    NCont = 600;

    Stops = array('d', [0.0, 0.2, 0.45, 0.55, 0.8, 1.00])

    Red   = array('d', [0.0, 0.2,  1.0,  1.0, 0.9,  0.6])
    Green = array('d', [0.0, 0.0,  1.0,  1.0, 0.0,  0.0])
    Blue  = array('d', [0.6, 0.9,  1.0,  1.0, 0.2,  0.0])

    TColor.CreateGradientColorTable(Number, Stops, Red, Green, Blue, NCont)

    TCan2 = TCanvas("b", "b", 1300, 1300)

    TCan2.Divide(7,6)
    for i in range(0, 41):
        TCan2.cd(1+i)
        hD_AziVZen[i].Draw("colz")
        hD_AziVZen[i].SetStats(0)
        hD_AziVZen[i].GetXaxis().SetTitle("azimuth [deg]")
        hD_AziVZen[i].GetYaxis().SetTitle("cos(zenith)")
        hD_AziVZen[i].GetZaxis().SetRangeUser(-1e-7, 1e-7)
    # end for()

    TCan3 = TCanvas("c", "c", 800, 800)
    TCan3.cd()
    #hD_AziVZen[0].SetTitle("(Spline-Honda)/Honda E=")
    hD_AziVZen[0].Draw("colz")
    hD_AziVZen[0].GetZaxis().SetRangeUser( -1e-7, 1e-7)
    hD_AziVZen[0].GetZaxis().SetRangeUser( -1e-7, 1e-7)

    raw_input ( 'Press <ret > to end -> ' )

    del TCan1
    del TCan2
    del TCan3
    
# end def()

CheckSplineFlavor("numu")
CheckSplineFlavor("numubar")
CheckSplineFlavor("nue")
CheckSplineFlavor("nuebar")
