Code Review
===========

This project was reviewed by K. Krings, April 1, 2016.

Project information
-------------------

* URL: http://code.icecube.wisc.edu/svn/projects/production-histograms/trunk
* Repository Root: http://code.icecube.wisc.edu/svn
* Repository UUID: 16731396-06f5-0310-8873-f7f720988828
* Revision: 144132
* Last Changed Author: olivas
* Last Changed Rev: 143960
* Last Changed Date: 2016-03-30 17:44:00 +0200 (Wed, 30 Mar 2016)

First glance
------------

The project looks quite complete: documentation, example scripts, and unit
tests are available. Although, there is no test for py:class:`HistogramModule`.
The project is completely python-based and thus does not need to be compiled.
All unit tests except ``test_example_sripts.py`` succeed.  This is due to
missing input data, which still has to be generated.

Documentation
-------------

The Sphinx documentation does not compile without errors/warnings:

    * ``tutorial.rst``, line 30: ERROR: Unexpected indentation.
    * ``tutorial.rst``, line 32: WARNING: Block quote ends without a blank
      line; unexpected unindent.

The reST-files ``expression_histograms.rst``, ``histogram_modules.rst``, and
``histograms.rst`` are not included in any toctree. The first one has no
content and it looks like the other two also contain syntax errors.

In general, the documentation describes the project pretty well and gives a lot
of examples how to use it. I have only some minor remarks: to me, it looks
weird that most toctree links forward you to the same page and would make more
use of Sphinx directives when talking about the project's Python classes.

Source code
-----------

Code structure
^^^^^^^^^^^^^^

In general, the project's code structure follows our guidelines. The only thing
I noticed is that `PoleMuonLineFit` and `PoleMuonLlhFit` belong in
:mod:`production_histograms.filtering` instead of
``histograms/L2/particles.py``.

Invalid syntax
^^^^^^^^^^^^^^

    * ``histogram_modules/histogram_module:19``

This only works if the object can be initialized without passing any arguments.

    * ``histogram_modules/simulation/corsika_weight.py:12``
    * ``histogram_modules/simulation/corsika_weight.py:13``
    * ``histogram_modules/simulation/nugen_weight.py:12``
    * ``histogram_modules/simulation/nugen_weight.py:13``

Here, also not all necessary arguments are passed to the constructor.

Redundant code
^^^^^^^^^^^^^^

    * ``histograms/filtering/linefit.py``
    * ``histograms/filtering/particles.py``

Both contain the exact same code.

    * ``histogram_modules/simulation/mctree.py``
    * ``histogram_modules_simulation/mctree_primary.py``

Both declare a `type_to_int_dict` dictionary, which has the same purpose.

Coding standards
^^^^^^^^^^^^^^^^

At several places, Python modules are included that are actually not used at
all and I noticed a lot of dead spaces, wrong number of blank lines and missing
spaces between parameters.


Response - A.Olivas
^^^^^^^^^^^^^^^^^^^

* Added test for HistogramModule 
* Added a comment to explain what's going on in histogram_module:19  Also it's a strict requirement that all histograms and histogram_modules be default constructible.  This was discussed and decided on during one of the code sprints.  I forget which one.  You can get around this by creating an instance, modifying parameters, and appending the instance instead, which was why the feature was added.
* Added docs for expression histogram.
* Added histogram, histogram_module, and expression_histogram to the toctree.
* Removed filtering/linefit.py
* The dictionaries type_to_int_dict have almost the same purpose, but are different dictionaries.  One is for primaries and the other is for secondaries.
* Need feedback on the included python modules that aren't used, dead spaces, wrong number of blank lines, and missing spaces.  I'm not seeing it.
