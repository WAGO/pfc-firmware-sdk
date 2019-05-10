<?php

include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>
<h2>Advanced Intrusion Detection Environment (AIDE)</h2>
<p>
Run AIDE check or init, if no reference database exists, at startup. This will increase
the time for booting the device!<br>
Remember to write down the hash of the database and compare always the hash value from
the log with the one you wrote down to ensure that it has not been tempered with.
</p>
<h3>Run AIDE check at startup</h3>
<form id="integrity_check_config_form" class="discard_form" action="javascript_requested.php" method="POST">
    <div class="config_form_box">
        <div class="config_data_list">
            <div class="label_value_pair">
            <div class="label_field"><label for="aide_check_active_state">Service active:</label></div>
            <div class="value_field">
                <input id="integrity_check_state" type="checkbox" value="change_state" name="integrity_check_state">
            </div>
        </div>
    </div>
    <p class="button_area">
        <input id="aide_check_submit_button" class="button" type="submit" value="Submit" name="submit">
    </p>
    </div>
</form>
<h3>Control AIDE and show log</h3>
<form id="integrity_log_form" action="javascript_requested.php" method="POST">
  <div class="config_form_box">
    <div class="config_data_list">
      <div class="label_value_pair">
        <div class="value_field">
          <label for="action">Select Action:</label>
          <select id="action" name="action" size="1">
            <option value="readlog">readlog</option>
            <option value="init">init</option>
            <option value="check">check</option>
            <option value="update">update</option>
          </select>
        </div>
      </div>
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
<div id="integrity_log"></div>
