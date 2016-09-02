from icecube import icetray, dataclasses
from ROOT import TTree, TFile, AddressOf, gROOT
from array import array
import math

class MakeTree(icetray.I3Module):

    def __init__(self, context):
        icetray.I3Module.__init__(self, context)
        self.AddParameter( 'Filename',
                           'Name of rootfile',
                           'test.root')
        self.AddOutBox("OutBox")

    def Configure(self):
        self.evtbuf    = {}
        self.filename  = self.GetParameter( 'Filename' )

        # create a root file and my result tree
        gROOT.Reset()
        self.rootf = TFile(self.filename, 'recreate')
        self.tree = TTree('tree','My tree')
        self.branched = False

    def Geometry(self,frame):
        pass

    def Calibration(self,frame):
        pass

    def DetectorStatus(self,frame):
        pass

    def FillValue(self, keyname, val):
        if not self.branched :
            v = array('d',[val])
            self.evtbuf[keyname] = v
        else :
            self.evtbuf[keyname][0] = val
    

    def FillTree(self):
        if not self.branched :
            for key, val in self.evtbuf.items():
                self.tree.Branch(key, val, key+'/D')
            self.branched = True

        self.tree.Fill()    

    def FillParticle(self, frame, particlename):
        p = frame.Get(particlename)

        # GetEnergy
        keyname = particlename + '_Ene'
        self.FillValue(keyname, p.energy)

        # GetZenith
        keyname = particlename + '_Zen'
        self.FillValue(keyname, p.zenith)

        # GetCosZen
        keyname = particlename + '_CosZen'
        self.FillValue(keyname, math.cos(p.zenith))

        # GetAzimuth
        keyname = particlename + '_Azi'
        self.FillValue(keyname, p.azimuth)

        # GetPosX
        keyname = particlename + '_PosX'
        self.FillValue(keyname, p.x)

        # GetPosX
        keyname = particlename + '_PosY'
        self.FillValue(keyname, p.y)

        # GetPosX
        keyname = particlename + '_PosZ'
        self.FillValue(keyname, p.z)

         
    def Physics(self, frame):

        if not frame.Has('I3MCWeightDict') :
            self.PushFrame(frame,"OutBox")
            return True

        weights = frame.Get( 'I3MCWeightDict' )

        if 'TotalInteractionProbabilityWeight'  not in weights:
            #print 'value doesn't exist ', self.value
            pass

        else :
            # get weight values
            for key, val in weights.items():
                self.FillValue('MCWeights'+key, val)

            # get primary particle info
            self.FillParticle(frame,'MCMaxPrimary')

            # get MaxInIceNu particle info
            self.FillParticle(frame,'MCMaxInIce_NuG_0')

            # get MaxInIceTrack particle info
            self.FillParticle(frame,'MCMaxInIceTrack')

            # fill data
            self.FillTree()
            

        self.PushFrame(frame,"OutBox");

        return True

    def Finish(self):
        self.rootf.Write()
        self.rootf.Close()


