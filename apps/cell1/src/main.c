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
#include <stdint.h>
#include <stdio.h>

#define JUMP_TO(addr) (((void(*)(void))addr)())

#define UART_REG(x)    ((volatile unsigned int *)(region_serial_base() + (x)))
#define BIT(n) (1ul<<(n))
#define UTXD  0x40 /* UART Transmitter Register */
#define UART_SR2_TXFIFO_EMPTY 14
#define USR2  0x98 /* UART Status Register 2 */
#define UART_URXD_READY_MASK (1 << 15)
#define UART_BYTE_MASK       0xFF
#define UART_SR2_RXFIFO_RDR    0
#define URXD  0x00 /* UART Receiver Register */

#if defined(SEL4_DEBUG_KERNEL)
#include <stdio.h>
#include <syscall_stubs_sel4.h>

MUSLC_SYSCALL_TABLE;
#define debug_printf(fmt, ...) printf("%25s:%4d \t" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define debug_printf(...) 

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

unsigned char _getchar() {
    uint32_t reg = 0;
    unsigned char character = 0;
    
    while (character == 0) {
            if (*UART_REG(USR2) & BIT(UART_SR2_RXFIFO_RDR)) {
                    reg = *UART_REG(URXD);

                    if (reg & UART_URXD_READY_MASK) {
                            character = reg & UART_BYTE_MASK;
                    }
            }
    }

    return character;
}


int cell_main(int argc, char ** argv) {
    int err, next;
    unsigned char ch[2];

    next = 0;

#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    _puts("CELL1 ALIVE\n");
    _puts("Please enter input.\n");

    while (1) {
            ch[0] = _getchar();
            if (ch[0] == 13)
                    ch[0] = '\n';
            ch[1] = '\0';
            _puts(ch);

            next++;

            err = region_output_write(&ch, 1, 0);
            err = region_output_write(&next, sizeof(next), 1);

            if (err) {
                    _puts("Error sending, aborting\n");
                    JUMP_TO(0xbad);
            }
    }

    _puts("CELL1 DONE OK\n");
    /* Fault on 0x40 to indicate success. */
    JUMP_TO(0x40);

    return 0;
}
