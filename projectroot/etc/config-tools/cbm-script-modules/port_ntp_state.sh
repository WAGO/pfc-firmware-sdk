# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

# evaluate state of server new after any changes (can switch to disabled due to errors)

state=$(./get_ntp_config state)

case "$state" in
  enabled)
    echo ".enabled $(./get_ntp_config active-state)";;
  disabled)
    echo "disabled $(./get_ntp_config active-state)";;
esac
