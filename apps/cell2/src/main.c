/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <cell2/cell2_driver.h>

#define JUMP_TO(addr) (((void(*)(void))addr)())

int cell_main(int argc, char ** argv) {
    int msg = 0, err;

    /* Send 0xcafe to cell1. */
    msg = 0xcafe;
    err = region_output_write(&msg, sizeof(msg), 0);
    if (err) {
        JUMP_TO(0xbad);
    }

    /* Wait for 0xbeef from cell1. */
    while (1) {
        err = region_input_read(&msg, sizeof(msg), 0);
        if (err) {
            JUMP_TO(0xbad);
        } else if (msg == 0xbeef) {
            break;
        }
    }

    /* read from the channel */
    msg = 0;
    channel_recv(foobar, &msg);
    if (msg != 1) {
        JUMP_TO(0xbad);
    }
    channel_recv(foobar, &msg);
    if (msg != 2) {
        JUMP_TO(0xbad);
    }

    /* Fault on 0x42 to indicate success. */
    JUMP_TO(0x42);

	return 0;
}
