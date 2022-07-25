
#include "EventFolder.hpp"

#include "Program.hpp"

namespace wago::snmp_config_lib {

void TriggerEventFolder() {
  auto run_parts_result = wago::util::Program::Execute("run-parts -a config /etc/config-tools/events/snmp/");

  if (run_parts_result.GetResult() != 0) {
    //TODO(TeamPnd) add error log
  }
}

}
