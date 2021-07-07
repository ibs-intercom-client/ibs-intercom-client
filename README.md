# ibs-intercom-client

The ibs-intercom-client is a simple graphical intercom client application including a SIP phone, a video player and a ringtone player. This allows users to answer incoming calls and to talk to visitors standing in front of the building's main entrance. If available the video stream of the door camera is displayed during each call. The intercom binary is named ``doorctrl``. This project also contains a ringtone player triggered by GPIO signals called ``doorbell``. 

## Functionality
### Play ringtone
An accoustic signal (ringtone) is played when the 'call' button is pressed. This button is usually located near the house's main entrance and is not part of the doorctrl project. The ringtone is played until the incoming call is answered or rejected or a 'call' timeout occured. The ringtone is loud and clear. It is played in a loop to generate different signal durations. There is a quiet part between two signals, a continuous signal is not desired.

### Set ringtone volume
The ringtone volume is adjustable by the user. The value is stored permantly. The volume is an integer value between 0 and 100.

### Talk to visitor
Doorctrl allows the user to talk to a visitor after the 'call'-button has been pressed at the house's  main entrance. Communication is always bidirectional and must be started by the user by answering a incoming call. Max talk time is limited by a timeout to close forgotten communication channels automatically. When a single talk is running doorctrl is locked and the user is not allowed to handle another incoming call.

### Unlock the door
Doorctrl allows the user to unlock the house's main entrance. A call must have been initiated and answered to do that. The user can not unlock the main entrance without a open bidirectional communication. A short acoustic signal indicates that the door opener has been activated.

### Finish talk
An open bidirectional communication to a visitor standing at the house's main entrance is either finished by the user, by the visitor or by timeout. Doorctrl then returns to initial state and standard view after the call has been finished. A short accoustic signal indicates that max communication time has reached and that the talk will be finished within the next few seconds.

### Reject call
The user may reject an incoming call without opening the communication channel and without talking to the visitor. A status display tells the visitor the the call has been requested.

### Set speaker volume 
Adjusting the speaker volume is essential to understand a visitor loud and clear. The user may adjust the speaker volume any time either by user settings or by additional controls when the communication channels are open.

### Mute microphone
The user is allowed to mute the microphone either by user settings or during a call.

### Show video stream
An video stream is displayed by doorctrl when the 'call'-button at the house's main entrance has been pressed and also when talking to the visitor. This is an optional feature only available when there is a camera at the main entrance. Disable this feature by configuration data when there is no camera. Video stream errors are displayed in doorctrl's status line. The video stream is finished when the talk is finished or rejected by the user.

## Building ibs-intercom-client 
### Build environment
The ``doorctrl`` application has been implemented using the following frameworks:
* [pjsip-2.11](https://www.pjsip.org) (SIP client and audio/video streams)
* [GTK-3+ v3.24.5](https://www.gtk.org) (UI-Widgets)
* [gstreamer 1.0 v1.14.4 + plugins](https://gstreamer.freedesktop.org) (videostream)
* [Homegear BaseLib](https://github.com/Homegear/libhomegear-base)

#### Install ALSA libs
ALSA support is provided by ``libasound2-dev``

#### Build pjsip library
Download the sources from [here](https://www.pjsip.org/download.htm) and follow the [build instructions](https://trac.pjsip.org/repos/wiki/Getting-Started/Autoconf). First install this package ``libasound2-dev`` then simply use the default build configuration and run the following commands:
* ``./configure``
* ``make dep``
* ``make``
* ``make install``

There is a ``pjproject`` binary Debian package which is quite outdated. Do not use!

#### Install GTK-3+
Install the following GTK 3+ debian packages:
* ``libgtk-3-dev``

#### Install gstreamer 1.0
The ``gstreamer 1.0`` base lib and tons of plugins are required to build and run the ``doorctrl`` application.
Install the following debian packages:
* ``libgstreamer1.0-dev``
* ``libgstreamer-plugins-base1.0-dev``
* ``gstreamer1.0-plugins-good``
* ``gstreamer1.0-plugins-bad``

The ``gst-launch-1.0`` tool is extremly useful to test and debug a ``gstreamer``-pipeline. It defines and runs a pipeline as shell command. [Documentation and examples](https://gstreamer.freedesktop.org/documentation/tools/gst-launch.html?gi-language=c).

#### Install Homegear BaseLib
Create a file named ``homegear-nightly.list`` in ``/etc/apt/sources.list.d``.  Add this line

``deb https://aptnightly.homegear.eu/Raspbian buster/``

Add the repository's public key

``wget -O - https://aptnightly.homegear.eu/Release.key | apt-key add -``

Finally update the package descriptions and install Homegear BaseLib

``apt update``
``apt install libhomegear-base``

There are some dependencies which are required to include the headers and to link the project.

* ``libgcrypt-dev``
* ``gnutls-dev``

#### Install boost libs
``Boost`` is required to run unit tests. There is no dependency to the ``doorctrl`` binary. 
* ``libboost-dev``

## Configuration
The ``ibs-intercom-client`` utilizes a config file to read program setting at runtime. There are also program options e.g. to override the log level and also values

## Configuration file
``doorctrl`` expects to read a configuration file named ``doorctrl.conf`` which is expected to be found at the following filesystem locations:
* ``/etc/doorctrl/doorctrl.conf``
* ``%HOME/.doorctrl/doorctrl.conf``
The user specific config file has a higher priority than the global config file ``/etc/doorctrl/doorctrl.conf``. This file won't be read at all when a user specific file is found. The content is not merged together!

NOTE: The logdir, logrotation and logfile-prefix option is obsolete and no longer used!

## Program options
Use option ``--console-debug N`` to override loglevel. Log messages are always written to stdout or redirected to ``/var/log/syslog`` when running ``doorctrl`` as user service.
