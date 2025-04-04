#include "motor.h"

#define timePerAngle 10 // Constant to translate

void left_fw(); // Helper functions for H-Bridge
void left_bw();
void left_off();
void right_fw();
void right_bw();
void right_off();

void motors_init() {
    P2->OUT = 0x00; // Set P2.4-2.7 to low GPIO out, 2.4, 2.5 for the left, 2.6, 2.7 for the right
    P2->SEL0 = 0x00;
    P2->SEL1 = 0x00;
    P2->DIR = 0xF0;
    return;
}

void left_fw() {
    P2->OUT &= (~BIT5);
    P2->OUT |= BIT4;
    return;
}
void left_bw() {
    P2->OUT &= (~BIT4);
    P2->OUT |= BIT5;
    return;
}

void left_off() {
    P2->OUT &= (~(BIT4 | BIT5));
    return;
}

void right_fw() {
    P2->OUT &= (~BIT7);
    P2->OUT |= BIT6;
    return;
}

void right_bw() {
    P2->OUT &= (~BIT6);
    P2->OUT |= BIT7;
    return;
}

void right_off() {
    P2->OUT &= (~(BIT6 | BIT7));
    return;
}


void forward(int time) {
    right_fw(); // Make both motors spin forward for a set time
    left_fw();
    SysTick_Delayms(time);
    right_off(); // Turn off both motors
    left_off();
    return;
}

void backward(int time) {
    right_bw(); // Make both motors spin backwards for a set time
    left_bw();
    SysTick_Delayms(time);
    right_off(); // Turn off both motors
    left_off();
    return;
}

void turn(int angle) {
    if (angle == 0 || angle > 180 || angle < -180) { // Invalid angles
    } else if (angle > 0){ // Counter-clockwise turns, spin right wheel forward and left wheel backwards
        right_fw();
        left_bw();
        SysTick_Delayms(angle * timePerAngle);
        right_off();
        left_off();
    } else { // Clockwise turns, spin left wheel forward and right wheel backwards
        right_bw();
        left_fw();
        SysTick_Delayms(angle * (-1) * timePerAngle);
        right_off();
        left_off();
    }
    return;
}

