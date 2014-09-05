/*
 * Copyright 2014, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

#include <cell1/cell1_driver.h>

#define JUMP_TO(addr) (((void(*)(void))addr)())

int cell_main(int argc, char ** argv) {
    int msg = 0, err;

    /* Wait til we get 0xcafe from cell2. */
    while (1) {
        err = region_input_read(&msg, sizeof(msg), 0);
        if (err) {
            JUMP_TO(0xbad);
        } else if (msg == 0xcafe) {
            break;
        }
    }

    /* Send 0xbeef back to them. */
    msg = 0xbeef;
    err = region_output_write(&msg, sizeof(msg), 0);
    if (err) {
        JUMP_TO(0xbad);
    }

    /* Write some values to a channel */
    msg = 1;
    channel_send(foobar, &msg);
    msg = 2;
    channel_send(foobar, &msg);

    /* Fault on 0x42 to indicate success. */
    JUMP_TO(0x42);

	return 0;
}
