<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>TFTP Server</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>TFTP Server</h3>
    
    <form id="tftp_config_form" class="discard_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="tftp_communication_active_state">Service active:</label></div>
            <div class="value_field">
              <input id="tftp_active_state"" type="checkbox" value="change_state" name="service_active_state" >
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="tftp_download_dir">Download directory:</label></div>
            <div class="value_field"><input id="tftp_download_dir" class="input_field_wide input_field" type="text" name="download_dir" ></div>
          </div>
        </div>
      
        <p class="button_area">
          <input id="tftp_submit_button" class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

