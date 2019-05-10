// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#ifndef MDMPARAMETERSTORAGE_H
#define MDMPARAMETERSTORAGE_H

class ParameterStorage;
class ModemManagementConfig;
class UserIdentityConfig;
class NetworkAccessConfig;
class GprsAccessConfig;
class SmsEventReportingConfig;
class SmsStorageConfig;

class MdmParameterStorage
{
private:
    ParameterStorage& _storage;

public:
    MdmParameterStorage(ParameterStorage& storageInterface) : _storage(storageInterface) {}
    ~MdmParameterStorage() = default;

    ModemManagementConfig getModemManagementConfig();
    void setModemManagementConfig (const ModemManagementConfig &newConfig);

    UserIdentityConfig getUserIdentityConfig();
    void setUserIdentityConfig(const UserIdentityConfig &newConfig);

    NetworkAccessConfig getNetworkAccessConfig();
    void setNetworkAccessConfig(const NetworkAccessConfig &newConfig);

    GprsAccessConfig getGprsAccessConfig();
    void setGprsAccessConfig(const GprsAccessConfig &newConfig);

    SmsEventReportingConfig getSmsEventReportingConfig();
    void setSmsEventReportingConfig(const SmsEventReportingConfig &newConfig);

    SmsStorageConfig getSmsStorageConfig();
    void setSmsStorageConfig(const SmsStorageConfig &newConfig);
};

#endif /*MDMPARAMETERSTORAGE_H*/
