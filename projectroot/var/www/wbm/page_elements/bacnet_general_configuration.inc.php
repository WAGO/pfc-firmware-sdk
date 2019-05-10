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
    
    <div class="config_form_box group">
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
    
    <div class="config_form_box group">
      <form id="bacnet_pers_del_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field">
              <label for="bacnet_pers_del_state">Delete Persistence<br>Data:</label>
            </div>
            <div class="value_field">
              <input id="bacnet_pers_del_active_state" type="checkbox" name="bacnet_pers_del_active_state" value="true" >
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_pers_del_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>
      </form>
    </div>
    
    <div class="config_form_box">
      <form id="bacnet_del_all_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field">
              <label for="bacnet_del_all_state">Reset all BACnet<br>Data and Settings<br>to Default:</label>
            </div>
            <div class="value_field">
              <input id="bacnet_del_all_active_state" type="checkbox" name="bacnet_del_all_active_state" value="true" >
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_del_all_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>
      </form>
    </div>
    
    <h3>BACnet Upload</h3>
    
    <div class="config_form_box">
      <form id="bacnet_upload_form" action="receive_upload.php" target="upload_bacnet_iframe" method="POST" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_port">New override.xml:</label></div>
            <div class="value_field">
              <input id="bacnet_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="bacnet_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="bacnet_upload_file" id="bacnet_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="bacnet_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="bacnet_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
      
      <form id="bacnet_delete_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">&nbsp;</div>
            <div class="value_field"><span id="spanWbmCopyId"></span></div>
          </div>
        </div>
        <p class="button_area">
          <input id="bacnet_delete_submit_button" class="button" type="submit" value="Delete" name="submit" disabled >
        </p>      
      </form>
    </div>
    
    <iframe id="upload_bacnet_iframe" name="upload_bacnet_iframe" class="upload_iframe"></iframe>