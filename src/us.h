#include "msp.h"
#include <stdint.h>
#include <stdio.h>

//higher number has lower priority
#define PORT4_PRIO 4
#define TIMEA0_PRIO 3
#define INTR_PRD 1000
#define DIST_DIV 1400
#define SAMPLE_SIZE 3

void Clock_init();
void TIMERA0_init();
void US_init();


long measure_dist();
char* get_time_stamp();

void Delay(uint32_t tick);
