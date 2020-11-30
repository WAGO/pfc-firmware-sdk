// SPDX-License-Identifier: LGPL-3.0-or-later
#pragma once

#include <string>

namespace netconf {

void ChronometerStart();
void ChronometerPrintTimeMs(const ::std::string& discripton);

}
