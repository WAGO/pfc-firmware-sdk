<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>NFS settings</h2>
    
    <p>Changes will take effect immediately.</p>

    <form id="nfs_config_form" class="discard_form" action="javascript_requested.php" method="POST">
      <h3>NFS</h3>
      <div class="config_form_box">
        <div class="config_data_list">
      
          <div class="label_value_pair">
            <div class="label_field">NFS enabled</div>
            <div class="value_field"><input id="nfs_enabled" type="checkbox" name="nfs_enabled" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="directory">Directory</label></div>
            <div class="value_field"><input id="directory" class="input_field_wide input_field_greyed" disabled type="text" name="directory" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="target">Target</label></div>
            <div class="value_field"><input id="target" class="input_field_wide input_field_greyed" disabled type="text" name="target" ></div>
          </div>
        </div>
      
        <p class="button_area">
          <input id="nfs_submit_button" class="button" type="submit" value="SUBMIT" name="submit">
        </p>
      </div>
    </form>

  </div>

