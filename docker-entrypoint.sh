#!/bin/bash

echo "DEP: Staring mission with Docker entrypoint script"
echo "DEP: To bypass this and drop into bash, add 'bash' to the end of the command you just ran"

# CD to location of entrypoint script (should be root of tree)
cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -z "$MISSION" ]; then
  echo "DEP: \$MISSION is unset! Please set to the mission you'd like to run"
  exit 1
fi

# CD into the mission requested, or fail
if [ -d "missions/$MISSION" ]; then
  cd "missions/$MISSION"
else
  echo "DEP: Unable to find mission by name '$MISSION'"
  exit 1
fi

# Launch the mission with all aditional args, or fail
if [ -x "launch.sh" ]; then
  ./launch.sh "$@"
  echo "DEP: Goodbye!"
else
  echo "DEP: Unable to find a 'launch.sh' script in mission '$MISSION'"
  exit 1
fi

