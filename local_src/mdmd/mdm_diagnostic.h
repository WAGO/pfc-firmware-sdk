// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef MDMDIAGNOSTIC_H
#define MDMDIAGNOSTIC_H


//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------
enum class MdmErrorState { NONE = 0, SIM_REMOVED = 1, SIM_FAILURE = 2, PORT_NOT_READY = 3, INIT_FAILED = 4, RESET_FAILED = 5 };
enum class MdmAccessClass { NONE = 0, GSM = 2, UMTS = 3, LTE = 4 };
enum class MdmSignalQualityLevel { NONE = 0, MARGINAL = 1, BAD = 2, SUFFICIENT = 3, STABLE = 4, GOOD = 5, EXCELLENT = 6 };

class MdmDiagnostic
{
  private:
    MdmErrorState _errorState;
    MdmAccessClass _accessClass;
    MdmSignalQualityLevel _sigLevel;

    void update_net_led(const MdmAccessClass accessClass, const MdmSignalQualityLevel sigLevel) const;
    void update_signal_quality_leds(const MdmSignalQualityLevel sigLevelCurrent, const MdmSignalQualityLevel sigLevelTarget) const;

  public:
    MdmDiagnostic();
    ~MdmDiagnostic() = default;

    MdmErrorState get_error_state() const;
    MdmAccessClass get_access_class() const;
    MdmSignalQualityLevel get_signal_quality_level() const;

    void set_error_state(const MdmErrorState newErrorState);
    void set_access_class(const MdmAccessClass newAccessClass);
    void set_signal_quality_level(const MdmSignalQualityLevel newSigLevel);
};

#endif /* MDMDIAGNOSTIC_H */

