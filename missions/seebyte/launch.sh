#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Conlan Cesar
#  LastEd: Spring 2020

#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "pkill -INT -P $$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=${TIME_WARP:-1}
COMMUNITY="seebyte"
NMEA_HOST="${NMEA_HOST:-localhost}"
NMEA_PORT="${NMEA_PORT:-10110}"
NMEA_CHECKSUM="${NMEA_CHECKSUM:-true}"
NMEA_TIME_DELTA="${NMEA_TIME_DELTA:-3}"
HERON_HOST="${HERON_HOST:-192.168.10.1}"

cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
  if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
  	echo "launch.sh [SWITCHES] [time_warp]   "
  	echo "  --help, -h                       "
    echo "  --nogui                          "
    echo "  --nochecksum  - don't check NMEA checksum"
    echo "  --notimestamp - don't check NMEA timestamp"
    echo "  --novalidate  - don't check NMEA checksum or timestamp"
    echo "  --shore=HOST:PORT                "
    echo "  --sim, -s                        "
	  exit 0
  elif [ "${ARGI}" = "--nogui" ]; then
    unset DISPLAY
  elif [ "${ARGI}" = "--nochecksum" ]; then
    NMEA_CHECKSUM="false"
  elif [ "${ARGI}" = "--notimestamp" ]; then
    NMEA_TIME_DELTA="-1"
  elif [ "${ARGI}" = "--novalidate" ]; then
    NMEA_CHECKSUM="false"
    NMEA_TIME_DELTA="-1"
  elif [ "${ARGI}" = "--sim" ] || [ "${ARGI}" = "-s" ]; then
    SIM="yes"
  elif [ "${ARGI:0:8}" = "--shore=" ] ; then
    SHORE="${ARGI#--shore=*}"
  elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
    TIME_WARP=$ARGI
  else 
    printf "Bad Argument: %s \n" $ARGI
    exit 0
  fi
done

#----------------------------------------------------------
#  Part 3: Build the targ_*.moos file
#----------------------------------------------------------
mkdir -p logs/
nsplug $COMMUNITY.moos targ_${COMMUNITY}.moos -f \
  DISPLAY=$DISPLAY WARP=$TIME_WARP COMMUNITY=$COMMUNITY \
  SIM=$SIM SHORE=$SHORE HERON_HOST=$HERON_HOST \
  NMEA_HOST=$NMEA_HOST NMEA_PORT=$NMEA_PORT \
  NMEA_TIME_DELTA=$NMEA_TIME_DELTA NMEA_CHECKSUM=$NMEA_CHECKSUM

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is" $TIME_WARP
pAntler targ_$COMMUNITY.moos >& /dev/null &

uMAC -t $COMMUNITY.moos

