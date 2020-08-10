// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

namespace netconf {

void ChronometerStart();
void ChronometerPrintTimeMs(const ::std::string& discripton);

}
