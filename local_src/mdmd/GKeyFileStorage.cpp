// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

#include "GKeyFileStorage.h"
#include <unistd.h>  //sync()
#include <glib.h>
#include <boost/optional.hpp>
#include "gerror_exception.h"

//TODO(DD): change boost::optional to std::optional when c++17 is available

GKeyFileStorage::GKeyFileStorage(const std::string& filepath, const std::string& groupName) : myGKeyFile(g_key_file_new(), g_key_file_free)
{
    myGroupName = groupName;
    myFilePath = filepath;
    load();
}

void GKeyFileStorage::load()
{
    GError *gError = nullptr;
    if (g_key_file_load_from_file( myGKeyFile.get(), myFilePath.c_str(), G_KEY_FILE_KEEP_COMMENTS, &gError) == FALSE)
    {
        throw GErrorException(gError);
    }
}

void GKeyFileStorage::store()
{
    GError *gError = nullptr;
    if (g_key_file_save_to_file(myGKeyFile.get(), myFilePath.c_str(), &gError) == TRUE)
    {
        sync(); //writes any data buffered in memory out to disk
    } else {
        throw GErrorException(gError);
    }
}

static bool handle_error(GError* gError)
{
    if(gError != nullptr)
    {   
        // This could be solved by a unique_ptr, if g_key_file_get_xxx functions would
        // not require an address to GError pointer
       g_error_free(gError);
       return false;
    }
    return true;
}

boost::optional<const bool> GKeyFileStorage::get_boolean(const std::string& key) const
{
    GError *gError = nullptr;
    const gboolean valueFromStorage = g_key_file_get_boolean(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), &gError);
    return boost::optional<const bool>{handle_error(gError),
                                      (valueFromStorage == TRUE) ? true : false};
}

void GKeyFileStorage::set_boolean(const std::string& key, const bool& value)
{
    g_key_file_set_boolean(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), ((value == true) ? TRUE : FALSE));
}

boost::optional<const int> GKeyFileStorage::get_integer(const std::string& key) const
{
    GError *gError = nullptr;
    const gint valueFromStorage = g_key_file_get_integer(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), &gError);
    return boost::optional<const int>{handle_error(gError),
                                      static_cast<const int>(valueFromStorage)};
}

void GKeyFileStorage::set_integer(const std::string& key, const int& value)
{
	g_key_file_set_integer(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), value);
}

boost::optional<const double> GKeyFileStorage::get_double(const std::string& key) const
{
    GError *gError = nullptr;
    const gdouble valueFromStorage = g_key_file_get_double(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), &gError);
    return boost::optional<const double>{handle_error(gError),
                                        static_cast<const double>(valueFromStorage)};
}

void GKeyFileStorage::set_double(const std::string& key, const double& value)
{
	g_key_file_set_double(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), value);
}

boost::optional<const std::string> GKeyFileStorage::get_string(const std::string& key) const
{
    GError *gError = nullptr;
    std::unique_ptr<gchar, decltype(&g_free)> valueFromStorage(g_key_file_get_string(myGKeyFile.get(),
                                                                                     myGroupName.c_str(),
                                                                                     key.c_str(),
                                                                                     &gError),
                                                               g_free);

    return boost::optional<const std::string>{handle_error(gError),
                                              valueFromStorage ? valueFromStorage.get() : std::string()};
}

void GKeyFileStorage::set_string(const std::string& key, const std::string& value)
{
	g_key_file_set_string(myGKeyFile.get(), myGroupName.c_str(), key.c_str(), value.c_str());
}
