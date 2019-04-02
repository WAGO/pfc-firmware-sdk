<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of Host- and Domainname</h2>

    <p>Changes will take effect immediately.</p>

    <form id="hostname_form" action="javascript_requested.php" method="POST">
     
      <h3>Hostname</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Currently used:</label></div>
            <div class="value_field"><span id="actual_hostname"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="hostname">Configured:</label></div>
            <div class="value_field"><input id="hostname" class="input_field" size="36" type="text" name="hostname" maxlength="200" value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="SUBMIT">
        </p>
      </div>
      
    </form>

    <form id="domain_name_form" action="javascript_requested.php" method="POST">
      <h3>Domain Name</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Currently used:</label></div>
            <div class="value_field"><span id="tcpip_common_actual_domainname"></span></div>
          </div>
        <div class="label_value_pair">
            <div class="label_field"><label for="domain-name">Configured:</label></div>
            <div class="value_field"><input id="domain-name" class="input_field" type="text" name="domain-name" maxlength="200" value="" ></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="SUBMIT">
        </p>
      </div>
    </form>


