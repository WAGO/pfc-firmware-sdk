<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of general SNMP parameters</h2>
    
    <p>Changes of Description and ObjectID will take effect after the next software or hardware reset.<br>
    All other changes will take effect immediately.</p>
    
    <h3>General SNMP Configuration</h3>
    
    <form id="general_snmp_config" action="javascript_requested.php" method="POST">
    
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="snmp_active_state">Service active:</label></div>
            <div class="value_field">
              <input id="snmp_active_state"" type="checkbox" value="change_state" name="service_active_state" >
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="device-name">Name of device:</label></div>
            <div class="value_field"><input id="device-name" class="input_field" type="text" name="device-name" size="30" maxlength="200" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="description">Description:</label></div>
            <div class="value_field"><input id="description" class="input_field" type="text" name="description" size="30" maxlength="200" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="physical-location">Physical Location:</label></div>
            <div class="value_field"><input id="physical-location" class="input_field" type="text" name="physical-location" size="30" maxlength="200" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="contact">Contact:</label></div>
            <div class="value_field"><input id="contact" class="input_field" type="text" name="contact" size="30" maxlength="200" value=""></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="sysObjectID">ObjectID:</label></div>
            <div class="value_field"><input id="sysObjectID" class="input_field" type="text" name="sysObjectID" size="30" maxlength="200" value=""></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="SUBMIT">
        </p>
      </div>
       
    </form>

