// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef MDMPARAMETERSTORAGE_H
#define MDMPARAMETERSTORAGE_H

class ParameterStorage;
class ModemManagementConfig;
class SimAutoActivation;
class NetworkAccessConfig;
class GprsAccessConfig;
class SmsEventReportingConfig;
class SmsStorageConfig;

class MdmParameterStorage
{
private:
    ParameterStorage& _storage;

public:
    MdmParameterStorage(ParameterStorage& storage) : _storage(storage) {}
    ~MdmParameterStorage() = default;

    ModemManagementConfig get_modem_management_config() const;
    void set_modem_management_config (const ModemManagementConfig &new_config);

    SimAutoActivation get_sim_autoactivation() const;
    void set_sim_autoactivation(const SimAutoActivation &new_config);

    NetworkAccessConfig get_network_access_config() const;
    void set_network_access_config(const NetworkAccessConfig &new_config);

    GprsAccessConfig get_gprs_access_config() const;
    void set_gprs_access_config(const GprsAccessConfig &new_config);

    SmsEventReportingConfig get_sms_event_reporting_config() const;
    void set_sms_event_reporting_config(const SmsEventReportingConfig &new_config);

    SmsStorageConfig get_sms_storage_config() const;
    void set_sms_storage_config(const SmsStorageConfig &new_config);

    MessageServiceConfig get_message_service_config() const;

};

#endif /*MDMPARAMETERSTORAGE_H*/
