## TEXTIOC

### About

*TEXTIOC* is an eZ80 library designed for the TI-84 Plus CE
that provides several functions for advanced text input and
output for C programs. It uses both the *GraphX* and *FontLibC*
libraries from the [CE SDK](https://github.com/CE-Programming/toolchain). *TEXTIOC* is not compatible
with the *GraphX* font but does support any custom fonts made
with *FontLibC*.

### Building

After you have downloaded the repository, put the folder in
the *CEdev* directory. To build the library, run *build.bat*.
In order to create programs using the library, he *textioc.h*
file should go in the *include* folder under *CEdev*. This
will generate an *.8xv* file and a *.lib* file. The *TEXTIOC.lib*
should go into the *CEdev/lib/libload* directory.

**IMPORTANT:** Until further notice, this library is *not* part of
the official CE SDK and is currently incomplete. If you wish to
release any programs that use the current version of *TEXTIOC*,
please include the *TEXTIOC.8xv* file with your program download.