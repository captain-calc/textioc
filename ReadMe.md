## TextIOC

### About

*TextIOC* is an eZ80 library designed for the TI-84 Plus CE
that provides several functions for advanced text input and
output for C programs. *TextIOC* is compatible with the *GraphX*
font, any *FontlibC* custom font, and TI-OS homescreen font.


### Building

After you have downloaded the repository, put the folder in
the *CEdev* directory. To build the library, run *build.bat*.
This script will generate a *.8xv* file and a *.lib* file.
In order to create programs using the library, the *textioc.h*
file should go in the *include* folder under *CEdev*. The
*TEXTIOC.lib* should go into the *CEdev/lib/libload* directory.

**IMPORTANT:** As of December 12, 2020, the TextIOC library is not part
of the offical CE C library release. The library is unfinished.