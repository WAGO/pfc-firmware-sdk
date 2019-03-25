# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG

function MainCODESYSProject
#
# Show window with several information about CODESYS project and state
#
{
  ShowEvaluateDataWindow "${ITEM_NAME} - Project Details"

  ./wdialog "--msgbox" \
            "$TITLE" \
            "Project Details" \
            "Date.........`./get_rts_info project date`" \
            "Title........`./get_rts_info project title`" \
            "Version......`./get_rts_info project version`" \
            "Author.......`./get_rts_info project author`" \
            "Description..`./get_rts_info project description`"
}

MainCODESYSProject

