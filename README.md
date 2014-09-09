Example Seperation Kernel-Style Application on seL4
===================================================

This seL4 project is a small example application for the `sk-config` tool.

The `sk-config` tool is a generic tool for building seperation kernel like
systems on seL4. This example application is one that contains two cells that
can communicate via shared memory.

This project is designed currently to run on the SABRE Lite board.


Building the Project
--------------------

The easiest way to build this project is via its project manifest:
    `sk-config-example-manifest`

Instructions on how to obtain projects using the `repo` tool are available at:
    http://sel4.systems/Download/

Once you have got the project, you should be able to build it as follows:

    $ make arm_capdl_defconfig
    $ make silentoldconfig
    $ make

This should produce an image file:
    `capdl-loader-experimental-image-arm-imx6`


Running the Image
-----------------

Instructions for running images on the SABRE Lite can be found at:

  * http://sel4.systems/Hardware/General/
  * http://sel4.systems/Hardware/sabreLite/


Debug vs. Non-Debug Builds
--------------------------

The project has two builds: debug and non-debug. The above instructions
build the debug version. To build the non-debug version change the first
step to isntead:

    $ make arm_capdl_nodebug_defconfig

(You may want to do a `make clean` first if you have already built the
debug verison.)

The debug version uses a debug build of the seL4 kernel. It also has the
cells link against the relatively large `libmuslc`, which allows them to
use e.g. printf() and so on for debug tracing.

The non-debug version uses a non-debug kernel and links the cells against
the far more minimal `libskc` library from the `sk-config` tool. 
