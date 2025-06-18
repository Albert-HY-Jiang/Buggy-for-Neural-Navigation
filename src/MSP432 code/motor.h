#include "msp.h"
#include <stdio.h>
#include "SysTick0.h"

void motors_init(); // Initialize ports controlling H-Bridge

void forward(int time); // Move buggy forward for a time in ms
void backward(int time); // Move buggy backward for a time in ms
void turn(int angle); // Turn buggy by an angle in degrees
