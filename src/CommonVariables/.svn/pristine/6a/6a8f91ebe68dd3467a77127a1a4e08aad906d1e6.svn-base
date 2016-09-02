#!/usr/bin/env python
"Find all executable python scripts in the examples folder and run them"
import os
import sys
import glob
import re
import tempfile
if sys.version_info < (2, 7):
	import unittest2 as unittest
else:
	import unittest
import subprocess as subp

# auto-generate the code for the test class
class_code = "class ExamplesTest(unittest.TestCase):"
for x in glob.glob(os.path.dirname(__file__) + "/../examples/*/*.py"):
	name = os.path.basename(x)[:-3]
	class_code += ("\n def test_{0}(self):"
		           "\n  f = tempfile.TemporaryFile()"
		           "\n  try:"
		           "\n   subp.check_call(['python', '{1}'], stdout=f, stderr=subp.STDOUT)"
		           "\n  except IOError:"
		           "\n   f.seek(0)"
		           "\n   sys.stderr.write(f.read())"
		           "\n   raise"
	               .format(name, x))
exec(class_code, locals(), globals())

if __name__ == '__main__':
	unittest.main()
