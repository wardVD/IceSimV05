..  $Date: 2016-03-08 12:23:35 -0600 (Tue, 08 Mar 2016) $
..  $Revision: 2454 $
..  $Author: nega $
..  $HeadURL: http://code.icecube.wisc.edu/icetray/projects/icetray/releases/V16-05-00/resources/docs/index.rst $
..  $Id: index.rst 2454 2016-03-08 18:23:35Z nega $

.. _icetray:

Icetray
=======

.. note::
   
   Herein, *module* has a couple of meanings.  One is ``I3Module``,
   which is the interface by which one writes plugins/extensions to
   icetray.  *Module* is also used in the sense of a group of python
   extensions, which, to add to the confusion, are more like icetray
   *projects* than icetray *modules*.  Were it possible at this stage
   I would rename the ``I3Module`` variety of module to 'plugin'.

   The use of *module* at the top right hand of this page (next to
   *index*), is in the python-module sense.  It is not supposed to
   take you to a listing of all I3Modules.

General Overview:

.. toctree::
   :maxdepth: 3

   release_notes
   summary
   pybindings
   icetray_quick_reference
   services
   project

Detailed Documentation:

.. toctree::
   :maxdepth: 3

   classes
   logging
   inspect
   config
   standard_modules
   scripting
   i3units
   keywordparams
   functions 
   modules
   services   
   segments
   qframes
   i3podholder
   I3Tray

Also see the `doxygen <../../doxygen/icetray/index.html>`_ docs.
