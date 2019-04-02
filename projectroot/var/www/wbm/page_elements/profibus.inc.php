<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of PROFIBUS DP Slave</h2>

    <p>Changes will take effect after next controller reboot.</p>

    <h3>PROFIBUS Slave Address</h3>
  
    <form id="ssa_reset_form" action="javascript_requested.php" method="POST">
      <input id="factory_slave_address" type="hidden" value="" name="factory_slave_address">
      <input id="factory_ssa_released" type="hidden" value="" name="factory_ssa_released">
      
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Stored slave address:</div>
            <div class="value_field"><span id="user_slave_address">unknown</span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Slave address assigned by:</div>
            <div class="value_field"><span id="profibus_ssa_slave_address_source_note">unknown</span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Permission to change slave station address:</div>
            <div class="value_field" id="user_ssa_released">unknown</div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">&nbsp;</div>
            <div class="value_field"> </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="ssa-reset-button" class="button" type="submit" value="Reset SSA Values" name="ssa_reset">
        </p>
      </div>
    </form>

