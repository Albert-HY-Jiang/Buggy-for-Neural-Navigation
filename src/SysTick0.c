// Functions to initialize SysTick for simple delay in ms or us

#include "msp.h"
#include <stdio.h>
#include "SysTick0.h"

void SysTick_Init();
void SysTick_Delayms();
void SysTick_Delayus();

// Function to initialize SysTick without interrupt enable
// No longer necessary, delay functions work indpendently
void SysTick_Init(){
    SysTick->CTRL = 0; // disable
    SysTick->LOAD = 0x00FFFFFF; // set reload to max
    SysTick->VAL = 0; // clear counter value
    SysTick->CTRL = 0x00000005 ; // set CLKSOURCE to core clock and enable to 1
    // clock is 3MHz, CTRL has interrupts disabled
    // tech reference manual 2.4.4.1
    return;
}

// Delay function with argument in milliseconds
void SysTick_Delayus(int time){
    SysTick->LOAD = (time * 3 - 1); // 3000t / 3 MHz = t ms
    // minimum 25 us, scales oddly until ~100 us
    SysTick->VAL = 0; // any write to VAL clears VAL, restart
    SysTick->CTRL = 0x00000005 ; // set CLKSOURCE to core clock and enable to 1
    while(!(SysTick->CTRL & 0x00010000)); // stays in loop until COUNTFLAG is true
    return;
}

// Delay function with argument in milliseconds
void SysTick_Delayms(int time){
    SysTick->LOAD = (time * 3000 - 1); // 3000t / 3 MHz = t ms
    SysTick->VAL = 0; // any write to VAL clears VAL, restart
    SysTick->CTRL = 0x00000005 ; // set CLKSOURCE to core clock and enable to 1
    while(!(SysTick->CTRL & 0x00010000)); // stays in loop until COUNTFLAG is true
    return;
}
