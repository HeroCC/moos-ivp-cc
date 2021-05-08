#!/bin/bash -e

echo "Entrypoint: Starting mission with Docker entrypoint script"

# CD to location of entrypoint script (should be root of tree)
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -z "$MISSION" ]; then
  echo "Entrypoint: \$MISSION is unset! Please set to the mission you'd like to run"
  exit 1
fi

# CD into the mission requested, or fail
if [ -d "missions/$MISSION" ]; then
  cd "missions/$MISSION"
else
  echo "Entrypoint: Unable to find mission by name '$MISSION'"
  exit 1
fi

# Launch the mission with all aditional args, or fail
if [ -n "$LAUNCH_TYPE" ] && [ -x "launch_${LAUNCH_TYPE}.sh" ]; then
  echo "Entrypoint: LAUNCH_TYPE specified -- this will be a ${LAUNCH_TYPE} container"
  ./launch_${LAUNCH_TYPE}.sh "$@"
elif [ -x "launch.sh" ]; then
  ./launch.sh "$@"
else
  echo "Entrypoint: Unable to find an appropriate launch script in mission '$MISSION'"
  echo "Entrypoint: Ensure a launch.sh or launch_\${LAUNCH_TYPE}.sh executable exist"
  exit 1
fi

