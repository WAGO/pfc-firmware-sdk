//------------------------------------------------------------------------------
// Copyright (c) WAGO GmbH & Co. KG
//
// PROPRIETARY RIGHTS are involved in the subject matter of this material. All
// manufacturing, reproduction, use and sales rights pertaining to this
// subject matter are governed by the license agreement. The recipient of this
// software implicitly accepts the terms of the license.
//------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include "file_accessor.hpp"
#include "test_base_iptables.hpp"

using namespace wago;

struct IptablesForwardProcessingData {

  IptablesForwardProcessingData()
      :
      interface1_ { },
      interface2_ { },
      state_ { },
      second_interface1_ { },
      second_interface2_ { } {
  };

  IptablesForwardProcessingData(::std::string interface1, ::std::string interface2, ::std::string state,
                                ::std::string second_interface1, ::std::string second_interface2)
      :
      interface1_ { interface1 },
      interface2_ { interface2 },
      state_ { state },
      second_interface1_ { second_interface1 },
      second_interface2_ { second_interface2 } {
  };

  ::std::string interface1_;
  ::std::string interface2_;
  ::std::string state_;
  ::std::string second_interface1_;
  ::std::string second_interface2_;
};

class IptablesForwardProcessingTestBase : public IptablesTestBase {

 public:

  IptablesForwardProcessingTestBase() = default;
  ~IptablesForwardProcessingTestBase() = default;

  bool interface_exists(const xmldoc &doc, const ::std::string &ifname1, const ::std::string &ifname2);
  IptablesForwardProcessingData get_iptables_processing_data(const xmldoc &doc, const ::std::string &ifname1,
                                                             const ::std::string &ifname2);
  ::std::string create_xpath_to_get(const ::std::string &ifname1, const ::std::string &ifname2);

  static ::std::string generate_test_name(testing::TestParamInfo<IptablesForwardProcessingData> data);

};
