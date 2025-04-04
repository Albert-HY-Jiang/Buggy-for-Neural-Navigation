/*
 * uart.c
 *
 *  Created on: Mar 10, 2025
 *      Author: albert
 */
#include "msp.h"
#include "uart.h"

void UART0_init();
int UART0_puts(const char *str);
void UART2_init();
int UART2_puts(const char *str);
int UART2_gets(char *str);

/**
 * This function is initializing the UART0(micro-USB port) for sending distance message to the console with baud rate of 9600
 *
 */
void UART0_init()
{
    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;             // set 2-UART pin as secondary function

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                     // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
    EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // take eUSCI out of reset mode
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE &= ~EUSCI_A_IE_RXIE;        // Disable USCI_A0 RX interrupt
}

/**
 * This function puts a string to transmit buffer in the UART0, which will be sent to console
 */
int UART0_puts(const char *str)
{
    int status = -1;

    if (str != '\0')
    {
        status = 0;

        while (*str != '\0')
        {
            /* Wait for the transmit buffer to be ready */
            while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
                ;

            /* Transmit data */
            EUSCI_A0->TXBUF = *str;

            /* If there is a line-feed, add a carriage return */
            if (*str == '\n')
            {
                /* Wait for the transmit buffer to be ready */
                while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
                    ;
                EUSCI_A0->TXBUF = '\r';
            }

            str++;
        }
    }

    return status;
}

/**
 * This function is initializing the UART2(pin 3.2,3.3) for sending distance message to the console with baud rate of 9600
 *
 */
void UART2_init()
{
    //pin 3.2 RX pin 3.3 TX
    // Configure UART pins
    P3->SEL0 |= BIT2 | BIT3;             // set 2-UART pin as secondary function
    P3->SEL1 &= ~(BIT2 | BIT3);

    // Configure UART
    EUSCI_A2->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A2->CTLW0 |= EUSCI_B_CTLW0_SSEL__SMCLK; // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A2->BRW = 78;                     // 12000000/16/9600
    EUSCI_A2->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
    EUSCI_A_MCTLW_OS16;

    EUSCI_A2->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // take eUSCI out of reset mode
    EUSCI_A2->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A2->IE &= ~EUSCI_A_IE_RXIE;        // Disable USCI_A2 RX interrupt

}

/**
 * This function puts a string to transmit buffer in the UART2, which will be sent to console
 */
int UART2_puts(const char *str)
{
    int status = -1;

    if (str != '\0') {
        status = 0;

        while (*str != '\0') {
            /* Wait for the transmit buffer to be ready */
            while (!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));

            /* Transmit data */
            EUSCI_A2->TXBUF  = *str;

            /* If there is a line-feed, add a carriage return */
            if (*str == '\n') {
                /* Wait for the transmit buffer to be ready */
                while (!(EUSCI_A2->IFG & EUSCI_A_IFG_TXIFG));
                EUSCI_A2->TXBUF = '\r';
            }

            str++;
        }
    }

    return status;
}

int UART2_gets(char *str)
{

    while (1) {
        /* Wait for the receive buffer to be ready */
        while (!(EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG));

        /* Transmit data */
        *str = EUSCI_A2->RXBUF;
        if(*str == '\n' || *str == '\0')
        {
            break;
        }
        str++;
    }


    return 1;
}
