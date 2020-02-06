#!/bin/bash -e

NETWORK_NAME="moos" # Containers should be on same network

main() {
  echo "Starting Multi-docker MOOS-IvP Example Mission"
  echo "Shoreside will be attached to console, other vehicles should be background"
  #echo "Pass --keep-logs to save logs to host computer"
  echo ""

  # Step: Prep

  docker ps >> /dev/null || (echo "Docker not found or daemon not running!" && exit 1)

  NETWORK_ID="$(docker network ls -q -f name=${NETWORK_NAME})"
  if [ -z $NETWORK_ID ]; then
    # We must create the network
    echo "Creating network $NETWORK_NAME"
    docker network create "$NETWORK_NAME"
    NETWORK_ID="$(docker network ls -q -f name=${NETWORK_NAME})"
  else
    echo "Found network $NETWORK_NAME as $NETWORK_ID"
  fi


  # Step: Run
  docker pull herocc/moos-ivp-cc:latest
  #trap quit INT
  docker run --rm -itd --name "shoreside" --net=${NETWORK_ID} --name=moos-ivp-cc-shoreside herocc/moos-ivp-cc:latest moos-ivp-cc/missions/docker-multicast/launch.sh shoreside 10
  sleep 2s
  docker run --rm -itd --net=${NETWORK_ID} --name=moos-ivp-cc-henry herocc/moos-ivp-cc:latest moos-ivp-cc/missions/docker-multicast/launch.sh henry 10 '0,0' 'x=0,y=-75'
  docker run --rm -itd --net=${NETWORK_ID} --name=moos-ivp-cc-gilda herocc/moos-ivp-cc:latest moos-ivp-cc/missions/docker-multicast/launch.sh gilda 10 '80,0' 'x=125,y=-50'
  docker attach moos-ivp-cc-shoreside
  echo "Stopping containers with Network ID ${NETWORK_ID}"
  docker stop $(docker ps -q -f network=${NETWORK_ID})
}

main "$@"
