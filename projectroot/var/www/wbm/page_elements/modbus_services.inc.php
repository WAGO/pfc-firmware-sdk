<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Modbus® Services Configuration</h2>
    
    <div class="modbus_no_codesys_3">
      <p>Not available as long as e!RUNTIME is not being used.</p>
    </div>
    
    <div class="modbus_codesys_3">
    
      <p>Changes will take effect immediately.</p>

      <h3>Modbus® TCP</h3>
    
      <form id="modbus_tcp_service_config_form" class="discard_form" action="javascript_requested.php" method="POST">
        <div class="config_form_box">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="modbus_tcp_service_active_state">Service active:</label></div>
              <div class="value_field">
                <input id="modbus_tcp_service_active_state"" type="checkbox" value="change_state" name="service_active_state" >
              </div>
            </div>
          </div>
      
          <p class="button_area">
            <input id="modbus_tcp_service_submit_button" class="button" type="submit" value="Submit" name="submit">
          </p>
        </div>
      </form>

      <h3>Modbus® UDP</h3>
    
      <form id="modbus_udp_service_config_form" class="discard_form" action="javascript_requested.php" method="POST">
        <div class="config_form_box">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="modbus_udp_service_active_state">Service active:</label></div>
              <div class="value_field">
                <input id="modbus_udp_service_active_state"" type="checkbox" value="change_state" name="service_active_state" >
              </div>
            </div>
          </div>
      
          <p class="button_area">
            <input id="modbus_udp_service_submit_button" class="button" type="submit" value="Submit" name="submit">
          </p>
        </div>
      </form>
    </div>

