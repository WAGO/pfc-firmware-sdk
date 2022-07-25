<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of SNMP v1/v2c parameters</h2>
    
    <p>Changes will take effect after the next software or hardware reset.</p>
    <p>Note: disabling the protocol will also delete the configured community name.</p>

    <h3>SNMP v1/v2c Manager Configuration</h3>

    <form id="change-v1-v2c-config-form" action="javascript_requested.php" method="POST">

      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="v1-v2c-protocol-state">Protocol enabled:</label></div>
            <div class="value_field"><input id="v1-v2c-protocol-state" type="checkbox" name="state" value="enabled" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="v1-v2c-community-name">Local community name:</label></div>
            <div class="value_field"><input id="v1-v2c-community-name" class="input_field" type="text" name="community-name" size="34" maxlength="200" value=""></div>
          </div>

        </div>
        <p class="button_area">
          <input class="button" type="SUBMIT" value="Change" name="SUBMIT">
        </p>
      </div>

    </form>
    
    <div id="v1_v2c_trap_receiver">
    
      <h3>Actually configured Trap Receivers</h3>
      <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="label_field">Count:</div>
          <div class="value_field"><span class="count">Unknown</span></div>
        </div>
      </div>
      </div>
      
      <div class="list"></div>
    
    </div>
    
    <h3>Add new Trap Receiver</h3>
    
    <form id="v1-v2c-trap-receiver-add-form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="add_v1_v2c_trap_receiver_address">IP Address:</label></div>
            <div class="value_field"><input id="add_v1_v2c_trap_receiver_address" class="input_field" type="text" name="address" size="43" maxlength="200" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="add_v1_v2c_trap_receiver_community_name">Community Name:</label></div>
            <div class="value_field"><input id="add_v1_v2c_trap_receiver_community_name" class="input_field" type="text" name="community_name" size="43" maxlength="200" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Version:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li><input id="add_v1_v2c_trap_receiver_version_v1" type="radio" name="version" value="v1" checked > 
                  <label for="add_v1_v2c_trap_receiver_version_v1">v1</label></li> 
                <li><input id="add_v1_v2c_trap_receiver_version_v2c" type="radio" name="version" value="v2c" >
                  <label for="add_v1_v2c_trap_receiver_version_v2c">v2c</label></li>
              </ul>
            </div>
          </div>

        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Add" name="SUBMIT">
        </p>
      </div>
    </form>

