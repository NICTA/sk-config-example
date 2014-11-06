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

int putchar(int c) {
    /* Wait for serial to become ready. */
    while (!(*UART_REG(USR2) & BIT(UART_SR2_TXFIFO_EMPTY)));

    /* Write out the next character. */
    *UART_REG(UTXD) = c;
    if (c == '\n') {
        putchar('\r');
    }

    return 1;
}

int puts(const char *s) {
  while (*s) {
    putchar(*s);
    s++;
  }

  return 1;
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
    char ch[2];
    unsigned char msg;

#if defined(SEL4_DEBUG_KERNEL)
    SET_MUSLC_SYSCALL_TABLE;
    platsupport_serial_setup_bootinfo_failsafe();
#endif

    puts("CELL1 ALIVE\n");
    puts("Please enter input.\n");

    // Read characters from the UART one at a time.  For each character, echo
    // it locally (converting RET to newline as appropriate), then write it to
    // the output memory region along with a sequence number.
    while (1) {
            msg = _getchar();
            if (msg == 13)
                    msg = '\n';

            channel_send(chan1, &msg);
            putchar(msg);
    }

    puts("CELL1 DONE OK\n");
    /* Fault on 0x40 to indicate success. */
    JUMP_TO(0x40);

    return 0;
}
