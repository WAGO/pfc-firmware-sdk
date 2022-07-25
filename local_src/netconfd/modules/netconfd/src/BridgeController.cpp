// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     Bridge.cpp
///
///  \brief    <short description of the file contents>
///
///  \author   <author> : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// include files
//------------------------------------------------------------------------------

#include "BridgeController.hpp"

#include <dirent.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/in.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>
#include <system_error>

#include "FileEditor.hpp"
#include "Logger.hpp"
#include "Socket.hpp"
#include "libbridge.h"

namespace bfs = boost::filesystem;
namespace ba  = boost::adaptors;
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

static int AddBridgeName(const char* bridge, void* vector_ptr) {
  auto bridges = reinterpret_cast<Bridges*>(vector_ptr);  // NOLINT: reinterpret_cast is unavoidable at this point

  bridges->push_back(bridge);

  return 0;
}

static int AddInterfaceName(const char* bridge, const char* interface, void* vector_ptr) {
  (void)bridge;

  auto interfaces = reinterpret_cast<Interfaces*>(vector_ptr);  // NOLINT: reinterpret_cast is unavoidable at this point

  interfaces->push_back(interface);

  return 0;
}

static void DisableIgmpSnooping(const Bridge& bridge) {
  // FileEditor file_editor;
  // auto status = file_editor.Write("/sys/class/net/" + bridge + "/bridge/multicast_snooping", "0");
  // if (status.IsNotOk()) {
  //   LogError("failed to disable igmp snooping for " + bridge);
  // }

  ::std::ofstream stream("/sys/class/net/" + bridge + "/bridge/multicast_snooping", ::std::ofstream::trunc);
  if (stream.good()) {
    stream << "0";
    stream.flush();
    stream.close();

  } else {
    LogError("failed to disable igmp snooping for " + bridge);
  }
}

Status BridgeController::AddBridge(const Bridge& bridge) const {
  if (br_add_bridge(bridge.c_str()) != 0) {
    return MAKE_SYSTEMCALL_ERROR("br_add_bridge");
  }

  DisableIgmpSnooping(bridge);

  return Status::Ok();
}

Status BridgeController::DeleteBridge(const Bridge& bridge) const {
  if (br_del_bridge(bridge.c_str()) != 0) {
    return MAKE_SYSTEMCALL_ERROR("br_del_bridge");
  }
  return Status::Ok();
}

Status BridgeController::AddInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_add_interface(bridge.c_str(), interface.c_str()) != 0) {
    return MAKE_SYSTEMCALL_ERROR(::std::string{"br_add_interface"} + interface);
  }
  return Status::Ok();
}

Status BridgeController::DeleteInterface(const Bridge& bridge, const Interface& interface) const {
  if (br_del_interface(bridge.c_str(), interface.c_str()) != 0) {
    return MAKE_SYSTEMCALL_ERROR("br_del_interface");
  }
  return Status::Ok();
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

  for (auto& entry : boost::make_iterator_range(bfs::directory_iterator(SYSFS_CLASS_NET), {}) |
                         ba::filtered(static_cast<bool (*)(const bfs::path&)>(&bfs::is_symlink))) {
    interfaces.emplace_back(entry.path().filename().string());
  }

  return interfaces;
}

static Status SetInterfaceUpDown(const ::std::string& name, bool up) {
  ifreq ifr = {};
  int16_t current_flags;

  int fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (-1 == fd) {
    return MAKE_SYSTEMCALL_ERROR("socket");
  }

  strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);  // NOLINT: do not access members of unions is unavoidable at this
                                                  // point
  ifr.ifr_addr.sa_family = AF_INET;  // NOLINT: do not access members of unions is unavoidable at this point

  if (ioctl(fd, SIOCGIFFLAGS, &ifr) < 0) {
    close(fd);
    return MAKE_SYSTEMCALL_ERROR("ioctl");
  }

  current_flags = ifr.ifr_flags;  // NOLINT: do not access members of unions is unavoidable at this point

  if (up) {
    ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);  // NOLINT: do not access members of unions is unavoidable at this point
  } else {
    ifr.ifr_flags &= ~(IFF_UP | IFF_RUNNING);  // NOLINT: do not access members of unions is unavoidable at this point
  }
  if (current_flags != ifr.ifr_flags) {  // NOLINT: do not access members of unions is unavoidable at this point
    if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0) {
      close(fd);
      return MAKE_SYSTEMCALL_ERROR("ioctl");
    }
  }

  close(fd);

  return Status(StatusCode::OK);
}

Status BridgeController::SetInterfaceUp(const ::std::string& name) const {
  return SetInterfaceUpDown(name, true);
}

Status BridgeController::SetInterfaceDown(const ::std::string& name) const {
  return SetInterfaceUpDown(name, false);
}

void BridgeController::SetAgetime(const Bridge& bridge, int seconds) const {
  struct timeval timeval {
    seconds, 0
  };
  br_set_ageing_time(bridge.c_str(), &timeval);
}

}  // namespace netconf
//---- End of source file ------------------------------------------------------
