.. _payload-parsing-main:

About Payload-Parsing
=====================

The payload-parsing project is an implementation of a data reader for the
IceCube DAQ data. It differs from other projects in that it extracts data
directly in to the offline data structures without any intermediate layer, and
only decodes the data that is explicitly needed offline.

All the code is written assuming that we are reading in an intrinsically big
endian data structure into a little endian machine. There will have to be some
work done to port the code to a big endian platform, if that's ever needed.

The code for decoding the engineering format was cut-paste-modified from the
domapp-tools project from glacier. To facilitate this, the code that is used
there to read big endian numbers from a byte buffer have been taken, and we have
essentially two implementations of this. The code routinely violates the
'no magic numbers' convention. For example this snippit:

.. code-block:: cpp

    uint32_t nReadoutRequest = decode<uint32_t>(BIG, start + 60, eventData);
    unsigned locationNComposites = 70 + (32 * nReadoutRequest);

This is intentional. It seems to me that the other option: define a 'static
const unsigned' somewhere with these magic numbers in it would just make the
code less understandable. These numbers are properly hardcoded. They are tightly
coupled to the format we are decoding. They are used only once. The code will be
*less* clear, I think, by defining them somewhere else and just using them here.

There are two major classes that are provided by payload-parsing. They are
I3PayloadParsingEventDecoder and I3PayloadParsingEventDecoderFactory.
I3PayloadParsingEventDecoder implements I3DAQEventDecoder from daq-decode and
the I3PayloadParsingEventDecoderFactory is an icetray service factory to install
this class as a service. All the configuration items required are passed into
the constructor of I3PayloadParsingEventDecoder, and it implements one important
method: ``I3Time I3PayloadParsingEventDecoder::FillEvent(I3Frame& frame,const
std::vector<char>& data) const`` which decodes the data from the buffer into
dataclasses structures and fills the frame with them.

That method is satisfied by a small number of stand-alone functions. The work of
decoding a payload of type ``payloadType`` is done by a function:

.. code-block:: cpp

    template <uint32_t payloadType>
        void do_payload_decode(DecodeTarget<payloadType>& target,
            const DecodeConfiguration& config,
            const vector<char>& eventData,
            unsigned start);

The function template is declared but the general template is never defined. All
implementations are specializations. The class template
``DecodeTarget<payloadType>`` is also a specialization which describes what kind of
dataclasses data will be found in the payload. It may seem kinda weird to have
templates where you must specialize them. What's the point of a template? Well,
the point here is that there is a function

.. code-block:: cpp

    template <unsigned payloadType>
        void decode_payload(DecodeTarget<payloadType>& target,
            const DecodeConfiguration& config,
            const vector<char> eventData,
            unsigned start);

which is *not* specialized. Because the other templates have been declared, this
thing can generically call ``do_payload_decode``. It does the error checking and
stuff that is common to all payloads. Also the templates emphasize a uniform
structure.


Usage
=====

The following code snippet allows the basic parsing of the data:

.. code-block:: python

    tray.AddService("I3XMLOMKey2MBIDFactory", "OmKey2MbId",)
    tray.AddService("I3PayloadParsingEventDecoderFactory", "EventDecoder")
    tray.AddModule("I3FrameBufferDecode", "fbdecoder")

This first adds a service provided by ``phys-services`` of translating MBIDs to
OMKeys for use by ``payload-parsing``/``daq-decode``. Next the
``I3PayloadParsingEventDecoderFactory`` service is added, configuring the
``I3PayloadParsingEventDecoder``. Finally, the data is decoded through the
``I3FrameBufferDecode`` module of ``daq-decode``. See the results of
``icetray-inspect`` for these projects for possible input arguments of these
services/modules which are too numerous to list here.

This basic snippet can be used as a tray segment with ``I3DOMLaunchExtractor``
provide by ``payload-parsing``.

Finally, a basic tray module to just extract the DOM launches from Type 21
events is provided by ``I3DAQDataTrimmer``.

The possible input arguments for these interfaces can be found using
``icetray-inspect payload-parsing``.
