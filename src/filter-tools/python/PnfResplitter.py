from icecube import icetray, dataclasses, dataio, phys_services
icetray.load('libtrigger-splitter', False)

@icetray.traysegment
def PnfResplitter(tray, name, doInIceSplit=True, doNullSplit=True, doIceTopSplit=True):

    # Script should be as simple as:
    #
    #tray.AddModule( "I3Reader", "Reader")
    #tray.AddSegment(filter_tools.PnfResplitter, "resplit")
    #tray.AddModule( "I3Writer", "EventWriter2" ) 
    #

    def MaskMaker(frame):
        if frame.Has('I3SuperDST') and frame.Has('DSTTriggers'): # save only filtered frames (traditional filter + SuperDST filter)
            pulses = dataclasses.I3RecoPulseSeriesMap.from_frame(frame, 'I3SuperDST')
            ii_mask = dataclasses.I3RecoPulseSeriesMapMask(frame, 'I3SuperDST')
            it_mask = dataclasses.I3RecoPulseSeriesMapMask(frame, 'I3SuperDST')
            for omkey in pulses.keys():
                ii_mask.set(omkey, omkey.om <= 60)
                it_mask.set(omkey, omkey.om > 60)
            frame['InIceDSTPulses']  = ii_mask
            frame['IceTopDSTPulses'] = it_mask

        else:
            return 0

    tray.AddModule("QConverter", name+"_convert")(
        ("WritePFrame", 0)
    )

    # Slip in a "EndQ/E" frame, act to prevent PacketModules from caching
    try:
        icetray.load('pfauxiliary', False)
        tray.AddModule("PFEmitFlushFrame", 'emit_flush')
    except:
        icetray.logging.log_warn("PFEmitFlushFrame module couldn't be loaded (not installed). It's not used.")

    tray.AddModule(MaskMaker, name+'_maskme', Streams = [icetray.I3Frame.DAQ])

    tray.AddModule("Delete", name+'_deleteme')(
        ("Keys", ['JEBEventInfo','PFContinuity'])
    )

    if doInIceSplit:
        tray.AddModule('I3TriggerSplitter','InIceSplit')(
            ("TrigHierName", 'DSTTriggers'), 
            ('InputResponses', ['InIceDSTPulses']),
            ('OutputResponses', ['SplitUncleanedInIcePulses']),
        )

    if doIceTopSplit:
        # I don't know the IT processing to get from IceTopDSTPulses to IceTopPulses_HLC
        # the icetop working group need to do this in the P-frame
        # for now, i'm splitting on IceTopPulses
        tray.AddModule('I3TriggerSplitter','IceTopSplit')(
            ("TrigHierName", 'DSTTriggers'),
            ('TriggerConfigIDs', [102,1006,1011]),
            ('InputResponses', ['IceTopDSTPulses']), #on PnF this is 'IceTopPulses_HLC' (HLC from tpx)
            ('OutputResponses', ['SplitUncleanedITPulses']),
        )

    if doNullSplit:
        tray.AddModule("I3NullSplitter", 'NullSplit')

        def TriggerHierarchyForNullSplit(frame):
            if frame['I3EventHeader'].sub_event_stream == 'NullSplit':
                frame['I3TriggerHierarchy'] = dataclasses.I3TriggerHierarchy.from_frame(frame,'DSTTriggers')

        tray.AddModule(TriggerHierarchyForNullSplit, 'trigHierWriter')

    substreamnames = []
    if doNullSplit:
        substreamnames += ['NullSplit']
    if doInIceSplit:
        substreamnames += ['InIceSplit']
    if doIceTopSplit:
        substreamnames += ['IceTopSplit']

    tray.AddModule("DistributePnFObjects", name+'_distribute')(
        ("SubstreamNames", substreamnames)
    )

@icetray.traysegment
def AnalysisClientRehydrate(tray, name, dbhost='dbs', dbuser='i3omdbro', doInIceSplit=True, doIceTopSplit=True, doNullSplit=True):

    tray.AddService("PFDbGCDServiceFactory",name + "_gcd",
                    databasename = "I3OmDb",
                    hostname = dbhost,
                    username = dbuser,
                    password = "",
                    omitcalibration = True,
                    omitgeometry = True,
                    omitstatus = False)

    tray.AddSegment(PnfResplitter, 'resplit',
                   doInIceSplit = doInIceSplit,
                   doIceTopSplit = doIceTopSplit,
                   doNullSplit = doNullSplit)
