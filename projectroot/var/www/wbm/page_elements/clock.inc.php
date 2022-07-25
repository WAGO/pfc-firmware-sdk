<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of time and date</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>Date on device</h3>
    <div class="config_form_box">
    
      <form id="date_local_form" action="javascript_requested.php" method="POST"> 
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="date_local">Local:</label></div>
            <div class="value_field"><input id="date_local" class="input_field" type="TEXT" name="date_local" maxlength="16"  value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="SUBMIT" value="Change date" name="Change">
        </p>
      </form>
    </div>
      
    <h3>Time on device</h3>
    <div class="config_form_box group">
      <form id="time_local_form" action="javascript_requested.php" method="POST"> 
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="time_local">Local:</label></div>
            <div class="value_field"><input id="time_local" class="input_field" type="TEXT" name="time_local" maxlength="11"  value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="SUBMIT" value="Change time" name="Change">
        </p>
      </form>
    </div>

    <div class="config_form_box group">
      <form id="time_utc_form" action="javascript_requested.php" method="POST"> 
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="time_utc">UTC:</label></div>
            <div class="value_field"><input id="time_utc" class="input_field" type="TEXT" name="time_utc" maxlength="11"  value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button delete_dns_server_button" type="SUBMIT" value="Change time" name="Change">
        </p>
      </form>
    </div>

    <div class="config_form_box">
      <form id="display_mode_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field"><label for="display_mode">12 hour format:</label></div>
            <div class="value_field"><input id="display_mode" type="checkbox" name="display_mode" value="12-hour-format" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button delete_dns_server_button" type="SUBMIT" value="Change format" name="Change">
        </p>
      </div>
    </form>


    <form id="timezone_form" action="javascript_requested.php" method="POST"> 
      <h3>Timezone</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <!--<div class="label_field"><label for="timezone">Select</label></div>-->
            <div class="value_field">
              <select id="timezone" class="wide" name="timezone" size="1">
                <!-- can also be filled by javaScript -->
                <!-- <option value="EST/EDT: Eastern Standard Time, New York, Toronto">EST/EDT: Eastern Standard Time, New York, Toronto</option>
                <option value="CST/CDT: Central Standard Time, Chicago, Winnipeg">CST/CDT: Central Standard Time, Chicago, Winnipeg</option>
                <option value="MST/MDT: Mountain Standard Time, Denver, Edmonton">MST/MDT: Mountain Standard Time, Denver, Edmonton</option>
                <option value="PST/PDT: Pacific Standard Time, Los Angeles, Whitehorse">PST/PDT: Pacific Standard Time, Los Angeles, Whitehorse</option>
                <option value="GTM/BST: Greenwich Main Time, GB, P, IRL, IS, ...">GTM/BST: Greenwich Main Time, GB, P, IRL, IS, ...</option>
                <option value="CET/CEST: Central European Time, B, DK, D, F, I, CRO, NL, ...">CET/CEST: Central European Time, B, DK, D, F, I, CRO, NL, ...</option>
                <option value="EET/EEST: East European Time, BUL, FI, GR, TR, ...">EET/EEST: East European Time, BUL, FI, GR, TR, ...</option>
                <option value="CST: China Standard Time">CST: China Standard Time</option>
                <option value="JST: Japan / Korea Standard Time">JST: Japan / Korea Standard Time</option> -->
              </select>
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="SUBMIT" value="Change" name="Change">
        </p>
      </div>
    </form>
    
    <form id="tz_string_form" action="javascript_requested.php" method="POST">
      <h3>TZ String</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <!--<div class="label_field"><label for="tz_string">Edit</label></div>-->
            <div class="value_field"><input id="tz_string" class="input_field" type="TEXT" name="tz_string" size="40" maxlength="80"  value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="SUBMIT" value="Change" name="Change">
        </p>
      </div>
    </form>
    

