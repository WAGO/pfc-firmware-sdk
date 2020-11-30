// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Bridge.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO Kontakttechnik GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include <system_error>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <unistd.h>

#include "Socket.h"
#include "BridgeController.hpp"
#include "libbridge.h"

#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>



namespace bfs = boost::filesystem;
namespace ba = boost::adaptors;
//------------------------------------------------------------------------------
// function implementation
//------------------------------------------------------------------------------

constexpr const char* SYSFS_CLASS_NET = "/sys/class/net/";

namespace netconf {

BridgeController::BridgeController() {
  if (br_init() < 0) {
    throw std::system_error(errno, std::generic_category(), "BridgeController: br_init failed");
  }
}

BridgeController::~BridgeController() {
  br_shutdown();
}

static int AddBridgeName(const char *bridge, void *vector_ptr) {

  auto bridges = reinterpret_cast<Bridges*>(vector_ptr);  //NOLINT: reinterpret_cast is unavoidable at this point

  bridges->push_back(bridge);

  return 0;
}

static int AddInterfaceName(const char *bridge, const char *interface, void *vector_ptr) {

  (void) bridge;

  auto interfaces = reinterpret_cast<Interfaces*>(vector_ptr);  //NOLINT: reinterpret_cast is unavoidable at this point

  interfaces->push_back(interface);

  return 0;
}

Error BridgeController::AddBridge(const Bridge& bridge) const {
  if (br_add_bridge(bridge.c_str()) != 0) {
    return MakeSystemCallError();
  }
  return Error::Ok();
}

Error BridgeController::DeleteBridge(const Bridge& bridge) const {
  if (br_del_bridge(bridge.c_str()) != 0) {
    return Error{ErrorCode::GENERIC_ERROR, "Failed to delete bridge " + bridge};
  }
  return Error::Ok();
}

Error BridgeController::AddInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_add_interface(bridge.c_str(), interface.c_str()) != 0) {
    return MakeSystemCallError();
  }
  return Error::Ok();
}

Error BridgeController::DeleteInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_del_interface(bridge.c_str(), interface.c_str()) != 0) {
    return MakeSystemCallError();
  }
  return Error::Ok();
}

Bridges BridgeController::GetBridges() const {

  Bridges bridges;

  br_foreach_bridge(&AddBridgeName, &bridges);

  return bridges;
}

Interfaces BridgeController::GetBridgeInterfaces(const Bridge& bridge) const {

  Interfaces interfaces;

  br_foreach_port(bridge.c_str(), &AddInterfaceName, &interfaces);

  return interfaces;
}

Interfaces BridgeController::GetInterfaces() const {

  Interfaces interfaces;

  for (auto &entry : boost::make_iterator_range(bfs::directory_iterator(SYSFS_CLASS_NET), { })
      | ba::filtered(static_cast<bool (*)(const bfs::path &)>(&bfs::is_symlink)))
  {
    interfaces.emplace_back(entry.path().filename().string());
  }

  return interfaces;
}

static Error SetInterfaceUpDown(const ::std::string& name, bool up) {
  ifreq ifr = { };

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (-1 == fd) {
    return MakeSystemCallError();
  }

  strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);  //NOLINT: do not access members of unions is unavoidable at this point
  ifr.ifr_addr.sa_family = AF_INET;               //NOLINT: do not access members of unions is unavoidable at this point

  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    close(fd);
    return MakeSystemCallError();
  }

  if (up) {
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);      //NOLINT: do not access members of unions is unavoidable at this point
  } else {
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);     //NOLINT: do not access members of unions is unavoidable at this point
  }
  if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
    close(fd);
    return MakeSystemCallError();
  }

  close(fd);

  return Error(ErrorCode::OK);
}

Error BridgeController::SetInterfaceUp(const ::std::string& name) const {
  return SetInterfaceUpDown(name, true);
}

Error BridgeController::SetInterfaceDown(const ::std::string& name) const {
  return SetInterfaceUpDown(name, false);
}

void BridgeController::SetAgetime(const Bridge& bridge, int seconds) const {
  struct timeval timeval {seconds, 0};
  br_set_ageing_time(bridge.c_str(), &timeval);
}

}  // namespace name
//---- End of source file ------------------------------------------------------

