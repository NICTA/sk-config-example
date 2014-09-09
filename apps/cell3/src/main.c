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
#include <cell3/cell3_driver.h>

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

void _putchar(int c) {
    /* Wait for serial to become ready. */
    while (!(*UART_REG(USR2) & BIT(UART_SR2_TXFIFO_EMPTY)));

    /* Write out the next character. */
    *UART_REG(UTXD) = c;
    if (c == '\n') {
        _putchar('\r');
    }
}

void _puts(const char *s) {
  while (*s) {
    _putchar(*s);
    s++;
  }
}
#endif

int cell_main(int argc, char ** argv) {
    int err, next, prev;
    unsigned char ch[2];
    char fmt[64];

#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    _puts("CELL3 ALIVE\n");

    next = prev = -1;
    ch[1] = '\0';

    // Read character data from the input memory region.  Each character is
    // paired with a sequence number.  If we read a character paired with a
    // sequence number greater than the next one that we expect, then print '_'
    // to indicate the detection of data loss in this case.
    while (1) {
            while (1) {
                    err = region_input_read(ch, 1, 0);
                    err = region_input_read(&next, sizeof(next), 1);
                    if (err) {
                            JUMP_TO(0xbad);
                    } else if (next > prev) {
                            if (next > prev + 1)
                                    ch[0] = '_';

                            _puts(ch);
                            prev = next;
                            break;
                    }
            }
    }

    debug_printf("CELL3: all done successfully. Faulting on address 0x40\n");

    _puts("CELL3 DONE OK\n");

    /* Fault on 0x40 to indicate success. */
    JUMP_TO(0x40);

    return 0;
}
