// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef DBUS_SRC_INTERFACE_H_
#define DBUS_SRC_INTERFACE_H_

#include <string>


typedef struct _GDBusObjectSkeleton GDBusObjectSkeleton;

namespace netconf {
namespace dbus {

class IDBusHandlerRegistry {
 public:
  IDBusHandlerRegistry() = default;
  virtual ~IDBusHandlerRegistry() = default;

  IDBusHandlerRegistry(const IDBusHandlerRegistry&) = default;
  IDBusHandlerRegistry& operator=(const IDBusHandlerRegistry&) = default;
  IDBusHandlerRegistry(IDBusHandlerRegistry&&) = default;
  IDBusHandlerRegistry& operator=(IDBusHandlerRegistry&&) = default;


  virtual GDBusObjectSkeleton* GetInterfaceObject() const = 0;
  virtual GDBusObjectSkeleton* GetIPObject() const = 0;
  virtual GDBusObjectSkeleton* GetBackupObject() const = 0;
  virtual GDBusObjectSkeleton* GetEventObject() const = 0;
};

} /* namespace dbus */
} /* namespace netconf */

#endif /* DBUS_SRC_INTERFACE_H_ */
