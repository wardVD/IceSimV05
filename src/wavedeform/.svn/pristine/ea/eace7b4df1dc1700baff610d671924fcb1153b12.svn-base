Reviewed by A.Olivas 4/7/15
***************************

- URL: http://code.icecube.wisc.edu/svn/projects/wavedeform/trunk
- Repository UUID: 16731396-06f5-0310-8873-f7f720988828
- Revision: 131173
- Last Changed Author: meike.dewith
- Last Changed Rev: 131078
- Last Changed Date: 2015-04-04 05:39:20 -0700 (Sat, 04 Apr 2015)

Documentation
*************
http://software.icecube.wisc.edu/simulation_trunk/projects/wavedeform/index.html

Documentation is a little sparse.  I get a rough sense of what the project is about,
but there aren't many details on the main page.  That main page is out-of-date too.
The following module parameters have no documentation :
- NoiseThreshold
- BasisThreshold
- WaveformTimeRange
- DeweightFADC
- ApplySPECorrections

There's really not much more to say about the docs since this is all there is,
except more is needed.

**JB: Descriptions for these parameters have been added to the docs**

Code
*************

I3Wavedeform.cxx
++++++++++++++++

WaveformTemplate :
o Public members of structure-less data aggregates don't use a 
trailing underscore.  Those denote private members of classes.
o What's the purpose of the 'valid' flag?  When is this data invalid?

**JB: Remove underscores, swap 'valid' --> 'filled' to be more accurate.**

I3Wavedeform :
o Why is GetPulses public?  This is an I3ContionalModule and has no need for a public
interface other than what it derives from I3ConditionalModule.

**JB: Done**

PULSE_WIDTH and PULSE_MIN - **"#16 Avoid macros."**  Make these inline functions.  There's
no need to make these macros and only propagates bad habits.  It seems in one of these,
perhaps some history is lost?  

// Beginning time (ns) of pulse template
#define PULSE_MIN(source)	(-2)

This acts like a function, but is always single-valued.  In code this still looks like 
a function and the reader needs to look back to remember that this is only ever -2.
Did this once have different values for ATWD and FADC?  Does anyone remember?

**JB: These are now inline functions.  I'm sure you're correct that the FADC/ATWD
templates had a differing start times at some point in the past. I've preserved
the single-valued PULSE_MIN as an inlin function since, theoretically, we could
change this behavior in the future, and having the function is not a style issue.**

Lines 139 and 145 - **"#17 : Avoid magic numbers."**  Setting the template bin spacing.
Most IceCubers will recognize where 3.3 and 25 come from, but what about new people?

**JB: Added comments to motivate these numbers**

One thing I don't get is that I3Wavedeform::FillTemplate is only callled if the template
is not valid.  In that case that method needs the I3DOMCalibration and will use the one 
for a whichever DOM happens to be first in the series in the map.  How is this applicable
to *all* subsequent DOMs.  If it is, for example the templates used in the calibration
are the same for all DOMs, then there's quite a bit of simplification that can happen
here, right?

**JB: The template used is the same for all DOMs when considered as voltage vs. time.
The template provided by I3DOMCalibration is slow to calculate, so we cache it
internally at high resolution.  We unfortunately cannot simplify further because
the ATWD bin width and ATWD-FADC offset varies DOM-by-DOM (if it didn't, we
could cache the normal basis matrix A^tA, resulting in a significant speedup).**

I3Wavedeform::GetPulses :  This is where all of the heavy-lifting is done.  Well it's
exported to C, but for the C++ functionality of this module, this is where it happens.
This method needs better documentation.  The name doesn't really tell me much other
than pulses are somehow gotten.  This continues on for over 400 more lines, in which
something magic happens and we get pulses in the end.  **"#20 : Avoid long functions.  
Avoid deep nesting** ...Excessively long functions and nested code blocks are often
caused by failing to give one function one cohesive responsility (**#5**).

Methods and functions that are broken down into cohesive units are easier to test too.

From the Linux Kernel Coding Standards (https://www.kernel.org/doc/Documentation/CodingStyle) :

"		Chapter 6: Functions

Functions should be short and sweet, and do just one thing.  They should
fit on one or two screenfuls of text (the ISO/ANSI screen size is 80x24,
as we all know), and do one thing and do that well.

The maximum length of a function is inversely proportional to the
complexity and indentation level of that function.  So, if you have a
conceptually simple function that is just one long (but simple)
case-statement, where you have to do lots of small things for a lot of
different cases, it's OK to have a longer function.

However, if you have a complex function, and you suspect that a
less-than-gifted first-year high-school student might not even
understand what the function is all about, you should adhere to the
maximum limits all the more closely.  Use helper functions with
descriptive names (you can ask the compiler to in-line them if you think
it's performance-critical, and it will probably do a better job of it
than you would have done)."

So it's not just Sutter and Alexandrescu...Linus Torvalds also thinks shorter
functions are just good practice.

 
**"#18 : Declare variables as locally as possible."** - I know this is C-style to
declare all your variables at the beginning of the function, but I3Wavedeform::GetPulses is
still in C++-land.  Crossing-over styles is just bad form.

**JB: I've added a long comment to demystify GetPulses.  GetPulses clearly has subtasks
and therefore can be refactored.  It isn't obvious to me that rewriting the code
with separate functions to handle these 100+ line subtasks would result in clearer
code.  I think we'd only get clearer code from a complete rewrite of this function.
I've created issue #1566 to keep track of this.**

Line 299 and 301 - base_weight is set to either 3 or 1.  Where does this come from?
More magic numbers later...lines 339 and 342...354...I ran out of steam around this point.

**JB: Added some comments here**

nnls.c - Any reason this needs to be in C?

**JB: No, but there is no compelling reason to change it either.  Ideally this would
be a template function, taking a vector type, and solver type, with
a SuiteSparse implementation of this template.**

Tests
+++++
http://software.icecube.wisc.edu/coverage/00_LATEST/wavedeform/private/wavedeform/index.html

This project is pretty well tested.  The branch testing should probably be better.

- Lines - 91.4%
- Functions - 100%
- Branches - 55.8%
