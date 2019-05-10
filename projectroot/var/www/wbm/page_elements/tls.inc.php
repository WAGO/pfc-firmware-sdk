<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Security Settings</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>Transport Layer Security Settings</h3>
    
    <form id="tls_config_form" class="discard_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">TLS Configuration:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="tls_cipher_list_class_standard" type="radio" name="tls-cipher-list-class" value="standard" >
                  <label for="tls_cipher_list_class_standard">Standard</label>
                </li>
                <li>
                  <input id="tls_cipher_list_class_strong" type="radio" name="tls-cipher-list-class" value="strong" >
                  <label for="tls_cipher_list_class_strong">Strong</label>
                </li>
              </ul>
            </div>
          </div>
        </div>
      
        <p class="button_area">
          <input id="tls_submit_button" class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

