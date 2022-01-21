## On MacOS
XQuartz and VirtualBox do not support the OpenGL extensions needed by Ignition Gazebo. You will need to install VMWare Fusion.
MIT has a site-wide license, otherwise you are free to use the app for non-commercial purposes or start a free trial.
To automate setup of the VM, you will need to install [Vagrant](https://www.vagrantup.com/downloads).

```sh
# Install Vagrant and Vagrant's VMWare Utility (or the equiv for your OS)
brew install vagrant vagrant-vmware-utility

# Install the VMware Vagrant plugin
vagrant plugin install vagrant-vmware-desktop

# Set the default provider to VMWare
export VAGRANT_DEFAULT_PROVIDER="vmware_desktop"
vagrant up --provider vmware_desktop
```
Reboot the VM if you don't see a GUI, and login using `vagrant`/`vagrant`. Then, continue the process from the 'On Ubuntu' section.


## On Ubuntu
On the desktop, in two terminals, run:
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
