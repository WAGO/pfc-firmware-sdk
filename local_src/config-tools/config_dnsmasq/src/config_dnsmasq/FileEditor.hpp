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

#ifndef SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_
#define SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_

#include "IFileEditor.hpp"

class FileEditor : public IFileEditor {
 public:
  FileEditor() = default;
  ~FileEditor() override = default;

  FileEditor(const FileEditor&) = delete;
  FileEditor& operator=(const FileEditor&) = delete;
  FileEditor(const FileEditor&&) = delete;
  FileEditor& operator=(const FileEditor&&) = delete;

  eStatusCode Read(const ::std::string& file_path, ::std::string& data) const override;
  eStatusCode Write(const ::std::string& file_path, const ::std::string& data) const override;
  eStatusCode Append(const ::std::string& file_path, const ::std::string& data) const override;

};

#endif // SRC_CONFIG_DNSMASQ_FILE_EDITOR_HPP_
