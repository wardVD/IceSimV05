Code review of genie-icetray
============================

31 Jan 2012

The stated purpose of this code is as an icetray wrapper to the 
externally-maintained Genie code. As such, we don't get to choose some parts, 
e.g. their choice of logging software. I'm going to ignore all those bits in 
this review and just deal with our wrapper software. 

My overall recommendation is that with the addition of some easy documentation 
and an extra error message, this should pass code review. Having this project 
available to simprod will be an immediate boon to several analyses. A second 
release could follow soon with and updated genie version: this project assumes 
genie 2.6.4 (from July 2011), and we could update to 2.6.6 (from Feb 2012).

This project is lacking sphinx or doxygen documentation, although the inline
comments are helpful, and the general structure, variable names, method names
etc are fairly self-explanatory. The code is written in an easily readable
style. All the physics parameters have I3Unit labels. There are many in-line 
tests of physical assumptions, raising log-fatal if they fail. Several of the
files already have some heading comments (often repeated between header and 
implementation files), but not yet all.

Author: Claudio Kopper

Sponsor/reviewer: Laura Gladstone (me), previously Markus Vehring

Consumer: Low-Energy working group, Oscillations group, (WIMP group?)

Maintainer: Ken Clark

Versions: 
  genie-icetray: http://code.icecube.wisc.edu/svn/sandbox/ckopper/genie-icetray, 
  r98044, last change 2013-01-26 by claudio.kopper

my OS: mac OS 10.8.2

genie (from ports): 2.6.4

root: root_5.30.06 (includes pythia)

my IceRec:
    release V04-03-00, 
	plus like half of simulation V03-03-02, from 19 Dec 2012: 
	sim-services V01-03-00
	cmc VV02-00-00
	simple-injector V02-00-01
	mmc-icetray V04-00-04
	c2j-icetray V04-00-01

procedure: https://wiki.icecube.wisc.edu/index.php/Code_Review_Guidelines
	
Installation
------------

First impression: getting the required ports to build is a pain, but is
outside the purview of this code review. 

What solved my problems: remove all installed root versions before starting
(I had 2 running and they faught, causing badly labeled seg faults)::

	$I3_PORTS/bin/port list
	(look for all root versions)
	$I3_PORT/bin/port uninstal -f root_5.28.00
	$I3_PORT/bin/port clean --all root_5.28.00
	$I3_PORT/bin/port uninstal -f root_5.30.06
	$I3_PORT/bin/port clean --all root_5.30.06

some port-build setup::

	export LANG=C (if you skip this, you get "illegal byte order" error)
	export FC=/usr/local/bin/gfortran 

then the actual genie port instal (this pulls in dependencies, including 
root 5.30)::

	$I3_PORTS/bin/port install genie_2.6.4

Then::

	make rebuild_cache
	make

in your IceRec build as usual. 

I had to tweak the cmake lists to get them to work in my IceRec (release, not 
trunk) specifically by adding log4cplus back to the tools lists (in 3 places.) 
This shouldn't be a problem in the future, because trunk IceRec doesn't use
log4cplus (ie, genie-icetray is set up for new trunk, not old release like I'm
using)

With that tweak, and the genie port installed, genie-icetray compiles without 
error in release and debug mode. There were several warnings, all saying that 
something "hides overloaded virtual function", but these were from within
genie internal functions, and are thus outside the purview of this code 
review. 

Summary of the Guts
-------------------

There are two modules in this project: I3GENIEGenerator and 
I3GENIEResultDictToMCTree. 

The first runs Genie (using an IceCube random number generator, settings from 
the run script, and cross sections from an xml file in resources/splines). The 
generated output is written to the frame as an I3Map of lots of stuff; the map
is called I3GENIEResultDict, so if you want to get stuff out of it later, you
can. 

The second module translates the GENIEResultDict to a standard I3Tree called 
"I3MCTree" (as usual) which can be used the same way as nugen-generated output.

There are several supporting files, which, for example, convert Particle Data 
Group style particle type code numbers to I3Particle types. This particular 
file could be replaced by the one in dataclasses (for which it was apparently 
an early draft).

The random number generator is an I3RandomService wrapped up to look like
root's TRamdom, which Genie is expecting. This lets us handle the re-seeding
more like we usually do, and not reinvent an IceCube wheel. 

The I3LoggingAppender: Genie generates many many log messages, and
genie-icetray has wrapped these in logging levels one level lower than Genie 
assumes, which was a great idea, and puts these messages more in line with
standard IceCube code. (eg, error--> warn, warn--> info, info--> debug etc) 
Pipe down, genie, and kudos, Claudio.

Completeness & example scripts
------------------------------

The existing documentation should be put into a more usable format (eg
doxygen), but in its current state the code is easily usable. 

The results of icetray-inspect included a helpful string for every parameter
(of which there were many). The two modules are descriptively named.

In the resources/scripts folder, the two example scripts would each benefit 
from a sentence at the top stating its purpose, and perhaps how long it might 
take to run a normal number of events. However, their descriptive titles and 
useful presence are noted, and they run without error (except that this was 
the point where I had to add all those simulation projects to my local build,
and it took me a while to figure out that I needed to download another file). 

NEEDED FEATURE: a big fat error message if there's not an xml file present. 
Genie uses an xml file of cross section information as an input, and if that 
file isn't present, it tries to regenerate it, which (I'm told) takes about a 
day. There's a README file in the resources directory that explains where to 
download the file, but that would be more useful as a runtime warning. Normal 
generation times are o(millisecond), so if you're running and it's taking
longer, and you don't know why, then you, too, need this big fat error 
message. 

The README file in the resources folder, which explains where to download the 
spline xml file, could easily be converted to part of a doxygen page. 
Alternatively, the spline file itself could be checked in as part of the
project, except it's 55 MB. 

Source Code Structure Comments
------------------------------

The source code has the standard public/private folder structure, including
the preferred practice of putting header files in the private folder when they 
aren't used by other projects. 

The python directory doesn't have a clear purpose: its only content is an 
__init__.py file, and that doesn't have any project-specific content. This
looks like it was part of a blank project template that just never got deleted.

The example scripts should be moved to a resources/examples folder instead of 
resources/scripts. 

Some test scripts would be great. Mmm yea. 

There's support built in for qp-frame style data as well as older p-frame-only
style, and this is mostly accomplished with preprocessor commands. Those 
generally make me a bit nervous, but it seems to work well on my q-frame data, 
and we'll only be using q-frames from here on out, as far as I know. So it 
shouldn't be a problem. The p-frame-only mode wasn't tested. 

The main function in which genie-icetray calls Genie-genie is appropriately
called "GenerateEvent", and is cushioned among many log messages and tests
flagging its input and output. These include tests for unphysical or null 
events, and a "log_fatal("WTF?!")" if the number of generated events is
negative. 

There is a section commented out with a ""#if 0" which should probably be just
removed. It's related to charm decay, so it could be something that Claudio
meant to add back in later, but at least the "#if 0" surrounding lines could
be taken out.

The GenieTree to I3MCTree converter contains a log-fatal test that leptonic
outputs are among standard lepton types (muplus, muminus, tauplus etc). It
actually has a log-fatal test every 30 lines (on average), which is
impressive. The only one I could think of to add was a check that the
generated vertex point is within the specified geometry... and then I got to
the spot where that was checked. 

Conclusions: 

See introductory section: this should pass with that one extra error message,
and some formatting documentation. 

Questions following
-------------------

Here are some questions from Juan Pablo Yanez, who looked at genie-icetray ~Oct 2012.
I haven't yet looked into them, but since he got back to me before the second 
mail went out, the questions are listed (albeit without answers): 

- What happened with the weights? There was a lot of buzz on whether the 
  weights were correct. This was thought to be related with how the non-physical 
  interactions were handled (by the interface, in this case, genie-icetray). Was 
  that solved? Are the weights reliable? Is the output comparable to the genie 
  standalone? I only got the response "that was understood" by Jason, but I 
  never saw proof.

- Is the GenVolDepth parameter working (or is it even there)? When I tested it 
  the first time, it was there but not working. This shift in center of the 
  detector volume could be very useful to avoid generating events very far away, 
  and really focus only around DC, speeding things up.

- What happens to all the particles that GENIE generates? Do they all go to 
  the I3MCTree with their correct type? Which module should propagate them? And 
  the light?


And some comments from Markus Vehring:

In most of the things I am on the same page as you. Some other remarks are:

- there are a huge bunch of commented lines in I3GENIEGenerator.cxx (not the "#if 0" block). If they are not needed, they can go.
- some of the variable names in ConvertToGST.cxx are little bit hard to decipher, if one does not search through the whole file (what does "ist" stand for? .. oh it's a particle status). Since this is based on GENIE code (gNtpConv), this is somewhat inherited from there.
- I3GENIEResultDictToMCTree.cxx line 196ff: It's not clear to me why Mus and Taus should always have the shape of a StartingTrack.


Suggestion: as in clsim, see the script to get it to download a file at compile time

Other Errors
------------

Melanie and Kotoyo had the following error::

     Fatal error type 1 has occured after        0 PYEXEC calls:
     (PY2ENT:) writing outside PYJETS memory
     Execution will now be stopped!

     PYSTOP called with code:    3

The solution is to manually set the environmental variable $GENIE to the
location of the genie port in $I3_PORTS and then add the $GENIE/bin to
PATH and $GENIE/lib to LD_LIBRARY_PATH. Sometimes there are other
libraries missing from LD_LIBRARY_PATH as well. Try running gevgen by
typing "gevgen" and then add whatever libraries it says are not in the
path to LD_LIBRARY_PATH. Possibly there is a more elegant way to do this
but this way works to fix the problem...
