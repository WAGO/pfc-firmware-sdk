// SPDX-License-Identifier: GPL-2.0-or-later

#include "FileEditor.hpp"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

namespace netconfd {

Status FileEditor::Read(const ::std::string& file_path, ::std::string& data) const {

  Status status(StatusCode::OK);
  ::std::ifstream stream(file_path);

  if (stream.good()) {
    data = ::std::string((std::istreambuf_iterator<char>(stream)),
                         std::istreambuf_iterator<char>());
    stream.close();

  } else {
    status.Prepend(StatusCode::FILE_READ_ERROR, "Read file " + file_path + " error");
  }

  return status;

}

Status FileEditor::Write(const ::std::string& file_path,
                         const ::std::string& data) const {
  Status status(StatusCode::OK);

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
    status.Prepend(StatusCode::FILE_WRITE_ERROR, "Write file " + file_path + " error");
  }

  return status;
}

Status FileEditor::Append(const ::std::string& file_path,
                          const ::std::string& data) const {
  Status status(StatusCode::OK);

  ::std::ifstream in_stream(file_path);
  if (not in_stream.good()) {
    status.Prepend(StatusCode::FILE_DOES_NOT_EXIST_ERROR,
               "Append file " + file_path + " error. File does not exist.");
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
    status.Prepend(StatusCode::FILE_WRITE_ERROR, "Write file " + file_path + " error");
  }

  return status;
}

} /* namespace netconfd */
