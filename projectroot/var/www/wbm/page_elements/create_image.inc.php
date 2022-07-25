<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>


    <h2>Create bootable image</h2>

    <p>Create a bootable image from active partition and copy it to selected destination. 
    Process will start immediately.</p>

    <!--
    <div>
      <p id="copy_firmware_image_note">Note: if USB stick should be used as boot medium, legacy USB support must be enabled.
      (BIOS -> Advanced -> USB configuration -> Legacy USB support)</p>
    </div>
    -->

    <form id="copy_firmware_image_form" action="javascript_requested.php" method="POST"> 
      <input id="source_medium" type="hidden" name="source_medium" value="">
      <h3>Create bootable image from active partition<span id="active_partition_medium"></span></h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="destination_medium">Destination:</label></div>
            <div class="value_field">
              <select class="initial_invisible" id="create_image_destination_medium" name="destination_medium" size="1" >
                <!-- filled by javaScript -->
              </select>
              <span id="create_image_read_only_destination_medium">Unknown</span>
            </div>
          </div>
          <div class="label_value_pair initial_invisible area_depending_destination_memory_card">
            <div class="label_field"><label for="create_image_target_image_size">Size of created image:</label></div>
            <div class="value_field">
              <select id="create_image_size" name="create_image_size">
                <option value="memory_card_size">Full card size</otion>
                <option value="as_small_as_possible">Reduced to content</option>
              </select>
            </div>
          </div>
        </div>

        <p class="button_area">
          <input id="backup_button" class="warning_button" type="SUBMIT" value="Start Copy" >
        </p>
      </div>
    </form>
    
    <div id="device_check_note" class="warning_box initial_invisible">
      <img alt="unknown" title="unknown" src="images/fragezeichen.gif">
    </div>

    