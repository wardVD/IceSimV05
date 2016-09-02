from icecube.load_pybindings import load_pybindings
load_pybindings(__name__,__path__)

import os
from icecube import icetray


@icetray.traysegment_inherit('I3PayloadParsingEventDecoderFactory')
def I3DOMLaunchExtractor(tray, name,
                         OMKey2MBIDXML=os.path.expandvars("$I3_SRC") + "/phys-services/resources/mainboard_ids.xml.gz",
                         BufferID="I3DAQData",
                         ExceptionId="I3DAQDecodeException",
                         If=lambda f: True,
                         **kwargs):
    """
    Extract DOMLaunches and triggers from raw DAQ data.

    :param OMKey2MBIDXML: Path to XML file mapping mainboard IDs to OMKeys.
    :param BufferID: Name of the raw data buffer in the frame.
    :param ExceptionId: Name of the boolean to put in the frame when an error occurs.

    Extra keyword arguments are passed to I3PayloadParsingEventDecoderFactory.
    """
    if OMKey2MBIDXML != None:
        tray.AddService("I3XMLOMKey2MBIDFactory", name + "_OmKey2MbId",
                        InFile=OMKey2MBIDXML)

    tray.AddService("I3PayloadParsingEventDecoderFactory", name + "_EventDecoder",
                    **kwargs)
    tray.AddModule("I3FrameBufferDecode", name + "_fbdecoder",
                   BufferID=BufferID,
                   ExceptionId=ExceptionId,
                   If=If)


del icetray, os, load_pybindings
