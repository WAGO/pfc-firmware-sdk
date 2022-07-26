<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>
    <h2>Firmware Restore</h2>

    <p><img style="vertical-align:middle" src="images/kaestchen_out.gif">Restore packages from selected source to active partition</p>
    
    <p>Note: restoring system, settings or PLC runtime project will reset the controller.<br>
    Firmware restore is not allowed, if active partition is "Memory Card".<br>
    The decryption passphrase is used for all selected and encrypted backup files.<br>
    For multiple encrypted backup files with different passphrases select files separately.</p>

    <form id="firmware_restore_form" action="receive_upload.php" target="firmware_restore_upload_iframe" method="POST" enctype="multipart/form-data" >

      <input type="hidden" name="upload_type" value="firmware_restore">
      <input type="hidden" name="csrf_id" value="">
      <!-- Let the browser password store fill in this fields.. --!>
      <input type="text" name="bogus_user" style="display:none;"/>
      <input type="password" name="bogus_password" style="display:none;"/>

      <div class="config_form_box">

        <table class="backup_sequence">
          <tr>
            <th class="source">Source</th>
            <th class="packages">Packages</th>
            <th class="destination">Active Partition</th>
          </tr>

          <tr>
            <td class="source">
              <select id="firmware_restore_source_medium_select" class="initial_invisible" name="source_medium" size="1">
              </select>
              <span id="firmware_restore_read_only_source_medium">Unknown</span>
            </td>

            <td class="packages">
              <ul id="firmware_restore_package_list" class="package_list" >
                <li>
                  <input type="checkbox" name="package-all">
                  <label for="package-all">All</label>
                </li>
                <li class="only_codesys">
                  <input type="checkbox" id="firmware_restore_package_checkbox_codesys" name="package-codesys">
                  <label for="firmware_restore_package_checkbox_codesys">PLC Runtime Project</label>
                </li>
                <li>
                  <input type="checkbox" id="firmware_restore_package_checkbox_settings" name="package-settings">
                  <label for="firmware_restore_package_checkbox_settings">Settings</label>
                </li>
                <li>
                  <input type="checkbox" id="firmware_restore_package_checkbox_system" name="package-system">
                  <label for="firmware_restore_package_checkbox_system">System</label>
                </li>
              </ul>
            </td>

            <td class="destination"><span class="active_partition_medium"></span></td>
          </tr>
        </table>


        <div id="firmware_restore_network_source_file">

          <div class=class="only_codesys">
            <div class="config_data_list">
              <div class="label_value_pair">
                <div class="label_field"><label for="codesys_backup_upload_file_fake_input">PLC runtime backup file:</label></div>
                <div class="value_field">
                  <input id="codesys_backup_upload_file_fake_input" class="input_field file_browse_fake_input" type="text" size="30" name="codesys_backup_upload_file_fake_input" value="">
                </div>
              </div>
            </div>
            <p class="button_area">
              <span class="fileinputs">
                <input class="file" type="file" name="codesys_source_file" id="codesys_backup_upload_file_real_input" >
                <span class="fakefile">
                  <button class="button file_browse_fake_button" id="codesys_backup_upload_file_fake_button">Browse</button>
                </span>
              </span>
            </p>
          </div>

          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="settings_backup_upload_file_fake_input">Settings backup file:</label></div>
              <div class="value_field">
                <input id="settings_backup_upload_file_fake_input" class="input_field file_browse_fake_input" type="text" size="30" name="settings_backup_upload_file_fake_input" value="">
              </div>
            </div>
          </div>
          <p class="button_area">
              <span class="fileinputs">
                <input class="file" type="file" name="settings_source_file" id="settings_backup_upload_file_real_input" >
                <span class="fakefile">
                  <button class="button file_browse_fake_button" id="settings_backup_upload_file_fake_button">Browse</button>
                </span>
              </span>
            </p>

          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="system_backup_upload_file_fake_input">System backup file:</label></div>
              <div class="value_field">
                <input id="system_backup_upload_file_fake_input" class="input_field file_browse_fake_input" type="text" size="30" name="system_backup_upload_file_fake_input" value="">
              </div>
            </div>
          </div>
          <p class="button_area">
              <span class="fileinputs">
                <input class="file" type="file" name="system_source_file" id="system_backup_upload_file_real_input" >
                <span class="fakefile">
                  <button class="button file_browse_fake_button" id="system_backup_upload_file_fake_button">Browse</button>
                </span>
              </span>
            </p>
        </div>
         <div id="decryption_area">
           <div class="config_data_list">
                 <div class="label_value_pair">
            <div class="label_field"><label for="decryption_active_state">Decryption Activation:</label></div>
            <div class="value_field"><input id="decryption_active_state" type="checkbox" value="change_state" checked="checked" name="decryption_active_state" >
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="decryption-passphrase">Decryption Passphrase:</label></div>
            <div class="value_field"><input autocomplete="off" id="decryption-passphrase" class="input_field" type="password" name="decryption-passphrase" size="30" maxlength="50" value=""></div>
          </div>
        </div>
        </div>

        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;
              <!--
              <input id="codesys_backup_upload_file_real_input" class="formular_feld real_file" name="codesys_source_file" type="file" >
              <input id="settings_backup_upload_file_real_input" class="formular_feld real_file" name="settings_source_file" type="file" >
              <input id="system_backup_upload_file_real_input" class="formular_feld real_file" name="system_source_file" type="file" >
               -->
            </div>
          </div>
        </div>
        <p class="button_area">
          <input id="firmware_restore_submit_button" class="button" type="submit" value="Submit" name="submit" >
        </p>
      </div>

    </form>

    <iframe id="firmware_restore_upload_iframe" name="firmware_restore_upload_iframe" class="upload_iframe"></iframe>
