//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
/// manufacturing, reproduction, use, and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// \file LED_ctl.h
///
/// \version $Revision: 65689 $
///
/// \brief <short description of the file contents>
///
/// \author <author name> : <company name>
//------------------------------------------------------------------------------

#ifndef _LED_CTL_H
#define _LED_CTL_H

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

//------------------------------------------------------------------------------
// Include files
//------------------------------------------------------------------------------
#include <sys/time.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------------
#define LEDCTL_S_ERROR           0xFF                  /* defctive state */
#define LEDCTL_S_DO_NOT_SET      0xC0                  /* spezial number omit LED */

#define LEDCTL_S_STATIC          0x00                  /* gloabl state STATIC */
#define LEDCTL_S_STATIC_OFF      0x00                  /* LED if OFF */
#define LEDCTL_S_STATIC_GRN      0x01                  /* LED gleams GREEN */
#define LEDCTL_S_STATIC_RED      0x02                  /* LED gleams RED */
#define LEDCTL_S_STATIC_YLW      0x03                  /* LED gleams YELLOW */
#define LEDCTL_S_STATIC_BLU      0x04                  /* LED gleams BLUE */
#define LEDCTL_S_STATIC_CYA      0x05                  /* LED gleams CYAN */
#define LEDCTL_S_STATIC_MAG      0x06                  /* LED gleams MAGENTA */
#define LEDCTL_S_STATIC_WHT      0x07                  /* LED gleams WHITE */

#define LEDCTL_DUAL_GRN_OFF      0x00
#define LEDCTL_DUAL_GRN_RED      0x01
#define LEDCTL_DUAL_GRN_YLW      0x02
#define LEDCTL_DUAL_GRN_BLU      0x03
#define LEDCTL_DUAL_GRN_CYA      0x04
#define LEDCTL_DUAL_GRN_MAG      0x05
#define LEDCTL_DUAL_GRN_WHT      0x06

#define LEDCTL_DUAL_RED_OFF      0x07
#define LEDCTL_DUAL_RED_GRN      0x08
#define LEDCTL_DUAL_RED_YLW      0x09
#define LEDCTL_DUAL_RED_BLU      0x0A
#define LEDCTL_DUAL_RED_CYA      0x0B
#define LEDCTL_DUAL_RED_MAG      0x0C
#define LEDCTL_DUAL_RED_WHT      0x0D

#define LEDCTL_DUAL_YLW_OFF      0x0E
#define LEDCTL_DUAL_YLW_GRN      0x0F
#define LEDCTL_DUAL_YLW_RED      0x10
#define LEDCTL_DUAL_YLW_BLU      0x11
#define LEDCTL_DUAL_YLW_CYA      0x12
#define LEDCTL_DUAL_YLW_MAG      0x13
#define LEDCTL_DUAL_YLW_WHT      0x14

#define LEDCTL_DUAL_BLU_OFF      0x15
#define LEDCTL_DUAL_BLU_GRN      0x16
#define LEDCTL_DUAL_BLU_RED      0x17
#define LEDCTL_DUAL_BLU_YLW      0x18
#define LEDCTL_DUAL_BLU_CYA      0x19
#define LEDCTL_DUAL_BLU_MAG      0x1A
#define LEDCTL_DUAL_BLU_WHT      0x1B

#define LEDCTL_DUAL_CYA_OFF      0x1C
#define LEDCTL_DUAL_CYA_GRN      0x1D
#define LEDCTL_DUAL_CYA_RED      0x1E
#define LEDCTL_DUAL_CYA_YLW      0x1F
#define LEDCTL_DUAL_CYA_BLU      0x20
#define LEDCTL_DUAL_CYA_MAG      0x21
#define LEDCTL_DUAL_CYA_WHT      0x22

#define LEDCTL_DUAL_MAG_OFF      0x23
#define LEDCTL_DUAL_MAG_GRN      0x24
#define LEDCTL_DUAL_MAG_RED      0x25
#define LEDCTL_DUAL_MAG_YLW      0x26
#define LEDCTL_DUAL_MAG_BLU      0x27
#define LEDCTL_DUAL_MAG_CYA      0x28
#define LEDCTL_DUAL_MAG_WHT      0x29

#define LEDCTL_DUAL_WHT_OFF      0x2A
#define LEDCTL_DUAL_WHT_GRN      0x2B
#define LEDCTL_DUAL_WHT_RED      0x2C
#define LEDCTL_DUAL_WHT_YLW      0x2D
#define LEDCTL_DUAL_WHT_BLU      0x2E
#define LEDCTL_DUAL_WHT_CYA      0x2F
#define LEDCTL_DUAL_WHT_MAG      0x30

#define LEDCTL_DUAL_OFF_GRN      0x31
#define LEDCTL_DUAL_OFF_RED      0x32
#define LEDCTL_DUAL_OFF_YLW      0x33
#define LEDCTL_DUAL_OFF_BLU      0x34
#define LEDCTL_DUAL_OFF_CYA      0x35
#define LEDCTL_DUAL_OFF_MAG      0x36
#define LEDCTL_DUAL_OFF_WHT      0x37

#if 1
#define LEDCTL_S_BLINK           0x40                  /* global state BLINK */
#define LEDCTL_S_BLINK_GRN       0x40                  /* blink GREEN  */
#define LEDCTL_S_BLINK_GRN_OFF   LEDCTL_S_BLINK_GRN
#define LEDCTL_S_BLINK_RED       0x41                  /* blink RED  */
#define LEDCTL_S_BLINK_RED_OFF   LEDCTL_S_BLINK_RED
#define LEDCTL_S_BLINK_YLW       0x42                  /* blink YELLOW */
#define LEDCTL_S_BLINK_YLW_OFF   LEDCTL_S_BLINK_YLW
#define LEDCTL_S_BLINK_GRN_RED   0x43                  /* blink GREEN/RED */
#define LEDCTL_S_BLINK_GRN_YLW   0x44                  /* blink GREEN/YELLOW  */
#define LEDCTL_S_BLINK_RED_YLW   0x45                  /* blink RED/YELLOW  */
#define LEDCTL_S_BLINK_OFF_GRN   0x46
#define LEDCTL_S_BLINK_OFF_RED   0x47
#define LEDCTL_S_BLINK_OFF_YLW   0x48
#define LEDCTL_S_BLINK_RED_GRN   0x49
#define LEDCTL_S_BLINK_YLW_GRN   0x4A
#define LEDCTL_S_BLINK_YLW_RED   0x4B
#else
#define LEDCTL_S_BLINK           0x40                  /* global state BLINK */
#define LEDCTL_S_BLINK_OFF_GRN   0x40
#define LEDCTL_S_BLINK_OFF_RED   0x41
#define LEDCTL_S_BLINK_OFF_YLW   0x42
#define LEDCTL_S_BLINK_GRN       0x43                  /* blink GREEN  */
#define LEDCTL_S_BLINK_GRN_OFF   LEDCTL_S_BLINK_GRN
#define LEDCTL_S_BLINK_GRN_RED   0x44                  /* blink GREEN/RED */
#define LEDCTL_S_BLINK_GRN_YLW   0x45                  /* blink GREEN/YELLOW  */
#define LEDCTL_S_BLINK_RED       0x46                  /* blink RED  */
#define LEDCTL_S_BLINK_RED_OFF   LEDCTL_S_BLINK_RED
#define LEDCTL_S_BLINK_RED_GRN   0x47
#define LEDCTL_S_BLINK_RED_YLW   0x48                  /* blink RED/YELLOW  */
#define LEDCTL_S_BLINK_YLW       0x49                  /* blink YELLOW */
#define LEDCTL_S_BLINK_YLW_OFF   LEDCTL_S_BLINK_YLW
#define LEDCTL_S_BLINK_YLW_GRN   0x4A
#define LEDCTL_S_BLINK_YLW_RED   0x4B
#endif
        
#define LEDCTL_S_BLINK_SEQ       0x80                  /* global state BLINK_SEQENZ*/
                                                       /* <times>/frequency> */
                                                       /*  <seconds> p (pause) */
#define LEDCTL_S_BLINK_SEQ_RESET 0xBF                  /* Reset all Blink Sequences */
#define LEDCTL_S_BLINK_SEQ_750   0x80                  /* standard: 10/10 1p X/1 1p Y/1  */

#define LEDCTL_S_FLASH           0x81                  /* First Number of Flash-Codes */
#define LEDCTL_S_FLASH_GRN_OFF   0x81                  /* Flash Green an set LED off */
#define LEDCTL_S_FLASH_RED_OFF   0x82                  /* Flash Red an set LED off */
#define LEDCTL_S_FLASH_YLW_OFF   0x83                  /* Flash Yellow an set LED off */
#define LEDCTL_S_FLASH_GRN_RED   0x84                  /* Flash Green an set LED Red */
#define LEDCTL_S_FLASH_GRN_YLW   0x85                  /* Flash Green an set LED Red */
#define LEDCTL_S_FLASH_RED_GRN   0x86                  /* Flash Yellow an set LED Red */
#define LEDCTL_S_FLASH_RED_YLW   0x87                  /* Flash Yellow an set LED Red */
#define LEDCTL_S_FLASH_YLW_GRN   0x88                  /* Flash Yellow an set LED Red */
#define LEDCTL_S_FLASH_YLW_RED   0x89                  /* Flash Yellow an set LED Red */
#define LEDCTL_S_FLASH_OFF_GRN   0x8A                  /* Flash Green an set LED off */
#define LEDCTL_S_FLASH_OFF_RED   0x8B                  /* Flash Red an set LED off */
#define LEDCTL_S_FLASH_OFF_YLW   0x8C                  /* Flash Red an set LED off */

#define LEDCTL_S_BLINK_SPECIAL   0x8D                  /* Special defined Blink Sequences */
#define LEDCTL_S_B_SPEC_CAN      0x0001                /* Special Blinkcodes for CAN var1 */
#define LEDCTL_S_B_SPEC_CAN_PREOP_WARN 0x0000          /* CAN Preopertional + Warning Level var2 */
#define LEDCTL_S_B_SPEC_CAN_PREOP_GERR 0x0001          /* CAN Preopertional + Guard Error   var2 */
#define LEDCTL_S_B_SPEC_CAN_PREOP_SERR 0x0002          /* CAN Preopertional + Sync Error    var2 */
#define LEDCTL_S_B_SPEC_CAN_OP_WARN    0x0003          /* CAN Opertional + Warning Level    var2 */
#define LEDCTL_S_B_SPEC_CAN_OP_GERR    0x0004          /* CAN Opertional + Guard Error      var2 */
#define LEDCTL_S_B_SPEC_CAN_OP_SERR    0x0005          /* CAN Opertional + Sync Error       var2 */





//#define LEDCTL_MAX_NUMBER        2                     /* number of LEDs */

#define LED_DBUS_PATH            "/wago/led/"
#define LED_DBUS_PATH_ALL        "/wago/led"
#define LED_DBUS_NAME            "wago.ledserver"
#define LED_DBUS_GET_STATE       "get"
#define LED_DBUS_GET_STATE_ALL   "getAll"

#define LED_INFO_FILE  (const char*)"/etc/specific/led_info.json"
//------------------------------------------------------------------------------
// Macros
//------------------------------------------------------------------------------
/* check if state is BLINK OR BLINK_SEQUENZ but not both */
#define LEDCTL_PER_XOR_SEQ(x)    (((x << 0x01) ^ (x)) & 0x80)

#define LEDCTL_IS_STATIC(x)      (!(x & 0xC0))

#define LEDCTL_IS_MULTI(x)       ((x & 0x80) && (x & 0x3F))

#define LEDCTL_IS_FLASH(x)       ( LEDCTL_IS_MULTI(x) && ((x & 0x3F) <= 0x0C))
#define LEDCTL_IS_SPECIAL(x)     ( LEDCTL_IS_MULTI(x) && ((x & 0x3F) == 0x0D))

//------------------------------------------------------------------------------
// Typedefs
//------------------------------------------------------------------------------
typedef struct {
    uint32_t  id;
    char    * info;
    uint32_t  placeholder;
}tIdCtrlInfo;

typedef struct {
  char               ledName[16];
  uint8_t            state;
  uint16_t           var1;
  uint16_t           var2;
  tIdCtrlInfo        idInfo;
  struct timeval     timestamp;
}tLedCtlState;


//------------------------------------------------------------------------------
// Global variables
//------------------------------------------------------------------------------

#ifdef __cplusplus
} // extern "C"
#endif


#endif
