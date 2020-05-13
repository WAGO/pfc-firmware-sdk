// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <string>

namespace netconfd {

void ChronometerStart();
void ChronometerPrintTimeMs(const ::std::string& discripton);

}
