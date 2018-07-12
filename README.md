# MOOS-IvP CC

The home of my extensions and trinkets related to MOOS-IvP

## Dependencies

The libraries this tree uses have dependencies of their own, in addition to normal MOOS and Aquaticus dependencies. You should make sure your compiler of choice supports C++11 or greater, and then install them like so:
 * MacPorts: `sudo port install portaudio libsndfile boost boost-build protobuf3-cpp grpc log4cpp`
 * Debian Variants: `sudo apt install libsndfile-dev libboost-all-dev libportaudio-dev libssl-dev libprotobuf-dev libgrpc++-dev liblog4cpp5-dev`

## Installing

To get my MOOS IvP tree, you will need to do the following:
 * Clone this repo next to your normal `moos-ivp/` directory
 * Run `git submodule update --init` from within this tree
 * Run `./build.sh` and wait for the compilation to complete
 * Add the newly generated `bin/` directory to your $PATH environmental variable

## What's Included 
 * pWebSocketServer: A websocket server that forwards and receives moos mail to connected applications (see MOOSMobile)
 * iMumbleClient: A VoIP bridge for the Mumble protocol
 * MIT 2.680 applications and missions
