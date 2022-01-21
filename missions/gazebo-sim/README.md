## If on MacOS
XQuartz and VirtualBox do not support the OpenGL extensions needed by Ignition Gazebo. You will need to install VMWare Fusion.
MIT has a site-wide license, otherwise you are free to use the app for non-commercial purposes or start a free trial.


```sh
brew install vagrant vagrant-vmware-utility
vagrant plugin install vagrant-vmware-desktop
export VAGRANT_DEFAULT_PROVIDER="vmware_desktop"
vagrant up --provider vmware_desktop
```
Reboot the VM if you don't see a GUI.

Login using `vagrant`/`vagrant`. On the desktop, in two terminals, run:
```sh
cd ~/ign-gazebo/example/worlds
ign gazebo -v 4 -r auv_controls.sdf --render-engine ogre
```
You should see an empty world with an AUV in the center.

And in the other terminal,
```sh
cd ~/moos-ivp-cc/missions/gazebo-sim
./launch.sh
```

Finally, in pMarineViewer, click `DEPLOY`. The AUV will begin to move around within Gazebo. Right now, the locations of the AUV in Gazebo and pMarineViewer will not be the same, but this will be fixed in time.
