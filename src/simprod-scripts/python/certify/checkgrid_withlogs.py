#!/usr/bin/env python
################################ Tray 0 ##############################
#   on 2010-01-11 
#   
######################################################################
import commands,os,sys,string
from os.path import expandvars
import glob

from I3Tray import *
from iceprod.modules import ipmodule


# Instantiate parameters expression parser 

def boolcast(s): return s not in ["False","0","","[]"]
double = float

class CertifyGrid(ipmodule.IPBaseClass):
   """
   SysChk for production grids
   """
   #def __init__(self):
#        ipmodule.IPBaseClass.__init__(self)
#
#	self.AddParameter('photorec','main photorec directory','$steering(photorec)')
#	
#	self.AddParameter('photorecmu','main photorecmu directory','$steering(photorecmu)')
#	
#	self.AddParameter('photorecdir1','Old photorec muons directory','$steering(photorecDir1)')
#	
#	self.AddParameter('photorecdir2','Old photorec showers directory','$steering(photorecDir2)')
#	
#	self.AddParameter('photorecmudir','New photorecmu dir','$steering(photorecmuDir)')
#	
#	self.AddParameter('photoreccscddir','New photoreccscd dir','$steering(photoreccscdDir)')
#	
#	self.AddParameter('photontablesdir','photonics tables dir','$steering(photontablesDir)')
#
#
#   def GetCheckSum(self,FileArg):
#	import md5 
#	try:
#		FileArgBin = open(FileArg, "rb")
#		FileArgBin1 = FileArgBin.read()
#		MCipher = md5.new()
#		MCipher.update(FileArgBin1)
#		CipherHex = MCipher.hexdigest()
#		CipherHex1 = ""
#		for i in range(len(CipherHex)):
#			if i%2==1 and i<len(CipherHex)-1:
#				CipherHex1+=CipherHex[i]+":"
#			else:
#				CipherHex1+=CipherHex[i]
#				
#		return CipherHex1
#	except:
#		return 0
#		
#
#   def DoFilesCheckSum(self,FileList,AllCheckSums,Samples):
#	CheckStatus = 1
#	
#	BaseFileList = []
#	for File in FileList: BaseFileList.append(os.path.basename(File))
#	
#	for Sample in Samples: 
#		if Sample in BaseFileList:
#			CheckSum = self.GetCheckSum(FileList[BaseFileList.index(Sample)])
#			if not CheckSum == AllCheckSums[Sample] : 
#				CheckStatus = 0
#				break
#		else:
#			CheckStatus = 0
#			break
#	return CheckStatus
#
#
   def Execute(self,stats):
	if not ipmodule.IPBaseClass.Execute(self,stats): return 0
	
        import icecube.icetray
	import cPickle
	import random
	import md5 
#
#	
#	# check for existence of photonics and photorec tables directory and do checksum of random
#	# selection of some of the files  
#	
#	PhotoRecCheckSums = cPickle.load(open('PhotoRecCheckSums.dat', 'rb'))
#	
#	stats["HasPhotonTables"] = 0
#	stats["HasSPICETables"] = 0
#	stats["SPICETablesChecksumPass"] = 0
#	try:
#		PhotonTablesDir = self.GetParameter('photontablesdir')
#		PhotonTablesDir = self.parser.parse(PhotonTablesDir)
#	
#		if os.path.exists(str(PhotonTablesDir) +"/tables"): stats["HasPhotonTables"] = 1
#		
#		SPICETablesDir = str(PhotonTablesDir) +"/tables/SPICE1_i3coords_cos80/"
#		if os.path.exists(SPICETablesDir): 
#			stats["HasSPICETables"] = 1
#			if os.path.exists(SPICETablesDir +"/level1/tables") and os.path.exists(SPICETablesDir +"/level2/standard/tables"):
#				SPICEFileList = glob.glob(SPICETablesDir +"/level1/tables/*_z80*")
#				SPICEFileList.extend(glob.glob(SPICETablesDir +"/level2/standard/tables/*_z80*"))
#				SPICEFilesChkSums = PhotoRecCheckSums["SPICE1Files"]
#				SPICESampleSize = min(max(50, int(len(SPICEFilesChkSums))/10), len(SPICEFilesChkSums)-1)
#				SPICEFilesSample = random.sample(SPICEFilesChkSums.keys(),SPICESampleSize)
#				if len(SPICEFileList):
#					stats["SPICETablesChecksumPass"] = self.DoFilesCheckSum(SPICEFileList,SPICEFilesChkSums,SPICEFilesSample)
#	
#	except:
#		pass	
#	
#	
#	stats["HasPhotoRec"] = 0
#	stats["PhotoRecChecksumPass"] = 0
#	try:
#		PhotoRecDir1 = self.GetParameter('photorecdir1')
#		PhotoRecDir1 = self.parser.parse(PhotoRecDir1)
#	
#		PhotoRecDir2 = self.GetParameter('photorecdir2')
#		PhotoRecDir2 = self.parser.parse(PhotoRecDir2)
#
#		if os.path.exists(str(PhotoRecDir1)) and  os.path.exists(str(PhotoRecDir2)):stats["HasPhotoRec"] = 1 
#		
#		if (stats["HasPhotoRec"]):
#			OldFileList = glob.glob(str(PhotoRecDir1)+"/*")
#			OldFileList.extend(glob.glob(str(PhotoRecDir2)+"/*"))
#			#OldFilesChkSums = PhotoRecCheckSums["muFilesOld"]
#			#OldSampleSize = min(max(50, int(len(OldFilesChkSums))/10), len(OldFilesChkSums)-1)
#			#OldFilesSample = random.sample(OldFilesChkSums.keys(),OldSampleSize)
#			muFilesChkSums = PhotoRecCheckSums["muFiles"]
#			muSampleSize = min(max(50, int(len(muFilesChkSums))/10), len(muFilesChkSums)-1)
#			muFilesSample = random.sample(muFilesChkSums.keys(),muSampleSize)
#			
#			
#			OldFilesChkSums = muFilesChkSums
#			OldFilesSample = muFilesSample
#			
#			
#			if len(OldFileList):
#				stats["PhotoRecChecksumPass"] = self.DoFilesCheckSum(OldFileList,OldFilesChkSums,OldFilesSample)
#			
#	except:
#		pass
#	
#	
#	# checks for tables with NEW mu and cscd photorec tables directory structure
#	stats["HasPhotoRecMu"] = 0
#	stats["PhotoRecMuChecksumPass"] = 0
#	try:
#		PhotoRecMuDir = self.GetParameter('photorecmudir')
#		PhotoRecMuDir = self.parser.parse(PhotoRecMuDir)
#	
#		if os.path.exists(str(PhotoRecMuDir)): stats["HasPhotoRecMu"] = 1
#		
#		if (stats["HasPhotoRecMu"]):
#			muFilesChkSums = PhotoRecCheckSums["muFiles"]
#			muSampleSize = min(max(50, int(len(muFilesChkSums))/10), len(muFilesChkSums)-1)
#			muFilesSample = random.sample(muFilesChkSums.keys(),muSampleSize)
#			muFileList = glob.glob(str(PhotoRecMuDir)+"/*")
#			
#			if len(muFileList):	
#				stats["PhotoRecMuChecksumPass"] = self.DoFilesCheckSum(muFileList,muFilesChkSums,muFilesSample)
#			
#			# if original photorec check failed, check if old photorec variable is pointed to the new photorecmu
#			if not stats["PhotoRecChecksumPass"]:
#				PhotoRec = self.GetParameter('photorec')
#				PhotoRec = self.parser.parse(PhotoRec)
#				PhotoRecMu = self.GetParameter('photorecmu')
#				PhotoRecMu = self.parser.parse(PhotoRecMu)
#				if os.path.exists(str(PhotoRec)) and str(PhotoRec).find(str(PhotoRecMu)) > -1: 
#					stats["HasPhotoRec"] = 1 
#					stats["PhotoRecChecksumPass"] = stats["PhotoRecMuChecksumPass"]
#	except:
#		pass
#	
#	
#
#	stats["HasPhotoRecCscd"] = 0
#	stats["PhotoRecCscdChecksumPass"] = 0
#	try:
#		PhotoRecCscdDir = self.GetParameter('photoreccscddir')
#		PhotoRecCscdDir = self.parser.parse(PhotoRecCscdDir)
#		
#		if os.path.exists(str(PhotoRecCscdDir)): stats["HasPhotoRecCscd"] = 1
#		
#		if (stats["HasPhotoRecCscd"]):
#			cscdFilesChkSums = PhotoRecCheckSums["cscdFiles"]
#			cscdSampleSize = min(max(50, int(len(cscdFilesChkSums))/10), len(cscdFilesChkSums)-1)
#			cscdFilesSample = random.sample(cscdFilesChkSums.keys(),cscdSampleSize)
#			cscdFileList = glob.glob(str(PhotoRecCscdDir)+"/*")
#			
#			
#			if len(cscdFileList):
#				stats["PhotoRecCscdChecksumPass"] = self.DoFilesCheckSum(cscdFileList,cscdFilesChkSums,cscdFilesSample)
#	except:
#		pass
#	
#	
#	#check system configurations, compiler etc. requirements
#	
#	stats["TotalMem(GB)"] = 0
#	try:
#		MemOutput = (commands.getoutput("less /proc/meminfo | grep MemTotal")).split()
#		MemTotal = float(MemOutput[1])/1024
#		stats["TotalMem(GB)"] = float("%.2f"%(MemTotal/1000))  
#	except:
#		pass
#
#	stats["MemPerCPU>=2GB"] = 0
#	try:
#		NoOfCPUs = commands.getoutput("cat /proc/cpuinfo | grep processor | wc -l")
#		MemPerCPU = float(MemTotal/float(NoOfCPUs))
#		stats["NoOfCPUs"] = float(NoOfCPUs)
#		stats["MemPerCPU(GB)"] = float("%.2f"%(MemPerCPU/1000))  
#		if MemPerCPU >= 1900.00: stats["MemPerCPU>=2GB"] = 1
#	except:
#		pass
#
#	stats["CPUSpeed>=2GHz"] = 0
#	try:
#		CPUOutput = (commands.getoutput("less /proc/cpuinfo | grep -e 'cpu MHz'")).split()
#		CPUSpeed = float(CPUOutput[3])
#		stats["CPUSpeed(GHz)"] = float("%.2f"%(CPUSpeed/1000)) 
#		if CPUSpeed >= 2000.00: stats["CPUSpeed>=2GHz"] = 1
#	except:
#		pass
#	
#	stats["gccVerion>=3.4.6"]=0
#	try:
#		gccOutput = (commands.getoutput("gcc --version | grep gcc")).split()
#		gccVersion = gccOutput[2]
#		if gccVersion >= "3.4.6" : stats["gccVerion>=3.4.6"] = 1
#	except:
#		pass
#
#        stats["HasJAVA_HOME"] = 0
#	stats["JAVA_Version>=1.4.2"] = 0
#	try:
#		JAVA_HOME = commands.getoutput("echo $JAVA_HOME")
#		if os.path.exists(str(JAVA_HOME)): stats["HasJAVA_HOME"] = 1
#		if (stats["HasJAVA_HOME"]):
#			JAVA_VERSION = (commands.getoutput("$JAVA_HOME/bin/java -version")).split()
#			JAVA_VERSION = JAVA_VERSION[2][1:6]
#			if JAVA_VERSION >= "1.4.2" : stats["JAVA_Version>=1.4.2"] = 1
#
#	except:
#		pass
#	

	#target    = self.GetParameter('target')
        #target    = self.parser.parse(target) #in case we are using default

	return 0

class CopyLogs(ipmodule.IPBaseClass):
   """
   Copy log/outfiles to storage units with GridFTP
   """
   def Execute(self,stats):
 	if not ipmodule.IPBaseClass.Execute(self,stats): return 0
	
	import icecube.icetray
        from iceprod.modules import gsiftp
        from iceprod.core.dataclasses import I3PostTray

        post    = I3PostTray()
        job     = int(self.parser.parse("$args(procnum)"))
        key     = self.parser.parse("$args(key)")
        dataset = int(self.parser.parse("$args(dataset)"))
	TopDir = expandvars("$TMPTOP")	
	#moniurl = self.parser.parse("$args(url)")
        #localcp = boolcast(self.parser.parse("$system(localcp)"))
	
	station = self.parser.parse("$system(gridname)")
	tx_protocol = "gsiftp://data1.icecube.wisc.edu"
	if str(station) == "npx3": tx_protocol = "file:"
	#
#	
#	os.system("ls -alhtr %s/*log* > log_files"%TopDir)
#	post.AddModule("gsiftp.URLCopy","uploadlogfilelist")(
#    	   ("source","file:%s" % os.path.join(os.getcwd(),"log_files")),
#	   ("destination","%s/data/user/ice3simusr/SysChkLogs/%s/log_files"%(tx_protocol,str(dataset))),
#	   )
#
#	os.system("ls -alhtr %s/*out* > out_files"%TopDir)
#	post.AddModule("gsiftp.URLCopy","uploadoutfilelist")(
#	   ("source","file:%s" % os.path.join(os.getcwd(),"out_files")),
#    	   ("destination","%s/data/user/ice3simusr/SysChkLogs/%s/out_files"%(tx_protocol,str(dataset))),
#	   )
#	
#	os.system("ls -alhtr %s/*err* > error_files"%TopDir)
#     	post.AddModule("gsiftp.URLCopy","uploaderrfilelist")(
#	   ("source","file:%s" % os.path.join(os.getcwd(),"error_files")),
#    	   ("destination","%s/data/user/ice3simusr/SysChkLogs/%s/error_files"%(tx_protocol,str(dataset))),
#	   #("destination","file:/data/user/ice3simusr/SysChkLogs/%s/error_files"%str(dataset)),
#	   )

	
#
#	log_files = glob.glob(TopDir+"/*log*")
#	for log_file in log_files:
#		mod_name = "upload_" + os.path.basename(log_file)
#		post.AddModule("gsiftp.URLCopy",mod_name)(
#	   	("source","file:%s" % os.path.join(os.getcwd(),log_file)),
#	  	("destination","%s/data/user/ice3simusr/SysChkLogs/%s/%s"%(tx_protocol,str(dataset),os.path.basename(log_file))),
#		)
#		
#	out_files = glob.glob(TopDir+"/*out*")
#	for out_file in out_files:
#		mod_name = "upload_" + os.path.basename(out_file)
#		post.AddModule("gsiftp.URLCopy",mod_name)(
#	   	("source","file:%s" % os.path.join(os.getcwd(),out_file)),
#    		("destination","%s/data/user/ice3simusr/SysChkLogs/%s/%s"%(tx_protocol,str(dataset),os.path.basename(out_file))),
#	  	)
#
#	err_files = glob.glob(TopDir+"/*err*")
#	for err_file in err_files:
#		mod_name = "upload_" + os.path.basename(err_file)
#		post.AddModule("gsiftp.URLCopy",mod_name)(
#	   	("source","file:%s" % os.path.join(os.getcwd(),err_file)),
#    		("destination","%s/data/user/ice3simusr/SysChkLogs/%s/%s"%(tx_protocol,str(dataset),os.path.basename(err_file))),
#		#("destination","file:/data/user/ice3simusr/SysChkLogs/%s/%s"%(str(dataset),os.path.basename(err_file))),
#	  	)
		
		
	os.system(""" tar -cvf tarred_file.tar %s/*log* %s/*out* %s/*err*"""%(TopDir,TopDir,TopDir))	
		
	post.AddModule("gsiftp.URLCopy","upload_tar")(
	("source","file:%s" % os.path.join(os.getcwd(),"tarred_file.tar")),
    	("destination","%s/data/user/ice3simusr/SysChkLogs/%s/%s/%s_%s_tarred_file.tar"%(tx_protocol,str(station),str(dataset),str(job),str(key))),
	)
	
	
	post.Execute()

	print("*******")
	print(job)
	print(key)
	#print """ tar -cvf tarred_file.tar %s/*log* %s/*out* %s/*err*"""%(TopDir,TopDir,TopDir)
	print("*********")
	raise ("!")

	return 0
