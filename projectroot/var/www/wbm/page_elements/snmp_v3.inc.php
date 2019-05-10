<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of SNMP v3 Users</h2>
    
    <p>Changes will take effect after the next software or hardware reset.</p>

    <div id="v3_user">
    
      <h3>Actually configured v3 Users</h3>
      <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="label_field">Count:</div>
          <div class="value_field"><span class="count">Unknown</span></div>
        </div>
      </div>
      </div>
      
      <div class="list"></div>
    
    </div>
    
    <h3>Add new v3 User</h3>
    
    <form id="v3-user-add-form" action="javascript_requested.php" method="POST">
      
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="snmp_v3_auth_name">Security Authentication Name:</label></div>
            <div class="value_field"><input id="snmp_v3_auth_name" class="input_field" type="text" name="auth-name" size="26" maxlength="200" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Authentication Type:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li><input id="snmp_v3_auth_type_none" type="radio" name="auth-type" value="none" checked ><label for="snmp_v3_auth_type_none">None</label></li>
                <li><input id="snmp_v3_auth_type_md5" type="radio" name="auth-type" value="MD5" ><label for="snmp_v3_auth_type_md5">MD5</label></li>
                <li><input id="snmp_v3_auth_type_sha" type="radio" name="auth-type" value="SHA" ><label for="snmp_v3_auth_type_sha">SHA</label></li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="snmp_v3_auth_key">Authentication Key (min. 8 char):</label></div>
            <div class="value_field"><input id="snmp_v3_auth_key" class="input_field" type="text" name="auth-key" size="26" maxlength="200" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Privacy:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li><input id="snmp_v3_privacy_none" type="radio" name="privacy" value="none" checked ><label for="snmp_v3_privacy_none">None</label></li>
                <li><input id="snmp_v3_privacy_des" type="radio" name="privacy" value="DES" ><label for="snmp_v3_privacy_des">DES</label></li>
                <li><input id="snmp_v3_privacy_aes" type="radio" name="privacy" value="AES" ><label for="snmp_v3_privacy_aes">AES</label></li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="snmp_v3_privacy_key">Privacy Key (min. 8 char):</label></div>
            <div class="value_field"><input id="snmp_v3_privacy_key" class="input_field" type="text" name="privacy-key" size="26" maxlength="200" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="snmp_v3_notification_receiver">Notification Receiver IP:</label></div>
            <div class="value_field"><input id="snmp_v3_notification_receiver" class="input_field" type="text" name="notification-receiver" size="26" maxlength="200" ></div>
          </div>

        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Add" name="SUBMIT">
        </p>
      </div>
    
    </form>
      
