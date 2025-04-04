/*
 * uart.h
 *
 *  Created on: Mar 10, 2025
 *      Author: albert
 */

#include "msp.h"

void UART0_init();
int UART0_puts(const char *str);
void UART2_init();
int UART2_puts(const char *str);
int UART2_gets(char *str);
