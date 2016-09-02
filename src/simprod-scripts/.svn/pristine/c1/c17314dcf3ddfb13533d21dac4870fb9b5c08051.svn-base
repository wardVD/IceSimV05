import sys,os
import os.path
from os.path import abspath,dirname,basename, exists
from optparse import OptionParser



def install(src, dest):
   target = dirname(dest)
   if abspath(src) == abspath(os.getcwd()): return
   if abspath(src) == abspath(target): return
   if not exists(abspath(src)): return
   print("%s ==> %s" %(src,os.path.abspath(dest)))
   os.system("mkdir -p %s" %target)
   os.system("cp -r -u -L %s %s/" %(src,target))


parser = OptionParser()
usage = """%prog [options]"""
parser.set_usage(usage)

parser.add_option("-p", "--prefix", action="store", type="string", default=os.getcwd(), 
		  dest="prefix", help="")
parser.add_option("-l", "--lib", action="store", type="string", default=sys.prefix+'/libpython2.6.so', 
		  dest="lib", help="")


(options,args) = parser.parse_args()

prefix = options.prefix
while prefix.endswith("/"): 
    prefix = prefix[:-1]

# Install executable
install(sys.executable, sys.executable.replace(sys.exec_prefix,prefix))
install(options.lib,options.lib.replace(sys.prefix,prefix))
for l in sys.path:
    if l.startswith(sys.prefix):
       install(l,l.replace(sys.prefix,prefix))
