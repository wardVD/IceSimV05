#!/bin/bash

# ulimit -v $[8*1024*1024]  # 8 Gb

if test "$#" -ne 2; then echo usage: "$0 [run] [nugen/corsika]"; exit; fi
if test "$1" = ""; then echo must specify valid run number; exit; fi
if test "$2" = "nugen" || test "$2" = "corsika"
then echo "accepting parameters: $*"
else echo "must specify nugen or corsika in second parameter"; exit; fi

while test -e stop; do echo -n "." 1>&2; sleep 100; done

base=/data/ana/IC86/ppc
dir=$base/in/$1

if test -d $dir; then echo "directory \"$dir\" exists"
else echo "directory \"$dir\" does not exist"; exit; fi

out=$base/mie/$1
mkdir -p $out

if test -d $out; then echo "directory \"$out\" exists"
else echo "directory \"$out\" does not exist"; exit; fi

mkdir -p $out/out
mkdir -p $out/log

if test -e count && test -e files; then echo starting with `cat count`; else ls $dir > files; echo 0 > count; fi
num=`awk 'END {print NR}' files`

mytemp(){ for i in $*; do while ! (set -C; echo -n > $i) 2>/dev/null; do sleep 0.1; done; echo $i; done; }
finish(){ touch stop; echo stopping at `cat count`/$num; rm stop.*; wait; }
trap "finish" INT TERM
host=`hostname -s`
tab='[ \t][ \t]*'

for ovr in 0; do
for gpu in $(sed -n '1,/#badmps/s/^'"$host$tab"'//ip' clist); do

while true; do

mytemp stop.$gpu-$host blockfile > /dev/null
next=`cat count`; next=$[$next+1]
if test -e stop || test "$next" -gt $num; then rm stop.$gpu-$host blockfile; break; fi
echo $next > count; rm stop.$gpu-$host blockfile

f=`awk 'NR=='$next files`; echo $gpu $ovr $next $f;
g=`echo $f | sed 's-\.i3\.gz$--; s-\.gz$--'`

if test -e $out/log/$g.txt; then
  echo file exists: `ls -l $out/log/$g.txt`
elif test -e $out/out/$g.i3.gz; then
  echo file exists: `ls -l $out/out/$g.i3.gz`
else
  while true; do
    badmp=$(sed -n '/#badmps/,$s/^'"$host$tab$gpu$tab"'/BADMP=/ip' clist)
    eval $badmp ./run.py $2 $gpu $next $dir/$f $out/out/$g.i3.gz >& $out/log/$g.txt; ec=$?
    grep Error $out/log/$g.txt | awk '{print "'"$gpu $ovr $next exit code=$ec"' :  "$0}'
    if test $ec = 0; then
      break
    elif test `grep MemoryError $out/log/$g.txt`"" != ""; then
      mkdir $out/bad 2> /dev/null
      mv $out/out/$g.i3.gz $out/log/$g.txt $out/bad
      echo "$gpu $ovr $next $f  - abandoning this job -"
      break
    else
      sleep 10
      if test -e stop; then break; else echo "$gpu $ovr $next $f  - resubmitting -"; fi
    fi
  done
fi

done | tee -a log.$host &
done
done
wait
