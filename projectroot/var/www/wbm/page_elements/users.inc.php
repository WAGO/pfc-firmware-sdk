<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of the users for the Web-based Management</h2>
  
    <p>Changes will take effect immediately.</p>

    <p>For reasons of security, we strongly recommend that you change all 
    passwords including those of Linux ® users. For details, please refer to the manual.</p>
    
    <h3>Change password for selected user</h3>
    <form id="config_user_form" action="javascript_requested.php" method="POST">
    
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="user">Select User:</label></div>
            <div class="value_field">
              <select id="user" name="user" size="1">
                <option value="user">user</option>
                <option value="admin">admin</option>
                <!-- also possible: filled by javaScript or PHP -->
              </select>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="new_password">New Password:</label></div>
            <div class="value_field"><input autocomplete="off" id="new_password" class="input_field" type="password" name="new_password" maxlength="32"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="confirm_password">Confirm Password:</label></div>
            <div class="value_field"><input autocomplete="off" id="confirm_password" class="input_field" type="password" name="confirm_password" maxlength="32"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="old_password">Old Password:</label></div>
            <div class="value_field"><input autocomplete="off" id="old_password" class="input_field" type="password" name="old_password" maxlength="32"></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Change Password" name="SUBMIT">
        </p>
      </div>
    </form>
    

