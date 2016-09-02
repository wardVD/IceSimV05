// LinkDef.h
// $Id: LinkDef.h 118217 2014-03-28 16:50:37Z olivas $
// $Date: 2014-03-28 11:50:37 -0500 (Fri, 28 Mar 2014) $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

// namespaces

#pragma link C++ namespace I3Units;
//#pragma link C++ namespace I3Constants;
#pragma link C++ namespace boost;

#pragma link C++ class I3Position+; //
#pragma link C++ class I3Direction+; //
#pragma link C++ class I3Particle+;
#pragma link C++ class vector<I3Particle>+;
//#pragma link C++ class I3Trigger+;
//#pragma link C++ class vector<I3Trigger>+;
//#pragma link C++ class TriggerKey+;
#pragma link C++ class I3Double+;
#pragma link C++ class I3Time+;

#endif




