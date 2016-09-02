#! /bin/sh 
# submit jobs to condor (for UW cluster)
# need to modify the script for your environment
#
# basedir : your icesim dir
# runscript : path to NuGenInEarthProp.py
# email : your email address 
#
# also, you have to save "mysetup.sh" in your basedir as
# 
# cd $basedir
# eval `/cvmfs/icecube.opensciencegrid.org/setup.sh` > mysetup.sh
#
# to see examples to run the script, see scratchpad.
#

basedir=${I3_BUILD}
runscript=$basedir/neutrino-generator/resources/examples/NuGenInEarthProp.py
tablescript=$basedir/neutrino-generator/resources/util/MakeInEarthPropWeightTable.py

if [ "$basedir" == "" ] ; then
  echo "define I3_BUILD first! exit."
  exit 0
fi

###

dataset=$1
nfiles=$2
outbasedir=$3
flavor=$4
ngen=$5
gamma=$6
wgamma=$7
erange=$8
zenrange=$9
zenweight=${10}
xsecmodel=${11}
globalscales=${12}
earthmodels=${13}
nzenbins=${14}
ncoszenbins=${15}
nebins=${16}
erange2=${17}
zenrange2=${18}
procno=${19}

outdir=$outbasedir/$dataset
submitdir=$outdir/submit
picklesdir=`printf Z%d_C%d_E%d $nzenbins $ncoszenbins $nebins`

wgamma2=`echo $wgamma | sed -e 's/:/,/g'`
#split
IFS=, eval 'wgammaarr=($wgamma2)'   

if [ "$procno" == "0" ] ; then
   if [ -d $outdir ] ; then
      echo "$outdir exist! you need to speciry different dataset because dataset number is used for seed."
      exit;
   fi
   mkdir -p $outdir
   mkdir -p $submitdir
   mkdir -p $outdir/$picklesdir

   for wgdir in "${wgammaarr[@]}"; do
      print $wgdir
      mkdir -p $outdir/$picklesdir/W$wgdir
   done
   echo "dir generated"
fi

# dagman setup
dagfile=$submitdir/$dataset_$procno.dag
if [ -f $dagfile ] ; then
   rm $dagfile
fi
dagconf=$basedir/neutrino-generator/resources/util/condor_submit/dagman.config

outrootfile=""
infiles=""
maxproc=`expr $procno + $nfiles`
lastprocno=$procno
filecount=0
echo "start forloop"

while [ $(($procno)) -lt $(($maxproc)) ] 
do
  seed=`expr $dataset + 100 \* $procno`
  echo "seed is $seed"

  # submit job
  outbase=`printf %06d_%06d $dataset $procno`
  outi3file=$outdir/$outbase.i3.bz2
  shellname=$outbase.sh
  submitname=$outbase.submit

  cat << EOF1 > $submitdir/$shellname
#!/bin/sh

cd $basedir
source $basedir/mysetup.sh

./env-shell.sh python $runscript $outi3file $flavor $ngen $gamma $erange $zenrange $zenweight $xsecmodel $globalscales $earthmodels $seed $nfiles $filecount

EOF1

for wgdir in "${wgammaarr[@]}"; do

echo "./env-shell.sh python $tablescript $flavor $dataset $nzenbins $ncoszenbins $nebins $erange2 $zenrange2 $gamma $wgdir $outdir/$picklesdir/W$wgdir/$outbase $outi3file" >> $submitdir/$shellname

done
 
    chmod a+x $submitdir/$shellname

    #generate submit file
    cat << EOF2 > $submitdir/$submitname
executable = $submitdir/$shellname
universe   = vanilla
error      = $submitdir/$outbase.error
output     = $submitdir/$outbase.out
log        = $submitdir/$outbase.condorlog
transfer_input_files = $runscript
should_transfer_files = YES
notification = Error
priority = 10
notify_user = $email
when_to_transfer_output = ON_EXIT_OR_EVICT
periodic_remove = CumulativeSuspensionTime > ((RemoteWallClockTime - CumulativeSuspensionTime) / 2.0)
queue
EOF2


  echo "JOB $filecount $submitdir/$submitname" >> $dagfile

  lastprocno=$procno
  procno=`expr $procno + 1`
  filecount=`expr $filecount + 1`

done

# copy run script for record.
cp $runscript $submitdir/

echo "condor_submit_dag -f -config $dagconf $dagfile" > $submitdir/run_$lastprocno.sh

sh $submitdir/run_$lastprocno.sh

echo "Done!"

