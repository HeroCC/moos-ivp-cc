# MOOS-IvP CC

The home of my extensions and trinkets related to MOOS-IvP

## Docker

If you'd rather use my tree as a docker image, you can pull it from DockerHub like so: `docker pull herocc/moos-ivp-cc:latest`, or GitHub Container Registry with `docker pull ghcr.io/herocc/moos-ivp-cc:latest`. To see all the versions avaliable, [click here](https://hub.docker.com/r/herocc/moos-ivp-cc/tags). Some missions in this repo are designed with docker in mind, others are not; docker missions are noted if so.

### Image automation

GitHub Actions validates both `linux/amd64` and `linux/arm64` images on native runners for pull requests, without registry credentials, publishing, or signing. A separate release workflow runs for pushes to `master` and `v*` tags. Docker's GitHub Builder builds each architecture on a native runner, merges the resulting digests into one multi-platform image, then publishes that exact image to both GitHub Container Registry and Docker Hub. Its built-in OIDC signing signs the generated provenance and SBOM attestations; the workflow does not create a second, separate attestation. The compiled `bin/` and `lib/` directories are extracted from the published GHCR digest and saved as workflow artifacts.

GHCR publishing uses the workflow's automatically generated `GITHUB_TOKEN`; grant it `packages: write` in the release job. Put the Docker Hub credentials at repository or organization scope:

* Actions variable: `DOCKER_HUB_USERNAME`
* Actions secret: `DOCKER_HUB_PUSH_KEY`

Protect `master`, restrict who can create matching release tags, and require pull-request review plus the Docker CI check before merging; these controls limit who can invoke publishing. Deploy consumers should use the published image digest rather than the mutable `latest` tag.

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
