#########################################################
#
# NuGen propagation weighter for FINALONLY dataset
# K.Hoshina (hoshina@icecuve.wisc.edu)
#
# This module applies propagation weight of neutrino
# during neutrino's propagation from the Earth's surface
# to detector, to the NuGen simulation set generated with
# simmode = "FINALONLY".
#
# Currently this script supports NuMu and NuMuBar only.
# We may add NuE with no change and NuEBar with Grashow Resonance
# off, if we generate InEarth propagation tables.
# NuTaus are not simple: because the arrival neutrinos are
# not only NuTau flavor.
#
# This module adds 
# "PrimaryNeutrinoEnergy[fileprefix]"
# "PropagationWeight[fileprefix]"
# "OneWeight[fileprefix]" 
# "TotalWeight[fileprefix]" 
# to include the effect of neutrino's absorption or energy loss 
# during propagation inside the Earth, with given earth model.
# Default model is the PREM, no oscillation, no sterile neutrinos.
#
# The module is aimed at simply replacing current NuMu
# production (with FULL simmode) to "FINALONLY" mode,
# so that does not support for NuTaus.
# NuE could be applicable if we switch off GrashowResonance interaction,
# though I have not generated InEarth table yet.  
# (we may generate dedicated datasets for GrashowResonance separately)
# 
# The current default table supports energy range 10^2 to 10^7, so that
# the the energy range of "FINALONLY" dataset must be within the table range. 
#

from icecube import icetray, dataclasses 
from array import array
import os
import numpy as np
import copy
import math
import random 
import bisect 
import sys
if sys.version_info[0] >= 3:
    import pickle
else:
    import cPickle as pickle

class apply_propagation_weights(icetray.I3ConditionalModule):

    def __init__(self, context):

        icetray.I3ConditionalModule.__init__(self, context)

        self.AddParameter( 'WeightFilePaths',
                           'path to the weight files',
                           [])

        self.AddParameter( 'WeightFilePrefixes',
                           'prefix of weight file name',
                           ['PREM_CSMS'])

        self.AddParameter( 'PrimaryGammaIndex',
                           'gamma index of primary neutrino of your NuGen simulation, positive value',
                           1.0
                           )

        self.AddParameter( 'EnergyLogMin',
                           'min log10(energy) of primary neutrino of your NuGen simulation',
                           2.0
                           )

        self.AddParameter( 'EnergyLogMax',
                           'max log10(energy) of primary neutrino of your NuGen simulation',
                           7.0
                           )

        self.AddParameter( 'ParamSuffixes',
                           'suffix to add "OneWeight" and "PrimaryNeutrinoEnergy"',
                           ['PREM_CSMS'])


        self.AddOutBox("OutBox")

    def GetDict(self, filename):
        # make it a function to minimize memory usage...
        file = open(filename, 'r')
        alldict = pickle.load(file)
        adict = {}
        adict["minElog"] = copy.copy(alldict["minElog"])
        adict["maxElog"] = copy.copy(alldict["maxElog"])
        adict["dElog"] = copy.copy(alldict["dElog"])
        adict["minzen"] = copy.copy(alldict["minzen"])
        adict["maxzen"] = copy.copy(alldict["maxzen"])
        adict["dzen"] = copy.copy(alldict["dzen"])
        adict["enebins"] = copy.copy(alldict["energy"][0])
        adict["weight"] = copy.copy(alldict["weight"])
        adict["count3D_w"] = copy.copy(alldict["count3D_w"])
        file.close()
        print("file %s is loaded" % (filename))
        return adict

    def Configure(self):

        self.fileprefixes = self.GetParameter('WeightFilePrefixes')
        self.filepaths = self.GetParameter('WeightFilePaths')
        I3_BUILD = os.environ["I3_BUILD"]
        if len(self.filepaths) == 0 :
            self.filepaths = ["%s/%s" % (I3_BUILD, "neutrino-generator/resources/propagation_weights")]

        self.gamma = float(self.GetParameter('PrimaryGammaIndex'))
        self.minElog = float(self.GetParameter('EnergyLogMin'))
        self.maxElog = float(self.GetParameter('EnergyLogMax'))
        self.suffix = self.GetParameter('ParamSuffixes')

        gammatag = "W1.0"
        if (self.gamma != 1.0) :
            gammatag = "W%1.1f" % (self.gamma)

        # now we support NuMu flavor only
        self.ptypes = ["NuMu","NuMuBar"]
        self.dicts = {}

        for i,  prefix in enumerate(self.fileprefixes):
            filepath = self.filepaths[i]
            for ptype in self.ptypes :
                filename = "%s/%s_%s_%s.pickles" % (filepath, prefix,gammatag, ptype)
                file = open(filename, 'r')
                adict = self.GetDict(filename)
                self.dicts[(prefix, ptype)] = adict
                file.close()

    def EstimatePrimaryEnergy(self, adict, izen, ielog, enebins, eshift) :
        count3D_w = adict["count3D_w"]

        primaries = count3D_w[izen][ielog]
        accumprim = []
        totalcount = 0
        debug = []
        for index, prim_count in enumerate(primaries) :
            totalcount += prim_count
            accumprim.append(totalcount)
            debug.append([index, (enebins[index], enebins[index+1]), prim_count, totalcount])

        if totalcount == 0 :
            '''
            Couldn't find any candidate of primary energy.
            With a given zenith angle, inice neutrino can't have 
            such a high energy.
            Return 0 that means we should not use the event 
            (anyway the propagation weight should be zero
            so that does not affect to the weighted plots.)
            '''
            return 0

        r = random.uniform(0, totalcount)
        i = bisect.bisect_left(accumprim, r)

        # find primary energy for index i
        shifted_enebins = np.array([e + eshift for e in enebins])
        prim_hilogE = shifted_enebins[i+1]
        prim_lowlogE = shifted_enebins[i]

        primloge = random.uniform( prim_lowlogE, prim_hilogE)
        prim_e = 10**primloge
       

        '''
        # select an energy within the given energy bin with the
        # given gamma index.
        if self.gamma == 1.0 :
            primloge = random.uniform( prim_lowlogE, prim_hilogE)
            prim_e = 10**primloge
        else :
            r2 = random.uniform(0., 1.);
            prim_lowE = 10**prim_lowlogE
            prim_hiE  = 10**prim_hilogE
            energyP = (1-r2)*(prim_lowE**(1-self.gamma)) + r2*(prim_hiE**(1-self.gamma))
            prim_e = energyP**(1./(1-self.gamma))

        '''

        #print debug
        #print("r %f index %f accumprim[index] %f, primloge %f " %(r, i, accumprim[i], math.log10(prim_e)))

        return prim_e

    def DAQ(self, frame):

        if not frame.Has('I3MCWeightDict') :
            self.PushFrame(frame,"OutBox");
            return True

        weights = copy.copy(frame.Get( 'I3MCWeightDict' ))
        if not 'TotalWeight' in weights :
            # somehow this event failed to make interaction
            self.PushFrame(frame,"OutBox");
            return True

        # get primary particle
        mctree = frame["I3MCTree"]
        primaries = mctree.get_primaries()
        injected_nus = []

        for p in primaries :
            if p.type_string in self.ptypes :
                injected_nus.append(p)

        if len(injected_nus) > 1 :
            print("your frame contains multiple neutrinos! TotalWeight may be nonsense.")

        injected_nu = injected_nus[0]
        injected_e  = injected_nu.energy
        injected_zen = math.degrees(injected_nu.dir.zenith)

        # get info from dictionary
        for i, prefix in enumerate(self.fileprefixes) :

            adict = self.dicts[(prefix, injected_nu.type_string)]
            minElog = adict["minElog"]
            maxElog = adict["maxElog"]
            dElog   = adict["dElog"]
            minzen  = adict["minzen"]
            maxzen  = adict["maxzen"]
            dzen    = adict["dzen"]

            # energy range check. Default tables supports energy 10^2 
            # to 10^7GeV, and it's not good using the table to weight
            # simulations generated above 10^7GeV.
            if (self.maxElog > maxElog) :
                print("Your input simlation contains injected energy higher than supported energy range of InEarth table. Use different table. Input minElog %f, table minElog %f" % (self.minElog, minElog))

            # range protection:
            if injected_zen < minzen :
                injected_zen = minzen
            if injected_zen > maxzen :
                injected_zen = maxzen

            injected_elog = math.log10(injected_e)

            if injected_elog < minElog:
                injected_elog = minElog 
            #if injected_elog > maxElog - dElog:
            if injected_elog > maxElog:
                injected_elog = maxElog - 1e-5*dElog

            # find a bin in PropagationWeight table
            ielog = int((injected_elog - minElog) / dElog) 
            eloglow = minElog + ielog * dElog
            elogratio = (injected_elog - eloglow) / (dElog)
            izen  = int((injected_zen - minzen) / dzen) 
            #print("minelog %f, maxelog %f, elog = %f, ielog %d, zen = %g, izen %d" % (minElog, maxElog, injected_elog, ielog, injected_zen, izen))

            # get propagation weight
            prop_w1 = adict['weight'][izen,ielog]
            #prop_w2 = adict['weight'][izen,ielog+1]
            #prop_w = prop_w1 + (prop_w2 - prop_w1) * elogratio
            prop_w = prop_w1

            # estimate primary energy.
            enebins = adict["enebins"]
            # shift to make inject_elog center of a bin. 
            eshift = injected_elog - (ielog*dElog + minElog) - 0.5*dElog
            prim_e = self.EstimatePrimaryEnergy(adict, izen, ielog, enebins, eshift)

            #old_totalweight = weights["TotalInteractionProbabilityWeight"]
            old_totalweight = weights["TotalWeight"]
            totalweight = 0
            onew = 0

            if prim_e > 0 :
                #
                # Found a candidate of primary energy.
                # recalculate OneWeight
                #
                solidangle = weights["SolidAngle"]
                injectionareaCGS = weights["InjectionAreaCGS"]
                totalweight = old_totalweight * prop_w

                # energy integral
                emax = 10**weights["MaxEnergyLog"]
                emin = 10**weights["MinEnergyLog"]
                eintg = 0
                if (self.gamma == 1) :
                    eintg = math.log(emax / emin)
                else :
                    eintg = (pow(emax , (1.- self.gamma)) - pow(emin, (1.- self.gamma))) / (1.- self.gamma)

                onew = totalweight * eintg / (prim_e**(-self.gamma)) * solidangle * injectionareaCGS

            # update parameters
            
            weights["PrimaryNeutrinoEnergy"+self.suffix[i]] = prim_e
            weights["PropagationWeight"+self.suffix[i]] = prop_w
            weights["OneWeight"+self.suffix[i]] = onew
            weights["TotalWeight"+self.suffix[i]] = totalweight
        frame.Delete("I3MCWeightDict")
        frame.Put("I3MCWeightDict", weights)

        self.PushFrame(frame,"OutBox");
        return True

    def Finish(self):
        return True




