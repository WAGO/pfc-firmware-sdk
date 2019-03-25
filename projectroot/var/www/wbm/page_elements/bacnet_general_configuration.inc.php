<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of General BACnet</h2>

    <p>Changes will take effect after next controller reboot.</p>

    <h3>BACnet Status</h3>
    
    <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="label_field">Device-ID:</div>
          <div class="value_field"><span id="spanDeviceId">updating... please wait</span></div>
        </div>
      </div>
    </div>
    
    <h3>BACnet General Configuration</h3>
    
    <div class="config_form_box group">
      <form id="bacnet_state_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field">
              <label for="bacnet_fieldbus_state">Service active:</label>
            </div>
            <div class="value_field">
              <input id="bacnet_fieldbus_active_state" type="checkbox" name="bacnet_fieldbus_active_state" value="true" >
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_state_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>
      </form>
    </div>
    
    <div class="config_form_box group">
      <form id="bacnet_port_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">
              <label for="bacnet_udp_port">Port Number:</label>
            </div>
            <div class="value_field">
              <input id="bacnet_port_number" class="input_field" type="text" name="port_number" size="4" >
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_port_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>
      </form>
    </div>
    
    <div class="config_form_box">
      <form id="bacnet_who_is_online_interval_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field"><label for="bacnet_who_is_online_interval">Who-Is online<br>interval time (sec):</label></div>
            <div class="value_field"><input id="bacnet_who_is_online_interval_number" class="input_field" type="text" name="who_is_online_interval_time" size="4" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_who_is_online_interval_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>
      </form>
    </div>
    
