#!/bin/bash
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Copyright (c) 2018-2022 WAGO GmbH & Co. KG

function ShowControllerDetails
#
# Show window with several information data from IPC 
# (Menu: Main -> Information -> ControllerDetails)
# Values cant't be changed here.
#
{
  ShowEvaluateDataWindow "Controller Details"
  

  ./wdialog "--msgbox" \
            "$TITLE " \
            "Controller Details" \
            "Product Description..........$(./get_coupler_details product-description)" \
            "Order Number.................`./get_coupler_details order-number`" \
            "License Information..........`./get_coupler_details license-information`" \
            "Firmware Revision............`./get_coupler_details firmware-revision`" \

}

ShowControllerDetails
