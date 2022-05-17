# Neptune MOOS Integration Mission

This guide shows how to install and run the testing mission for MOOS + Seebyte Neptune integration. It requires Docker (and ideally docker-compose) to be installed on your machine.

## Running

### Full Docker Deployment

* Download and Extract the Neptune distribution file -- something like `Neptune-MOOS v0.5.0.zip` (provided separately)
* Import the extracted image file to your Docker via `docker load -i neptune-moos-<arch>-<version>.tar.gz`
* If you wish to enable XForwarding on a Mac (to see apps like pMarineViewer), start XQuarts and run `xhost +`. You should also uncomment the *DISPLAY=host.docker.internal:0* env variable, or add it to your *docker-compose.override.yml*
  * On XQuarts, you will need to enable access from networked clients in your settings
  * You must have IGLX enabled or pMarineViewer may crash. See the [xquartz](https://unix.stackexchange.com/a/642954/126262) and [linux](https://askubuntu.com/a/932912/353466) fixes.
  * If you choose not to do this step, you can visualize the mission post-completion via the logs in `./logs/moos-logs`
  * For security, run `xhost -` once you're done.
* Run the images with `docker-compose up` -- this will occupy your terminal, so you should use tmux or open another terminal window
* To (optionally) attach to the MOOS *uMAC* console, run `docker attach seebyte_moos_usv_1`
* Wait until Neptune finishes starting (~1 minute, or until the warnings in *iNeptune* are gone)
* Once Neptune is fully initialized, run `docker exec -it seebyte_neptune_<uuv OR usv>_1 bash -ic 'rosservice call /<uuv OR usv>/seebyte_arbiter/control "{enable: 1, running: 1}"'` to start the mission on that vehicle
  * The `uuv` and `usv` tags are set in the environment variables `$CONFIG_TYPE`, `$NPTN_ID`, and `$SOLAR_VEHICLE_NAME`. They should all be the same value.
* You should see the mission being executed. Check the AppCasts of applicable apps (eg. `pHelmIvP`, `iNeptune`, `pMarineViewer`) to see progress
* Once you're finished, you can end the mission by killing (Ctrl-C) the docker-compose window

### Native MOOS, Dockerized Neptune

To run MOOS natively and Neptune in a container, you need to do the following:

* Launch Neptune with `docker-compose run --service-ports neptune_uuv` (and / or `neptune_usv`)
  * See the ports exposed in `docker-compose.yml` -- probably `10110` and `10111` for usv and uuv respectively
* Launch MOOS
  * Use the `--sim` flag to simulate using `uSimMarine` and generated obstacles
  * A proper `--shore` flag is required for inter-vehicle communication
  * Potential Launch Commands for simulating on one machine:
    * Shoreside: `./launch_shoreside.sh --sim`
    * USV: `COMMUNITY=seebyte_usv ./launch_vehicle.sh --sim --shore=localhost:9300`
      * The `DB_PORT`, `NMEA_PORT`, `PSHARE_PORT` have defaults if not explicitly set, see `launch_vehicle.sh`
    * UUV: `COMMUNITY=seebyte_uuv DB_PORT=9006 NMEA_PORT=10111 PSHARE_PORT=9306 ./launch_vehicle.sh --sim --shore=localhost:9300`

If launching on a real vehicle, adapt the instructions above as necessary. You may need to drop `--sim`, set `$HOST_IP`, and launch your own vehicle driver manually or in a separate bridged community. You can launch MOOS completely natively, in a docker container system, or a hybrid of the iNeptune MOOS in docker, and your vehicle & sensor suite natively (with pShare).

## Troubleshooting

> The system appears to start and connect, but Neptune will not respond to the launch command, and no NMEA messages are received by MOOS

One issue may be the permissions on the logs folder, which opens as root but is unwritable to the Neptune container. Neptune will silently fail to execute. This usually presents itself on Linux systems, but not on Mac. Try running `sudo chmod $USER -Rv logs/` and relaunching, or disabling the log mount from the docker-compose file if Neptune logs aren't needed.

> I see lots of warnings about "Time difference 3600 > 3, ignoring message"

Check to ensure there isn't a genuine issue of latency between systems. If this is a false alarm, it is likely caused due to differing timezones between the Neptune container and your local system timezone. You can ignore these errors by launching the vehicle with `--notimestamp`

> When I run `docker-compose up`, pMarineViewer appears momentarily then crashes.

This is an issue with XQuartz on MacOS. There is no known permanent fix, but a workaround is to fully quit XQuartz, then run `xhost +` in the terminal to relaunch it.

> I want to impersonate Neptune to send NMEA messages to MOOS manually.

You can use Netcat to host a server locally. Instead of launching the Neptune docker image, run `nc -4lk localhost 10110` to start a server. Note, you may wish to launch your vehicles with `--notimestamp` and `--nochecksum` to ignore time and checksum differences.
