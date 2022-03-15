// SPDX-License-Identifier: GPL-2.0-or-later

#include "FileEditor.hpp"

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>

namespace netconf {

Status FileEditor::Read(const ::std::string& file_path, ::std::string& data) const {

  ::std::ifstream stream(file_path);

  if (stream.good()) {
    data = ::std::string((std::istreambuf_iterator<char>(stream)),
                         std::istreambuf_iterator<char>());
    stream.close();

  } else {
    return Status{StatusCode::FILE_READ, file_path};
  }

  return Status::Ok();

}

Status FileEditor::Write(const ::std::string& file_path,
                         const ::std::string& data) const {
  Status status(StatusCode::OK);

  ::std::ofstream stream(file_path,::std::ofstream::trunc);

  if (stream.good()) {

    stream << data;
    stream.flush();
    stream.close();

  } else {
    return Status{StatusCode::FILE_WRITE, file_path};
  }

  return status;
}

Status FileEditor::WriteAndReplace(const ::std::string& file_path,
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
    return Status{StatusCode::FILE_WRITE, file_path};
  }

  return status;
}

Status FileEditor::Append(const ::std::string& file_path,
                          const ::std::string& data) const {

  ::std::ifstream in_stream(file_path);
  if (not in_stream.good()) {
    return Status{StatusCode::FILE_WRITE, file_path};
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
    return {StatusCode::FILE_WRITE, file_path };
  }

  return Status::Ok();
}

} /* namespace netconf */
