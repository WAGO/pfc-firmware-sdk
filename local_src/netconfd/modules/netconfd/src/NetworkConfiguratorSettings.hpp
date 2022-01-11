// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once
namespace netconf {

  /**
   * Configuration if the initial port state at startup of the configurator
   * It can be used to force an alternative initial port state for e.g. FW update case.
   */
  enum class StartWithPortstate{
    Normal,         //!< Normal - startup as usually with the settings from the persistence or defaults
    Down,           //!< Down - Startup with all ports forced down
    Up              //!< Up - Startup with all ports forced up
  };
} // namespace netconf
