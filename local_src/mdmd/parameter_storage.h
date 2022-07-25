// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG

#ifndef PARAMETERSTORAGE_H
#define PARAMETERSTORAGE_H
#include <string>
#include <boost/optional.hpp>

class ParameterStorage  //Interface class for parameter storage
{
public:
    virtual void store() = 0;

    virtual boost::optional<const bool> get_boolean(const std::string& key) const = 0;
    virtual void set_boolean(const std::string& key, const bool& value) = 0;
    virtual boost::optional<const int> get_integer(const std::string& key) const = 0;
    virtual void set_integer(const std::string& key, const int& value) = 0;
    virtual boost::optional<const double> get_double(const std::string& key) const = 0;
    virtual void set_double(const std::string& key, const double& value) = 0;
    virtual boost::optional<const std::string> get_string(const std::string& key) const = 0;
    virtual void set_string(const std::string& key, const std::string& value) = 0;

    virtual ~ParameterStorage() = default; 
};
#endif /*PARAMETERSTORAGE_H*/
