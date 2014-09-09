/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <autoconf.h>
#include <cell2/cell2_driver.h>

#define JUMP_TO(addr) (((void(*)(void))addr)())

#if defined(SEL4_DEBUG_KERNEL)
#include <stdio.h>
#include <syscall_stubs_sel4.h>

MUSLC_SYSCALL_TABLE;
#define debug_printf(fmt, ...) printf("%25s:%4d \t" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif

int cell_main(int argc, char ** argv) {
    int err;
    int next;
    unsigned char ch;

#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    // Read characater data from the input memory region, filter it (remove
    // vowels), and then emit it to the output memory region.
    //
    // This implementation uses a sequence number paired with each character to
    // facilitate data loss detection when scheduling interferes with memory
    // read/write operations in adjacent cells.
    while (1) {
        err = region_input_read(&ch, sizeof(ch), 0);
        err = region_input_read(&next, sizeof(next), sizeof(ch));
        if (err) {
            JUMP_TO(0xbad);
        } else if (ch > 0) {

            if (ch == 'a' ||
                ch == 'e' ||
                ch == 'i' ||
                ch == 'o' ||
                ch == 'u')
                    ch = '*';

            err = region_output_write(&ch, sizeof(ch), 0);
            err = region_output_write(&next, sizeof(next), sizeof(ch));
            if (err) {
                    JUMP_TO(0xbad);
            }
        }
    }

    debug_printf("CELL2: all done successfully. Faulting on address 0x40\n");

    /* Fault on 0x40 to indicate success. */
    JUMP_TO(0x40);

    return 0;
}
