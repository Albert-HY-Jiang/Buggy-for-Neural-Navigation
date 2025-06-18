#include "msp.h"
#include <stdint.h>
#include <stdio.h>

//higher number has lower priority
#define PORT4_PRIO 4
#define TIMEA0_PRIO 3
#define INTR_PRD 1000
#define DIST_DIV 1400
#define SAMPLE_SIZE 3

//This function initializes the clock signal
void Clock_init();

//This function initializes the TIMERA0 for recording time_stamp          
void TIMERA0_init();     

//This function initializes pins for trigger ultrasonic sensor and record echo signals. 
//It uses pin 4.6 as trigger and pin 4.7 for echo. It also registers IRQ for receiving interrupt from rising or falling edge at pin 4.7
void US_init();             

//This function measures the distance between the ultrasonic sensor and the object in front of it. 
//It triggers the sensor several times and return the average value in cm. 
//The specific size of the sample can be modified by changing the macro in us.h. The range of distance is (0, 300)
long measure_dist();

//This function returns the time passed since the start of the measurement (marked by US_init). The time stamp is record as “min,sec” in string
char* get_time_stamp();

void Delay(uint32_t tick);
