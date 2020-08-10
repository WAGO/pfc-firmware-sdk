// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     IBridge.hpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------
#ifndef BAL_EXTERN_IBRIDGECONTROLLER_HPP_
#define BAL_EXTERN_IBRIDGECONTROLLER_HPP_

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------
#include <string>
#include <vector>

#include "Status.hpp"
#include "Types.hpp"


namespace netconf {


class IBridgeController {
 public:

  IBridgeController() = default;
  virtual ~IBridgeController() = default;

  IBridgeController(const IBridgeController&) = default;
  IBridgeController& operator=(const IBridgeController&) = default;
  IBridgeController(IBridgeController&&) = default;
  IBridgeController& operator=(IBridgeController&&) = default;


  virtual Status AddBridge(const Bridge& bridge) const = 0;
  virtual Status DeleteBridge(const Bridge& bridge) const = 0;
  virtual Status AddInterface(const Bridge& bridge, const Interface& interface) const = 0;
  virtual Status DeleteInterface(const Bridge& bridge, const Interface& interface) const = 0;

  virtual Bridges GetBridges() const = 0;
  virtual Interfaces GetBridgeInterfaces(const Bridge& bridge) const = 0;

  virtual Interfaces GetInterfaces() const = 0;

  virtual Status SetInterfaceUp(const ::std::string& name) const = 0;
  virtual Status SetInterfaceDown(const ::std::string& name) const = 0;

  virtual Status SetAgetime(const Bridge& bridge, int seconds) const = 0;
};

}

#endif /* BAL_EXTERN_IBRIDGECONTROLLER_HPP_ */
//---- End of source file ------------------------------------------------------

