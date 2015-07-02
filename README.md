# GnublinBootSplash
Basic linux framebuffer bootsplash daemon, to display configureable boot screen on small, embedded, LCD screens which are supported by a framebuffer driver/module, opening a framebuffer (e.g.: /dev/fb1) device.
Originally it was made for a GNUbLIN (http://gnublin.embedded-projects.net) mini LINUX ARM computer with a greyscale LCD for an embedded project. However GnublinBootSplash is now ported to Raspberry PI, tested with ARCH linux (but as well debian and for sure runs with others...). It runs with traditional SystemV startup scripts but as well systemd, using kdbus to obtain boot information.

## Notice
This is a experimental project shared here. It doesn't claim to be complete neither in code nor in documentation. 

## dependencies
- Directfb >= 0.9.20
- Glib >= 2.0
- libsystemd if using together with systemd and kdbus
- kdbus enabled linux kernel

## How it works
### basic configuration
GnublinBootSplash takes a small number of configuration parameters from command line and is configured trough a GLib style configuration file (/etc/gnublinbootsplash.cfg). 
It is started either as a SystemV or as a systemd daemon.

### Legacy mode
- in "legacy mode" GnublinBootSplash opens a fifo and takes text based messages to be displayed and to set progress bar e.g:
  ```bash
  ~> echo "message=booting service xyz; percent=20" >/tmp/bootsplash.fifo
  ```
Following parameters in messages are understood:
- message=<message>       : let GnublinBootSplash print a status message to the display
- percent=<number 0-100>  : sets the progress bar of the bootsplash
- help                    : prints a help to the system log ;-)
- exit                    : let the bootsplash daemon clean up and exit

multiple parameters might be combined and seperated by a ';'. 
The bootinformation passed to the fifo need to come from the SystemV startup scripts by adding the above printend "echo" line to the startup scripts.

### Systemd mode
GnublinBootSplash connects to 0-system kdbus and optains boot information there. Once it is started it works automatically. there is no fifo created and there is no need to pass any message to it.

## Installation
clone GnublinBootSplash to your local computer and run
```bash
./configure
``` 
if kdbus/systemd otion should be enabled 
```bash
./configure --enable-sdbusio
``` 
enables systemd mode.
next is to do a
```bash
make
make install
``` 
finally add the bootsplash daemon to startup by creating either a SystemV startup script or a systemd service file.
