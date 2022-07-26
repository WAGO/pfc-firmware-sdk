<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>I/O-Check Port Configuration</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>I/O-Check Port</h3>
    
    <div id="iocheck_firewall_service_config_area">
    </div>
    
    <form id="iocheck_port_config_form" class="discard_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="iocheck_port_active_state">Service active:</label></div>
            <div class="value_field">
              <input id="iocheck_port_active_state"" type="checkbox" value="change_state" name="service_active_state" >
            </div>
          </div>
        </div>
      
        <p class="button_area">
          <input id="iocheck_submit_button" class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

