# MOOS-IvP CC

The home of my extensions and trinkets related to MOOS-IvP

## Docker

If you'd rather use my tree as a docker image, you can pull it from DockerHub like so: `docker pull herocc/moos-ivp-cc:latest`. To see all the versions avaliable, [click here](https://hub.docker.com/r/herocc/moos-ivp-cc/tags). Some missions in this repo are designed with docker in mind, others are not; docker missions are noted if so.

### Image automation

GitHub Actions validates both `linux/amd64` and `linux/arm64` images for pull requests without registry credentials. A separate release workflow runs for pushes to `master` and `v*` tags; it builds once and publishes the resulting digest to both Quay and Docker Hub. It also attests the Quay image and saves the compiled `bin/` and `lib/` directories as workflow artifacts.

Create a protected `release` environment before enabling publishing. Put the following configuration in that environment and require an appropriate reviewer for it:

* Environment variables: `QUAY_USERNAME`, `DOCKER_HUB_USERNAME`
* Environment secrets: `QUAY_PUSH_KEY`, `DOCKER_HUB_PUSH_KEY`

Protect `master`, restrict who can create matching release tags, and require pull-request review plus the Docker CI check before merging. Deploy consumers should use the published image digest rather than the mutable `latest` tag.

The former GitLab schedule was configured outside this repository, so recreate its cadence with a `schedule` trigger in [the release workflow](.github/workflows/release-image.yml) if periodic image rebuilds are required.

## Dependencies

The libraries this tree uses have dependencies of their own, in addition to normal MOOS and Aquaticus dependencies. You should make sure your compiler of choice supports C++11 or greater, and then install them like so:
 * MacPorts: `sudo port install portaudio libsndfile boost boost-build protobuf3-cpp grpc log4cpp`
 * Homebrew: `brew install log4cpp opus protobuf portaudio openssl boost pkgconfig`
 * Debian Variants: `sudo apt install libsndfile-dev libboost-all-dev portaudio19-dev libssl-dev libprotobuf-dev libgrpc++-dev liblog4cpp5-dev`

## Installing

To get my MOOS IvP tree, you will need to do the following:
 * Clone this repo next to your normal `moos-ivp/` directory
 * Run `git submodule update --init` from within this tree
 * Run `./build.sh` and wait for the compilation to complete
 * Add the newly generated `bin/` directory to your $PATH environmental variable

## What's Included 
 * pWebSocketServer: A websocket server that forwards and receives moos mail to connected applications (see MOOSMobile)
 * iMumbleClient: A VoIP bridge for the Mumble protocol
 * uRaspiMon: A wrapper for raspbian tools to measure system temperature and throttling status
 * iNeptune (and related NMEA tools): An interface between Seebyte's Neptune platform and MOOS
 * MIT 2.680 applications and missions
