#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Conlan Cesar

#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "pkill -INT -P $$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=${TIME_WARP:-1}
COMMUNITY="seebyte"
NMEA_HOST="${NMEA_HOST:-127.0.0.1}"
NMEA_PORT="${NMEA_PORT:-10110}"
NMEA_CHECKSUM="${NMEA_CHECKSUM:-true}"
NMEA_TIME_DELTA="${NMEA_TIME_DELTA:-3}"
SHORE_PORT="${SHORE_PORT:-9300}"
HERON_HOST="${HERON_HOST:-192.168.10.1}"
PSHARE_PORT="${PSHARE_PORT:-9305}"

cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

pwd

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
    echo "  --shore=HOST[:PORT]              "
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
    __SHORE_ARG="${ARGI#--shore=*}"
    IFS=':' read -ra __SHORE <<< "$__SHORE_ARG"
    SHORE_HOST="${__SHORE[0]}"
    SHORE_PORT="${__SHORE[1]:=SHORE_PORT}"
    echo "Shoreside set to $SHORE"
  elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then 
    TIME_WARP=$ARGI
  else 
    printf "Bad Argument: %s \n" $ARGI
    exit 0
  fi
done

# Resolve IPs where applicable
if [ -n $SHORE_HOST ] && echo $SHORE_HOST | grep -v '[0-9]\{1,3\}\(\.[0-9]\{1,3\}\)\{3\}'; then 
  SHORE_HOST="$(ping -c 1 -t 1 $SHORE_HOST | head -1 | cut -d ' ' -f 3 | tr -d '()\:')"
  if [ -z $SHORE_HOST ]; then
    echo "Unable to resolve SHORE_HOST '$SHORE_HOST' to IP!"
    exit 1
  fi
  echo "Resolved shoreside hostname to '$SHORE_HOST'"
fi

[ -n $SHORE_HOST ] && [ -z $SHORE ] && SHORE="$SHORE_HOST:$SHORE_PORT"


#----------------------------------------------------------
#  Part 3: Build the targ_*.moos file
#----------------------------------------------------------
mkdir -p logs/
nsplug meta_${COMMUNITY}.moos targ_${COMMUNITY}.moos -f \
  DISPLAY=$DISPLAY WARP=$TIME_WARP COMMUNITY=$COMMUNITY \
  SIM=$SIM HERON_HOST=$HERON_HOST \
  SHORE=$SHORE \
  PSHARE_PORT=$PSHARE_PORT HOST_IP=$HOST_IP \
  NMEA_HOST=$NMEA_HOST NMEA_PORT=$NMEA_PORT \
  NMEA_TIME_DELTA=$NMEA_TIME_DELTA NMEA_CHECKSUM=$NMEA_CHECKSUM

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is $TIME_WARP"
pAntler targ_$COMMUNITY.moos >& /dev/null &

uMAC -t targ_$COMMUNITY.moos

