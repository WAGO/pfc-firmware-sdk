<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>BACnet Diagnostic Information</h2>

    <form id="refresh_bacnet_diag_list" action="javascript_requested.php" method="POST">
 
      <!--<h3>Count of notifications to read</h3>--> 
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">
              <input id="limit_none" type="radio" name="limit" value="none" >
              <label for="limit_none">Read all notifications</label>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="value_field">
              <input id="limit_count" type="radio" name="limit" value="count" >
              <label for="limit_count">Read only the last</label>
              <input id="count" class="input_field" type="text" name="count" value="" >
            </div>
          </div>
          <div class="label_value_pair">
            <div class="value_field">
              <input id="automatic_refresh_flag" type="checkbox" name="automatic_refresh_flag" value="automatic_refresh" >
              <label for="automatic_refresh_flag">Automatic refresh cycle (sec)</label>
              <input id="automatic_refresh_cycle_time" class="input_field" type="text" name="automatic_refresh_cycle_time" value="" >
            </div>
          </div>
          </div>
      
        <p class="button_area">
          <input class="button" type="submit" value="Refresh" name="refresh">
        </p>
      </div>
    </form>
    
    <div id="bacnet_diag_list"></div>
    
