#! /bin/sh 
# submit jobs to condor (for UW cluster)
# need to modify the script for your environment
#
# basedir : your icesim dir
# runscript : path to your script
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
if [ -z "$basedir" ] ; then
    echo "basedir is empty! you have to set I3_BUILD environment valiable."
    exit
fi

runscript=$basedir/neutrino-generator/resources/util/MakeInEarthPropWeightTable.py
inputsuffix=".i3.bz2"


###
flavor=$1
dataset=$2
nzenbins=$3
ncoszenbins=$4
nebins=$5
elogs=$6
zens=$7
gengamma=$8
weightgamma=$9
inputdir=${10}
outdir=${11}
mergefiles=${12}

echo $inputdir
echo $outdir

submitdir=$outdir/submit
if [ -e $outdir ] ; then
   echo "outdir exists! exit now."
   exit 0
   #rm -rf $outdir
fi
echo $submitdir
mkdir -p $submitdir

submitbasename="MakeTable"

# dagman setup
dagfile=$submitdir/$submitbasename.dag
dagconf=$basedir/neutrino-generator/resources/util/condor_submit/dagman.config
if [ -f $dagfile ] ; then
   rm $dagfile
fi

infiles=""
ifile=0
count=0
procno=0
echo "start forloop"

for infile in `ls $inputdir/*$inputsuffix | sed -e "s/$inputsuffix//"`; do
  echo "input file is " $infile

  # list input files
  infiles="$infiles ${infile}${inputsuffix}"

  # incriment ifile counter
  ifile=`expr $ifile + 1`

  # submit job
  if [ "$ifile" = "$mergefiles" ] ; then
    outfilebase=`basename $infile`
    outfile=$outdir/$outfilebase
    outbase=${outfilebase}_MakeTable
    shellname=$outbase.sh
    submitname=$outbase.submit

    cat << EOF1 > $submitdir/$shellname
#!/bin/sh

cd $basedir

source $basedir/mysetup.sh

$basedir/env-shell.sh python $runscript $flavor $dataset $nzenbins $ncoszenbins $nebins $elogs $zens $gengamma $weightgamma $outfile $infiles

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

    echo "JOB $procno $submitdir/$submitname" >> $dagfile

    ifile=0
    outfile=""
    infiles=""
    procno=`expr $procno + 1`

  fi

done

# submit last job

if [ $ifile -ne 0 ] ; then 
    outfilebase=`basename $infile`
    outfile=$outdir/$outfilebase
    outbase=${outfilebase}_MakeTable
    shellname=$outbase.sh
    submitname=$outbase.submit

    cat << EOF1 > $submitdir/$shellname
#!/bin/sh

cd $basedir
source $basedir/mysetup.sh

./env-shell.sh python $runscript $flavor $dataset $nzenbins $ncoszenbins $nebins $elogs $zens $gengamma $weightgamma $outfile $infiles

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

    echo "JOB $procno $submitdir/$submitname" >> $dagfile
fi

# copy run script for record.
cp $runscript $submitdir/

echo "condor_submit_dag -f -config $dagconf $dagfile" > $submitdir/run_MakeTable.sh

sh $submitdir/run_MakeTable.sh

echo "Done!"

