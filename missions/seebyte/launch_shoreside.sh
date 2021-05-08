#!/bin/bash -e
#--------------------------------------------------------------
#   Script: launch_shoreside.sh                                    
#   Author: Conlan Cesar
#     Date: April 2020

trap "pkill -INT -P $$" EXIT SIGTERM SIGHUP SIGINT SIGKILL

#--------------------------------------------------------------  
#  Part 1: Declare global var defaults
#--------------------------------------------------------------
TIME_WARP="${TIME_WARP:-1}"
PSHARE_PORT="${PSHARE_PORT:-9300}"
COMMUNITY="seebyte-shoreside"
JUST_MAKE="no"
AUTO=""

#--------------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#--------------------------------------------------------------
for ARGI; do
    if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ] ; then
	echo "launch_shoreside.sh [SWITCHES] [time_warp]        "
	echo "  --just_make, -j                                 " 
	echo "  --help, -h                                      " 
	echo "  --ip=<addr>       (default is localhost)        " 
	echo "  --pshare=<port>   (default is 9300)             " 
    echo "  --auto, -a        Auto-launched. uMAC not used. "
	exit 0;
    elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
        TIME_WARP=$ARGI
    elif [ "${ARGI}" = "--just_make" -o "${ARGI}" = "-j" ] ; then
	JUST_MAKE="yes"
    elif [ "${ARGI}" = "--auto" -o "${ARGI}" = "-a" ]; then
        AUTO="yes"
    elif [ "${ARGI:0:5}" = "--ip=" ]; then
        HOST_IP="${ARGI#--ip=*}"
    elif [ "${ARGI:0:9}" = "--pshare=" ]; then
        PSHARE_PORT="${ARGI#--pshare=*}"
    else 
	echo "launch_shoreside.sh: Bad Arg: " $ARGI
	exit 1
    fi
done

#--------------------------------------------------------------
#  Part 3: Create the .moos and .bhv files using nsplug
#--------------------------------------------------------------
# What is nsplug? Type "nsplug --help" or "nsplug --manual"

mkdir -p logs/

NSFLAGS="-s -f"
if [ "${AUTO}" = "" ]; then
    NSFLAGS="-i -f"
fi
nsplug meta_shoreside.moos targ_shoreside.moos $NSFLAGS WARP=$TIME_WARP  \
       HOST_IP=$HOST_IP PSHARE_PORT=$PSHARE_PORT SIM=$SIM


if [ ${JUST_MAKE} = "yes" ] ; then
    exit 0
fi

#--------------------------------------------------------------
#  Part 4: Launch the processes
#--------------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community WARP:" $TIME_WARP
pAntler targ_shoreside.moos >& /dev/null &
echo "Done launching $COMMUNITY"

#-------------------------------------------------------------- 
#  Part 5: Unless auto-launched, launch uMAC until mission quit          
#-------------------------------------------------------------- 
if [ "${AUTO}" = "" ]; then
    uMAC targ_shoreside.moos
fi
