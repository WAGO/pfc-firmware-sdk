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
///  \file     sequence_handler.h
///
///  \version  $Revision: 12545 $
///
///  \brief    <Insert description here>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef SEQUENCE_HANDLER_H_
#define SEQUENCE_HANDLER_H_

#include <ledserver_API.h>

tLedReturnCode SEQHANDLER_GetSequenceHandler(tLedStateClass status, tLedSchedHandler ** handler);

#endif /* SEQUENCE_HANDLER_H_ */
//---- End of source file ------------------------------------------------------
