What is this?
=============

I wanted to play a prank on a colleague who likes [Half-Life] series and
install a Black Mesa-themed screensaver on the lab machine he uses. This
seemed like a nice occasion to learn Win32 screensaver API.

How do I compile it?
====================

Use [MinGW]. I abused the ability of gnu objcopy to [create object files
from arbitrary binary blobs][objcopy]. Make sure to have done a recursive
clone to get the [stb] submodule.

Any other bits of information?
==============================

Lucian Wischik's [Holistic Screensavers][holscr] web page proved very
useful during the screensaver development.

Bugs?
-----

Absolutely. GIF transparency is not supported (Instead, pixels are assumed
to be 100% opaque). Scaling code tries to maximize the size of the picture
without cropping it while preserving the aspect ratio but is not tested
on anything besides the original gif (which happens to be square).

[Half-Life]: https://en.wikipedia.org/wiki/Half-Life_(series)
[MinGW]: https://sourceforge.net/projects/mingw-w64/
[objcopy]: https://balau82.wordpress.com/2012/02/19/linking-a-binary-blob-with-gcc/
[stb]: https://github.com/nothings/stb
[holscr]: http://www.wischik.com/scr/holsavers.html
