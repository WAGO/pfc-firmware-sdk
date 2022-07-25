// SPDX-License-Identifier: GPL-2.0-or-later
//------------------------------------------------------------------------------
///  \file     InterfaceTreatment.hpp
///
///  \brief    How shall an interface be treated by netconfd's internals
///
///  \author   u014487 : WAGO GmbH & Co. KG
//------------------------------------------------------------------------------
#pragma once

struct InterfaceTreatment {
 bool data_persistence;
 bool ip_change_trigger;
};

//---- End of header file ------------------------------------------------------

