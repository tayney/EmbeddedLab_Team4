#include "esos.h"
#include "string.h"
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
uint32_t u32_tempValue = 0;
uint8_t u8_mode = 1;
uint8_t u8_samples = 0;
uint16_t u16_data = 0x0000;

static esos_sensor_process_t processingMode = 0x00;

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
			if (SW3_PRESSED) {
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			    ESOS_TASK_WAIT_ON_SEND_STRING("\n---PROCESSING MODE---\n");
    			ESOS_TASK_WAIT_ON_SEND_STRING("1 - One-shot\n2 - Average\n3 - Minimum\n4 - Maximum\n5 - Median");
			    ESOS_TASK_WAIT_ON_SEND_STRING("\nEnter a processing mode: ");
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

			    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			    ESOS_TASK_WAIT_ON_GET_UINT8(u8_mode);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

			    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			    ESOS_TASK_WAIT_ON_SEND_STRING("\n---NUMBER OF SAMPLES---\n");
    			ESOS_TASK_WAIT_ON_SEND_STRING("1 - Two\n2 - Four\n3 - Eight\n4 - Sixteen\n5 - Thirty-two\n6 - Sixty-four");
			    ESOS_TASK_WAIT_ON_SEND_STRING("\nEnter number of samples: ");
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

			    ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
			    ESOS_TASK_WAIT_ON_GET_UINT8(u8_samples);
			    ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM();

			    processingMode = 
			    	// Average
						u8_mode == 2 && u8_samples == 1 ? ESOS_SENSOR_AVG2
			        :   u8_mode == 2 && u8_samples == 2 ? ESOS_SENSOR_AVG4
			        :   u8_mode == 2 && u8_samples == 3 ? ESOS_SENSOR_AVG8
			        :   u8_mode == 2 && u8_samples == 4 ? ESOS_SENSOR_AVG16
			        :   u8_mode == 2 && u8_samples == 5 ? ESOS_SENSOR_AVG32
			        :   u8_mode == 2 && u8_samples == 6 ? ESOS_SENSOR_AVG64
			        // Minimum
			        :   u8_mode == 3 && u8_samples == 1 ? ESOS_SENSOR_MIN2
			        :   u8_mode == 3 && u8_samples == 2 ? ESOS_SENSOR_MIN4
			        :   u8_mode == 3 && u8_samples == 3 ? ESOS_SENSOR_MIN8
			        :   u8_mode == 3 && u8_samples == 4 ? ESOS_SENSOR_MIN16
			        :   u8_mode == 3 && u8_samples == 5 ? ESOS_SENSOR_MIN32
			        :   u8_mode == 3 && u8_samples == 6 ? ESOS_SENSOR_MIN16
			        // Maximum
			        :   u8_mode == 4 && u8_samples == 1 ? ESOS_SENSOR_MAX2
			        :   u8_mode == 4 && u8_samples == 2 ? ESOS_SENSOR_MAX4
			        :   u8_mode == 4 && u8_samples == 3 ? ESOS_SENSOR_MAX8
			        :   u8_mode == 4 && u8_samples == 4 ? ESOS_SENSOR_MAX16
			        :   u8_mode == 4 && u8_samples == 5 ? ESOS_SENSOR_MAX32
			        :   u8_mode == 4 && u8_samples == 6 ? ESOS_SENSOR_MAX16
			        // Median
			        :   u8_mode == 5 && u8_samples == 1 ? ESOS_SENSOR_MEDIAN2
			        :   u8_mode == 5 && u8_samples == 2 ? ESOS_SENSOR_MEDIAN4
			        :   u8_mode == 5 && u8_samples == 3 ? ESOS_SENSOR_MEDIAN8
			        :   u8_mode == 5 && u8_samples == 4 ? ESOS_SENSOR_MEDIAN16
			        :   u8_mode == 5 && u8_samples == 5 ? ESOS_SENSOR_MEDIAN32
			        :   u8_mode == 5 && u8_samples == 6 ? ESOS_SENSOR_MEDIAN16
			        // Default one-shot
			        :   ESOS_SENSOR_ONE_SHOT;
			}
		} else if (SW1_PRESSED || SW2_PRESSED || SW3_PRESSED){
				esos_ClearUserFlag(SW2_TOGGLE_ON);
				b_printPot = 0;
				ESOS_TASK_WAIT_UNTIL(SW1_RELEASED && SW2_RELEASED);
			
		}
		ESOS_TASK_WAIT_TICKS(250);
	}
    ESOS_TASK_END();
}


// Print values to screen
ESOS_USER_TASK(printPot){
	ESOS_TASK_BEGIN();

	//This function is in the sensor files, it takes a channel and voltage 
	ESOS_TASK_WAIT_ON_AVAILABLE_SENSOR(ESOS_SENSOR_CH03, ESOS_SENSOR_VREF_3V3);

	while(TRUE){
		if (b_printPot) {
				ESOS_TASK_WAIT_SENSOR_READ(u16_data, processingMode, 0x80);


			   u32_tempValue = u16_data * 1000;
            	u32_tempValue = (u32_tempValue - 42400) / 625;


				// Send the value to the screen.
				ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM();
				ESOS_TASK_WAIT_ON_SEND_UINT32_AS_HEX_STRING((uint32_t) u16_data);
				ESOS_TASK_WAIT_ON_SEND_STRING("\t");
				ESOS_TASK_WAIT_ON_SEND_UINT8_AS_DEC_STRING((uint8_t) u16_data);
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
    CONFIG_SW3();



    esos_RegisterTask(buttonStates);
    esos_RegisterTask(printPot);
    esos_RegisterTimer(heartbeat, 250);
}
