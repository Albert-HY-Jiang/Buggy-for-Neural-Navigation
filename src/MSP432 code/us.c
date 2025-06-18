#include "us.h"


long intrcnt = 0;
long clktick = 0;

long start = 0;
long end = 0;

int min = 0;
int sec = 0;
char buffer[20];

long US_scan_once();

void Clock_init()
{
    //setting up clock signals
    CS->KEY = CS_KEY_VAL;                // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 24MHz
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                      // Lock CS module from unintended accesses

}

void TIMERA0_init()
{
    //setting up timer A0
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;       // CCR0 interrupt enabled
    TIMER_A0->CCR[0] = INTR_PRD - 1; // interrupt is raised for every 1000 clock tick
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR; // SMCLK, upmode, TA clear

    // Enable Timer interrupt for timing and set priority as 3
    NVIC_SetPriority(TA0_0_IRQn, TIMEA0_PRIO);
    NVIC_EnableIRQ(TA0_0_IRQn);


}

void US_init()
{
    // Configure GPIO
    //set P4 as GPIO
    P4->SEL0 = 0;
    P4->SEL1 = 0;

    // uses P4.7 as echo
    P4->DIR &= ~BIT7;            // P4.7 as input pin
    P4->REN |= BIT7;            // P4.7 pull resistor enabled
    P4->OUT &= ~BIT7;            // P4.7 selected as pull down (active low)

    // uses P4.6 as trigger
    P4->DIR |= BIT6;          // trigger pin as output

    // receiving interrupt from P4.7
    P4->IFG = 0;                // clean pending interrupt flag
    P4->IES &= ~BIT7;           // enable rising edge interrupt
    P4->IE |= BIT7;             // enable interrupt


    // Enable Port4 interrupt for echo and set priority as 4
    NVIC_SetPriority(PORT4_IRQn, PORT4_PRIO);
    NVIC_EnableIRQ(PORT4_IRQn);
}

/**
 * This function delay for the given amount of clock cycle
 */
void Delay(uint32_t tick)
{
    // initialize timer32 1 with give amount of tick
    TIMER32_1->LOAD = tick;
    //no prescaler, periodic wrapping mode, disable interrupt, 32-bit timer
    TIMER32_1->CONTROL = 0xc2;

    //spin wait until the time is reached
    while ((TIMER32_1->RIS & 1) == 0)
        ;
    TIMER32_1->INTCLR = 0;      //clear raw interrupt flag

}

long US_scan_once()
{
    // uses pin 4.6 for trigger
    P4->OUT |= BIT6;            // generate pulse
    Delay(240);                 // 240 cycle in 24mhz is about 10us
    P4->OUT &= ~BIT6;           // stop pulse

    P4->IFG = 0;                // clear P4 interrupt just in case anything happened before
    P4->IES &= ~BIT7;           // wait for rising edge on ECHO pin
    Delay(660000);              //set this to 100us - 60ms

    if(end > start)
    {
       return (end - start) / DIST_DIV;    // converting ECHO time into cm
    }
    else
    {
        return 300;
    }
}

long measure_dist()
{
    long res = 0;
    int i;
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
        long dist = US_scan_once();
        if(dist > 300)
            res += 300;
        else
            res += dist;
    }

    return res/SAMPLE_SIZE;

}

char* get_time_stamp()
{
    sec = intrcnt / 24000;
    min = sec / 60;
    sec %= 60;
    sprintf(buffer, "%d,%d", min, sec);
    return buffer;
}

/**
 * Port4 interrupt service routine, start timing echo from rising edge to falling edge
 */
void PORT4_IRQHandler(void)
{
    // check if interrupt is pending for P4.7
    if (P4->IFG & BIT7)
    {
        //if we are checking for raising edge, we are at the beginning of the echo
        if (!(P4->IES & BIT7 ))
        {
            //record starting time of the echo
            start =  intrcnt * 1000 + (long) TIMER_A0->R;
            //now checks for falling edge, which is the end of the echo
            P4->IES |= BIT7;
        }
        else
        {
            end = intrcnt * 1000 + (long) TIMER_A0->R; //calculating ECHO length
            //now back to checking rising edge, which is the beginning of the echo
            P4->IES &= ~BIT7;

        }
        P4->IFG &= ~BIT7;             //clear flag
    }
}

/**
 * TimerA0 interrupt service routine, updates count of interrupt and clears interrupt flag
 */
void TA0_0_IRQHandler(void)
{
    //    Interrupt gets triggered for every 1000 clock cycle in SMCLK
    intrcnt++;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
}



