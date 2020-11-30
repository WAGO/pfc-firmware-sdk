/// Copyright (c) 2020 WAGO Kontakttechnik GmbH & Co. KG
///
/// PROPRIETARY RIGHTS of WAGO Kontakttechnik GmbH & Co. KG are involved in
/// the subject matter of this material. All manufacturing, reproduction,
/// use, and sales rights pertaining to this subject matter are governed
/// by the license agreement. The recipient of this software implicitly
/// accepts the terms of the license.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
///
///  \brief    Write data to and read data from files.
///
///  \author   WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

#ifndef SRC_CONFIG_DNSMASQ_IFILE_EDITOR_HPP_
#define SRC_CONFIG_DNSMASQ_IFILE_EDITOR_HPP_

#include <string>
#include "ct_error_handling.h"

class IFileEditor {
 public:

  IFileEditor() = default;
  virtual ~IFileEditor() = default;
  IFileEditor(const IFileEditor&) = default;
  IFileEditor& operator=(const IFileEditor&) = default;
  IFileEditor(IFileEditor&&) = default;
  IFileEditor& operator=(IFileEditor&&) = default;

  virtual eStatusCode Read(const ::std::string &file_path, ::std::string &data) const =0;
  virtual eStatusCode Write(const ::std::string &file_path, const ::std::string &data) const =0;
  virtual eStatusCode Append(const ::std::string &file_path, const ::std::string &data) const =0;

};

#endif // SRC_CONFIG_DNSMASQ_IFILE_EDITOR_HPP_
