#include "uart.h"
#include "lfb.h"
#include "bool.h"

void main()
{
    bool _exitFlag = false;

    // set up serial console and linear frame buffer
    uart_init();
    lfb_init();

    // display a string on screen
    // lfb_print(10, 5, "Hello World!");
    int i = 0;

    // I had to use a while loop because it didn't know what to do with 'for' loops
    // - Chris
    while (i < 20)
    {
        if (i == 0)
        {
            lfb_print(0, 0, "Hello World!");
        }
        else if (i == 10)
        {
            lfb_print(0, 10, "Welcome to PegasOS!");
        }
        else
        {
            lfb_print(0, i, "x: empty");
        }

        i++;
    }

    int counter = 0;
    // echo everything back
    while (!_exitFlag)
    {
        // This line doesn't work rn because it needs input that we can't read yet
        // - Chris
        // uart_send(uart_getc());

        counter++;

        lfb_print(counter % 100, 16 + (counter / 100), ".");

        if (counter > 500)
        {
            lfb_print(25, 25, "EXITING...");
        }
        if (counter > 600)
        {
            _exitFlag = true;
        }
    }

    lfb_print(0, 26, "DONE!");
}
