/*******************************************************************
 * 
 * C code framework for ESOS user-interface (UI) service
 * 
 *    requires the EMBEDDED SYSTEMS target rev. F14
 * 
 * ****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "revF14.h"

#include <esos.h>
#include "esos_pic24.h"
#include <pic24_all.h>
#include "esos_pic24_rs232.h"
#include <p33EP512GP806.h>
#include "esos_f14ui.h"
#include "esos_f14ui.c"

// Define global variables

// Switches
int16_t i16_SW1Timer = 0;
int16_t i16_SW1State = 0;
int16_t i16_SW2Timer = 0;
int16_t i16_SW2State = 0;
uint8_t u8_SWvalue = 0;
uint16_t u16_SWvalueMS = 0;

// RPG
uint8_t u8_RPGvalue = 0;
uint16_t u16_RPGvalueMS = 0;
uint16_t u16_RPGTicks = 0;
uint16_t u16_RPGDirection = 0;
uint16_t u16_RPGCounterd_last = 0;
uint16_t u16_RPGCounterd = 0;
uint16_t u16_RPG_LED = 0;
int16_t u16_RPGCounter = 0;
int16_t u16_lastRPGCounter = 0;

BOOL b_isCW;

#define SW3_TOGGLE_ON   ESOS_USER_FLAG_0

// Checks if SW3 is pressed to decide to show SW1 or SW2
ESOS_USER_TASK(toggleSW3) {
    ESOS_TASK_BEGIN();
    while(TRUE){
        ESOS_TASK_WAIT_TICKS(50);
        SW3_PRESSED ? esos_SetUserFlag(SW3_TOGGLE_ON) : esos_ClearUserFlag(SW3_TOGGLE_ON);
    }
    ESOS_TASK_END();
}

// Called every 500 ms to create heartbeat 
ESOS_USER_TIMER(heartbeat) {
    esos_uiF14_toggleLED3();
}

// Detrmines whether the RPG is moving 
// Clockwise (CW) or Counter Clockwise (CCW)
ESOS_USER_TASK(RPGDirection) {
    ESOS_TASK_BEGIN();
    while (TRUE) {
        if(u16_RPGDirection == 0) {
            if(RPGA == 0 && RPGB == 1) {
                u16_RPGDirection = 1; 
                b_isCW = TRUE;
                u16_RPGCounter++;
            } else if(RPGA == 1 && RPGB == 0) {
                u16_RPGDirection = 3; 
                b_isCW = FALSE;
                u16_RPGCounter--;
            }
        } else if(u16_RPGDirection == 1) {
            if(RPGA == 1 && RPGB == 1) {
                u16_RPGDirection = 2;
                b_isCW = TRUE; 
                u16_RPGCounter++;
            } else if(RPGA == 0 && RPGB == 0) {
                u16_RPGDirection = 0;
                b_isCW = FALSE; 
                u16_RPGCounter--;
            }
        } else if(u16_RPGDirection == 2) {
            if(RPGA == 1 && RPGB == 0) {
                u16_RPGDirection = 3;
                b_isCW = TRUE;
                u16_RPGCounter++;
            } else if(RPGA == 0 && RPGB == 1) {
                u16_RPGDirection = 1;
                b_isCW = FALSE;
                u16_RPGCounter--;
            }
        } else if(u16_RPGDirection == 3) {
            if(RPGA == 0 && RPGB == 0) {
                u16_RPGDirection = 0;
                b_isCW = FALSE;
                u16_RPGCounter++;
            } else if(RPGA == 1 && RPGB == 1) {
                u16_RPGDirection = 2;
                b_isCW = TRUE;
                u16_RPGCounter--;
            }
        }
        ESOS_TASK_YIELD();
        ESOS_TASK_END();
}
}

// UIF14 task to manage user-interface
ESOS_USER_TASK(main_task) {
    ESOS_TASK_BEGIN();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("\n---TASK 3 MENU---\n");
    ESOS_TASK_WAIT_ON_SEND_STRING("Ticks between double-press (ms): ");
    ESOS_TASK_WAIT_ON_SEND_STRING("\nEnter a number less than 10: ");
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_SWvalue);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("\nUser input double-press value: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8(u8_SWvalue);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    u16_SWvalueMS = u8_SWvalue * 10;

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("\n\nInput time between RPG thresholds: ");
    ESOS_TASK_WAIT_ON_SEND_STRING("\nEnter a number less than 10: ");
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_GET_UINT8(u8_RPGvalue);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
    ESOS_TASK_WAIT_ON_SEND_STRING("\nUser input RPG threshold value: ");
    ESOS_TASK_WAIT_ON_SEND_UINT8(u8_RPGvalue);
    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
    u16_RPGvalueMS = u8_RPGvalue * 10;

    while(TRUE) {
        if (!esos_IsUserFlagSet(SW3_TOGGLE_ON)) {
            if(i16_SW1State == 0) {        // Not pressed
                if(SW1_PRESSED) {
                    i16_SW1State = 1;
                }
            } else if(i16_SW1State == 1) { // 1st press
                i16_SW1Timer++;            // Start timer for SW1
                if(i16_SW1Timer > 15) {    // Long enough to be considered a single press
                    i16_SW1State = 3;
                    i16_SW1Timer = 0;
                    ESOS_TASK_WAIT_ON_SEND_STRING("\nSW1 pressed");
                }
                if(SW1_RELEASED) {
                    i16_SW1State = 2;
                }
            } else if(i16_SW1State == 2) { // Wait for second press
                i16_SW1Timer++;            // Continue timer
                if(i16_SW1Timer >= u16_SWvalueMS) {   // Timed out based on user values. Reset timer and state.
                    i16_SW1Timer = 0;
                    i16_SW1State = 0;
                } else {
                    if(SW1_PRESSED) {   // Double pressed
                        i16_SW1State = 4;
                    }
                }
            } else if(i16_SW1State == 3) { // Single press LED control
                    esos_uiF14_turnLED1On();
                    if(SW1_RELEASED) {
                        esos_uiF14_turnLED1Off();
                        i16_SW1State = 0;
                        i16_SW1Timer = 0;
                    }
            } else if(i16_SW1State == 4) { // 2nd press
                if(SW1_RELEASED) {
                    ESOS_TASK_WAIT_ON_SEND_STRING("\nSW1 double-pressed");
                    i16_SW1State = 5;
                }
            } else if(i16_SW1State == 5) { // Double press LED control
                i16_SW1Timer++;
                if(i16_SW1Timer >= 0 && i16_SW1Timer <= 30) { // 1st flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW1Timer >= 50 && i16_SW1Timer <= 75) { // 2nd flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW1Timer >= 95 && i16_SW1Timer <= 120) { // 3rd flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW1Timer > 120) {
                    esos_uiF14_turnLED1Off();
                    i16_SW1Timer = 0;
                    i16_SW1State = 0;
                } else {
                    esos_uiF14_turnLED1Off();
                }
            }
        } else if (esos_IsUserFlagSet(SW3_TOGGLE_ON)){
            if(i16_SW2State == 0) {        // Not presed
                if(SW2_PRESSED) {
                    i16_SW2State = 1;
                }
            } else if(i16_SW2State == 1) { // 1st press
                i16_SW2Timer++;            // Start SW2 timer
                if(i16_SW2Timer > 15) {    // Long enough to be considered a single press
                    i16_SW2State = 3;
                    i16_SW2Timer = 0;
                    ESOS_TASK_WAIT_ON_SEND_STRING("\nSW2 pressed");
                }
                if(SW2_RELEASED) {
                    i16_SW2State = 2;
                }
            } else if(i16_SW2State == 2) { // Wait for 2nd press
                i16_SW2Timer++;            // Continue timer for SW2
                if(i16_SW2Timer >= u16_SWvalueMS) { // Timed out based on user values. Reset timer and state.
                    i16_SW2Timer = 0;
                    i16_SW2State = 0;
                } else {
                    if(SW2_PRESSED) {
                        i16_SW2State = 4;
                    }
                }
            } else if(i16_SW2State == 3) { // Single press LED control
                    esos_uiF14_turnLED1On();
                    if(SW2_RELEASED) {
                        esos_uiF14_turnLED1Off();
                        i16_SW2State = 0;
                        i16_SW2Timer = 0;
                    }
            } else if(i16_SW2State == 4) { // 2nd press
                if(SW2_RELEASED) {
                    ESOS_TASK_WAIT_ON_SEND_STRING("\nSW2 double-pressed");
                    i16_SW2State = 5;
                }
            } else if(i16_SW2State == 5) { // Double press LED control
                i16_SW2Timer++;
                if(i16_SW2Timer >= 0 && i16_SW2Timer <= 30) { // 1st flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW2Timer >= 50 && i16_SW2Timer <= 75) { // 2nd flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW2Timer >= 95 && i16_SW2Timer <= 120) { // 3rd flash
                    esos_uiF14_turnLED1On();
                } else if(i16_SW2Timer > 120) {
                    esos_uiF14_turnLED1Off();
                    i16_SW2Timer = 0;
                    i16_SW2State = 0;
                } else {
                    esos_uiF14_turnLED1Off();
                }
            }
        }
        RPGSpeed();
        controlLEDs();

            // Display RPG State to UI
            if(u16_RPGCounterd != u16_RPGCounterd_last) {
            u16_RPGCounterd_last = u16_RPGCounterd;
            if (b_isCW) {
            if(u16_RPGCounterd == 0) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nNo movement on RPG");
            } else if(u16_RPGCounterd > 0 && u16_RPGCounterd <= 3) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning fast CW");
            } else if(u16_RPGCounterd > 3 && u16_RPGCounterd <= 100) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning medium CW");
            } else if(u16_RPGCounterd > 100) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning slow CW");
            }   
        }
        if (!b_isCW) {
            if(u16_RPGCounterd == 0) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nNo movement on RPG");
            } else if(u16_RPGCounterd > 0 && u16_RPGCounterd <= 3) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning slow CCW ");
            } else if(u16_RPGCounterd > 3 && u16_RPGCounterd <= 100) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning medium CCW");
            } else if(u16_RPGCounterd > 100) {
                ESOS_TASK_WAIT_ON_SEND_STRING("\nRPG turning fast CCW ");
            }   

        }
        }

        ESOS_TASK_YIELD();
        ESOS_TASK_WAIT_TICKS(__ESOS_UIF14_UI_PERIOD_MS);
    }
    ESOS_TASK_END();
}

// Detects the speed of RPG and LEDs act accordingly
inline void RPGSpeed(void) {
        if(u16_RPGTicks > u16_RPGvalueMS) {
            u16_RPGCounterd = u16_RPGCounter - u16_lastRPGCounter;
            u16_lastRPGCounter = u16_RPGCounter;
            u16_RPGTicks = 0;
        }
        u16_RPGTicks++;
        if(u16_RPGCounterd == 0) {
            esos_uiF14_turnLED2Off();
            u16_RPG_LED = 0;
        } else if(u16_RPGCounterd > 0 && u16_RPGCounterd <= 25) {
            if (!b_isCW) {
                esos_uiF14_turnLED2On();
                u16_RPG_LED = 0;
            }
            else if (b_isCW) {
            if(u16_RPG_LED > 15) {
            esos_uiF14_toggleLED2();
            u16_RPG_LED = 0;
        }
            u16_RPG_LED++;
        }
        } else if(u16_RPGCounterd > 25 && u16_RPGCounterd <= 100) {
            if(u16_RPG_LED > 50) {
                esos_uiF14_toggleLED2();
                u16_RPG_LED = 0;
            }
            u16_RPG_LED++;
        } else if(u16_RPGCounterd > 100) {
                esos_uiF14_turnLED2On();
                u16_RPG_LED = 0;
        }
        return;
}

inline void controlLEDs() {
    LED1 = _st_esos_uiF14Data.b_LED1On;
    LED2 = _st_esos_uiF14Data.b_LED2On;
    LED3 = _st_esos_uiF14Data.b_LED3On;
    return;
}

void user_init(void) {

  __esos_unsafe_PutString( HELLO_MSG );

  // Confugure switches
  CONFIG_SW1();
  CONFIG_SW2();
  CONFIG_SW3();

  // Configure LEDs
  CONFIG_LED1();
  CONFIG_LED2();
  CONFIG_LED3();

 // Configure RPG
  CONFIG_RPGA();
  CONFIG_RPGB();

  //esos_RegisterTask(turningSW3);
  //esos_RegisterTask(SW3_Behavior);
  esos_RegisterTask(RPGDirection);
  esos_RegisterTimer(heartbeat,500);
  esos_RegisterTask(main_task);
  esos_RegisterTask(toggleSW3);

} 