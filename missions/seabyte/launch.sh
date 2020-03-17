#!/bin/bash -e
#----------------------------------------------------------
#  Script: launch.sh
#  Author: Michael Benjamin, Conlan Cesar
#  LastEd: Winter 2019

#----------------------------------------------------------
#  Part 1: Set Exit actions and declare global var defaults
#----------------------------------------------------------
trap "kill -- -$$" EXIT SIGTERM SIGHUP SIGINT SIGKILL
TIME_WARP=${TIME_WARP:-1}
COMMUNITY="seabyte"
GUI="${GUI:-yes}"
SIM="${SIM:-no}"
NMEA_HOST="${NMEA_HOST:-localhost}"
NMEA_PORT="${NMEA_PORT:-10110}"

cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

#----------------------------------------------------------
#  Part 2: Check for and handle command-line arguments
#----------------------------------------------------------
for ARGI; do
  if [ "${ARGI}" = "--help" -o "${ARGI}" = "-h" ]; then
  	echo "launch.sh [SWITCHES] [time_warp]   "
  	echo "  --help, -h                       " 
    echo "  --nogui                          "
    echo "  --sim, -s                        "
	  exit 0
  elif [ "${ARGI}" = "--nogui" ]; then
    GUI="no"
  elif [ "${ARGI}" = "--sim" ] || [ "${ARGI}" = "-s" ]; then
    SIM="yes"
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
  GUI=$GUI WARP=$TIME_WARP COMMUNITY=$COMMUNITY \
  SIM=$SIM \
  NMEA_HOST=$NMEA_HOST NMEA_PORT=$NMEA_PORT

#----------------------------------------------------------
#  Part 4: Launch the processes
#----------------------------------------------------------
echo "Launching $COMMUNITY MOOS Community. WARP is" $TIME_WARP
pAntler targ_$COMMUNITY.moos >& /dev/null &

uMAC -t $COMMUNITY.moos

sleep 3s
