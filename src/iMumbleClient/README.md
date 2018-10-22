# iMumbleClient

A C++ application for MOOS IvP to connect clients together via a Mumble VoIP server. 

## Dependencies

On a Mac with MacPorts, install all requirements with: `sudo port install portaudio libsndfile boost protobuf3-cpp log4cpp`

## Usage

This testing can be done on one computer, but for best results should be used on two or three

### Client

 * Install a MOOS IvP tree that has this app (moos-ivp-aquaticus or this one for example) the normal way
 * Install the MOOS config file to your mission, and configure it as you like (pay attention to the `AUDIO_TX_KEY` and `SERVER_IP`)
 * Launch a mission (eg. aquaticus2.0) with the `iMumbleClient` app added to the Antler config
 * Set the trigger key to `TRUE` (usually either BUTTON_VOIP or SPEECH_BUTTON) via uPokeDB to start speaking
 * Listen on other devices for your voice

### Server

These instructions are to launch the server without installing it to any specific mission. You can adapt these steps to make a mission where murmur launches from shoreside by default (see aquaticus2.0).
You may also want to install the GUI Mumble client to be able to monitor and communicate with clients, add and modify channels, etc.

 * Download the [Murmur server](https://wiki.mumble.info/wiki/Main_Page) for your OS and add the `murmurd` binary to your $PATH
 * Launch the murmur server (optionally, include a config file for extra confirgurations, example found in the murmur zip file or aquaticus2.0)
 * Launch any shoreside mission
