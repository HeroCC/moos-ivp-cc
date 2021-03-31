# Neptune MOOS Integration Mission

This guide shows how to install and run the testing mission for MOOS + Seebyte Neptune integration. It requires Docker (and ideally docker-compose) to be installed on your machine.

## Running

* Download and Extract the Neptune distribution file -- something like `Neptune-MOOS v0.5.0.zip` (provided separately)
* Import the extracted image file to your Docker via `docker load -i neptune-moos-<arch>-<version>.tar.gz`
* If you wish to enable XForwarding on a Mac (to see apps like pMarineViewer), start XQuarts and run `xhost +`. You should also uncomment the *DISPLAY=host.docker.internal:0* env variable, or add it to your *docker-compose.override.yml*
  * You must have IGLX enabled or pMarineViewer may crash. See the [xquartz](https://unix.stackexchange.com/a/642954/126262) and [linux](https://askubuntu.com/a/932912/353466) fixes.
  * If you choose not to do this step, you can visualize the mission post-completion via the logs in `./logs/moos-logs`
  * For security, run `xhost -` once you're done.
* Run the images with `docker-compose up` -- this will occupy your terminal, so you should use tmux or open another terminal window
* To (optionally) attach to the MOOS *uMAC* console, run `docker attach seebyte_moos_1`
* Wait until Neptune finishes starting (~1 minute, or until the warnings in *iNeptune* are gone)
* Once Neptune is fully initialized, run `docker exec -it seebyte_neptune_1 bash -ic 'rosservice call /usv1/seebyte_arbiter/control "{enable: 1, running: 1}"'` to start the mission
* You should see the mission being executed. Check the AppCasts of applicable apps (eg. `pHelmIvP`, `iNeptune`, `pMarineViewer`) to see progress
* Once you're finished, you can end the mission by killing (Ctrl-C) the docker-compose window

## Debugging


