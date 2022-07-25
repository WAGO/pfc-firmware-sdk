//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

/*
 * leds.h
 */
#ifndef __LEDS_
#define __LEDS_

#include <sys/types.h>
#include <stdint.h>

#define LED_OFF         0x00
#define LED_GREEN       0x01
#define LED_RED         0x02
#define LED_YELLOW      0x03
#define LED_BLUE        0x04
#define LED_CYAN        0x05
#define LED_MAGENTA     0x06
#define LED_WHITE       0x07

typedef uint8_t tLedColors;

/*
 * Inital LED-Hardware
 */
int     InitLed();

/*
 * Set Led Value
 */
int8_t  SetLed(uint8_t led_nr,uint8_t color);

/*
 * Set LED to OFF
 */
int8_t  DelLed(uint8_t led_nr);

/*
 * Get actual Status of LED
 */
uint8_t GetLed(uint8_t led_n);

/*
 * Get the number of real LEDs
 */
int     GetNoOfLeds(void);

/*
 * Get the determined name of the LED
 * (for scanning by SysFS
 */
int     GetLedBaseName(uint8_t led_nr,char *name_buffer);

#endif
