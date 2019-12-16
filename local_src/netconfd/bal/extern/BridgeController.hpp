// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Bridge.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef BAL_SRC_BRIDGECONTROLLER_HPP_
#define BAL_SRC_BRIDGECONTROLLER_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include "IBridgeController.hpp"

//------------------------------------------------------------------------------
// defines; structure, enumeration and type definitions
//------------------------------------------------------------------------------

namespace netconfd {

class BridgeController : public IBridgeController{
 public:
  explicit BridgeController();
  ~BridgeController() override;

  BridgeController(const BridgeController&) = delete;
  BridgeController& operator=(const BridgeController&) = delete;
  BridgeController(const BridgeController&&) = delete;
  BridgeController& operator=(const BridgeController&&) = delete;

  Status AddBridge(const Bridge& bridge) const override;
  Status DeleteBridge(const Bridge& bridge) const override;
  Status AddInterface(const Bridge& bridge, const Interface& interface) const override;
  Status DeleteInterface(const Bridge& bridge, const Interface& interface) const override;

  Bridges GetBridges() const override;
  Interfaces GetBridgeInterfaces(const Bridge& bridge) const override;

  Interfaces GetInterfaces() const override;


  Status SetInterfaceUp(const ::std::string& name) const override;
  Status SetInterfaceDown(const ::std::string& name) const override;

  Status IsInterfaceUp(const ::std::string& name, bool& is_up) const override;

  Status SetAgetime(const Bridge& bridge, int seconds) const override;
};


}  // namespace netconfd


#endif /* BAL_SRC_BRIDGECONTROLLER_HPP_ */
//---- End of source file ------------------------------------------------------

