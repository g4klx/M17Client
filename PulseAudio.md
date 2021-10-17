M17Client Daemon contains an optional PulseAudio backend. Along with enabling the use of Bluetooth headsets through newer releases of BlueZ which do not include an ALSA backend, it also grants the ability to dynamically route audio streams over a local network to a remote PulseAudio server. When used in conjunction with existing M17Client configuration options for daemon and client socket addresses, it is possible to separate the daemon and client for remote use over a network.

The original ALSA implementation is preserved as the default and optional PulseAudio support is enabled through a make variable.

Build M17Client Daemon with PulseAudio support:

```
$ make AUDIO=pulse
```

Remote example setup, assuming a client at 192.168.0.10 connecting to a daemon running at 192.168.0.20:

1. **Client**: Enable PulseAudio network listen server

```
$ pactl load-module module-native-protocol-tcp listen=192.168.0.10
```

2. **Client**: Update M17Client GUI configuration (~/.M17GUI) with network addresses

```
DaemonAddress=192.168.0.20
DaemonPort=7658
SelfAddress=192.168.0.10
SelfPort=7659
```

3. **Client**: Run M17Client GUI

```
$ ./M17GUI
```

4. **Daemon**: Update M17Client Daemon configuration (M17Client.ini) with network addresses

```
[Control]
RemoteAddress=192.168.0.10
RemotePort=7659
LocalAddress=192.168.0.20
LocalPort=7658
```

5. **Daemon**: Run M17Client Daemon with location of remote PulseAudio server

```
$ PULSE_SERVER=192.168.0.10 ./M17Client ./M17Client.ini
```

6. Operate like usual

**Note**: The PulseAudio server listens on port 4713/TCP, so make sure it is not blocked by firewall policy.

