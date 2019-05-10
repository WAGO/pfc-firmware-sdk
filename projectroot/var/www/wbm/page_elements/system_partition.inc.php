<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>System Partition</h2>

    <p>Changes will take effect when controller boots from NAND next time.</p>

    <p>Note: Controller will boot the other NAND system partition. <br>
    This partition must contain a functional firmware backup!</p>
        
    <h3>Current active partition</h3>
    <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="label_field"><span class="active_partition"></span> <span class="active_partition_medium"></span></div>
          <div class="value_field"> </div>
        </div>
      </div>
    </div>
          
    <h3>Set inactive NAND partition active</h3>
    <div class="config_form_box">
      <p class="process_button_area">
        <input id="start_backup_system_button" class="warning_button" type="SUBMIT" value="Activate Partition" >
      </p>
    </div>

