Example Seperation Kernel-Style Application on seL4
===================================================

This seL4 project is a small example application for the `sk-config` tool.

The `sk-config` tool is a generic tool for building seperation kernel like
systems on seL4. This example application is one that contains two cells that
can communicate via shared memory.

This project is designed currently to run on the KZM board, and has 
most recently been tested under QEMU.

The easiest way to build this project is via its project manifest:
 `sk-config-example-manifest`.

Once you have done that, you should be able to build it as follows:

    $ make arm_capdl_defconfig
    $ make silentoldconfig
    $ make

This should produce an image file: capdl-loader-experimental-image-arm-imx31
which can be run e.g. under QEMU as follows:

    $ qemu-system-arm -M kzm -nographic -kernel images/capdl-loader-experimental-image-arm-imx31