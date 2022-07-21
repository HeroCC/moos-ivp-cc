#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Conlan Cesar

#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "pkill -INT -P $$ && wait $(jobs -p)" EXIT SIGTERM SIGHUP SIGINT SIGKILL

TIME_WARP=${TIME_WARP:-1}
COMMUNITY="${COMMUNITY:-seebyte}"
DB_PORT="${DB_PORT:-9005}"
NMEA_HOST="${NMEA_HOST:-127.0.0.1}"
NMEA_PORT="${NMEA_PORT:-10110}"
NMEA_CHECKSUM="${NMEA_CHECKSUM:-true}"
NMEA_TIME_DELTA="${NMEA_TIME_DELTA:-3}"
SHORE_PORT="${SHORE_PORT:-9300}"
VEHICLE_ROUTE="${VEHICLE_ROUTE}"
PSHARE_PORT="${PSHARE_PORT:-9305}"
OBSTACLE_POINT_KEY="${OBSTACLE_POINT_KEY:-TRACKED_FEATURE}"

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
    echo "  --vehicle=HOST:PORT - pShare DESIRED_* out to this route"
    echo "  --sim, -s                        "
    echo "  --auto, -a                       "
	  exit 0
  elif [ "${ARGI}" = "--nogui" ]; then
    unset DISPLAY
  elif [ "${ARGI}" = "--auto" ] || [ "${ARGI}" = "-a" ]; then
    AUTO="yes"
  elif [ "${ARGI}" = "--nochecksum" ]; then
    NMEA_CHECKSUM="false"
  elif [ "${ARGI}" = "--notimestamp" ]; then
    NMEA_TIME_DELTA="-1"
  elif [ "${ARGI}" = "--novalidate" ]; then
    NMEA_CHECKSUM="false"
    NMEA_TIME_DELTA="-1"
  elif [ "${ARGI}" = "--sim" ] || [ "${ARGI}" = "-s" ]; then
    SIM="yes"
  elif [ "${ARGI:0:10}" = "--vehicle=" ] ; then
    VEHICLE_ROUTE="${ARGI#--vehicle=*}"
  elif [ "${ARGI:0:8}" = "--shore=" ] ; then
    __SHORE_ARG="${ARGI#--shore=*}"
    IFS=':' read -ra __SHORE <<< "$__SHORE_ARG"
    SHORE_HOST="${__SHORE[0]}"
    SHORE_PORT="${__SHORE[1]:=SHORE_PORT}"
    unset SHORE
  elif [ "${ARGI//[^0-9]/}" = "$ARGI" -a "$TIME_WARP" = 1 ]; then
    TIME_WARP=$ARGI
  else
    printf "Bad Argument: %s \n" $ARGI
    exit 0
  fi
done

# Resolve IPs where applicable
if [ -n "$SHORE_HOST" ] && echo "$SHORE_HOST" | grep -v '[0-9]\{1,3\}\(\.[0-9]\{1,3\}\)\{3\}'; then
  SHORE_HOST="$(ping -c 1 -t 1 $SHORE_HOST | head -1 | cut -d ' ' -f 3 | tr -d '()\:')"
  if [ -z "$SHORE_HOST" ]; then
    echo "Unable to resolve SHORE_HOST '$SHORE_HOST' to IP!"
    exit 1
  fi
fi

# Build into a string
if [ -n "$SHORE_HOST" ] && [ -z "$SHORE" ]; then
  SHORE="$SHORE_HOST:$SHORE_PORT"
  echo "Shoreside set to $SHORE"
  unset DISPLAY # If we have a shoreside, there is no need for a pMarineViewer window
fi


#----------------------------------------------------------
#  Part 3: Build the targ_*.moos file
#----------------------------------------------------------
mkdir -p logs/
nsplug meta_seebyte.moos targ_${COMMUNITY}.moos -f \
  DISPLAY=$DISPLAY WARP=$TIME_WARP COMMUNITY=$COMMUNITY \
  SIM=$SIM VEHICLE_ROUTE=$VEHICLE_ROUTE \
  SHORE=$SHORE DB_PORT=$DB_PORT \
  PSHARE_PORT=$PSHARE_PORT HOST_IP=$HOST_IP \
  NMEA_HOST=$NMEA_HOST NMEA_PORT=$NMEA_PORT \
  NMEA_TIME_DELTA=$NMEA_TIME_DELTA NMEA_CHECKSUM=$NMEA_CHECKSUM \
  OBSTACLE_POINT_KEY=$OBSTACLE_POINT_KEY BROADCAST_MAILS=$BROADCAST_MAILS


#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is $TIME_WARP"
pAntler targ_$COMMUNITY.moos >& /dev/null &

if [ "${AUTO}" = "" ]; then
  uMAC -t targ_$COMMUNITY.moos
else
  wait
fi
