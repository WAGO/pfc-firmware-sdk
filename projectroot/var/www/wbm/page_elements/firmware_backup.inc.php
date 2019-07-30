<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Firmware Backup</h2>

    <p><img style="vertical-align:middle; margin-right:2px;" src="images/kaestchen_in.gif">Save packages from
    active partition to selected destination.</p>
       
    <p>Note: Only one package at a time can be saved via network.</p>
      
    <form id="firmware_backup_form" action="javascript_requested.php" method="POST"> 

      <input type="hidden" name="session_id" value="">
      <!-- Let the browser password store fill in this fields.. --!>
      <input type="text" name="bogus_user" style="display:none;"/>
      <input type="password" name="bogus_password" style="display:none;"/>

    
      <div class="config_form_box">
        <table class="backup_sequence">
          <tr>
            <th class="source">Active Partition</th>
            <th class="packages">Packages</th>
            <th class="destination">Destination</th>
          </tr>
        
          <tr>
            <td class="source"><span class="active_partition_medium"></span></td>
          
            <td class="packages">
              <ul id="firmware_backup_package_list" class="package_list" >
                <li>
                  <input type="checkbox" name="package-all">
                  <label for="package-all">All</label>
                </li>
              
                <li class="only_codesys">
                  <input type="checkbox" id="firmware_backup_package_checkbox_codesys" name="package-codesys">
                  <label for="firmware_backup_package_checkbox_codesys">PLC Runtime Project</label>
                </li>
                <li>
                  <input id="firmware_backup_package_checkbox_settings" type="checkbox" name="package-settings">
                  <label for="firmware_backup_package_checkbox_settings">Settings</label>
                </li>
                <li>
                  <input id="firmware_backup_package_checkbox_system" type="checkbox" name="package-system">
                  <label for="firmware_backup_package_checkbox_system">System</label>
                </li>
              </ul>
            </td>
          
            <td class="destination">

              <select id="firmware_backup_destination_medium_select" class="initial_invisible" name="destination_medium" size="1" >
              </select>
              <span id="firmware_backup_read_only_destination_medium">Unknown</span>
              
            </td>
          </tr>
        </table>

	<div id=encryption_area>
        <div class="label_value_pair">
           <p><input id="encryption_active_state" type="checkbox" value="change_state" checked="checked" name="encryption_active_state" />
           <label for="encryption_active_state">Encryption Activation:</label></p>
        </div>

          <div class="config_data_list">
            <div id="encryption_passwords" >
             <div class="label_value_pair">
              <div class="label_field"><label for="encryption-passphrase">Encryption Passphrase:</label></div>
              <div class="value_field"><input autocomplete="off" id="encryption-passphrase" class="input_field" type="password" name="encryption-passphrase" size="30" maxlength="50" value=""></div>
             </div>
             <div class="label_value_pair">
              <div class="label_field"><label for="confirm_encryption-passphrase">Confirmation Passphrase:</label></div>
              <div class="value_field"><input autocomplete="off" id="confirm_encryption-passphrase" class="input_field" type="password" name="confirm_encryption-passphrase" size="30" maxlength="50" value=""></div>
             </div>
            </div>
          </div>
        </div>

        <div id="firmware_backup_auto_update_area">
          <p><input id="firmware_backup_auto_update_checkbox" type="checkbox" name="auto_update" >
          <label for="firmware_backup_auto_update_checkbox">Activate "auto update feature"</label></p>
        </div>

        <p class="button_area">
          <input id="firmware_restore_submit_button" class="button" type="submit" value="Submit" name="submit" >
        </p>
      </div>
    </form>

    <iframe id="firmware_backup_download_iframe" name="firmware_backup_download_iframe" class="upload_iframe" ></iframe>
