//------------------------------------------------------------------------------
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

#include "FileEditor.hpp"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

eStatusCode FileEditor::Read(const ::std::string& file_path, ::std::string& data) const {

  eStatusCode status = SUCCESS;
  ::std::ifstream stream(file_path);

  if (stream.good()) {
    data = ::std::string((std::istreambuf_iterator<char>(stream)),
                         std::istreambuf_iterator<char>());
    stream.close();

  } else {
    status = FILE_READ_ERROR;
  }

  return status;

}

eStatusCode FileEditor::Write(const ::std::string& file_path,
                         const ::std::string& data) const {
  eStatusCode status = SUCCESS;

  ::std::string file_path_tmp = file_path + ".tmp";

  umask(0022);

  ::std::ofstream stream(file_path_tmp);

  if (stream.good()) {

    stream << data;
    stream.flush();
    stream.close();

    sync();
    ::std::rename(file_path_tmp.c_str(), file_path.c_str());
    sync();

  } else {
    status = FILE_WRITE_ERROR;
  }

  return status;
}

eStatusCode FileEditor::Append(const ::std::string& file_path,
                          const ::std::string& data) const {
  eStatusCode status = SUCCESS;

  ::std::ifstream in_stream(file_path);
  if (not in_stream.good()) {
    status = FILE_READ_ERROR;
    return status;
  }
  in_stream.close();

  ::std::ofstream stream;
  stream.open(file_path, ::std::ios_base::app);

  if (stream.good()) {

    stream << data;
    stream.flush();
    stream.close();

    sync();

  } else {
    status = FILE_WRITE_ERROR;
  }

  return status;
}
