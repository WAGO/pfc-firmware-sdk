<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>SSH Server Settings</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>SSH Server</h3>
    
    <form id="ssh_config_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">

          <div class="label_value_pair">
            <div class="label_field"><label for="ssh_active_state">Service active:</label></div>
            <div class="value_field">
              <input id="ssh_active_state" type="checkbox" value="change_state" name="service_active_state" >
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="ssh_port_number">Port Number:</label></div>
            <div class="value_field"><input id="ssh_port_number" class="input_field" type="text" name="port_number" size="4"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="ssh_root_access_state">Allow root login:</label></div>
            <div class="value_field"><input id="ssh_root_access_state" type="checkbox" name="root_access_state"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="ssh_password_request_state">Allow password login:</label></div>
            <div class="value_field"><input id="ssh_password_request_state" type="checkbox" name="password_request_state"></div>
          </div>
        </div>
      
        <p class="button_area">
          <input id="ssh_submit_button" class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

