//------------------------------------------------------------------------------
/// Copyright (c) WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     ledserver_API.h
///
///  \version  $Revision: 12608 $
///
///  \brief    This is the API to the LED-Server Library used to set the LEDs of PFC and PAC devices
///
///  \author   HFS : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef LEDSERVER_API_H_
#define LEDSERVER_API_H_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// defines; structure,enumeration and type definitions
//------------------------------------------------------------------------------
typedef enum {
  LED_RETURN_OK = 0,
  LED_RETURN_ERROR,                     //common error
  LED_RETURN_ERROR_NO_NAME_AVAILABLE,   //LED has no name
  LED_RETURN_ERROR_NUMBER,              //given LED number is out of range
  LED_RETURN_ERROR_NAME,                //LED Name is unknown
  LED_RETURN_ERROR_PARAMETER,           //Problem with an Parameter
  LED_RETURN_ERROR_STATECLASS,          //unknown stateclass
  LED_RETURN_ERROR_RESET_LED,           //error resetting LED
  LED_RETURN_ERROR_RESET_LED_USER_DATA, //error resetting user Data
  LED_RETURN_ERROR_SET_LED,             //error setting LED
  LED_RETURN_ERROR_GET_LED,             //error getting LED
  LED_RETURN_ERROR_BUFFERSIZE           //buffer to small
}tLedReturnCode;

typedef enum {
  LED_STATE_OFF,          //internal state LED uninitialized not to be set by app
  LED_STATE_STATIC,       //led static on or off
  LED_STATE_BLINK,        //led blinking in two colors
  LED_STATE_750_ERR,      //errorcode for products of series 750
  LED_STATE_FLASH,        //led performes a flash or has flashed
  LED_STATE_CAN,          //led blinks a Can Code
  LED_STATE_ROCKET_ERR2,  //rocket error code 2
  LED_STATE_ROCKET_ERR3   //rocket error code 3
}tLedStateClass;

//typedef for the led numbering
typedef int tLedNr;

//typedef for the ledhandler funktion-pointer
typedef int(*tLedHandler)(tLedNr, void*,void*);

/*
 * this struct is used for setting the handler for crrating a new blink-sequence
 */
typedef struct{
    tLedHandler setLedHandler;//REQUIRED
    tLedHandler resetLedHandler;//OPTIONAL
    tLedHandler resetUserDataHandler;//OPTIONAL
    tLedHandler getLedHandler;//OPTIONAL
    //no use for this function???
    //int (*cleanupHandler)(void*);//OPTIONAL
}tLedSchedHandler;


//type for static led color
//typedef uint8_t tLedStaticColor;
typedef enum {
  LED_COLOR_OFF     = 0x00,
  LED_COLOR_GREEN   = 0x01,
  LED_COLOR_RED     = 0x02,
  LED_COLOR_YELLOW  = 0x03,
  LED_COLOR_BLUE    = 0x04,
  LED_COLOR_CYAN    = 0x05,
  LED_COLOR_MAGENTA = 0x06,
  LED_COLOR_WHITE   = 0x07
}tLedStaticColor;

//typedef for the static ledHandler
typedef tLedStaticColor tLedStatic;

//type for led dual blinking
typedef struct {
    tLedStaticColor color1;
    tLedStaticColor color2;
    uint16_t        time1;
    uint16_t        time2;
}tLedBlink;

//struct for the flash sequence
typedef struct {
    tLedStaticColor   flashColor;
    uint16_t          flashTime;
    tLedStaticColor   staticColor;
}tLedFlash;

//struct for the error blinkcodes of the 750 series
typedef struct {
    uint16_t          errorCode;
    uint16_t          errorArg;
}tLed750;

//struct for special CAN blinkcodes
typedef struct {
    uint16_t          canCode;
}tLedCan;

//struct for the error blinkcodes of the rocket series
typedef struct {
    uint8_t           errorGroup;
    uint8_t           errorCode;
}tLedRocketErr2;

//struct for the error blinkcodes of the rocket series
typedef struct {
    uint8_t           errorGroup;
    uint8_t           errorCode;
    uint16_t          errorAddress;
}tLedRocketErr3;

//------------------------------------------------------------------------------
// function prototypes
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_Init()
///  Initiate the internal data-structures of the LED System.
///  Must be called before any other library function.
///
///  \return  LED_RETURN_ERROR: Init failed.
///           LED_RETURN_OK:    Init was successful.
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_Init(void);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_Deinit()
///  Deinit the internal data strctures of the LED-System.
//------------------------------------------------------------------------------
void ledserver_LEDCTRL_Deinit(void);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_SetLed()
///  Set a LED into a new State. Handled by the Internal LED-Number
///
///  \param ledNr     Library internal Number of the LED
///  \param status    The new State-Class of the LED
///  \param data      An Instancy of the Datastructure used by the choosen state-class implementation
///  \param userData  (optional) additional user-data or NULL
///
///  \return  LED_RETURN_ERROR_NUMBER:     ledNr is out of range
///           LED_RETURN_ERROR_STATECLASS: unknown stateclass
///           LED_RETURN_ERROR_PARAMETER:  data invalid
///           LED_RETURN_ERROR_SET_LED:    error while setting LED into newState
///           LED_RETURN_OK:               LED was set successful
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_SetLed(tLedNr ledNr, tLedStateClass status, void * data,void * userData);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_SetLedByName()
///  Set a LED into a new State. Handled by the official LED-Name
///
///  \param ledName   Name of the LED
///  \param status    The new State-Class of the LED
///  \param data      An Instancy of the Datastructure used by the choosen state-class implementation
///  \param userData  (optional) additional user-data or NULL
///
///  \return  LED_RETURN_ERROR_NAME:              Unknown name
///           LED_RETURN_ERROR_NO_NAME_AVAILABLE: LEDs have no names
///           LED_RETURN_ERROR_NUMBER:            ledNr is out of range
///           LED_RETURN_ERROR_STATECLASS:        unknown stateclass
///           LED_RETURN_ERROR_PARAMETER:         data or ledName invalid
///           LED_RETURN_ERROR_SET_LED:           error while setting LED into newState
///           LED_RETURN_OK:                      LED was set successful
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_SetLedByName(const char * ledName, tLedStateClass status, void * data,void * userData);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_GetLed()
///  Determine Status of a LED by its internal Number
///
///  \param ledNr    Library internal Number of the LED
///  \param status   buffer for returning the stateclass
///  \param data     buffer for additional data
///  \param dataSize buffersize of data.
///  \param userData pointerbuffer for returning the additional user-Data
///
///  \return  LED_RETURN_ERROR_NUMBER:            ledNr is out of range
///           LED_RETURN_ERROR_PARAMETER:         data or status invalid (NULL)
///           LED_RETURN_ERROR_BUFFERSIZE:        data-buffer to small
///           LED_RETURN_ERROR_STATECLASS:        unknown stateclass
///           LED_RETURN_ERROR_GET_LED:           error while getting LED-Status
///           LED_RETURN_OK:                      LED state successful determined
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_GetLed(tLedNr ledNr, tLedStateClass * status, void * data, size_t dataSize,void ** userData);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_GetLedByName()
///  Determine Status of a LED by its Name
///
///  \param ledName  Name of the LED
///  \param status   buffer for returning the stateclass
///  \param data     buffer for additional data
///  \param dataSize buffersize of data.
///  \param userData pointerbuffer for returning the additional user-Data
///
///  \return  LED_RETURN_ERROR_NAME:              Unknown name
///           LED_RETURN_ERROR_NO_NAME_AVAILABLE: LEDs have no names
///           LED_RETURN_ERROR_NUMBER:            ledNr is out of range
///           LED_RETURN_ERROR_PARAMETER:         data, status or ledName invalid (NULL)
///           LED_RETURN_ERROR_BUFFERSIZE:        data-buffer to small
///           LED_RETURN_ERROR_STATECLASS:        unknown stateclass
///           LED_RETURN_ERROR_GET_LED:           error while getting LED-Status
///           LED_RETURN_OK:                      LED state successful determined
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_GetLedByName(const char * ledName, tLedStateClass * status, void * data, size_t dataSize,void ** userData);

/*
 * get the name/number of the LED determined by the driver
 */
//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_GetLedName()
///  Determine Name of LED by its Number.
///  returnes the name of the LED into the ledNameBuffer. If buffer is to small the name will be written partially
///
///  \param ledNr             Library internal Number of the LED
///  \param ledNameBuffer     Buffer for returning the LED-Name
///  \param ledNameBufferSize size of the LED-Name Buffer
///
///  \return  LED_RETURN_ERROR_PARAMETER:         ledNameBuffer invalid (NULL)
///           LED_RETURN_ERROR_NUMBER:            ledNr is out of range
///           LED_RETURN_OK:                      LED name successful determined
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_GetLedName(tLedNr ledNr, char * ledNameBuffer,size_t ledNameBufferSize);

//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_GetLedNumber()
///  Determine Library internal LED-Number
///
///  \param ledName LED-Name
///  \param ledNr   buffer for returing the LED-Number
///
///  \return  LED_RETURN_ERROR_NAME:              Unknown name
///           LED_RETURN_ERROR_PARAMETER:         ledNr invalid (NULL)
///           LED_RETURN_ERROR_NO_NAME_AVAILABLE: LEDs have no names
///           LED_RETURN_OK:                      LED-Number succesful determined
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_GetLedNumber(const char * ledName,tLedNr * ledNr);

//get the number of available leds
//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_GetLedCount()
///  Get the Number of available LEDs
///
///
///  \return  Number of LEDs (MAX LED-Number+1)
//------------------------------------------------------------------------------
tLedNr ledserver_LEDCTRL_GetLedCount(void);



/*
 * set Handler for reset the additional UserData for a led.
 */
//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_SetResetUserDataHandler()
///  Set a Handler which is called when a state will be resettet.
///  This happens always when the LED is set into a new State and while Deinit Library
///
///  \param status               stateclass to be connected to the handler
///  \param resetUserDataHandler functionpointer to the handler
///
///  \return  LED_RETURN_ERROR_STATECLASS:        unknown stateclass
///           LED_RETURN_OK:                      LED-Reset-Handler was setted successful
//------------------------------------------------------------------------------
tLedReturnCode ledserver_LEDCTRL_SetResetUserDataHandler(tLedStateClass status,tLedHandler resetUserDataHandler);

//this function to be used only from a ledHandler
//------------------------------------------------------------------------------
///  ledserver_LEDCTRL_SetLedStateData()
///  This function is to be used only from a ledHandler.
///  When the LED is set into a new State the status-data must be copied into the led-Object.
///  This will be performed with this function
///
///  \param ledNr    Library internal Number of the LED
///  \param data     buffer for the data to be copied
///  \param dataSize size of the Data-Buffer
//------------------------------------------------------------------------------
void ledserver_LEDCTRL_SetLedStateData(tLedNr ledNr,void * data,size_t dataSize);

#endif /* LEDSERVER_API_H_ */
//---- End of source file ------------------------------------------------------
