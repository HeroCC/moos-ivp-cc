#!/bin/bash 
#-----------------------------------------------------------
#  Part 1: Check for and handle command-line arguments
#-----------------------------------------------------------
SHORE_LISTEN="9300"

exit_handler() {
  echo "Recieved exit signal, killing all processes..."
  ktm
  #kill -- -$$
  echo "Done killing processes."
}

main() {
  if [ $# -eq 0 ]; then
    echo "Invalid number of arguments, try"
    echo "launch.sh shoreside"
    echo "launch.sh VNAME TIME_WARP START_POS LOITER_POS"
    exit 1
  fi
  
  TIME_WARP="$2"
  VNAME="$1"
  START_POS="$3"
  LOITER_POS="$4"

  SCRIPT_ABS_DIR="$(cd $(dirname "$0") && pwd -P)"
  cd "$SCRIPT_ABS_DIR"

  if [ "$VNAME" != "shoreside" ]; then 

    nsplug meta_vehicle.moos targ_$VNAME.moos -f WARP=$TIME_WARP  \
      VNAME=$VNAME       START_POS=$START_POS                   \
      VPORT="9001"       SHARE_LISTEN="9301"                    \
      VTYPE="kayak"      SHORE_LISTEN=$SHORE_LISTEN             \
      WSPort="9091"

    nsplug meta_vehicle.bhv targ_$VNAME.bhv -f VNAME=$VNAME       \
      START_POS=$START_POS LOITER_POS=$LOITER_POS       

    printf "Launching $VNAME MOOS Community (WARP=%s) \n" $TIME_WARP
    pAntler targ_$VNAME.moos >& /dev/null &
    uMAC targ_$VNAME.moos

  else

    nsplug meta_shoreside.moos targ_shoreside.moos -f WARP=$TIME_WARP \
      VNAME="shoreside"  SHARE_LISTEN=$SHORE_LISTEN  VPORT="9000"     

    printf "Launching $SNAME MOOS Community (WARP=%s) \n"  $TIME_WARP
    pAntler targ_shoreside.moos >& /dev/null &
    uMAC targ_shoreside.moos

  fi

  sleep 2s
  exit_handler
}

main "$@"
