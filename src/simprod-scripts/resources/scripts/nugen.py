#!/usr/bin/env python
import logging
logging.basicConfig()
rootLogger = logging.getLogger('')
rootLogger.setLevel(logging.INFO)

from icecube.simprod.modules.nugen import NuGen

if __name__ == '__main__':
   stats = {}
   gen = NuGen()
   gen.ExecuteOpts(stats)
