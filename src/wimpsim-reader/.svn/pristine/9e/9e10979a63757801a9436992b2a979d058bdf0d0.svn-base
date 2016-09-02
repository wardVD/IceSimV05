"""
Traysegements for WimpSimReader
"""

from icecube import icetray, dataclasses, simclasses
from I3Tray import I3Units


@icetray.traysegment
def WimpSimReaderSUN(tray, name, Infile=None, StartMJD=None, EndMJD=None):
  """
  run this traysegement for Sun files

  :param Infile: infile to read
  :param StartMJD: read only from this MJD
  :param EndMJD: read only until this MJD
  """
  if (Infile == None):
    print('choose an infile')
    
  tray.AddModule("I3WimpSimReader", name+"WimpSimReader",
    FileNameList = [Infile], #default ""
    StartMJD = StartMJD, #default 0
    EndMJD = EndMJD, #default 0
    NEvents = 0, #default 0
    Oversampling = 0, #default 0
    PositionLimits = [-800.,800.,-800.,800.,-800.,800.], #default [-800*I3Units.meter,800*I3Units.meter,-800*I3Units.meter,800*I3Units.meter,-800*I3Units.meter,800*I3Units.meter]
    InjectionRadius = 0*I3Units.meter , #default 0*I3Units.meter
    LowerZenCut = 0*I3Units.degree , #default 0*I3Units.degree
    UpperZenCut = 180*I3Units.degree , #default 180*I3Units.degree
    UseElectrons = False, #default False
    UseMuons = True, #default True
    UseTaus = False, #default False
    UseNC = False, #default False
    SensitiveHeight = 700*I3Units.meter, #default 0*I3Units.meter
    SensitiveRadius = 1800*I3Units.meter, #default 0*I3Units.meter
  )
  
  
@icetray.traysegment
def WimpSimReaderEarth(tray, name, GCDFileName=None, Infile=None, StartMJD=None, EndMJD=None):
  """
  run this traysegement for earth files

  :param Infile: infile to read
  :param GCDFileName: specify either this to take the detector start_time as event-time -or- use a random time-stamp in [startMJD, endMJD]
  :param StartMJD: pick a random MJD starting from this time
  :param EndMJD: pick a random MJD ending at this time
  """
  def get_time(gcdfile):
    frame = gcdfile.pop_frame()
    while not frame.Has("I3DetectorStatus"):
      frame = gcdfile.pop_frame()
    return frame.Get("I3DetectorStatus").start_time

  time = dataclasses.I3Time()
  if GCDFileName != None:
    print("Will use (GCDFile)DetectorStatus.start_time for timing")
    if not GCD == "": #if -g was specified used the runstartttime of that GCD file
      time = get_time(dataio.I3File(options.GCD))
  
  if (Infile == None):
    print('choose an infile')

  tray.AddModule("I3WimpSimReader", name+"WimpSimReader",
    FileNameList = [Infile], #default ""
    StartMJD = StartMJD, #default 0
    EndMJD = EndMJD, #default 0
    NEvents = 0, #default 0
    Oversampling = 0, #default 0
    PositionLimits = [-800.,800.,-800.,800.,-800.,800.], #default [-800*I3Units.meter,800*I3Units.meter,-800*I3Units.meter,800*I3Units.meter,-800*I3Units.meter,800*I3Units.meter]
    InjectionRadius = 0*I3Units.meter , #default 0*I3Units.meter
    LowerZenCut = 0*I3Units.degree , #default 0*I3Units.degree
    UpperZenCut = 180*I3Units.degree , #default 180*I3Units.degree
    UseElectrons = False, #default False
    UseMuons = True, #default True
    UseTaus = False, #default False
    UseNC = False, #default False

    SensitiveHeight = 700*I3Units.meter, #default 0*I3Units.meter
    SensitiveRadius = 1800*I3Units.meter, #default 0*I3Units.meter
  )
  
  
#=============================================================================
class VeffCollector(icetray.I3ConditionalModule):
  """
  collect the Veff from all events; do not forget to devide by the weight-sum in the end
  """
  def __init__(self, context):
    super(VeffCollector, self).__init__(context)
    self.wimpparams_name = "WIMP_params"
    self.AddParameter("WIMPparamsName","The name of the input WIMPparams",self.wimpparams_name)
    self.AddOutBox("OutBox")
  def Configure(self):
    self.wimpparams_name = self.GetParameter("WIMPparamsName")
    self.vw_coll = 0.
  def Finish(self):
    icetray.logging.log_notice("The collected weighted eff Volume ( ==Sum(Vgen*lep_weight) ) is: %s" % (str(self.vw_coll)))
    icetray.logging.log_notice("You need to devide this value by the total weight from the generated files;\n you can use 'wimpsim_reader/resources/scripts/FileWeightCollector.py' to do this")
  def DAQ(self, frame):
    if frame.Has(self.wimpparams_name):
      wp = frame[self.wimpparams_name]
      self.vw_coll += wp.vgen * wp.lep_weight
    else:
      icetray.logging.log_fatal("The key "+self.wimpparams_name+" is not valid")
    self.PushFrame(frame)
    return
  
#===============================================================================
class NuMuValueCollector(icetray.I3ConditionalModule):
  """
  collect the Veff from all events; do not forget to devide by the weight-sum in the end
  """
  def __init__(self, context):
    super(NuMuValueCollector, self).__init__(context)
    self.wimpparams_name = "WIMP_params"
    self.AddParameter("WIMPparamsName","The name of the input WIMPparams",self.wimpparams_name)
    self.AddOutBox("OutBox")
  def Configure(self):
    self.wimpparams_name = self.GetParameter("WIMPparamsName")
    self.nu_nu_coll = 0.
    self.nu_nubar_coll = 0.
    self.lep_nu_coll = 0.
    self.lep_nubar_coll = 0.
    self.nuv_nu_coll = 0.
    self.nuv_nubar_coll = 0.
    self.lepv_nu_coll = 0.
    self.lepv_nubar_coll = 0.
    self.nuflux_nu_coll = 0.
    self.nuflux_nubar_coll = 0.
    self.lepflux_nu_coll = 0.
    self.lepflux_nubar_coll = 0.

  def Finish(self):
    icetray.logging.log_notice("---------- Only NuMu + NuMuBar CC interactions -------")
    icetray.logging.log_notice("---------- Neutrino weight -------")
    icetray.logging.log_notice("The collected weight ( ==Sum(nu_weight) is: %e + %e" % (self.nu_nu_coll, self.nu_nubar_coll))
    icetray.logging.log_notice("The collected eff number-flux ( == Sum(nu_weight/sigma_nu(E)) is: %e + %e" % (self.nuflux_nu_coll, self.nuflux_nubar_coll))
    icetray.logging.log_notice("The collected weighted eff Volume ( ==Sum(Vgen*nu_weight) ) is: %e + %e" % (self.nuv_nu_coll, self.nuv_nubar_coll))
    icetray.logging.log_notice("---------- Lepton weight -------")
    icetray.logging.log_notice("The collected weight ( ==Sum(lep_weight) is: %e + %e" % (self.lep_nu_coll, self.lep_nubar_coll))
    icetray.logging.log_notice("The collected eff number-flux ( == Sum(lep_weight/sigma_nu(E)) is: %e + %e" % (self.lepflux_nu_coll, self.lepflux_nubar_coll))
    icetray.logging.log_notice("The collected weighted eff Volume ( ==Sum(Vgen*lep_weight) ) is: %e + %e" % (self.lepv_nu_coll, self.lepv_nubar_coll))
  def DAQ(self, frame):
    if frame.Has(self.wimpparams_name) and frame.Has("I3MCTree"):
      wp = frame[self.wimpparams_name]
      nu = dataclasses.get_most_energetic_neutrino(frame["I3MCTree"])
      nu_sigma= self.xsNeutrino(nu)
      if nu.type == dataclasses.I3Particle.NuMu:
        self.nu_nu_coll += wp.nu_weight
        self.lep_nu_coll += wp.lep_weight
        self.nuv_nu_coll += wp.vgen * wp.nu_weight
        self.lepv_nu_coll += wp.vgen * wp.lep_weight
        self.nuflux_nu_coll += wp.nu_weight/nu_sigma
        self.lepflux_nu_coll += wp.lep_weight/nu_sigma
      elif nu.type == dataclasses.I3Particle.NuMuBar:
        self.nu_nubar_coll += wp.nu_weight
        self.lep_nubar_coll += wp.lep_weight
        self.nuv_nubar_coll += wp.vgen * wp.nu_weight
        self.lepv_nubar_coll += wp.vgen * wp.lep_weight
        self.nuflux_nubar_coll += wp.nu_weight/nu_sigma
        self.lepflux_nubar_coll += wp.lep_weight/nu_sigma
    else:
      icetray.logging.log_fatal("The key "+self.wimpparams_name+" or I3MCTree is not valid")
    self.PushFrame(frame)
    return

  def xsNeutrino(self, p):
    """
    Compute the cross-section for this neutrino done in pythia

    Parametrization is
    sigma_numu = 10.* a_numu * Energy**(b_numu) + 8.* c_numu* Energy**(d_numu)
    NOTE: can only do nu_mu and nu_mubar to date

    :param p: the particle to compuyte the cross-section for
    :return: the crosssection in units cm^-2
    """
    nupa = 5.43e-39
    nupb = 0.965
    nubpa = 4.59e-39
    nubpb = 0.978
    nuna = 1.23e-38
    nunb = 0.929
    nubna =  2.19e-39
    nubnb = 1.022
    nump = 10.
    numn = 8.
    
    
    energy = p.energy
    
    if not numpy.isfinite(energy):
      icetray.logging.log_fatal("energy of this particle is not finite")
    if (energy<1): #GeV
      icetray.logging.log_error("particle Energy might be outside the feasible parametization range check results!")
      
    if p.type == dataclasses.I3Particle.NuMu:
      return nump*nupa*pow(energy,nupb) + numn*nuna*pow(energy,nunb)
    elif p.type == dataclasses.I3Particle.NuMuBar:
      return nump*nubpa*pow(energy,nubpb) + numn*nubna*pow(energy,nubnb)
    else:
      icetray.logging.log_fatal_stream("Cannot compute the cross-setion for particle Type "+p.typeString())
  
  
WimpSimReader=[
  'I3MCTree',
  'WIMP_Params',
  'I3EventHeader']
  
