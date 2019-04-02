<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <!--<p>Changes of default gateway configuration will take effect immediately.</p>-->

    <!-- 
    <form id="default_gateway_form" action="javascript_requested.php" method="POST">
    
      <h3>Default Gateway</h3>
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">State:</div>
            <div class="value_field"><input id="none" type="radio" name="interface" value="none" ><label for="none">Disabled</label></div>
          </div>

          <div class="label_value_pair" id="device-selection-eth0">
            <div class="label_field"></div>
            <div class="value_field"><input id="eth0" type="radio" name="interface" value="eth0" ><label for="eth0">Enabled <span id="device-id-eth0"></span></label></div>
          </div>
          <div class="label_value_pair" id="device-selection-eth1" style="display: none;">
            <div class="label_field"></div>
            <div class="value_field"><input id="eth1" type="radio" name="interface" value="eth1" ><label for="eth1">Enabled <span id="device-id-eth1"></span></label></div>
          </div>

          <div class="label_value_pair">
            <div class="label_field"><label for="default-gateway-value">Gateway:</label></div>
            <div class="value_field"><input id="default-gateway-value" class="input_field" type="text" name="default-gateway-value" maxlength="16"  value="" ></div>
          </div>
        </div>
        
        <p class="button_area">
          <input class="button" type="submit" value="SUBMIT" name="SUBMIT">
        </p>
      </div>

    </form>
     -->

    <h3>Default Gateway</h3>
    
    <form id="default_gateway_X1_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">State X1:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li><input id="default_gateway_X1_state_disabled"  type="radio" name="state" value="disabled" ><label for="disabled">Disabled</label></li>
                <li><input id="default_gateway_X1_state_enabled"   type="radio" name="state" value="enabled" ><label for="enabled">Enabled</label></li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_value_pair">
              <div class="label_field"><label for="default-gateway-value">Gateway X1:</label></div>
              <div class="value_field"><input id="default-gateway-value" class="input_field" type="text" name="default-gateway-value" maxlength="16"  value="" ></div>
            </div>
          </div>
          
          <p class="button_area">
            <input class="button" type="submit" value="Submit" name="SUBMIT">
          </p>
        </div>
      </div>
    </form>

    <form id="default_gateway_X2_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">State X2:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li><input id="default_gateway_X2_state_disabled"  type="radio" name="state" value="disabled" ><label for="disabled">Disabled</label></li>
                <li><input id="default_gateway_X2_state_enabled"   type="radio" name="state" value="enabled" ><label for="enabled">Enabled</label></li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_value_pair">
              <div class="label_field"><label for="default-gateway-value">Gateway X2:</label></div>
              <div class="value_field"><input id="default-gateway-value" class="input_field" type="text" name="default-gateway-value" maxlength="16"  value="" ></div>
            </div>
          </div>
          
          <p class="button_area">
            <input class="button" type="submit" value="Submit" name="SUBMIT">
          </p>
        </div>
      </div>
    </form>
    
    