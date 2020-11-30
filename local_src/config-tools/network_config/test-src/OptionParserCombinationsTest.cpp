// SPDX-License-Identifier: GPL-2.0-or-later

#include "OptionParser.hpp"

#include <array>
#include <gtest/gtest.h>
#include <stdexcept>

namespace po = boost::program_options;
namespace network_config
{

  class OptionParserCombinationsTest : public testing::Test
  {
    public:
      OptionParserCombinationsTest() : vm_{sut_.GetVariableMap()} {

      }

      void SetUp() override {

      }

      OptionParser sut_;
      const po::variables_map& vm_;
  };

  template<typename ... Args>
  auto createArgPair(Args ... args) -> ::std::array<const char*, sizeof...(args)>
  {
    return ::std::array<const char*, sizeof...(args)>{{args...}};
  }

  TEST_F(OptionParserCombinationsTest, GetRequest)
  {
    auto args = createArgPair("program", "--bridge-config", "--get", "--format", "json");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));

    EXPECT_EQ(3, vm_.size());
  }

  TEST_F(OptionParserCombinationsTest, GetRequestDefaultFormatJson)
  {
    auto args = createArgPair("program", "--bridge-config", "--get");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(3, vm_.size()); // 2 + default value: --format
    EXPECT_EQ("text", vm_["format"].as<FormatOption>().value);
  }

  TEST_F(OptionParserCombinationsTest, GetRequestUnexpectedFormat)
  {
    auto args = createArgPair("program", "--bridge-config", "--get", "--format", "xxx");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::validation_error);
  }

  TEST_F(OptionParserCombinationsTest, RequestGetCurrentOption)
  {
    auto args = createArgPair("program", "--ip-config", "--get-current");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
  }

  TEST_F(OptionParserCombinationsTest, SetRequest)
  {
    auto args = createArgPair("program", "--bridge-config", "--set", "{}");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(3, vm_.size()); // 2 + default value: --format
  }

  TEST_F(OptionParserCombinationsTest, GetBridgeConfigFaultyRequest)
  {
    auto args = createArgPair("program", "--bridge-config", "--get", "--set", "--format=json");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, RequestMultiplePrimaryOptions)
  {
    auto args = createArgPair("program", "--bridge-config", "--ip-config", "--get");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, RequestUnknownOption)
  {
    auto args = createArgPair("program", "--firewall-config", "--get");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, RequestDuplicateOption)
  {
    auto args = createArgPair("program", "--set", "{}", "--ip-config", "--set", "{}");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, RequestMissingOption)
  {
    auto args = createArgPair("program", "--ip-config");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, RequestMissingSetValue)
  {
    auto args = createArgPair("program", "--ip-config", "--set");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, IllegalSetOfBothNetmaskAndPrefix)
  {
    auto args = createArgPair(
        "program", "--ip-config", "--set", "--device", "br0",
        "--netmask", "255.255.255.0", "--prefix", "24");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, Backup)
  {
    auto args = createArgPair("program", "--backup", "/the/file/path");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(2, vm_.size()); // 2 + default value: --format
  }

  TEST_F(OptionParserCombinationsTest, Retore)
  {
    auto args = createArgPair("program", "--restore", "/the/file/path");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(2, vm_.size()); // 2 + default value: --format
  }

  TEST_F(OptionParserCombinationsTest, BackupFailsWithoutFilepath)
  {
    auto args = createArgPair("program", "--backup");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, SetFixIp)
  {
    auto args = createArgPair("program", "--fix-ip");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(2, vm_.size()); // 2 + default value: --format
  }

  TEST_F(OptionParserCombinationsTest, GetMacAddress)
  {
    auto args = createArgPair("program", "--mac-address", "--get", "--device=br0");

    EXPECT_NO_THROW(sut_.Parse(args.size(), args.data()));
    EXPECT_EQ(4, vm_.size()); // 3 + default value: --format
  }

  TEST_F(OptionParserCombinationsTest, GetMacAddressMissingGetOption)
  {
    auto args = createArgPair("program", "--mac-address");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, GetMacAddressMissingGetOptionValue)
  {
    auto args = createArgPair("program", "--mac-address", "--get");

    EXPECT_THROW(sut_.Parse(args.size(), args.data()), po::error);
  }

  TEST_F(OptionParserCombinationsTest, QuietAndErrMsgDstDontGoAlong)
  {
    auto args_nok = createArgPair("program", "--mac-address", "--device=br0", "--get", "--quiet", "--error-msg-dst", "file.txt");
    EXPECT_THROW(sut_.Parse(args_nok.size(), args_nok.data()), po::error);

    auto args_ok1 = createArgPair("program", "--mac-address", "--device=br0", "--get", "--quiet");
    EXPECT_NO_THROW(sut_.Parse(args_ok1.size(), args_ok1.data()));

    auto args_ok2 = createArgPair("program", "--mac-address", "--get", "--device=br0", "--error-msg-dst", "file.txt");
    EXPECT_NO_THROW(sut_.Parse(args_ok2.size(), args_ok2.data()));
  }


}  // namespace network_config
