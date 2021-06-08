An M17 client designed for use with MMDVM modems and hotspots to allow for use as an M17 radio.

This requires access to an M17 capable MMDVM modem or hotspot and of course a suitable radio if using the modem version.

The M17 Client is split into two parts, the Daemon and the front end. The Daemon is common to all of the front ends, and is the part that handles the audio conversion, the MMDVM control, and the configuration. Optionally it can use HamLib to control suitable transceivers. The front-end is designed to be the part that people use.

It is hoped to eventually have at least two front ends for the M17 Client, the first a standard GUI which already exists for desktop operation, and a different user interface more geared towards use in a mobile setting with PTT input and other controls via GPIO inputs and possibly a touch screen.

