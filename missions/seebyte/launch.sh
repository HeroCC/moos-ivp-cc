#!/bin/bash -eo pipefail
#--------------------------------------------------------------
#   Script: launch.sh                                    
#   Author: Conlan Cesar
#     Date: Summer 2021

function cleanup {
    echo "Quitting, please wait, do not spam Ctrl-C..."
    set -x
    pkill -INT -P $(jobs -p)
    [ -n "$SET_XHOST" ] && xhost -
    docker-compose down
    wait 
    exit
}

function showHelp {
    echo "$0 --type=<launch type>        "
    echo "  --help, -h                                      " 
    echo "  --warp <time warp>, \$TIME_WARP                 " 
    echo "  --type <launch type> \$LAUNCH_TYPE              " 
    echo "    Launch type: docker "
    echo "         Prepares and runs docker-compose "
    echo "    Launch type: docker-attach "
    echo "         Prepares and runs docker-compose, then attaches to the shoreside container uXMS"
    echo "    Launch type: native "
    echo "         Runs only the neptune container, and launches the shoreside & vehicle moos natively"
    echo "    Launch type: moos "
    echo "         Runs only the shoreside & vehicle moos natively -- no neptune"
}

trap cleanup SIGTERM SIGHUP SIGINT SIGKILL

cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

: "${LAUNCH_TYPE:=docker}"
: "${TIME_WARP:=5}"

export SIM="yes"

# Gather arguments
while [[ "$1" != "" ]]; do
    case $1 in
        -h|--help) showHelp; exit 0 ;;
        -w|--warp) TIME_WARP="$2"; shift ;;
        -t|--type) LAUNCH_TYPE="$2"; shift ;;
        --) shift; EXTRA_ARGS="$@"; break ;; # Anything after the '--' should be passed to the launch command(s)
        *) echo "$0: Unknown parameter passed: $1"; showHelp; exit 1 ;;
    esac
    shift
done

echo "Launching with mode $LAUNCH_TYPE, extra args $EXTRA_ARGS"

mkdir -p logs/
if [ ! -w "logs/" ]; then
    echo "ERROR: logs/ directory is not writable!"
    exit 1
fi

if [[ "$LAUNCH_TYPE" == "docker"* ]]; then
    [ -n "$DISPLAY" ] && xhost + && SET_XHOST="true"
    if [[ "$LAUNCH_TYPE" == "docker-attach" ]]; then
        docker-compose up -d $EXTRA_ARGS
        sleep 1s
        docker attach seebyte_moos_vehicle_1
    else
        docker-compose up $EXTRA_ARGS
    fi
elif [[ "$LAUNCH_TYPE" == "native" || "$LAUNCH_TYPE" == "moos" ]]; then
    [ "$LAUNCH_TYPE" == "native" ] && docker-compose run -d --service-ports --name=seebyte_neptune_1 neptune
    sleep .5s
    SHORE_HOST="localhost" ./launch_vehicle.sh --auto --notimestamp $EXTRA_ARGS > /dev/null &
    sleep .5s
    ./launch_shoreside.sh $EXTRA_ARGS
fi

cleanup
