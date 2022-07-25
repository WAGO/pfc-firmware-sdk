<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of Service Interface</h2>

    <p>Changes will take effect after the next controller reboot.</p>

    <h3>Service Interface assigned to</h3>
    <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="value_field"><span id="service_interface_current_owner"></span></div>
        </div>
      </div>
    </div>
          
    <form id="config_service_interface_form" action="javascript_requested.php" method="POST">
      <h3>Assign Owner of Service Interface (active after next controller reboot)</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="service_interface_owner_wago" type="radio" name="service_interface_mode" value="service" >
                  <label for="service_interface_owner_wago">Wago Service Communication</label>
                </li>
                <li>
                  <input id="service_interface_owner_linux" type="radio" name="service_interface_mode" value="linux"  >
                  <label for="service_interface_owner_linux">Linux Console</label>
                </li>
                <li>
                  <input id="service_interface_owner_free" type="radio" name="service_interface_mode" value="free" > 
                  <label for="service_interface_owner_free">Unassigned</label>
                </li>
              </ul>
            </div>
          </div>
        </div>

        <p class="button_area">
          <input class="button" name="submit" type="submit" value="Change Owner" >
        </p>
      </div>
    </form>
    
