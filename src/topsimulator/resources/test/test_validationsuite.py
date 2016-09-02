#!/usr/bin/env python
from icecube.topsimulator.validationsuite import ValidationReport, Cmp

report = ValidationReport("test self", verbosity=2)

with report.section("section a"):
    report.success()
    try:
        raise StandardError("bla bla bla "*10)
    except:
        report.exception()
    with report.section("section b"):
        for i in xrange(10):
            report.failure()
        report.info("info 1")
        report.info("info 2")
    with report.section("section c"):
        for i in xrange(100):
            report.success()
    with report.section("section d with " + ("very "*40) + "long header"):
        report.success()
        report.info("info 3, multi-line\nbla bla bla\nfoo foo foo\nbar bar")

class Dummy:
    pass

try:
    a = Dummy()
    b = Dummy()
    Cmp(report, a, b)
except:
    report.exception()

Cmp.register(Dummy, lambda rep, cur, ref:True)
Cmp(report, a, b)

for verbosity in (0, 1, 2):
    report.verbosity = verbosity
    print "verbosity %i:" % verbosity
    print report
    print
