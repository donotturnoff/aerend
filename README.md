A networked display server and remote client library, specifically aimed at clients with limited resources such as IoT devices, and at low-bandwidth or high-latency networks. Includes a compositing window manager and a graphical widget toolkit similar to a subset of Swing.

Tested on Linux, but should also work on *BSD. Requires libdrm and evdev. To compile, run `make`. The server executable is `bin/aerend`, and the client library is `lib/libaerend.a`. To run the server, ensure the user is in the `video` group (or run as root), and execute from a virtual terminal with no other display server running in it.

Fonts in `res/` are distributed under the GUST licence. This licence in full, font information and font manifests are included in `res/`.
