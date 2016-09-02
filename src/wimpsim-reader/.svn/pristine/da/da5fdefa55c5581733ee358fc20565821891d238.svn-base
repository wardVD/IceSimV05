#!/usr/bin/python
# -*- coding: utf-8 -*-
###
# A script to analyse the vertex position and energy distribution of wimpsim-generated files
###

import os, sys, glob
import random, numpy
from optparse import OptionParser

from I3Tray import *
from icecube import icetray, dataclasses, dataio
from icecube import wimpsim_reader
from icecube import phys_services

parser = OptionParser()
#parser.add_option("-g", "--GCDfile", action="store", default="GCD.i3.gz", dest="GCD", help="Name of the GCD file")
parser.add_option("-i", "--infile", action="store", default=os.path.expandvars("$I3_SRC/wimpsim-reader/resources/test_data_sun.dat"), dest="INFILE", help="Name of the input (.dat) file")
parser.add_option("-o", "--outfile", action="store", default="wimpsimreader_plots", dest="OUTFILE", help="Name of the output (.root) file")
parser.add_option("-n", "--number", action="store", type="int", default="0", dest="NUMBER", help="Number of events to process")
parser.add_option("-s", "--seed", action="store", type="int", default="0", dest="SEED", help="Seed to propagate")
parser.add_option("--i3file", action="store_true", default=False, dest="I3FILE", help="should write i3-file")
parser.add_option("--rootfile", action="store_true", default=False, dest="ROOTFILE", help="should write root-file")

(options, args) = parser.parse_args()

### ROOT Histogram definitions

####################### Tray 0 ###############################

tray = I3Tray()
tray.AddService("I3SPRNGRandomServiceFactory","sprngrandom",
  NStreams = 2,
  Seed = options.SEED,
  StreamNum = 1,)

tray.AddModule("I3WimpSimReader","wimpsim-reader",
  FileNameList = [options.INFILE], #default ""
#  NEvents = 0, #default 0
#  StartMJD = 55694, #default NAN
#  EndMJD = (55694+365.25), #default NAN
#  Oversampling = 0, #default 0
  PositionLimits = [-800.,800.,-800.,800.,-800.,800.], #default [-800,800,-800,800,-800,800]
  InjectionRadius = 700.*I3Units.meter, #default NAN
#  LowerZenCut = 0*I3Units.degree, #default 0*I3Units.degree,
#  UpperZenCut = 180*I3Units.degree, #default 180*I3Units.degree
  SensitiveHeight = 1300*I3Units.meter, #default NAN
  SensitiveRadius = 700*I3Units.meter, #default NAN
  UseElectrons = False, #default False
  UseMuons = True, #default True
  UseTaus = False, #default False
  UseNC = False, #default False
  )
"""
def select(frame):
  if frame.Has("I3MCTree"):
    if dataclasses.get_most_energetic_muon(frame["I3MCTree"]) == None:
      return True
    else:
      return False

tray.AddModule(select, "selectmuons",
  Streams = [icetray.I3Frame.DAQ])
"""
if (options.ROOTFILE):
  from ROOT import gROOT, TCanvas, TH1D, TH2D, TFile

  gROOT.Reset()

  histo_MCPrimary_zenith_nuweight = TH1D("MCPrimary_zenith_nuweight","MCPrimary_Zenith_nuweight;zenith [rad];[flux]",100,0.,3.1415)
  histo_MCPrimary_azimuth_nuweight = TH1D("MCPrimary_azimuth_nuweight","MCPrimary_Azimuth_nuweight;azimuth [rad];[flux]",100,0.,2*3.1415)
  histo_MCPrimary_energy_nuweight = TH1D("MCPrimary_energy_nuweight","MCPrimary_Energy_nuweight;log(energy) [GeV];[flux]",100,0,3)
  histo_MCPrimary_x = TH1D("MCPrimary_x","MCPrimary_x;pos_x [m];[]",100,-1000,+1000)
  histo_MCPrimary_y = TH1D("MCPrimary_y","MCPrimary_y;pos_y [m];[]",100,-1000,+1000)
  histo_MCPrimary_z = TH1D("MCPrimary_z","MCPrimary_z;pos_z [m];[]",100,-1000,+1000)
  histo_wimp_params_nu_weight = TH1D("WIMPparams_nu_weight","WIMPparams_nu_weight;weight [];[]",100,1e-34,1e-35)
  histo_wimp_params_lep_weight = TH1D("WIMPparams_lep_weight","WIMPparams_lep_weight;log10(weight) [];[]",100,-47,-49)
  histo_wimp_params_had_weight = TH1D("WIMPparams_had_weight","WIMPparams_had_weight;log10(weight) [];[]",100,-47,-49)
  histo_wimp_params_vgen = TH1D("WIMPparams_vgen","WIMPparams_vgen;vgen [];[]",100,1e+9,1e+10)
  histo_wimp_params_time = TH1D("WIMPparams_time","WIMPparams_time;time [mjd];[]",100,55690,56090)
  histo_wimp_params_time_nuweight = TH1D("WIMPparams_time_nuweight","WIMPparams_time_nuweight;time [mjd];[]",100,55690,56090)
  histo_MCMuon_zenith_nuweight = TH1D("MCMuon_zenith_nuweight","MCMuon_Zenith_nuweight;zenith [rad];[flux]",100,0.,3.1415)
  histo_MCMuon_azimuth_nuweight = TH1D("MCMuon_azimuth_nuweight","MCMuon_Azimuth_nuweight;azimuth [rad];[flux]",100,0.,2*3.1415)
  histo_MCMuon_energy_nuweight = TH1D("MCMuon_energy_nuweight","MCMuon_Energy_nuweight;energy [GeV];[flux]",100,-3,3)
  histo_MC_angle = TH1D("MC_angle","PointSpread;angle [rad];[]",100,0.,3.1415/2.)
  histo_MC_angle_lepweight = TH1D("MC_angle_lepweight","PointSpread lep_weighted;angle [rad];[]",100,0.,3.1415/2.)

  histosTH1D=[
    histo_MCPrimary_zenith_nuweight,
    histo_MCPrimary_azimuth_nuweight,
    histo_MCPrimary_energy_nuweight,
    histo_MCPrimary_x,
    histo_MCPrimary_y,
    histo_MCPrimary_z,
    histo_wimp_params_nu_weight,
    histo_wimp_params_lep_weight,
    histo_wimp_params_had_weight,
    histo_wimp_params_vgen,
    histo_wimp_params_time,
    histo_wimp_params_time_nuweight,
    histo_MCMuon_zenith_nuweight,
    histo_MCMuon_azimuth_nuweight,
    histo_MCMuon_energy_nuweight,
    histo_MC_angle,
    histo_MC_angle_lepweight,
    ]

  histo_MCPrimary_xy = TH2D("MCPrimary_xy","MCPrimary Vertex (xy);x [m];y [m];[]",100,-1000,+1000,100,-1000,+1000)
  histo_MCPrimary_xz = TH2D("MCPrimary_xz","MCPrimary Vertex (xz);x [m];z [m];[]",100,-1000,+1000,100,-1000,+1000)
  histo_MCPrimary_yz = TH2D("MCPrimary_yz","MCPrimary Vertex (yz);y [m];z [m];[]",100,-1000,+1000,100,-1000,+1000)
  histo_nuenergy_lepweight = TH2D("MC_nuenergy_lepweight","MC_nuenergy_lepweight;log(energy) [GeV];log(weight) [];[]",100,0,3,100,-49,-44)
  histo_nuenergy_lepweight_lepweight = TH2D("MC_nuenergy_lepweight_lepweight","MC_nuenergy_lepweight_lepweight;log(energy) [GeV];log(weight) [];[]",100,0,3,100,-49,-44)
  histo_MC_energy_angle = TH2D("MC_nuenergy_angle","MC_nuenergy_angle;log(energy) [GeV];angle [rad];[]",100,0,3,100,0.,3.1415/2.)
  histo_MC_nuenergy_angle_lepweight = TH2D("MC_nuenergy_angle_lepweight","MC_nuenergy_angle_lepweight;log(energy) [GeV];angle [rad];[]",100,0,3,1000,0.,3.1415/2.)
  histo_MC_muenergy_angle_lepweight = TH2D("MC_muenergy_angle_lepweight","MC_muenergy_angle_lepweight;log(energy) [GeV];angle [rad];[]",100,0,3,1000,0.,3.1415/2.)

  histosTH2D=[
    histo_MCPrimary_xy,
    histo_MCPrimary_xz,
    histo_MCPrimary_yz,
    histo_MC_energy_angle,
    histo_MC_nuenergy_angle_lepweight,
    histo_MC_muenergy_angle_lepweight,
    histo_nuenergy_lepweight,
    histo_nuenergy_lepweight_lepweight,
    ]
  ### EXTRACTION BLOCK ###  
  from icecube import icetray, dataclasses
  class HistoFiller(icetray.I3ConditionalModule):
    def __init__(self, context):
      # Call __init__ method of the I3ConditionalModule class.
      super(HistoFiller, self).__init__(context)
      self.AddOutBox("OutBox")
    #-------------------
    def Configure(self):
      pass
    #-------------------
    def DAQ(self, frame):
      if frame.Has("I3MCTree"):
        MCprimary = dataclasses.get_most_energetic_primary(frame["I3MCTree"])
        MCmuon = dataclasses.get_most_energetic_muon(frame["I3MCTree"])
        histo_MCPrimary_x.Fill(MCprimary.pos.x)
        histo_MCPrimary_y.Fill(MCprimary.pos.y)
        histo_MCPrimary_z.Fill(MCprimary.pos.z)
        histo_MCPrimary_xy.Fill(MCprimary.pos.x, MCprimary.pos.y)
        histo_MCPrimary_xz.Fill(MCprimary.pos.x, MCprimary.pos.z)
        histo_MCPrimary_yz.Fill(MCprimary.pos.y, MCprimary.pos.z)
        histo_MC_angle.Fill(phys_services.I3Calculator.angle(MCprimary,MCmuon))
        histo_MC_energy_angle.Fill(numpy.log10(MCprimary.energy), phys_services.I3Calculator.angle(MCprimary,MCmuon))
      if frame.Has("WIMP_params"):
	wimp_params = frame["WIMP_params"]
	histo_wimp_params_nu_weight.Fill(wimp_params.nu_weight)
	histo_wimp_params_lep_weight.Fill(numpy.log10(wimp_params.lep_weight))
	histo_wimp_params_had_weight.Fill(numpy.log10(wimp_params.had_weight))
	histo_wimp_params_vgen.Fill(wimp_params.vgen)
	histo_wimp_params_time.Fill(float(wimp_params.time.mod_julian_day_double))
      if frame.Has("WIMP_params") and frame.Has("I3MCTree"):
	histo_wimp_params_time_nuweight.Fill(float(wimp_params.time.mod_julian_day_double), wimp_params.nu_weight)
	histo_MCPrimary_zenith_nuweight.Fill(MCprimary.dir.zenith, wimp_params.nu_weight)
	histo_MCPrimary_azimuth_nuweight.Fill(MCprimary.dir.azimuth, wimp_params.nu_weight)
	histo_MCPrimary_energy_nuweight.Fill(numpy.log10(MCprimary.energy), wimp_params.nu_weight)
	histo_MCMuon_zenith_nuweight.Fill(MCmuon.dir.zenith, wimp_params.nu_weight)
	histo_MCMuon_azimuth_nuweight.Fill(MCmuon.dir.azimuth, wimp_params.nu_weight)
	histo_MCMuon_energy_nuweight.Fill(numpy.log10(MCmuon.energy), wimp_params.nu_weight)
	histo_MC_angle_lepweight.Fill(phys_services.I3Calculator.angle(MCprimary,MCmuon), wimp_params.lep_weight)
	histo_MC_nuenergy_angle_lepweight.Fill(numpy.log10(MCprimary.energy), phys_services.I3Calculator.angle(MCprimary,MCmuon), wimp_params.lep_weight)
	histo_MC_muenergy_angle_lepweight.Fill(numpy.log10(MCmuon.energy), phys_services.I3Calculator.angle(MCprimary,MCmuon), wimp_params.lep_weight)
	histo_nuenergy_lepweight.Fill(numpy.log10(MCprimary.energy), numpy.log10(wimp_params.lep_weight))
	histo_nuenergy_lepweight_lepweight.Fill(numpy.log10(MCprimary.energy), numpy.log10(wimp_params.lep_weight), wimp_params.lep_weight)
      self.PushFrame(frame)
      return

  tray.AddModule(HistoFiller, "HistoFiller",)

if options.I3FILE:
  tray.AddModule("I3Writer","writer",
    streams = [],#[icetray.I3Frame.DAQ,icetray.I3Frame.Physics],
    filename = options.OUTFILE+".i3",)

tray.AddModule( "TrashCan", "Bye" )

if (options.NUMBER==0):
  tray.Execute()
else:
  tray.Execute(options.NUMBER)
  
tray.Finish()

del tray

### last compution
if (options.ROOTFILE):
  #______________________________________________________________
  def MEANConverter(histo2D):
    #mean (gewichtetes mittel)
    xproj = TH1D(histo2D.ProjectionX()) #helper
    yproj = TH1D(histo2D.ProjectionY()) #helper
    
    resultMEAN =TH1D(histo2D.ProjectionX("")) #copyconstructor
    resultMEAN.Reset()
    resultMEAN.SetName(histo2D.GetName()+"_binnedMEAN")
    resultMEAN.SetTitle(histo2D.GetTitle()+"_binnedMEAN")
    for xbin in range (1, histo2D.GetNbinsX()+1):
      totweight = 0.
      sumweight = 0.
      for ybin in range (1, histo2D.GetNbinsX()+1):
	sumweight += histo2D.GetBinContent(xbin,ybin)*yproj.GetBinCenter(ybin);
	totweight += histo2D.GetBinContent(xbin,ybin);
      if (totweight != 0):
	sumweight /= totweight
      resultMEAN.SetBinContent(xbin, sumweight)
    return resultMEAN;
    
  histo_MC_muenergy_angle_lepweight_binnedMEAN = MEANConverter(histo_MC_muenergy_angle_lepweight)
  histo_MC_nuenergy_angle_lepweight_binnedMEAN = MEANConverter(histo_MC_nuenergy_angle_lepweight)
  #______________________________________________________________
  def MEDIANConverter(histo2D):
    # median angular resultion (half left, half right)
    xproj = TH1D(histo2D.ProjectionX()) #helper
    yproj = TH1D(histo2D.ProjectionY()) #helper
    
    resultMEDIAN =TH1D(histo2D.ProjectionX("")) #copyconstructor
    resultMEDIAN.Reset()
    resultMEDIAN.SetName(histo2D.GetName()+"_binnedMEDIAN")
    resultMEDIAN.SetTitle(histo2D.GetTitle()+"_binnedMEDIAN")
    for xbin in range (1, histo2D.GetNbinsX()+1):
      colweight=0.
      for ybin in range (1, histo2D.GetNbinsX()+1):
	if (xproj.GetBinContent(xbin)==0): # this bin-projection is empty
	  resultMEDIAN.SetBinContent(xbin, 0.)
	  break
	colweight += histo2D.GetBinContent(xbin,ybin)
	if (colweight>= xproj.GetBinContent(xbin)/2.): #reached the critical mark
	  resultMEDIAN.SetBinContent(xbin, yproj.GetBinCenter(ybin))
	  break
    return resultMEDIAN

  histo_MC_muenergy_angle_lepweight_binnedMEDIAN = MEDIANConverter(histo_MC_muenergy_angle_lepweight)
  histo_MC_nuenergy_angle_lepweight_binnedMEDIAN = MEDIANConverter(histo_MC_nuenergy_angle_lepweight)

  ### plotting ###

  rootfile = TFile(options.OUTFILE+".root","RECREATE")

  for histoTH1D in histosTH1D:
  #  canvasTH1D = TCanvas( "canvas"+histoTH1D.GetName(), histoTH1D.GetTitle(), 600, 400 )

    #canvasTH1D.cd()
    #canvasTH1D.Clear()

    #canvasTH1D.SetGridx()
    #canvasTH1D.SetGridy()
    #canvasTH1D.SetLogy()

  #  histoTH1D.Draw()
    histoTH1D.Write()
  #  canvasTH1D.Write()

  for histoTH2D in histosTH2D:
    #canvasTH2D = TCanvas( "canvas"+histoTH2D.GetName(), histoTH2D.GetTitle(), 600, 400 )

    #canvasTH2D.cd()
    #canvasTH2D.Clear()

  #  histoTH2D.Draw("colz")
    histoTH2D.Write()
  #  canvasTH2D.Write()
    
  histo_MC_nuenergy_angle_lepweight_binnedMEAN.Write()
  histo_MC_nuenergy_angle_lepweight_binnedMEDIAN.Write()
  histo_MC_muenergy_angle_lepweight_binnedMEAN.Write()
  histo_MC_muenergy_angle_lepweight_binnedMEDIAN.Write()

  rootfile.Close()
