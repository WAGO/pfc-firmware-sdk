// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef GKEYVALUEFILE_H
#define GKEYVALUEFILE_H
#include <glib.h>
#include <memory>
#include <boost/optional.hpp>
#include "parameter_storage.h"

class GKeyFileStorage : public ParameterStorage
{
public:
	GKeyFileStorage(const std::string& filepath, const std::string& groupName);
	~GKeyFileStorage() = default;

	void store() override;

    boost::optional<const bool> get_boolean(const std::string& key) const override;
	void set_boolean(const std::string& key, const bool& value) override;
    boost::optional<const int> get_integer(const std::string& key) const override;
	void set_integer(const std::string& key, const int& value) override;
    boost::optional<const double> get_double(const std::string& key) const override;
	void set_double(const std::string& key, const double& value) override;
    boost::optional<const std::string> get_string(const std::string& key) const override;
	void set_string(const std::string& key, const std::string& value) override;
private:
	std::string myFilePath;
	std::string myGroupName;
    std::unique_ptr<GKeyFile, decltype(&g_key_file_free)> myGKeyFile; 

    void load();
};
#endif /*GKEYVALUEFILE_H*/
