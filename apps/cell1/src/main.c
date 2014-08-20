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
#include <cell1/cell1_driver.h>

#define JUMP_TO(addr) (((void(*)(void))addr)())

#if defined(SEL4_DEBUG_KERNEL)
#include <stdio.h>
#include <syscall_stubs_sel4.h>

MUSLC_SYSCALL_TABLE;
#define debug_printf(fmt, ...) printf("%25s:%4d \t" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define debug_printf(...) 

#define UART_REG(x)    ((volatile unsigned int *)(region_serial_base() + (x)))
#define BIT(n) (1ul<<(n))
#define UTXD  0x40 /* UART Transmitter Register */
#define UART_SR2_TXFIFO_EMPTY 14
#define USR2  0x98 /* UART Status Register 2 */

void putchar(int c) {
    /* Wait for serial to become ready. */
    while (!(*UART_REG(USR2) & BIT(UART_SR2_TXFIFO_EMPTY)));

    /* Write out the next character. */
    *UART_REG(UTXD) = c;
    if (c == '\n') {
        putchar('\r');
    }
}

void puts(const char *s) {
  while (*s) {
    putchar(*s);
    s++;
  }
}
#endif


int cell_main(int argc, char ** argv) {
    int msg = 0, err;

#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    puts("CELL1 ALIVE\n");

    debug_printf("CELL1: waiting for msg on input region\n");
    /* Wait til we get 0xcafe from cell2. */
    while (1) {
        err = region_input_read(&msg, sizeof(msg), 0);
        if (err) {
            debug_printf("CELL1: region_input_read failed. Faulting on 0xbad\n");
            JUMP_TO(0xbad);
        } else if (msg == 0xcafe) {
            break;
        }
    }

    debug_printf("CELL1: read 0x%x from input region\n",msg);
    /* Send 0xbeef back to them. */
    msg = 0xbeef;
    debug_printf("CELL1: writing 0x%x to output region\n",msg);
    err = region_output_write(&msg, sizeof(msg), 0);
    if (err) {
        JUMP_TO(0xbad);
    }
    debug_printf("CELL1: all done successfully. Faulting on address 0x40\n");

    puts("CELL1 DONE OK\n");
    /* Fault on 0x40 to indicate success. */
    JUMP_TO(0x40);

    return 0;
}
