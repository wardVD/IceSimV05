#!/usr/bin/env python
"""
Helper script to migrate classes from one project to another.

You need to run this script from the top-level source directory. 
It relies on the usual subdirectory structure in projects.
It can deal only with the simplest case, if someone did something
fancy in their project, manual action is needed.

The script moves the header from public, the implementation from
private and the pybindings. It does not handle converters.
"""
import os
import subprocess as subp

import argparse

def move(a, b):
	cmd = "svn mv {0} {1}".format(a, b)
	print cmd
	subp.check_call(cmd.split())

def replace(fn, a, b):
	print "replace",fn,a,b
	with open(fn, "r") as f:
		content = f.read()
	open(fn, "w").write(content.replace(a, b))

def projectname(fn):
	return fn.strip("/").split("/")[-1]

def files_with_pattern(pattern):
	result = []
	cmd = "grep -e {0} -l -R .".format(pattern)
	result = subp.Popen(cmd.split(),
		stdout=subp.PIPE, stderr=open("/dev/null", "w")).communicate()[0].split("\n")
	return [x for x in result if x and not x.isspace() and not ".svn" in x]

def locate_file(path, fname, filter=lambda x:True):
	result = []
	cmd = "find {0} -path */{1}".format(path, fname)
	result = subp.Popen(cmd.split(),
		stdout=subp.PIPE, stderr=open("/dev/null", "w")).communicate()[0].split("\n")
	result = [x for x in result
	          if x and not x.isspace() and not ".svn" in x
	          and filter(x)]
	if len(result) == 0:
		raise ValueError("no match for " + fname)
	if len(result) > 1:
		raise ValueError("result ambiguous for {0}:\n{1}".format(fname, "\n".join(result)))
	return result[0]

parser = argparse.ArgumentParser()
parser.add_argument("-s", help="Source directory")
parser.add_argument("-d", help="Destination directory")
parser.add_argument("classes", nargs="+")

args = parser.parse_args()

src = args.s
dst = args.d
src_project = projectname(args.s)
dst_project = projectname(args.d)

for cl in args.classes:
	header_path = locate_file(src, cl + ".h")

	# replace paths to header
	for fn in files_with_pattern("include.*{src_project}.*/{cl}"
		                         .format(**locals())):
		ind = header_path.index("public/")
		header = header_path[ind+7:]
		replace(fn, header,
		        "{dst_project}/{cl}.h".format(**locals()))

	# move header
	move(header_path,
		 "{dst}/public/{dst_project}/{cl}.h"
		 .format(**locals()))

	# move implementation
	try:
		impl_path = locate_file(src, cl + ".cxx", lambda x: "pybindings" not in x)
		move(impl_path,
			 "{dst}/private/{dst_project}/{cl}.cxx"
			 .format(**locals()))
	except:
		print "Couldn't move implementation for", cl

	# move pybindings
	try:
		pyb_path = locate_file(src, cl + ".cxx", lambda x: "pybindings" in x)
		move(pyb_path,
			 "{dst}/private/pybindings/{cl}.cxx"
			 .format(**locals()))
	except:
		print "Couldn't move pybindings for", cl
