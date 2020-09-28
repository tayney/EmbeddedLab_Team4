/*
 * "Copyright (c) 2008 Robert B. Reese, Bryan A. Jones, J. W. Bruce ("AUTHORS")"
 * All rights reserved.
 * (R. Reese, reese_AT_ece.msstate.edu, Mississippi State University)
 * (B. A. Jones, bjones_AT_ece.msstate.edu, Mississippi State University)
 * (J. W. Bruce, jwbruce_AT_ece.msstate.edu, Mississippi State University)
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice, the following
 * two paragraphs and the authors appear in all copies of this software.
 *
 * IN NO EVENT SHALL THE "AUTHORS" BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE "AUTHORS"
 * HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE "AUTHORS" SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE "AUTHORS" HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."
 *
 * Please maintain this header in its entirety when copying/modifying
 * these files.
 *
 *
 */

/**
 * \addtogroup ESOS_Task_Sensor_Service
 * @{
 */

/*** I N C L U D E S *************************************************/
#include  "esos_pic24_sensor.h"
#include  "esos_sensor.h"
#include  "pic24_adc.h"
#include  "esos.h"
#include  "esos_pic24.h"
#include  "esos_pic24_rs232.h"
#include  "esos_pic24_spi.h"
#include  "pic24_adc.h"
#include  <stdio.h>
#include  <string.h>
#include  "revF14.h"
#include  "pic24_chip.h"
#include  "pic24_util.h"
#include  "pic24_all.h"

uint8_t u8_wdtstate;

/*** T H E   C O D E *************************************************/

/*********************************************************
 * Public functions intended to be called by other files *
 *********************************************************/

/** \file
 *  Sensor support functions. \see hwxxx_sensor.h for details.
 */

/**
Configure and enable the sensor module for hwxxx hardware.
\param e_senCh   specifies sensor channel
\param e_senVRef specifies sensor voltage reference
\hideinitializer
 */
void esos_sensor_config_hw (esos_sensor_ch_t e_senCh, esos_sensor_vref_t e_senVRef)
{
	//Configuring the ADC
    // Configure the internal ADC
	AD1CON1 = ADC_CLK_AUTO | ADC_AUTO_SAMPLING_OFF;

	//Sample-and-hold amplifier,
	//Start sampling input
    AD1CON1bits.SAMP = 1;

	//Configure channel
    AD1CHS0 = e_senCh;
    AD1CON3 = ADC_CONV_CLK_INTERNAL_RC | (31<<8);

	//Configure A/D Voltage reference
    AD1CON2 = ADC_VREF_AVDD_AVSS;

	//Turn it on
    AD1CON1bits.ADON = 1;
}

/**
Determine truth of: the sensor is currently converting
\hideinitializer
 */
BOOL esos_sensor_is_converting_hw (void)
{
    return (AD1CON1bits.SAMP == 0);
}

/**
Initiate a conversion for a configured sensor
\hideinitializer
 */
void esos_sensor_initiate_conversion_hw (void)
{
    //Start Sampling
    AD1CON1bits.SAMP = 1;
    DELAY_MS(50);

    //Watchdog
    u8_wdtstate = _SWDTEN;
    _SWDTEN = 1;

    //Stop sampling and do convert
    AD1CON1bits.SAMP = 0;
    while(AD1CON1bits.DONE != 1);
    _SWDTEN = u8_wdtstate;

    //Do sampling again
    AD1CON1bits.SAMP = 1;
    AD1CON1bits.DONE = 0;
}

/**
Receive the value from a conversion that has already been initiated
\hideinitializer
 */
uint16_t esos_sensor_getvalue_u16_hw (void)
{
    return ADC1BUF0;
}

/**
Release any pending conversions for the sensor
\hideinitializer
 */
void esos_sensor_release_hw (void)
{
    AD1CON1bits.ADON = 0;
}
