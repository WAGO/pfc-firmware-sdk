// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>

namespace netconf {
namespace eth {


enum class EthernetMauType
  : ::std::uint32_t
  {
    TYPE_10BASE_T = (0x0005U),
  TYPE_10BASE_TXHD = (0x000AU),
  TYPE_10BASE_TXFD = (0x000BU),
  TYPE_10BASE_FLHD = (0x000CU),
  TYPE_10BASE_FLFD = (0x000DU),
  TYPE_100BASE_TXHD = (0x000FU),
  TYPE_100BASE_TXFD = (0x0010U),
  TYPE_100BASE_FXHD = (0x0011U),
  TYPE_100BASE_FXFD = (0x0012U),
  TYPE_1000BASE_XHD = (0x0015U),
  TYPE_1000BASE_XFD = (0x0016U),
  TYPE_NONE = 0
};

enum class TpMdiType
  : ::std::uint8_t
  {
    Invalid = 0x00,
  II = 0x01,
  X = 0x02,
  Auto = 0x03,
};

enum class MediaType
  : ::std::uint32_t
  {
    TP = 0x00,
  AUI = 0x01,
  MII = 0x02,
  FIBRE = 0x03,
  BNC = 0x04,
  DA = 0x05,
  NONE = 0xef,
  OTHER = 0xff,
};

enum class DeviceState
  : bool
  {
    Down,
  Up
};

enum class InterfaceLinkState
  : bool
  {
    Down,
  Up
};

enum class Autoneg {
  Off,
  On
};


enum class Duplex {
  Half = 0,
  Full = 1,
  Unknown = 255
};


}
}


