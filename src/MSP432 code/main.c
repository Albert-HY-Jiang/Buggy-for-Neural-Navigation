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

    char readbuf[128];
    char writebuf[128];
    int step = 1;       //number of forward step taken at each round
    long distance;      //distance measured by ultrasonic sensor
    char* time_stamp;   //time stamp starting
    int i;

    enum buggieState currState = INIT;

    while(1)
    {
        switch(currState)
        {
        case INIT:
            //this state initialize all components
            Clock_init();
            motors_init();      //motors for moving
            UART2_init();       //uart2(3.2, 3.3) for communicating with ESP32 cam
            TIMERA0_init();
            US_init();
            currState = IDLE;
            break;

        case IDLE:
            //this state wait for the ESP32 cam to be ready and send "go" through uart port
            UART2_gets(readbuf);
            if(strncmp(readbuf, "go",2) == 0)
            {
                __enable_irq();             // enables interrupts to start timer for time stamps, irq for ultrasonic sensor
                currState = MOVE;           // starts moving
            }

            break;

        case MOVE:
            forward(unit_step * step);      //the forward steps will gradually increase
            step++;
            currState = SCAN;
            break;

        case SCAN:
            for(i = 0; i < 2; i++){
                distance = measure_dist();
                time_stamp = get_time_stamp();
                sprintf(writebuf, "%s, %d\n", time_stamp, distance); // format the output string
                UART2_puts(writebuf);   //write to ESP32 cam
                UART2_gets(readbuf);    //wait for ready signal from ESP32 cam
                turn(-120);         //it will end up turning for 240 degree
            }

            currState = MOVE;
            break;
        }

    }
}



