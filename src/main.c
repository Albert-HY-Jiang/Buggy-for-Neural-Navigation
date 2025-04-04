#include "msp.h"
#include <stdio.h>
#include <string.h>

#include "uart.h"
#include "motor.h"
#include "us.h"

#define unit_step 1000

enum buggieState {
    INIT,
    IDLE,
    MOVE,
    SCAN
};

void main(void)
{

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    char readbuf[20];
    char writebuf[20];
    int step = 1;
    long distance;
    char* time_stamp;
    int i;

    enum buggieState currState = INIT;

    while(1)
    {
        switch(currState)
        {
        case INIT:
            Clock_init();
            motors_init();
            UART2_init();
            TIMERA0_init();
            US_init();
            UART2_puts("INIT\n");
            currState = IDLE;
            break;

        case IDLE:

            UART2_gets(readbuf);
            sprintf(writebuf, "%d\n", strncmp(readbuf, "go",2));
            UART2_puts("buggie started");

            if(strncmp(readbuf, "go",2) == 0)
            {
                __enable_irq();             // Enables interrupts to the system
                currState = MOVE;
            }

            break;

        case MOVE:
            forward(unit_step * step);
            step++;
            currState = SCAN;
            break;

        case SCAN:
            for(i = 0; i < 2; i++){
                distance = measure_dist();
                time_stamp = get_time_stamp();
                sprintf(writebuf, "%s, %d\n", time_stamp, distance); // format the output string
                UART2_puts(writebuf);
                turn(-120);
            }

            currState = MOVE;
            break;
        }

    }
}



