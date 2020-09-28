#include "esos.h"
#include "esos_pic24.h"
#include "esos_pic24_rs232.h"
#include "esos_sensor.h"
#include "esos_sensor.c"
#include "esos_pic24_sensor.c"
#include "esos_pic24_sensor.h"
#include <stdio.h>
#include "revF14.h"
#include "pic24_all.h"

// Flag to determine is SW2 is pressed
#define SW2_TOGGLE_ON	ESOS_USER_FLAG_0

BOOL b_printPot = 0;
uint16_t u16_potValue = 0x0000;

// Called every 250 ms to create heartbeat 
ESOS_USER_TIMER(heartbeat) {
    LED3 = !LED3;
}

// Determine the states on Switch 1 and 2 and set b_printPot
// Print everytime SW1 is pressed. If SW2 is pressed, print every second until either
// SW1 or SW2 is pressed.
ESOS_USER_TASK(buttonStates){
	ESOS_TASK_BEGIN();
	while(TRUE){
		if (!esos_IsUserFlagSet(SW2_TOGGLE_ON)) {
			if(SW1_PRESSED){
				b_printPot = 1;
			} else b_printPot = 0;
		
		    if (SW2_PRESSED) {
				 b_printPot = 1;
				 esos_SetUserFlag(SW2_TOGGLE_ON);
			}
		} else if (SW1_PRESSED || SW2_PRESSED){
				esos_ClearUserFlag(SW2_TOGGLE_ON);
				b_printPot = 0;
				ESOS_TASK_WAIT_UNTIL(SW1_RELEASED && SW2_RELEASED);
			
		}
		ESOS_TASK_WAIT_TICKS(250);
	}
    ESOS_TASK_END();
}

ESOS_USER_TASK(printPot){
	ESOS_TASK_BEGIN();

	//This function is in the sensor files, it takes a channel and voltage 
	ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH02, ESOS_SENSOR_VREF_3V3);

	while(TRUE){
		if (b_printPot) {
				ESOS_TASK_WAIT_SENSOR_QUICK_READ(u16_potValue);
				// Send the value to the screen.
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t) u16_potValue);
				ESOS_TASK_WAIT_ON_SEND_STRING("\n");
				ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();
			}
			ESOS_TASK_WAIT_TICKS(1000);
		}
	ESOS_SENSOR_CLOSE();
	ESOS_TASK_END();
}

void user_init(void){
	__esos_unsafe_PutString(HELLO_MSG);

	esos_ClearUserFlag(SW2_TOGGLE_ON);

	CONFIG_LED3();

    CONFIG_SW1();
    CONFIG_SW2();

    esos_RegisterTask(buttonStates);
    esos_RegisterTask(printPot);
    esos_RegisterTimer(heartbeat, 250);
}
