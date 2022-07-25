<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of User Filter</h2>
    
    <p>The firewall blocks communication by default. Filters are available to allow communication with the set parameters.</p>
    <p>Changes will take effect immediately.</p>

    <h3>User filter</h3>
    <div class="config_form_box">
      <div class="config_data_list">
        <div class="label_value_pair">
          <div class="label_field">Count:</div>
          <div class="value_field"><span id="firewall_user_filter_count"></span></div>
        </div>
      </div>
    </div>
     
    <div id="firewall_user_filter_area">
    </div>
    
    <h3>Add new user filter</h3>
    <form id="add_firewall_user_filter_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="add_firewall_user_filer_policy">Policy:</label></div>
            <div class="value_field">
              <ul class="check_list_important policy_selection">
                <li><input type="radio" id="firewall_policy_accept" name="firewall_policy" value="accept" checked/>
                  <label for="firewall_policy_accept">Accept</label>
                </li>
                <li><input type="radio" id="firewall_policy_drop" name="firewall_policy" value="drop"/>
                  <label for="firewall_policy_drop">Drop</label>
                </li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="add_firewall_user_filter_source_ip_address">Source IP address:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_source_ip_address" class="input_field" type="text" name="source_ip_address" size="16"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for ="add_firewall_user_filter_source_netmask">Source netmask:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_source_netmask" class="input_field" type="text" name="source_netmask" size="16"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="add_firewall_user_filter_source_port">Source port:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_source_port" class="input_field" type="text" name="source_port" size="5"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for ="add_firewall_user_filter_destination_ip_address">Destination IP address:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_destination_ip_address" class="input_field" type="text" name="destination_ip_address" size="16"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="add_firewall_user_filter_destination_subnet_mask">Destination subnet mask:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_destination_subnet_mask" class="input_field" type="text" name="destination_subnet_mask" size="16"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field"><label for="add_firewall_user_filter_destination_port">Destination port:</label></div>
            <div class="value_field"><input id="add_firewall_user_filter_destination_port" class="input_field" type="text" name="destination_port" size="5" ></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Protocol:</div>
            <div class="value_field">
              <ul class="check_list_important protocol_selection">
                <!--
                <li>
                  <input id="add_firewall_user_filter_protocol_tcp" type="radio" name="protocol" value="none" checked >
                  <label for="add_firewall_user_filter_protocol_tcp">None</label>
                </li>
                -->
                <li>
                  <input id="add_firewall_user_filter_protocol_tcp" type="checkbox" name="protocol_tcp" value="tcp" >
                  <label for="add_firewall_user_filter_protocol_tcp">TCP</label>
                </li>
                <li>
                  <input id="add_firewall_user_filter_protocol_udp" type="checkbox" name="protocol_udp" value="udp" >
                  <label for="add_firewall_user_filter_protocol_udp">UDP</label>
                </li>
              </ul>
            </div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Input interface:</div>
            <div class="value_field">
              <ul class="check_list_important interfaces_selection">
              </ul>
            </div>
          </div>
        </div>
        <p class="button_area">
          <input id="add_firewall_user_filter_submit_button" class="button" type="submit" value="Add" name="submit">
        </p>
      </div>
    </form>
          
    
