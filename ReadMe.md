## TextIOC

### About

*TextIOC* is an eZ80 library designed for the TI-84 Plus CE
that provides several functions for advanced text input and
output for C programs. It uses the *GraphX* library from the
[CE SDK](https://github.com/CE-Programming/toolchain). *TextIOC* is compatible with both the *GraphX* font and the TI-OS homescreen.

Support for the FontLibC library by DrDnar will be added soon!

### Building

After you have downloaded the repository, put the folder in
the *CEdev* directory. To build the library, run *build.bat*.
This script will generate a *.8xv* file and a *.lib* file.
In order to create programs using the library, the *textioc.h*
file should go in the *include* folder under *CEdev*. The
*TEXTIOC.lib* should go into the *CEdev/lib/libload* directory.

**IMPORTANT:** As of December 6, 2020, the TextIOC library is not part
of the offical CE C library release. If you wish to create programs
with *TextIOC*, please include the *TEXTIOC.8xv* appvar with your
program download.