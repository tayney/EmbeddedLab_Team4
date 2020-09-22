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

#define true 1
#define false 0

// PRIVATE FUNCTIONS
inline void _esos_uiF14_setRPGCounter (uint16_t newValue) {
    _st_esos_uiF14Data.u16_RPGCounter = newValue;
    return;
}

inline void _esos_uiF14_setLastRPGCounter (uint16_t newValue) {
    _st_esos_uiF14Data.u16_lastRPGCounter = newValue;
    return;
}

// PUBLIC SWITCH FUNCTIONS

// SW1 
inline BOOL esos_uiF14_isSW1Pressed (void) {
    return (_st_esos_uiF14Data.b_SW1Pressed== true);
}

inline BOOL esos_uiF14_isSW1Released (void) {
    return (_st_esos_uiF14Data.b_SW1Pressed== false);
}

inline BOOL esos_uiF14_isSW1DoublePressed (void) {
    return (_st_esos_uiF14Data.b_SW1DoublePressed== true);
}

// SW2
inline BOOL esos_uiF14_isSW2Pressed (void) {
    return (_st_esos_uiF14Data.b_SW2Pressed== true);
}

inline BOOL esos_uiF14_isSW2Released (void) {
    return (_st_esos_uiF14Data.b_SW2Pressed== false);
}

inline BOOL esos_uiF14_isSW2DoublePressed (void) {
    return (_st_esos_uiF14Data.b_SW2DoublePressed== true);
}

// SW3
inline BOOL esos_uiF14_isSW3Pressed (void) {
    return (_st_esos_uiF14Data.b_SW3Pressed== true);
}

inline BOOL esos_uiF14_isSW3Released (void) {
    return (_st_esos_uiF14Data.b_SW3Pressed== false);
}

inline BOOL esos_uiF14_isSW3DoublePressed (void) {
    return (_st_esos_uiF14Data.b_SW3DoublePressed== true);
}

// PUBLIC LED FUNCTIONS
// LED1
inline BOOL esos_uiF14_isLED1On (void) {
    return (_st_esos_uiF14Data.b_LED1On== true);
}

inline BOOL esos_uiF14_isLED1Off (void) {
    return (_st_esos_uiF14Data.b_LED1On== false);
}

inline void esos_uiF14_turnLED1On (void) {
    _st_esos_uiF14Data.b_LED1On =  true;
    return;
}

inline void esos_uiF14_turnLED1Off (void) {
    _st_esos_uiF14Data.b_LED1On =  false;
    return;
}

inline void esos_uiF14_toggleLED1 (void) {
    _st_esos_uiF14Data.b_LED1On ^= 1;
    return;
}

inline void esos_uiF14_flashLED1( uint16_t u16_period) {
    _st_esos_uiF14Data.u16_LED1FlashPeriod = u16_period;
    return;
}

// LED2
inline BOOL esos_uiF14_isLED2On (void) {
    return (_st_esos_uiF14Data.b_LED2On== true);
}

inline BOOL esos_uiF14_isLED2Off (void) {
    return (_st_esos_uiF14Data.b_LED2On== false);
}

inline void esos_uiF14_turnLED2On (void) {
    _st_esos_uiF14Data.b_LED2On =  true;
    return;
}

inline void esos_uiF14_turnLED2Off (void) {
    _st_esos_uiF14Data.b_LED2On =  false;
    return;
}

inline void esos_uiF14_toggleLED2 (void) {
    _st_esos_uiF14Data.b_LED2On ^= 1;
    return;
}

inline void esos_uiF14_flashLED2( uint16_t u16_period) {
    _st_esos_uiF14Data.u16_LED2FlashPeriod = u16_period;
    return;
}

// LED3
inline BOOL esos_uiF14_isLED3On (void) {
    return (_st_esos_uiF14Data.b_LED3On== true);
}

inline BOOL esos_uiF14_isLED3Off (void) {
    return (_st_esos_uiF14Data.b_LED3On== false);
}

inline void esos_uiF14_turnLED3On (void) {
    _st_esos_uiF14Data.b_LED3On =  true;
    return;
}

inline void esos_uiF14_turnLED3Off (void) {
    _st_esos_uiF14Data.b_LED3On =  false;
    return;
}

inline void esos_uiF14_toggleLED3 (void) {
    _st_esos_uiF14Data.b_LED3On ^= 1;
    return;
}

inline void esos_uiF14_flashLED3( uint16_t u16_period) {
    _st_esos_uiF14Data.u16_LED3FlashPeriod = u16_period;
    return;
}

// PUBLIC RPG FUNCTIONS

inline uint16_t esos_uiF14_getRPGValue_u16 ( void ) {
    return _st_esos_uiF14Data.u16_RPGCounter;
}

inline BOOL esos_uiF14_isRPGTurning ( void ) {
    return (esos_uiF14_getRPGVelocity_i16() != 0);
}

inline BOOL esos_uiF14_isRPGTurningSlow( void ) {
    if(esos_uiF14_isRPGTurning()) 
       return (esos_uiF14_getRPGVelocity_i16() > 0 && esos_uiF14_getRPGVelocity_i16() <= 10);
    else 
       return  false;
}

inline BOOL esos_uiF14_isRPGTurningMedium( void ) {
      if(esos_uiF14_isRPGTurning()) 
        return (esos_uiF14_getRPGVelocity_i16() > 10 && esos_uiF14_getRPGVelocity_i16() <= 20);
      else 
        return  false;
}

inline BOOL esos_uiF14_isRPGTurningFast( void ) {
      if(esos_uiF14_isRPGTurning()) 
        return esos_uiF14_getRPGVelocity_i16() > 20;
      else 
        return  false;
}

inline BOOL esos_uiF14_isRPGTurningCW( void ) {
      if(esos_uiF14_isRPGTurning()) 
        return esos_uiF14_getRPGVelocity_i16() < 0;
      else 
        return  false;
}

inline BOOL esos_uiF14_isRPGTurningCCW( void ) {
      if(esos_uiF14_isRPGTurning()) 
        return esos_uiF14_getRPGVelocity_i16() > 0;
      else 
        return  false;
}

int16_t esos_uiF14_getRPGVelocity_i16( void ) {
    return (_st_esos_uiF14Data.u16_RPGCounter - _st_esos_uiF14Data.u16_lastRPGCounter);
}

// UIF14 task to manage user-interface
ESOS_USER_TASK( __esos_uiF14_task ){
  
  ESOS_TASK_BEGIN();
  while( true) {
    // do your UI stuff here
    ESOS_TASK_WAIT_TICKS( __ESOS_UIF14_UI_PERIOD_MS );
  }
  ESOS_TASK_END();
}

// UIF14 INITIALIZATION FUNCTION

void config_esos_uiF14() {
  // setup your UI implementation

  // Confugure LEDs
  CONFIG_LED1();
  CONFIG_LED2();
  CONFIG_LED3();

  //Configure switches
  CONFIG_SW1();
  CONFIG_SW2();
  CONFIG_SW3();

  //Configure RPG
  CONFIG_RPGA();
  CONFIG_RPGB();
  //RPG_TURN_CW();
  //RPG_TURN_CCW();
  //RPG_STOPPED();
  
  esos_RegisterTask( __esos_uiF14_task );
}



