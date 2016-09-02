#!/usr/bin/env python

from icecube import icetray, recclasses

time = 4.8
distance = 262.3
key = icetray.OMKey(14,52)

record = recclasses.I3ShieldHitRecord()
record.time_residual = time
record.distance = distance
record.DOMkey = key

if(record.time_residual != time):
	print "time_residual property unreliable!"
	exit(1)
if(record.distance != distance):
	print "distance property unreliable!"
	exit(1)
if(record.DOMkey != key):
	print "DOMkey property unreliable!"
	exit(1)

record_list = recclasses.I3VectorShieldHitRecord()

if(len(record_list) != 0):
	print("I3VectorShieldHitRecord initial length not zero")
	exit(1)

record_list.append(record)

if(len(record_list)!=1):
	print("I3VectorShieldHitRecord.append does not work")
	exit(1)
if(record_list[0].time_residual!=time):
	print("I3VectorShieldHitRecord.append does not work")
	exit(1)
if(record_list[0].distance!=distance):
	print("I3VectorShieldHitRecord.append does not work")
	exit(1)
if(record_list[0].DOMkey!=key):
	print("I3VectorShieldHitRecord.append does not work")
	exit(1)
