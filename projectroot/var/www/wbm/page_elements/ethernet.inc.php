<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Ethernet Configuration</h2>
	  
    <p>Changes will take effect immediately.</p>
    <p>Please note: Activating “Fast Aging” has no effect as long as the switch is operated in “Separate” mode.</p>

    <div id="dsa_configuration_area">
      <h3>Switch Configuration</h3>
      <form id="dsa_state_form" action="javascript_requested.php" method="POST">
        <div class="config_form_box group">
          <div class="config_data_list">
      
            <div class="label_value_pair">
              <div class="label_field">Interfaces:</div>
              <div class="value_field">
                <ul class="check_list_important">
                  <li><input type="radio" id="switchconfig_switched" name="switchconfig" value="0" />
                    <label for="switchconfig_switched">Switched</label> 
                  </li>
                  <li><input type="radio" id="switchconfig_seprated" name="switchconfig" value="1" />
                    <label for="switchconfig_seprated">Separated</label>
                  </li>
                </ul>
              </div>
            </div>
          </div>
        
          <p class="button_area">
            <input id="change_state_button_dsa" class="button" type="submit" value="Submit" name="change_state">
          </p>
        </div>
      </form>
    </div>

    <div id="switch_configuration_area">
      <form id="switch_config_form" action="javascript_requested.php" method="POST">
        <div class="config_form_box">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field">Port Mirror:</div>
              <div class="value_field">
                <ul class="check_list_important">
                 <li><input id="switch_config_port_mirror_0" type="radio" name="port_mirror" value="0" >
                   <label for="switch_config_port_mirror_0">None</label></li>
                 <li><input id="switch_config_port_mirror_1" type="radio" name="port_mirror" value="1" >
                   <label for="switch_config_port_mirror_1">X1</label></li>
                 <li><input id="switch_config_port_mirror_2" type="radio" name="port_mirror" value="2" >
                   <label for="switch_config_port_mirror_2">X2</label></li>
                </ul>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="switch_config_fast_aging_enabled">Fast Aging enabled:</label></div>
              <div class="value_field"><input id="switch_config_fast_aging_enabled" type="checkbox" name="fast_aging_enabled" value="1" ></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="switch_config_broadcast_protection">Broadcast Protection:</label></div>
              <div class="value_field">
                <select id="switch_config_broadcast_protection" class="input_field" name="broadcast_protection">
                  <option value="0">Disabled</option>
                  <?php 
                  printf("\n");
                  for($validValue = 1; $validValue <= 5; ++$validValue)
                  {
                    printf("                  <option value=\"%d\">%2d %%</option>\n", $validValue, $validValue);
                  } 
                  ?>
                </select>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="switch_config_rate_limit">Rate Limit:</label></div>
              <div class="value_field">
                <select id="switch_config_rate_limit" class="input_field" name="rate_limit" >
                </select>
              </div>
            </div>
          </div>
          <p class="button_area">
            <input id="change_switch_config_button" class="button" type="submit" value="Submit" name="change_switch_config">
          </p>
          </div>
      </form>
    </div>
    
    <div id="eth_config_ifaces_area"></div>

