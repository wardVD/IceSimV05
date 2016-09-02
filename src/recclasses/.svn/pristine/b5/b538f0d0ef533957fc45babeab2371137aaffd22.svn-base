import glob
import re
from collections import namedtuple
import sys
import os

class class_data:
	def __init__(self, project, converter):
		self.project = project
		self.converter = converter

re_pattern = (
	"/\*\*" # begin doxy comment
  + ".+?see project ([a-zA-Z_\-0-9]+).+?" # match project
  + "\*/" # end doxy comment
  + "\s*" # optional space
  + "class\s+([A-Za-z_0-9]+)" # match class
)

cls = {}
for header in glob.glob("../../public/recclasses/*.h"):
	tx = open(header).read()
	for project, class_name in re.findall(re_pattern, tx, re.DOTALL):
		cls[class_name] = class_data(project, None)
	for class_name in re.findall("class\s+([A-Za-z_0-9]+)\s*:\s*public\s*I3FrameObject", tx):
		if class_name not in cls:
			cls[class_name] = class_data(None, None)

# C++ converter
for converter in glob.glob("../../private/recclasses/converter/*.h"):
	basename = os.path.basename(converter)
	class_name = basename[:basename.index("Converter")]
	if class_name not in cls:
		raise ValueError(class_name + "Converter has no corresponding key in cls")
	cls[class_name].converter = class_name + "Converter"

# Python converter
for converter in glob.glob("../../python/*Converter.py"):
	basename = os.path.basename(converter)
	class_name = basename[:basename.index("Converter")]
	if class_name not in cls:
		raise ValueError(class_name + "Converter has no corresponding key in cls")
	cls[class_name].converter = class_name + "Converter (Python)" 

# sorting
l = []
for x in cls:
	y = x[2:] if x.startswith("I3") else x
	l.append((y, x))
l.sort()
l = [x[1] for x in l]

table = []
width = [50, 50, 50]
for class_name in l:
	data = cls[class_name]
	project = data.project
	converter = data.converter
	row = ["`{class_name} <../../doxygen/recclasses/class{class_name}.html>`_".format(**locals()),
	       "{converter}".format(**locals()) if converter else "",
	       ":ref:`{project}`".format(**locals()) if project else ""]
	for i, c in enumerate(row):
		width[i] = max(width[i], len(c) + 1)
	table.append(row)
sep = [['=' * w for w in width]]
table = sep + [["storage classes", "converter", "associated project"]] + sep + table + sep

for row in table:
	for i, c in enumerate(row):
		row[i] += " " * (width[i] - len(row[i]))
	line = "  ".join(row)
	sys.stdout.write(line + "\n")
