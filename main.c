#include "uart.h"
#include "lfb.h"

void main()
{
    // set up serial console and linear frame buffer
    uart_init();
    lfb_init();

    // display a string on screen
    lfb_print(10, 5, "Hello World!");

    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}
