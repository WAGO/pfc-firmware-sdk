<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of MAC address filter</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>Global MAC address filter state</h3>
    <div class="config_form_box">
      <form id="mac_address_filter_mode_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="label_field"><label for="mac_address_filter_mode">Filter enabled:</label></div>
            <div class="value_field">
              <input id="mac_address_filter_mode_enabled" type="checkbox" name="mac_address_filter_mode_enabled" value="enabled" >
            </div>
          </div>
        </div>
        <p class="button_area">
          <input id="change_state_button_mac_address_filter_mode" class="button" type="submit" value="Submit" name="change_mode">
        </p>
      </form>
    </div>
      
    <div id="firewall_ethernet_interface_config_area">
    </div>
    
    <h3>MAC address filter whitelist</h3>

    <div id="mac_address_filter_whitelist_area">
      
      <div class="config_form_box group">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="value_field">Unknown</div>
          </div>
        </div>
      </div>
    </div>
    
    <div class="config_form_box">
    
      <form id="add_mac_address_filter_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="new_mac_address">MAC address:</label></div>
            <div class="value_field"><input id="new_mac_address" class="input_field" type="text" name="new_mac_address" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="new_mac_mask">MAC mask:</label></div>
            <div class="value_field"><input id="new_mac_mask" class="input_field" type="text" name="new_mac_mask" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="new_mac_address_filter_state">Filter enabled:</label></div>
            <div class="value_field">
              <input id="new_mac_address_filter_state" type="checkbox" name="new_mac_address_filter_state" value="enabled" >
            </div>
          </div>
        </div>
     
        <p class="button_area">
          <input id="ssh_submit_button" class="button" type="submit" value="Add" name="submit">
        </p>
      </form>
    </div>

    
    
