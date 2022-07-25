<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

  <h2>Software Uploads</h2>

    <p>First step is to upload the new software. Second is to activate it. Afterwards, 
    the uploaded file will automatically be deleted. Rebooting will delete the file automatically.</p>
    
    <h3>Upload new software</h3>

    <form id="package_update_upload_form" action="receive_upload.php" target="upload_package_update_iframe" method="post" enctype="multipart/form-data">
      <input type="hidden" name="upload_type" value="software_upload">
      <input type="hidden" name="csrf_id" value="">
      
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_port">Software file:</label></div>
            <div class="value_field">
              <input id="software_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="software_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="software_upload_file" id="software_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="software_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="software_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </div>
    </form>
      
    
    <iframe id="upload_package_update_iframe" name="upload_package_update_iframe" class="upload_iframe"></iframe>
    

    <h3>Activate new software</h3>
    
    <form id="package_update_activate_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Software File:</div>
            <div class="value_field"><span class="actual_update_package">Unknown</span></div>
          </div>
      
          <div class="label_value_pair upload_handling_area initial_invisible">
            <div class="label_field">Action:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="package_update_activate_radiobutton" type="radio" name="package_update_handling" value="activate" checked >
                  <label for="package_update_activate_radiobutton">Activate</label>
                </li>
                  <input id="package_update_force_radiobutton" type="radio" name="package_update_handling" value="force">
                  <label for="package_update_force_radiobutton">Force (Manual reboot afterwards needed)</label>
                </li>
                <li> 
                  <input id="package_update_delete_radiobutton" type="radio" name="package_update_handling" value="delete" >
                  <label for="package_update_delete_radiobutton">Discard (delete upload)</label>
                </li>
              </ul> 
            </div>
          </div>
        </div>
        
        <div class="upload_handling_area initial_invisible">
          <p class="button_area">
            <input class="button" type="submit" value="Submit" name="submit" >
          </p>
        </div>
      
      </div>
    </form>
    
