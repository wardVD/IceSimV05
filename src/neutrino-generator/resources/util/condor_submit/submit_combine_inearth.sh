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

#basedir=/net/user/hoshina/SIMULATION/trunk/release2.7
basedir=${I3_BUILD}
runscript=$basedir/neutrino-generator/resources/util/CombineInEarthPropWeightTable.py

if [ "$basedir" == "" ] ; then
  echo "define I3_BUILD first! exit."
  exit 0
fi

###

outbasedir=$1
outfilebase=$2
infilebase=$3

outdir=$outbasedir
submitdir=$outdir/submit

filecount=1

if [ -d $submitdir ] ; then
#rm -rf $submitdir
echo "dir exist"
else
mkdir -p $submitdir
echo "dir generated"
fi

# dagman setup
dagfile=$submitdir/$outfilebase.dag
if [ -f $dagfile ] ; then
   rm $dagfile
fi
dagconf=$basedir/neutrino-generator/resources/util/condor_submit/dagman.config

# submit job
shellname=$outfilebase.sh
submitname=$outfilebase.submit

cat << EOF1 > $submitdir/$shellname
#!/bin/sh

cd $basedir
source $basedir/mysetup.sh

./env-shell.sh python $runscript $outbasedir/$outfilebase $outbasedir/$infilebase 

EOF1

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

cp $runscript $submitdir/

echo "condor_submit_dag -f -config $dagconf $dagfile" > $submitdir/${outfilebase}_run.sh

sh $submitdir/${outfilebase}_run.sh

echo "Done!"

