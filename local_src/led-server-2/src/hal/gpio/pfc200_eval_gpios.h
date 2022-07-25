//------------------------------------------------------------------------------
/// Copyright (c) WAGO GmbH & Co. KG
///
/// PROPRIETARY RIGHTS are involved in the subject matter of this material.
/// All manufacturing, reproduction, use and sales rights pertaining to this
/// subject matter are governed by the license agreement. The recipient of this
/// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \file     pfc200_gpios.h
///
///  \version  $Revision: 65689 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef PFC200_GPIOS_H_
#define PFC200_GPIOS_H_

//PAC200 EVAL_BOARD CONFIGURTION
#define BOARD_SYS_GRN      6
#define BOARD_RW_GRN       139
#define BOARD_NS_GRN       4
#define BOARD_NS_RED       5
#define BOARD_MS_GRN       2
#define BOARD_MS_RED       3
#define BOARD_RUN_GRN      152
#define BOARD_RUN_RED      153
#define BOARD_ERR_GRN      154
#define BOARD_ERR_RED      155
#define BOARD_TX_GRN       156
#define BOARD_TX_RED       157
#define BOARD_RX_GRN       158
#define BOARD_RX_RED       159

#define GPIO_BF_GRN         BOARD_MS_GRN /*BORAD_RW_GRN*/
#define GPIO_BF_RED         BOARD_MS_RED /*GPIO_INVALID*/
#define GPIO_BF_BLUE        -1
#define GPIO_SYS_GRN        BOARD_SYS_GRN
#define GPIO_SYS_RED        GPIO_INVALID
#define GPIO_SYS_BLUE       -1
#define GPIO_RUN_GRN        BOARD_RUN_GRN
#define GPIO_RUN_RED        BOARD_RUN_RED
#define GPIO_RUN_BLUE       -1
#define GPIO_IO_GRN         BOARD_ERR_GRN
#define GPIO_IO_RED         BOARD_ERR_RED
#define GPIO_IO_BLUE        -1
#define GPIO_CAN_GRN        BOARD_TX_GRN /*GPIO_INVALID*/
#define GPIO_CAN_RED        BOARD_TX_RED /*GPIO_INVALID*/
#define GPIO_CAN_BLUE       -1
#define GPIO_FB1_GRN        GPIO_INVALID /*BOARD_MS_GRN*/
#define GPIO_FB1_RED        GPIO_INVALID /*BOARD_MS_RED */
#define GPIO_FB1_BLUE       -1
#define GPIO_FB2_GRN        GPIO_INVALID /*BOARD_NS_GRN*/
#define GPIO_FB2_RED        GPIO_INVALID /*BOARD_NS_RED*/
#define GPIO_FB2_BLUE       -1
#define GPIO_U3_GRN         GPIO_INVALID
#define GPIO_U3_RED         GPIO_INVALID
#define GPIO_U3_BLUE        -1
#define GPIO_DIA_GRN        BOARD_NS_GRN /*BOARD_RX_GRN*/
#define GPIO_DIA_RED        BOARD_NS_RED /*BOARD_RX_RED*/
#define GPIO_DIA_BLUE       -1
#define GPIO_U4_GRN         GPIO_INVALID
#define GPIO_U4_RED         GPIO_INVALID
#define GPIO_U4_BLUE        -1
#define GPIO_U1_GRN         BOARD_RX_GRN /*BOARD_TX_GRN*/
#define GPIO_U1_RED         BOARD_RX_RED /*BOARD_TX_RED*/
#define GPIO_U1_BLUE        -1
#define GPIO_U2_GRN         BOARD_RW_GRN /*GPIO_INVALID*/
#define GPIO_U2_RED         GPIO_INVALID
#define GPIO_U2_BLUE         -1

#endif /* PFC200_GPIOS_H_ */
//---- End of source file ------------------------------------------------------
