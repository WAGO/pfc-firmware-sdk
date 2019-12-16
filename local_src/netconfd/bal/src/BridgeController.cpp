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

#define SYSFS_CLASS_NET "/sys/class/net/"

namespace netconfd {

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

Status BridgeController::AddBridge(const Bridge& bridge) const {
  if (br_add_bridge(bridge.c_str()) != 0) {
    return Status(StatusCode::ERROR, "Failed to add bridge " + bridge + ", system call error");
  }
  return Status(StatusCode::OK);
}

Status BridgeController::DeleteBridge(const Bridge& bridge) const {
  if (br_del_bridge(bridge.c_str()) != 0) {
    return Status(StatusCode::ERROR, "Failed to delete bridge " + bridge + ", system call error");
  }
  return Status(StatusCode::OK);
}

Status BridgeController::AddInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_add_interface(bridge.c_str(), interface.c_str()) != 0) {
    return Status(StatusCode::ERROR,
                  "Failed to add interface " + interface + " to bridge " + bridge + ", system call error");
  }
  return Status(StatusCode::OK);
}

Status BridgeController::DeleteInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_del_interface(bridge.c_str(), interface.c_str()) != 0) {
    return Status(StatusCode::ERROR,
                  "Failed to delete interface " + interface + " from bridge " + bridge + ", system call error");
  }
  return Status(StatusCode::OK);
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

static Status SetInterfaceUpDown(const ::std::string& name, bool up) {
  ifreq ifr = { };

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (-1 == fd) {
    return Status(StatusCode::ERROR, "system call open socket failed");
  }

  strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);  //NOLINT: do not access members of unions is unavoidable at this point
  ifr.ifr_addr.sa_family = AF_INET;               //NOLINT: do not access members of unions is unavoidable at this point

  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    close(fd);
    return Status(StatusCode::ERROR, "system call get interface flags error (SIOCGIFFLAGS)");
  }

  if (up) {
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);      //NOLINT: do not access members of unions is unavoidable at this point
  } else {
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);     //NOLINT: do not access members of unions is unavoidable at this point
  }
  if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
    close(fd);
    return Status(StatusCode::ERROR, "system call set interface flags error (SIOCSIFFLAGS)");
  }

  close(fd);

  return Status(StatusCode::OK);
}

Status BridgeController::SetInterfaceUp(const ::std::string& name) const {
  Status status = SetInterfaceUpDown(name, true);
  if (status.NotOk()) {
    status.Prepend("Failed to set up interface " + name + ", ");
  }
  return status;
}

Status BridgeController::SetInterfaceDown(const ::std::string& name) const {
  Status status = SetInterfaceUpDown(name, false);
  if (status.NotOk()) {
    status.Prepend("Failed to set down interface " + name + ", ");
  }
  return status;
}

Status BridgeController::IsInterfaceUp(const ::std::string& name, bool& is_up) const {
  ifreq ifr = { };
  try {
    strncpy(ifr.ifr_name, name.c_str(), ::std::min(name.size(), static_cast<size_t>(IFNAMSIZ))); // NOLINT: Accessing legacy API data structures

    Socket sock { PF_INET, SOCK_DGRAM, IPPROTO_IP };

    struct ethtool_value eth_value { };
    eth_value.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = reinterpret_cast<__caddr_t >(&eth_value); // NOLINT: Accessing legacy API data structures
    if (ioctl(sock, SIOCETHTOOL, &ifr) < 0) {
      return Status(StatusCode::ERROR, "ioctl() ETHTOOL_GLINK failed");
    }
    is_up = static_cast<bool>(eth_value.data);
    return Status(StatusCode::OK);
  } catch (...) {
    return Status(StatusCode::ERROR, "Failed to get state of interface " + name + ", system call open socket error");
  }
}

Status BridgeController::SetAgetime(const Bridge& bridge, int seconds) const {
  struct timeval timeval {seconds, 0};

  auto result = br_set_ageing_time(bridge.c_str(), &timeval);
  if (result != 0)
   return Status{StatusCode::ERROR, "Failed to set ageing time for bridge: " + bridge};

  return Status{};
}

}  // namespace name
//---- End of source file ------------------------------------------------------

