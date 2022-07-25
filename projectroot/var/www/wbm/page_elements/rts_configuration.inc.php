<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>General PLC Runtime Configuration</h2>
	  
    <p>Changing PLC runtime version will take effect immediately.<br>
    Note: All data of the currently running runtime system will be deleted. Same function as Reset (origin).</p>
    
    <p>Changing home directory for the PLC boot project will take effect after the next controller reboot.<br>
    A Reset (origin) is required prior to submit a change.</p>
    
    <h3>General PLC Runtime Configuration</h3>
    
    <form id="codesys_version_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">PLC runtime version:</div>
            <div class="value_field">
              <ul class="check_list_important rts_version_list">
              </ul>
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="SUBMIT">
        </p>
      </div>
    </form>
        
    <form id="rts_configuration_home_directory_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="rts_configuration_home_directory_on_sd_card_enabled">Home directory on memory card enabled:</label></div>
            <div class="value_field" >
              <input id="rts_configuration_home_directory_on_sd_card_enabled" type="checkbox" name="home_directory_on_sd_card_enabled">
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

