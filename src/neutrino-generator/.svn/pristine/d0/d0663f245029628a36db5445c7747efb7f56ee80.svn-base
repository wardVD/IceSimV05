#
# Make InEarth propagation weight table from InEarth simulation
# Parameters are fixed order. See code for details.
# 

import sys
import glob
if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

import numpy as np
import math as m

from I3Tray import *
from icecube import icetray, dataclasses, dataio, phys_services
# for logging
icetray.I3Logger.global_logger = icetray.I3PrintfLogger()
icetray.set_log_level(icetray.I3LogLevel.LOG_WARN)

flavor = sys.argv[1]             # flavor of neutrino, ex. NuMu, NuMuBar
histid = int(sys.argv[2])        # id for the dataset
nzenbins = int(sys.argv[3])      # number of zenith bins
ncoszenbins = int(sys.argv[4])   # number of coszen bins
nebins= int(sys.argv[5])         # number of energy(log10) bins
elogs_st = (sys.argv[6]).split(':')   # energy range, min:max in log10 
elogs = [float(elogs_st[0]), float(elogs_st[1])]
zens_st = (sys.argv[7]).split(':')  # zenith range, min:max in degree
zens = [float(zens_st[0]), float(zens_st[1])]
gengamma = sys.argv[8]           # energy gamma index of input generated NuGen dataset (positive value)
weightgamma = sys.argv[9]        # energy gamma index you want to weight to (positive value)
                                 # (for example, if you want to generate weight table 
                                 # to use with simulation generated with E^-2, then set 
                                 # the weightgamma = 2. Note that you will have severe
                                 # systematic problems if you set a harder (smaller) number 
                                 # than gengamma.)

outfilebase = sys.argv[10]       # basename of output pickles file

nargv=len(sys.argv)              # from nargv-th argument you may set a list of 
                                 # input nugen filenames. filenames may contain "*"
iargv = 11 
inputfiles = []
while iargv < nargv :
    file=str(sys.argv[iargv])
    if file.count("*") > 0 :
        print "glob is choosen ", file
        filelist = glob.glob(file)
        filelist.sort()
        inputfiles.extend(filelist)
    else :
        inputfiles.append(file)
    iargv = iargv+1

class Fill(icetray.I3ConditionalModule) : 
    def __init__(self, ctx):
        icetray.I3ConditionalModule.__init__(self, ctx)

        #-------------------------------
        # set zenith range etc. here
        #

        # zenith range [in degree]
        self.zens = [90., 180]

        # coszen range 
        self.coszens = [m.cos(self.zens[1]*I3Units.degree),
                        m.cos(self.zens[0]*I3Units.degree)]

        # energy(log10) range
        self.enelogs = [2.0, 7.0]

        # do you want to save text data?
        #self.save_text = False
        self.save_text = True

        #-------------------------------

        self.outfilebase = "test"
        self.flavor = "NuMu"
        self.nzenbins = 9
        self.ncoszenbins = 10
        self.nebins = 6
        self.histid = 0
        self.gen_gamma = 1
        self.weight_gamma = 1
        self.energy_factor = 1

        self.AddParameter( 'outfilebase',
                           'Name of outfile base',
                           self.outfilebase)

        self.AddParameter( 'Flavor',
                           'Neutrino flavor',
                           self.flavor)
          
        self.AddParameter( 'histid',
                           'histogram id',
                           self.histid)

        self.AddParameter( 'nzenbins',
                           'number of zenith bins',
                           self.nzenbins)

        self.AddParameter( 'ncoszenbins',
                           'number of coszenith bins',
                           self.ncoszenbins)

        self.AddParameter( 'nebins',
                           'number of energy bins',
                           self.nebins)

        self.AddParameter( 'zeniths',
                           'zenith range',
                           self.zens)

        self.AddParameter( 'energylogs',
                           'energy range (log10)',
                           self.enelogs)

        self.AddParameter( 'savetext',
                           'do you want to save text file?',
                           self.save_text)

        self.AddParameter( 'gengamma',
                           'generation gamma index',
                            self.gen_gamma)

        self.AddParameter( 'weightgamma',
                           'weighting gamma index',
                            self.weight_gamma)

        self.AddOutBox("OutBox")

    def InitDict(self) :
        mydict = {
                "histid" : self.histid,
                "flavor" : self.flavor,
                "gengamma" : self.gen_gamma,
                "weightgamma" : self.weight_gamma,
                "minzen" : self.zens[0],
                "maxzen" : self.zens[1],
                "dzen"   : self.dzen,
                "nzenbins" : self.nzenbins,
                "mincoszen" : self.coszens[0],
                "maxcoszen" : self.coszens[1],
                "dcoszen"   : self.dcoszen,
                "ncoszenbins" : self.ncoszenbins,
                "minElog"   : self.enelogs[0],
                "maxElog"   : self.enelogs[1],
                "dElog"     : self.delog,
                "nEbins"    : self.nebins,
                "zenith"    : np.zeros((1,1)),  # xaxis np.array zenith, dummy size
                "coszenith" : np.zeros((1,1)),  # xaxis np.array coszen
                "energy"    : np.zeros((1,1)),  # yaxis np.array log10(energy) 
                "energycos" : np.zeros((1,1)),  # yaxis np.array log10(energy) (for coszen-energy plot)
                "weight"    : np.zeros((1,1)),  # z value, ratio, np.array, weight in zenith-energy array
                "weightcos" : np.zeros((1,1)),  # z value, ratio, np.array, weight in coszen-energy array
                "primary"      : np.zeros((self.nzenbins, self.nebins)),  # primary count np.array in zenith-energy array
                "primary_w"    : np.zeros((self.nzenbins, self.nebins)),  # primary weight np.array in zenith-energy array
                "primarycos"   : np.zeros((self.ncoszenbins, self.nebins)), # primary count np.array in coszen-energy array
                "primarycos_w" : np.zeros((self.ncoszenbins, self.nebins)) ,  # primary weight np.array in coszen-energy array
                "inice"        : np.zeros((self.nzenbins, self.nebins)), # inice count np.array in zenith-energy array
                "inice_w"      : np.zeros((self.nzenbins, self.nebins)), # inice weight np.array in zenith-energy array
                "inicecos"     : np.zeros((self.ncoszenbins, self.nebins)), # inice count np.array in coszen-energy array
                "inicecos_w"   : np.zeros((self.ncoszenbins, self.nebins)),  # inice weight np.array in coszen-energy array
                #"count3D"   : np.zeros((self.nzenbins, self.nebins, self.nebins)),  # inice weight np.array in coszen-energy array
                "count3D_w"   : np.zeros((self.nzenbins, self.nebins, self.nebins)),  # inice weight np.array in coszen-energy array
                #"count3D_w_noprimw"   : np.zeros((self.nzenbins, self.nebins, self.nebins))  # inice weight np.array in coszen-energy array
                }

        return mydict


    def CalcEnergyIntg(self, emin, emax, gamma) :
        if gamma == 1 :
            return m.log(emax/emin)
        else :
            return (emax**(1. - gamma) - emin**(1. - gamma)) / (1.- gamma)

        
    def Configure(self):

        self.outfilebase = self.GetParameter('outfilebase')
        self.flavor= self.GetParameter('Flavor')
        self.histid = self.GetParameter('histid')
        self.nzenbins = self.GetParameter('nzenbins')
        self.ncoszenbins = self.GetParameter('ncoszenbins')
        self.nebins = self.GetParameter('nebins')
        self.zens = self.GetParameter('zeniths')
        self.enelogs = self.GetParameter('energylogs')
        self.save_text = self.GetParameter('savetext')
        self.gen_gamma = float(self.GetParameter('gengamma'))
        self.weight_gamma = float(self.GetParameter('weightgamma'))

        self.coszens = [m.cos(self.zens[1]*I3Units.degree),
                        m.cos(self.zens[0]*I3Units.degree)]

        # set binwidth
        self.dzen = (self.zens[1] - self.zens[0]) / self.nzenbins
        self.dcoszen = (self.coszens[1] - self.coszens[0]) / self.ncoszenbins
        self.delog = (self.enelogs[1] - self.enelogs[0]) / self.nebins
        # result 2D arrays
        self.dictNu = self.InitDict()
        self.dictANu = self.InitDict()
        self.dictAll = self.InitDict()

        # calc energy factor
        if self.gen_gamma != self.weight_gamma :
            emin = 10**self.enelogs[0]
            emax = 10**self.enelogs[1]
            e_intg_gen = self.CalcEnergyIntg(emin, emax, self.gen_gamma)
            e_intg_weight= self.CalcEnergyIntg(emin, emax, self.weight_gamma)
            # ratio = (1/s2*E^-2) / (1/s1*E^-1) = s1/s2 * (E^-2/E^-1)
            self.energy_factor = e_intg_gen / e_intg_weight

    def is_old_format(self, frame):
        if frame.Has("I3MCWeightDict") :
            return True
        return False

    def extract_inices(self, frame):
        return_list = [] 
        if not frame.Has('NuGInIceNeutrinos') :
            return return_list

        inicenus = frame['NuGInIceNeutrinos']

        if self.is_old_format(frame) :
            # inicenus is just an vector<I3Particle>
            for nu in inicenus :
                pair = [nu, nu.speed]   # for old version
                return_list.append(pair)

        else :
            # inicenus is map<I3ParticleID, double>
            tree = frame['I3MCTree']
            for id, weight in inicenus :
                p = tree.get_particle(id)
                pair = [p, weight]   # for new version
                return_list.append(pair)

        return return_list


    def Physics(self, frame):

        prim_w = 1.0

        if self.is_old_format(frame) :
            wdict = frame['I3MCWeightDict']
            prim_w = wdict['TotalPrimaryWeight']

        mctree = frame['I3MCTree']
        prim = mctree.get_head()
        prim_elog10 = m.log10(prim.energy)
        prim_zen = prim.dir.zenith
        prim_coszen = m.cos(prim_zen)

        prim_zen /= I3Units.degree # change val to degree

        # skip all particles which doesn't fit the energy-genith range
        if (prim_elog10 > self.enelogs[1] or prim_elog10 < self.enelogs[0]) :
            self.PushFrame(frame,"OutBox");
            return True

        if (prim_zen > self.zens[1] or prim_zen < self.zens[0]) :
            self.PushFrame(frame,"OutBox");
            return True

        # set primary flux weight 
        if self.gen_gamma != self.weight_gamma :
            ene = prim.energy
            prim_w *= (self.energy_factor * ene**(-self.weight_gamma) / ene**(-self.gen_gamma))

        # set reference to proper dict 
        dict = self.dictNu
        if (int(prim.type) < 0) :
                dict = self.dictANu

        prim_zeni = int((prim_zen - self.zens[0])/self.dzen) 
        prim_coszeni = int((prim_coszen - self.coszens[0])/self.dcoszen) 

        prim_ei = int((prim_elog10 - self.enelogs[0])/self.delog) 
        dict["primary"][prim_zeni, prim_ei] += 1.0
        dict["primary_w"][prim_zeni, prim_ei] += prim_w
        dict["primarycos"][prim_coszeni, prim_ei] += 1.0
        dict["primarycos_w"][prim_coszeni, prim_ei] += prim_w

        icetray.logging.log_info("primary zen %f elog10 %f is added to [%d %d], current primary %f" 
               % (prim_zen/I3Units.degree, prim_elog10,
                  prim_zeni, prim_ei, dict["primary"][prim_zeni, prim_ei]))

        # take InIce Neutrino's info, and skip it if it's energy 
        # is out of range.

        # since (for now) we use numu channel, size of inice array is always 1
        # and we stored propagation weight in speed field!

        inices = self.extract_inices(frame)

        # if no inice candidate exist, return now.
        if len(inices) == 0 :
            self.PushFrame(frame,"OutBox");
            return True

        #print(inices)

        inice_valid = False
        for pair in inices:
            inice = pair[0]
            inice_w = pair[1] * prim_w
            inice_zen = inice.dir.zenith
            inice_coszen = m.cos(inice_zen)
            inice_zen /= I3Units.degree # change val to degree
            inice_zeni = int((inice_zen - self.zens[0])/self.dzen) 
            inice_coszeni = int((inice_coszen - self.coszens[0])/self.dcoszen) 
            inice_elog10 = m.log10(inice.energy)
            if (inice_elog10 < self.enelogs[0]) :
                continue

            inice_ei = int((inice_elog10 - self.enelogs[0])/self.delog) 
            dict["inice"][inice_zeni, inice_ei] += 1.0
            dict["inice_w"][inice_zeni, inice_ei] += inice_w
            dict["inicecos"][inice_coszeni, inice_ei] += 1.0
            dict["inicecos_w"][inice_coszeni, inice_ei] += inice_w
            #dict["count3D"][inice_zeni, inice_ei, prim_ei ] += 1.0
            dict["count3D_w"][inice_zeni, inice_ei, prim_ei ] += inice_w
            #dict["count3D_w_noprimw"][inice_zeni, inice_ei, prim_ei ] += pair[1]
            icetray.logging.log_info("inice zen %f elog10 %f is added to [%d %d]" 
                   % (inice_zen/I3Units.degree, inice_elog10,
                      inice_zeni, inice_ei))

        self.PushFrame(frame,"OutBox");

        return True


    def Finish(self):

        #
        # calculate weights
        #

        self.dictNu["weight"] = self.dictNu["inice_w"] / self.dictNu["primary_w"]
        self.dictANu["weight"] = self.dictANu["inice_w"] / self.dictANu["primary_w"]
        self.dictNu["weightcos"] = self.dictNu["inicecos_w"] / self.dictNu["primarycos_w"]
        self.dictANu["weightcos"] = self.dictANu["inicecos_w"] / self.dictANu["primarycos_w"]

        #
        # for drowing
        # make axes 

        self.dictNu["zenith"],self.dictNu["energy"] = np.mgrid[self.zens[0]:self.zens[1]:(self.nzenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]
        self.dictANu["zenith"],self.dictANu["energy"] = np.mgrid[self.zens[0]:self.zens[1]:(self.nzenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]
        self.dictAll["zenith"],self.dictAll["energy"] = np.mgrid[self.zens[0]:self.zens[1]:(self.nzenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]

        # make axes for coszen bins
        # size of energycos is not same as energy, so we have to prepare
        # new array for energy.

        self.dictNu["coszenith"],self.dictNu["energycos"] = np.mgrid[self.coszens[0]:self.coszens[1]:(self.ncoszenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]
        self.dictANu["coszenith"],self.dictANu["energycos"] = np.mgrid[self.coszens[0]:self.coszens[1]:(self.ncoszenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]
        self.dictAll["coszenith"],self.dictAll["energycos"] = np.mgrid[self.coszens[0]:self.coszens[1]:(self.ncoszenbins+1)*1j, self.enelogs[0]:self.enelogs[1]:(self.nebins+1)*1j]

        #
        # write binaries in pickle
        #

        outfileNu = "%s_W%2.1f_%s" % (self.outfilebase, self.weight_gamma, self.flavor)
        f = open(outfileNu + ".pickles", 'w')
        pickle.dump(self.dictNu, f)
        f.close()

        outfileNuBar = "%s_W%2.1f_%sBar" % (self.outfilebase, self.weight_gamma, self.flavor)
        f = open(outfileNuBar + ".pickles", 'w')
        pickle.dump(self.dictANu, f)
        f.close()


        if (self.save_text) :
            # these are for Neutrino only
            np.savetxt(outfileNu +"_weight.txt",self.dictNu["weight"])
            np.savetxt(outfileNu +"_zenith.txt",self.dictNu["zenith"])
            np.savetxt(outfileNu +"_energy.txt",self.dictNu["energy"])
            np.savetxt(outfileNu +"_weightcos.txt",self.dictNu["weightcos"])
            np.savetxt(outfileNu +"_coszenith.txt",self.dictNu["coszenith"])
            np.savetxt(outfileNu +"_energycos.txt",self.dictNu["energycos"])

            # these are for Anti-Neutrino only
            np.savetxt(outfileNuBar +"_weight.txt",self.dictANu["weight"])
            np.savetxt(outfileNuBar +"_zenith.txt",self.dictANu["zenith"])
            np.savetxt(outfileNuBar +"_energy.txt",self.dictANu["energy"])
            np.savetxt(outfileNuBar +"_weightcos.txt",self.dictANu["weightcos"])
            np.savetxt(outfileNuBar +"_coszenith.txt",self.dictANu["coszenith"])
            np.savetxt(outfileNuBar +"_energycos.txt",self.dictANu["energycos"])


ievt = 0
def evtcounter(frame):
    global ievt
    header = frame['I3EventHeader']
    runid = header.run_id
    evtid = header.event_id
    if ievt % 10000 == 0 :
        print "event %s start *** runid %d eventid %d " % (ievt, runid, evtid)
    ievt = ievt + 1

#----------------
# start icetray
#----------------

tray = I3Tray()
print "input filenames are ", inputfiles

tray.AddModule('I3Reader', 'reader', FilenameList=inputfiles)

tray.AddModule("I3NullSplitter", "fullevent")

tray.AddModule (evtcounter, "evtcounter")

tray.AddModule(Fill,'fill', 
               histid=histid,
               flavor=flavor,
               nzenbins=nzenbins, 
               ncoszenbins=ncoszenbins, 
               nebins=nebins, 
               energylogs=elogs,
               zeniths=zens,              
               outfilebase=outfilebase,
               savetext=False,
               gengamma=gengamma,
               weightgamma=weightgamma
               )

tray.AddModule('TrashCan','can')
tray.Execute()
tray.Finish()


