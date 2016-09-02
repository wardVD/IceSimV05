#!/bin/bash

## DO NOT COPY THIS SCRIPT INTO YOUR PROJECT
## Ask on the dataclass e-mail list how to do this

# Feb 17 2014 K.Hoshina
# I copied it from clsim/resources/download_safeprimes.sh shamelessly...
#

set -e

BASE_URL="http://code.icecube.wisc.edu/tools/neutrino-generator"
XSEC_FILE="nugen-v3-tables.tgz"
MD5_FILE="nugen-v3-tables.md5sum"
XSEC_DIR="cross_section_data"

# require exactly one argument
if [ $# -ne 1 ]; then
    echo "Usage: `basename $0` {resources_dir}"
    exit 1
fi
RESOURCES_DIR=$1

#--------------------------
# remove old data
#--------------------------
rm -rf $RESOURCES_DIR/$XSEC_FILE
rm -rf $RESOURCES_DIR/$MD5_FILE
rm -rf $RESOURCES_DIR/$XSEC_DIR

#--------------------------
# check existance of tools
#--------------------------

set +e
command -v curl >/dev/null && USE_CURL=1 || USE_CURL=0
if [ $USE_CURL -eq 0 ]; then
    # no curl, maybe wget?
    command -v wget >/dev/null || { echo >&2 "  this script requires either curl or wget to be installed on your system"; exit 2; }
    # USE_CURL=0 imples wget
fi
set -e

set +e
command -v tar >/dev/null && USE_TAR=1 || USE_TAR=0
set -e
if [ $USE_TAR -eq 0 ]; then
    echo "you need tar command. exit now."
    exit 0
fi

set +e
command -v awk >/dev/null && USE_AWK=1 || USE_AWK=0
set -e
if [ $USE_AWK -eq 0 ]; then
    echo "you need awk command. exit now."
    exit 0
fi

set +e
UNAME=`uname`
md5=md5
if [ x"$UNAME" = "xDarwin" ]; then
    md5=/sbin/md5
fi
command -v $md5 >/dev/null && FOUND_MD5=1 || FOUND_MD5=0
if [ $FOUND_MD5 -eq 0 ]; then
    # exit if there is no md5 or md5sum tool available
    command -v md5sum >/dev/null && FOUND_MD5=2 || FOUND_MD5=0
    if [ $FOUND_MD5 -eq 0 ]; then
        echo "you need md5 or md5sum command. exit now."
        exit 0
    fi
fi
set -e


#--------------------
# download table data
#--------------------
echo "trying to download ${RESOURCES_DIR}/${XSEC_FILE}..."

# xsec file
echo "  downloading ${BASE_URL}/${XSEC_FILE}..."
if [ $USE_CURL -eq 1 ]; then
    curl -L -o $RESOURCES_DIR/$XSEC_FILE $BASE_URL/$XSEC_FILE
else
    wget -O $RESOURCES_DIR/$XSEC_FILE $BASE_URL/$XSEC_FILE
fi

if [ $FOUND_MD5 -eq 1 ]; then
    MD5_SUM_FILE=`$md5 -q $RESOURCES_DIR/$XSEC_FILE`
else
    md5=`md5sum $RESOURCES_DIR/$XSEC_FILE`
    MD5_SUM_FILE="${md5%% *}" # remove the first space and everything after it
fi

# md5 file
echo "  downloading ${BASE_URL}/${MD5_FILE}..."
if [ $USE_CURL -eq 1 ]; then
    curl -L -o $RESOURCES_DIR/$MD5_FILE $BASE_URL/$MD5_FILE
else
    wget -O $RESOURCES_DIR/$MD5_FILE $BASE_URL/$MD5_FILE
fi

MD5_SUM=`cat $RESOURCES_DIR/$MD5_FILE | awk '{print $1}'`

echo "  check md5sum ..."

count=0
if [ "$MD5_SUM_FILE" != "$MD5_SUM" ]; then
    echo "checksum is not okay! downloading again."
    echo "     found: $MD5_SUM_FILE"
    echo "  expected: $MD5_SUM"

    # re-run this shell script
    if [ $count -gt 10 ]; then
        echo "download failed 10 times. try it later. exit"
        exit 0
    fi
    count=`expr $count + 1`
    exec $0 $RESOURCES_DIR
fi


# and expand
echo "  extracting $BASE_URL/$XSEC_FILE ..."

tar -zxf $RESOURCES_DIR/$XSEC_FILE -C $RESOURCES_DIR

rm -rf $RESOURCES_DIR/$XSEC_FILE
rm -rf $RESOURCES_DIR/$MD5_FILE

echo "  $RESOURCES_DIR/$XSEC_FILE is downloaded."
