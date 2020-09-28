// Header file for ECE 4743 labs.
// Citing Microprocessors file "C:\ece3724\esos\examples\app_ecan_sender.c" as a reference used.

#ifndef REVF14_H
#define REVF14_H

#include "pic24_all.h"

// LED1 - Red LED
// LED2 - Yellow LED
// LED3 - Green LED

#define CONFIG_LED1() CONFIG_RF4_AS_DIG_OUTPUT()
#define CONFIG_LED2() CONFIG_RB14_AS_DIG_OUTPUT()
#define CONFIG_LED3() CONFIG_RB15_AS_DIG_OUTPUT()
#define LED1  (_LATF4)
#define LED2  (_LATB14)
#define LED3  (_LATB15)

// SW1
void CONFIG_SW1()  {
  CONFIG_RB13_AS_DIG_INPUT();
  ENABLE_RB13_PULLUP();
  DELAY_US(1);
}

#define SW1_PRESSED (_RB13 == 0)
#define SW1_RELEASED (_RB13 == 1)


// SW2
void CONFIG_SW2()  {
  CONFIG_RB12_AS_DIG_INPUT();
  ENABLE_RB12_PULLUP();
  DELAY_US(1);
}

#define SW2_PRESSED (_RB12 == 0)
#define SW2_RELEASED (_RB12 == 1)


// SW3
void CONFIG_SW3()  {
  CONFIG_RC15_AS_DIG_INPUT();
  ENABLE_RC15_PULLUP();
  DELAY_US(1);
}

#define SW3_PRESSED (_RC15 == 0)
#define SW3_RELEASED (_RC15 == 1)

// RPGA
void CONFIG_RPGA()  {
  CONFIG_RB8_AS_DIG_INPUT();
  ENABLE_RB8_PULLUP();
  DELAY_US(1);
}
#define RPGA_LOW (_RB8 == 0)
#define RPGA_HIGH (_RB8 == 1)

// RPGB
void CONFIG_RPGB()  {
  CONFIG_RB9_AS_DIG_INPUT();
  ENABLE_RB9_PULLUP();
  DELAY_US(1);
}

// VPOT
void CONFIG_VPOT() {
  CONFIG_RB2_AS_DIG_INPUT();
  ENABLE_RB2_PULLUP();
  DELAY_US(1);
}

// TEMP
void CONFIG_TEMP() {
  CONFIG_RB3_AS_DIG_INPUT();
  ENABLE_RB3_PULLUP();
  DELAY_US(1);
}



#define RPGB_LOW (_RB9 == 0)
#define RPGB_HIGH (_RB9 == 1)

#define CONFIG_RPGA()    CONFIG_RB8_AS_DIG_INPUT()
#define RPGA             _RB8
#define CONFIG_RPGB()    CONFIG_RB9_AS_DIG_INPUT()
#define RPGB             _RB9
#define RPG_TURN_CW()    (RPGA == 1 && RPGB == 0)
#define RPG_TURN_CCW()   (RPGA == 0 && RPGB == 1)
#define RPG_STOPPED()     (RPGA == 0 && RPGB == 0)
#define CONFIG_VPOT()    CONFIG_RB2_AS_DIG_INPUT()
#define VPOT             _RB2
#define CONFIG_TEMP()    CONFIG_RB3_AS_DIG_INPUT()
#define TEMP             _RB3

#endif
