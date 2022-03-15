#pragma once

#include "Status.hpp"
#include "DynamicIPEventAction.hpp"

namespace netconf {
namespace api {

Status NotifyDynamicIPAction(const ::std::string &interface, DynamicIPEventAction action);


Status MakeDynamicIPAction(const std::string &json_str, ::std::string &interface, DynamicIPEventAction& action);

Status ReloadHostConf();

}

}

