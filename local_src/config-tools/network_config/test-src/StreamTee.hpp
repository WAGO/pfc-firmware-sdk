#pragma once

#include <fstream>
#include <boost/iostreams/tee.hpp>
#include <boost/iostreams/stream.hpp>

class StreamTee{

  using TeeDevice = boost::iostreams::tee_device<::std::ostream, ::std::ofstream> ;
  using TeeStream = boost::iostreams::stream<TeeDevice> ;
 public:
  StreamTee(::std::ostream& s, ::std::string filename):
    filestream_{filename},
    tmp_stream_{s.rdbuf()},
    outputDevice_{tmp_stream_, filestream_},
    logger_{outputDevice_},
    orig_stream_{s}
    {
      s.rdbuf(logger_.rdbuf());
    }

  ~StreamTee(){
    orig_stream_.rdbuf(tmp_stream_.rdbuf());
    filestream_.close();
  }

  ::std::ofstream filestream_;
  ::std::ostream tmp_stream_;
  TeeDevice outputDevice_;
  TeeStream logger_;
  ::std::ostream& orig_stream_;

};
