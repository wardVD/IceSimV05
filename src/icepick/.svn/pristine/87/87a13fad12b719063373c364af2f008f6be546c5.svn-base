from icecube import icetray,dataclasses

class I3LogicalOrTestModule(icetray.I3Module):
    def __init__(self, context):
        icetray.I3Module.__init__(self,context)
        
        self.ev1_ = None
        self.ev2_ = None

        self.AddOutBox("OutBox")
        self.AddParameter("FirstEventID","",self.ev1_)
        self.AddParameter("SecondEventID","",self.ev2_)

    def Configure(self):
        self.ev1_ = self.GetParameter("FirstEventID")
        self.ev2_ = self.GetParameter("SecondEventID")

    def Physics(self, frame):
        evHeader = frame.Get("I3EventHeader")
        evNum = evHeader.event_id
        if not(evNum==self.ev1_ or evNum==self.ev2_):
            print("Logical OR filter test failed")
            assert();
        self.PushFrame(frame)
