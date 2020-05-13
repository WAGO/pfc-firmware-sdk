#include "DbusError.hpp"

namespace netconf {

DbusError::DbusError() {
  dbus_error_init(&err);
}

DbusError::~DbusError() {
  if (dbus_error_is_set(&err) != 0) {
    dbus_error_free(&err);
  }
}

}  // namespace netconf
