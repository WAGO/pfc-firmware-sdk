// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     DSAJsonConverter.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef BRIDGECONFIG_DSA_TEST_SRC_DSAJSONCONVERTER_HPP_
#define BRIDGECONFIG_DSA_TEST_SRC_DSAJSONCONVERTER_HPP_

#include <JsonConfigConverter/JsonConfigConverter.hpp>
#include "TypesHelper.hpp"
#include "Types.hpp"
#include "CommandExecutor.hpp"

namespace netconfd {

class DSAJsonConverter {
 public:
  DSAJsonConverter() = default;
  virtual ~DSAJsonConverter() = default;

  Status DSAToJson(const std::uint32_t mode,
                        std::string& json) const;
  Status JsonToDSA(const std::string& json, std::uint32_t& dsa_flag) const;

 private:

  std::string GetActualBrigeConfigFromNetconfd() const;

  JsonConfigConverter json_config_converter_;
  CommandExecutor command_executor_;
};

} /* namespace netconfd */

#endif /* BRIDGECONFIG_DSA_TEST_SRC_DSAJSONCONVERTER_HPP_ */
