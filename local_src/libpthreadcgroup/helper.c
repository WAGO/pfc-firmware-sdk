// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018-2022 WAGO GmbH & Co. KG


#include "helper.h"
#include <string.h>

void helper_replace_char(char *target, char to_replace, char replacement)
{
    do
    {
        if(*target == to_replace)
            *target = replacement;
    } while(*++target != '\0');
}
