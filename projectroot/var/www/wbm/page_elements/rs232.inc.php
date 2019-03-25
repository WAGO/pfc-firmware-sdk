<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of Serial Interface RS232</h2>

    <p>Changes will take effect after the next controller reboot.</p>
    <p>Caution! Switching the owner of RS232 to "Linux Console" can harm the RS485 devices connected to the interface.</p>

    <h3>Serial Interface assigned to</h3>
    <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <!--
          <div class="label_field">Owner:</div>
          -->
          <div class="value_field"><span id="rs232_current_owner"></span></div>
        </div>
      </div>
    </div>
          
    <form id="config_rs232_form" action="javascript_requested.php" method="POST">
      <h3>Assign Owner of Serial Interface (active after next controller reboot)</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <!--
            <div class="label_field">Owner:</div>
            -->
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="linux" type="radio" name="rs232_owner" value="Linux"  >
                  <label for="linux">Linux Console</label>
                </li>
                <li>
                  <input id="none" type="radio" name="rs232_owner" value="None" > 
                  <label for="none">Unassigned</label>
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
    
