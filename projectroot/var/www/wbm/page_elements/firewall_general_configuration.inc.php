<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>General Firewall Configuration</h2>
    
    <p>Changes will take effect immediately.</p>

    <h3>Global Firewall Parameter</h3>
     
    <form id="form_firewall_state" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="label_field" ><label for="firewall_state">Firewall enabled entirely:</label></div>
            <div class="value_field" id="firewall_state" >
              <input id="firewall_state" type="checkbox" name="firewall_state">
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>
     
    <form id="form_firewall_icmp_echo_broadcast_protection" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="label_field" ><label for="state_firewall_icmp_echo_broadcast_protection">ICMP echo broadcast protection:</label></div>
            <div class="value_field" id="state_firewall_icmp_echo_broadcast_protection" >
              <input id="state_firewall_icmp_echo_broadcast_protection" type="checkbox" name="state_firewall_icmp_echo_broadcast_protection">
            </div>
          </div>
        </div>
        <p class="button_area">
          <input id="change_state_button_firewall_icmp_echo_broadcast_protection" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>
     
    <form id="form_firewall_limit_configuration" action="javascript_requested.php" method="POST">
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="firewall_max_udp_connections">Max. UDP connections per <span class="firewall_max_udp_connections_time_unity">???</span>:</label></div>
            <div class="value_field">
              <input id="firewall_max_udp_connections" class="input_field" type="text" size="5" name="firewall_max_udp_connections">
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="firewall_max_tcp_connections">Max. TCP connections per <span class="firewall_max_tcp_connections_time_unity">???</span>:</label></div>
            <div class="value_field">
              <input id="firewall_max_tcp_connections" class="input_field" type="text" size="5" name="firewall_max_tcp_connections">
            </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
    </form>

    <div id="firewall_interface_general_config_area">
    </div>
     
