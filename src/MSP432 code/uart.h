#include "msp.h"

//UART0 is at the micro-USB port. UART2 is at pin 3.2 and pin 3.3. These two ports allow serial communication between the microcontroller and the ESP32-CAM.

//This function initializes UART0.
void UART0_init();

//This function sends string through UART0 by spin waiting.
int UART0_puts(const char *str);

//This function initializes UART2.
void UART2_init();

//This function sends string through UART2 by spin waiting. The transmit pin is pin 3.3. 
int UART2_puts(const char *str);

//This function receives string through UART2 by spin waiting. The receive pin is pin 3.2. 
//It returns after the receiving line feed ‘\n’ or end of the string ‘\0’
int UART2_gets(char *str);
