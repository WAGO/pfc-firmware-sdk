// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#include "mdm_diagnostic.h"
extern "C"
{
  #include <diagnostic/diagnostic_API.h>
} // extern "C"
#include <array>
#include <cassert>
#include <diagnostic/mdmd_diag.h>
#include <stdint.h>

MdmDiagnostic::MdmDiagnostic() : _errorState(MdmErrorState::NONE)
                                ,_accessClass(MdmAccessClass::NONE)
                                ,_sigLevel(MdmSignalQualityLevel::NONE)
{
  log_EVENT_Init("mdmd");
  int32_t signalLevel = 0;
  log_EVENT_LogIdParam(DIAG_3GMM_OPER_SIGNAL_CHANGE, true, LOG_TYPE_INT32, &signalLevel,
                       LOG_TYPE_INVALID);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_6_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_5_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_4_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_3_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_2_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_SIGNAL_1_OFF , true);
  log_EVENT_LogId(DIAG_3GMM_OPER_NO_NET, true);
}

void MdmDiagnostic::update_net_led(const MdmAccessClass accessClass, const MdmSignalQualityLevel sigLevel) const
{
  if (sigLevel == MdmSignalQualityLevel::NONE)
  {
    log_EVENT_LogId(DIAG_3GMM_OPER_NO_NET, true);
  }
  else
  {
    switch (accessClass)
    {
      default:
        //no break;
      case MdmAccessClass::NONE:
        log_EVENT_LogId(DIAG_3GMM_OPER_NO_NET, true);
        break;
      case MdmAccessClass::GSM:
        log_EVENT_LogId(DIAG_3GMM_OPER_NET_2G, true);
        break;
      case MdmAccessClass::UMTS:
        log_EVENT_LogId(DIAG_3GMM_OPER_NET_3G, true);
        break;
      case MdmAccessClass::LTE:
        //to be defined: log_EVENT_LogId(DIAG_3GMM_OPER_NET_4G, true);
        log_EVENT_LogId(DIAG_3GMM_OPER_NET_3G, true);
        break;
    }
  }
}

static log_tEventId get_led_on_id(std::size_t led_index) {
    assert(led_index <= 5);
    switch(led_index) {
        case 0: return DIAG_3GMM_OPER_SIGNAL_1_ON;
        case 1: return DIAG_3GMM_OPER_SIGNAL_2_ON;
        case 2: return DIAG_3GMM_OPER_SIGNAL_3_ON;
        case 3: return DIAG_3GMM_OPER_SIGNAL_4_ON;
        case 4: return DIAG_3GMM_OPER_SIGNAL_5_ON;
        case 5: return DIAG_3GMM_OPER_SIGNAL_6_ON;
        default: return 0;
    }
    return 0;
}

static log_tEventId get_led_off_id(std::size_t led_index) {
    assert(led_index <= 5);
    switch(led_index) {
        case 0: return DIAG_3GMM_OPER_SIGNAL_1_OFF;
        case 1: return DIAG_3GMM_OPER_SIGNAL_2_OFF;
        case 2: return DIAG_3GMM_OPER_SIGNAL_3_OFF;
        case 3: return DIAG_3GMM_OPER_SIGNAL_4_OFF;
        case 4: return DIAG_3GMM_OPER_SIGNAL_5_OFF;
        case 5: return DIAG_3GMM_OPER_SIGNAL_6_OFF;
        default: return 0;
    }
    return 0;
}

static void turn_signal_leds_on(std::size_t signalLevelOldIndex, std::size_t signalLevelNewIndex) {
    for(std::size_t i = signalLevelOldIndex; i < signalLevelNewIndex; ++i){
        log_EVENT_LogId(get_led_on_id(i), true);
    }
}

static void turn_signal_leds_off(std::size_t signalLevelOldIndex, std::size_t signalLevelNewIndex) {
    for(std::size_t i = signalLevelOldIndex; signalLevelNewIndex < i; --i){
        log_EVENT_LogId(get_led_off_id(i - 1), true);
    }
}

void MdmDiagnostic::update_signal_quality_leds(const MdmSignalQualityLevel sigLevelOld, const MdmSignalQualityLevel sigLevelNew) const
{
    const auto signalLevelOldIndex = static_cast<std::size_t>(sigLevelOld); 
    const auto signalLevelNewIndex = static_cast<std::size_t>(sigLevelNew);

    if(signalLevelOldIndex != signalLevelNewIndex) {
      int32_t signalLevel = static_cast<int32_t>(sigLevelNew);
      log_EVENT_LogIdParam(DIAG_3GMM_OPER_SIGNAL_CHANGE, true, LOG_TYPE_INT32, &signalLevel,
                           LOG_TYPE_INVALID);
    }

    if(signalLevelOldIndex < signalLevelNewIndex) {
        turn_signal_leds_on(signalLevelOldIndex, signalLevelNewIndex);
    } else if(signalLevelOldIndex > signalLevelNewIndex) {
        turn_signal_leds_off(signalLevelOldIndex, signalLevelNewIndex);
    } else {
        //do nothing 
    }
}

bool MdmDiagnostic::set_error_state(const MdmErrorState newErrorState)
{
    bool state_changed = false;
    if (_errorState != newErrorState)
    {
      switch (newErrorState)
      {
        default:
          //no break;
        case MdmErrorState::NONE:
          //update NET LED when error code is not set anymore
          update_net_led(_accessClass, _sigLevel);
          break;
        case MdmErrorState::SIM_REMOVED:
          log_EVENT_LogId(DIAG_3GMM_ERR_NOSIM, true);
          break;
        case MdmErrorState::SIM_INVALID:
          log_EVENT_LogId(DIAG_3GMM_ERR_SIM_INVALID, true);
          break;
        case MdmErrorState::SIM_PIN_NEEDED:
          log_EVENT_LogId(DIAG_3GMM_ERR_SIM_PIN_NEEDED, true);
          break;
        case MdmErrorState::SIM_PUK_NEEDED:
          log_EVENT_LogId(DIAG_3GMM_ERR_SIM_PUK_NEEDED, true);
          break;
        case MdmErrorState::SIM_NOT_READY:
          log_EVENT_LogId(DIAG_3GMM_ERR_SIM_NOT_READY, true);
          break;
        case MdmErrorState::PORT_NOT_READY:
          log_EVENT_LogId(DIAG_3GMM_ERR_PORT_NOT_READY, true);
          break;
        case MdmErrorState::INIT_FAILED:
          log_EVENT_LogId(DIAG_3GMM_ERR_INIT_FAIL, true);
          break;
        case MdmErrorState::RESET_FAILED:
          log_EVENT_LogId(DIAG_3GMM_ERR_RESET_FAIL, true);
          break;
        case MdmErrorState::NET_NO_SERVICE:
          log_EVENT_LogId(DIAG_3GMM_OPER_NO_SERVICE, true);
          break;
      }
      _errorState = newErrorState;
      state_changed = true;
    }
    return state_changed;
}

void MdmDiagnostic::set_access_class(const MdmAccessClass newAccessClass)
{
  if (_accessClass == newAccessClass)
  {
      return;
  }

  if (_sigLevel != MdmSignalQualityLevel::NONE)
  {
    if (newAccessClass == MdmAccessClass::NONE)
    { //switch off SIGNAL QUALITY LEDs when no network accessClass type is set now
      update_signal_quality_leds(_sigLevel, MdmSignalQualityLevel::NONE);
    }

    if (_errorState == MdmErrorState::NONE)
    {
      //update NET LED when error code is not set
      update_net_led(newAccessClass, _sigLevel);
    }
    else
    { //do nothing, NET LED shows prior ERROR blink code or zero signal quality overrides network state
    }

    if (_accessClass == MdmAccessClass::NONE)
    { //switch on SIGNAL QUALITY LEDs when no network accessClass type was set before
      update_signal_quality_leds(MdmSignalQualityLevel::NONE, _sigLevel);
    }
  }
  else
  { //do nothing, zero signal quality overrides any network state
  }
  _accessClass = newAccessClass;
}

void MdmDiagnostic::set_signal_quality_level(const MdmSignalQualityLevel newSigLevel)
{
  if (_sigLevel == newSigLevel)
  {
      return;
  }

  if (_accessClass != MdmAccessClass::NONE)
  {
    if ((_sigLevel == MdmSignalQualityLevel::NONE) && (_errorState == MdmErrorState::NONE))
    {
      //update NET LED when signal quality changes from zero to any other value and error code is not set
      update_net_led(_accessClass, newSigLevel);
    }

    update_signal_quality_leds(_sigLevel, newSigLevel);

    if ((newSigLevel == MdmSignalQualityLevel::NONE) && (_errorState == MdmErrorState::NONE))
    {
      //update NET LED when signal quality changes to zero to any other value and error code is not set
      update_net_led(_accessClass, newSigLevel);
    }
  }
  else
  { //do nothing, SIGNAL QUALITY LEDs should be off when no network accessClass type is set
  }
  _sigLevel = newSigLevel;
}

//---- End of source file ------------------------------------------------------

