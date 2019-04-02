// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG


#include <stdlib.h>
#include <fcntl.h>

#include <stdio.h>

int make_pty_fd()
{
    int retval = posix_openpt(O_RDWR | O_NONBLOCK);
    grantpt(retval);
    unlockpt(retval);
    //printf( "pts name %s\n", ptsname(retval) );

    return retval;
}
