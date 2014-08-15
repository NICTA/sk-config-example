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
#else
#define debug_printf(...) 
#define UART_PADDR     0x43f90000
#define UART_REG(x)    ((volatile unsigned int *)(region_serial_base() + (x)))
#define UART_SR1_TRDY  13
#define UART_SR1_RRDY  9
#define UTXD           0x40
#define URXD           0x0
#define USR1           0x94
#define UCR1           0x80

#define UART_URXD_READY_MASK (1 << 15)
#define UART_BYTE_MASK       0xFF
#define BIT(n) (1ul<<(n))

void putchar(int c) {
    while (!(*UART_REG(USR1) & BIT(UART_SR1_TRDY)));

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
#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
#endif
    int msg = 0, err;

#if defined(SEL4_DEBUG_KERNEL)
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    puts("CELL2 ALIVE\n");

    /* Send 0xcafe to cell1. */
    msg = 0xcafe;
    debug_printf("CELL2: sending msg 0x%x on output region\n",msg);
    err = region_output_write(&msg, sizeof(msg), 0);
    if (err) {
        debug_printf("CELL2: error writing to output region. Faulting on 0xbad\n");
        JUMP_TO(0xbad);
    }

    /* Wait for 0xbeef from cell1. */
    debug_printf("CELL2: waiting for msg on input region\n");
    while (1) {
        err = region_input_read(&msg, sizeof(msg), 0);
        if (err) {
            JUMP_TO(0xbad);
        } else if (msg == 0xbeef) {
            break;
        }
    }

    debug_printf("CELL2: read 0x%x from input region\n",msg);
    debug_printf("CELL2: all done successfully. Faulting on address 0x42\n");

    puts("CELL2 DONE OK\n");

    /* Fault on 0x42 to indicate success. */
    JUMP_TO(0x42);

    return 0;
}
