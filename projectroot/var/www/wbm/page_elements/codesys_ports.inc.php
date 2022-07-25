<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of PLC Runtime Services</h2>

    <p>Changes of port authentication state will take effect after the next controller reboot.<br>All other changes will take effect immediately.</p>

    <h3>General Configuration</h3>
    
    <form id="change_port_2455_password_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="password">Port Authentication Password:</label></div>
            <div class="value_field"><input id="port_2455_new_password" class="input_field" type="password" name="port_2455_new_password" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="confirm_password">Confirm Password:</label></div>
            <div class="value_field"><input id="port_2455_confirm_password" class="input_field" type="password" name="port_2455_confirm_password" ></div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_port_2455_password" class="button" type="submit" value="Submit" name="change_password">
        </p>
      </div>
    </form>
    
    <div id="codesys_ports_codesys_2_area" class="initial_invisible">
    <h3>CODESYS 2 </h3>
    
    <div class="config_form_box group">
      <div class="config_data_list">
        <div class="label_value_pair" id="codesys_services_codesys_2_state"" >
          <div class="label_field">CODESYS 2 State:</div>
          <div class="value_field"><span id="state_output_codesys_services_codesys_2_state">-</span></div>
        </div>
      </div>
    </div>
          
    <form id="codesys_webserver_port_state_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_services_webserver_enabled">Webserver enabled:</label></div>
            <div class="value_field">
              <input id="codesys_services_webserver_enabled" type="checkbox" name="service_state" value="enabled" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_codesys_webserver" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>

    <div class="config_form_box group">
      <form id="codesys_communication_active_state_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_communication_active_state">Communication enabled:</label></div>
            <div class="value_field">
              <input id="codesys_communication_active_state"" type="checkbox" value="change_state" name="service_active_state" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="change_service_active_state">
        </p>
      </form>
    </div>
    
    <div class="config_form_box group">
      <form id="change_codesys_communication_port_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_port">Communication Port Number:</label></div>
            <div class="value_field"><input id="codesys_port" class="input_field" type="text" name="codesys_port" value=""></div>
          </div>
        </div>
        
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="change_port">
        </p>
      </form>
    </div>
    
    <form id="codesys_port_2455_auth_state_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_services_port_authentication_enabled">Port Authentication enabled:</label></div>
            <div class="value_field">
              <input id="codesys_services_port_authentication_enabled" type="checkbox" name="service_state" value="enabled" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_port_2455_authentication" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>
    </div>
      
    
    <div id="codesys_ports_codesys_3_area" class="initial_invisible">
    <h3>e!RUNTIME</h3>
    <div class="config_form_box group">
      <div class="config_data_list">
        <div class="label_value_pair" id="codesys_services_codesys_3_state"" >
          <div class="label_field">e!RUNTIME State:</div>
          <div class="value_field"><span id="state_output_codesys_services_codesys_3_state">-</span></div>
        </div>
      </div>
    </div>
          
    <form id="codesys_webserver_3_port_state_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="codesys_services_webserver_3_enabled">Webserver enabled:</label></div>
            <div class="value_field">
              <input id="codesys_services_webserver_3_enabled" type="checkbox" name="service_state" value="enabled" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_codesys_3_webserver" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>
    
    <form id="codesys_port_codesys3_port_2455_authentication_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <!--
            <div class="label_field">Current State:</div>
            <div class="value_field" id="state_output_port_2455_authentication">-</div>
            -->
            <div class="label_field"><label for="codesys_services_codesys3_port_authentication_enabled">Port Authentication enabled:</label></div>
            <div class="value_field">
              <input id="codesys_services_codesys3_port_authentication_enabled" type="checkbox" name="service_state" value="enabled" >
            </div>
            </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_codesys3_port_2455_authentication" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>
    </div>
      
    