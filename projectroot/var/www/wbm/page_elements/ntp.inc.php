<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of NTP Client</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>NTP Client Configuration</h3>
    <form id="config_ntp" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="label_field"><label for="ntp_client_state">Service enabled:</label></div>
            <div class="value_field"><input id="ntp_client_state" type="checkbox" name="ntp_client_state" ><!-- <span id="ntp_client_active_state"></span> --></div>
          </div>
          <div class="label_value_pair" id="ntp_client_active_state_area">
            <div class="label_field">Service Result:</div>
            <div class="value_field"><span id="ntp_client_active_state"></span></div>
          </div>
          <?php
          printf("\n");
          $timeServerNo = 1;
          for($timeServerNo = 1; $timeServerNo <= 4; ++$timeServerNo)
          {
            printf("\n            <div class=\"label_value_pair\">\n");
            printf("              <div class=\"label_field\"><label for=\"ntp_client_time_server\">Time Server %d:</label></div>\n", $timeServerNo);
            printf("              <div class=\"value_field\">\n");
            printf("                <input id=\"ntp_client_time_server_%d\" class=\"input_field\" type=\"text\" name=\"time_server_%d\" maxlength=\"16\" >\n", $timeServerNo, $timeServerNo);
            printf("              </div>\n");
            printf("            </div>\n");
          }
          ?>
          <div class="label_value_pair">
            <div class="label_field"><label for="ntp_client_update_time">Update Interval (sec):</label></div>
            <div class="value_field"><input id="ntp_client_update_time" class="input_field" type="text" name="update_time" maxlength="16" value="" ></div>
          </div>
        </div>
        
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

    <div class="config_form_box">
      <div class="config_data_list" >
        <div class="label_value_pair">
          <div class="label_field"><label for="ntp_client_state">Additionally used:<br>(assigned by DHCP):</label></div>
          <div class="value_field"><span id="ntp_client_time_servers_currently_used"></span></div>
        </div>
      </div>
    </div>
    
    <h3>NTP Single Request</h3> 
    <div class="config_form_box">
      <p class="process_button_area">
        <input id="ntp_trigger_button" class="warning_button" type="submit" value="Update Time now" >
      </p>
    </div>
    
