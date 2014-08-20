Example Seperation Kernel-Style Application on seL4
===================================================

This seL4 project is a small example application for the `sk-config` tool.

The `sk-config` tool is a generic tool for building seperation kernel like
systems on seL4. This example application is one that contains two cells that
can communicate via shared memory.

This project is designed currently to run on the SABRE Lite board.

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

Instructions for running images on the SABRE Lite can be found at:

  * http://sel4.systems/Hardware/General/
  * http://sel4.systems/Hardware/sabreLite/
