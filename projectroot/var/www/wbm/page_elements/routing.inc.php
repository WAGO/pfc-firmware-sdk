<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Routing</h2>
    
    <p>Changes will take effect immediately.</p>
    <p>Notice: Switched network interfaces do not provide routing in the sense of forwarding IP packets.</p>

    <h3>IP Forwarding through multiple interfaces</h3>
    <form id="config_routing" action="javascript_requested.php" method="POST">

      <!-- GENERAL CONFIG BEGIN -->
      <div class="config_form_box">
        <div class="config_data_list" >
          <div class="label_value_pair">
            <div class="label_field"><label for="routing_state">Enabled:</label></div>
            <div class="value_field"><input id="routing_state" type="checkbox" name="routing_state"></div>
          </div>
        </div>
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
      <!-- GENERAL CONFIG END -->

    </form>

    <h3>Static Routes</h3>
    <div id="static_routes_area">

      <!-- REPEAT BEGIN  -  for every static route -->
      <div class="config_form_box group" style="display: none">
        <form id="template_static_route" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="static_route_state">Enabled:</label></div>
              <div class="value_field"><input type="checkbox" name="static_route_state"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="static_route_destination_address">Destination Address:</label></div>
              <div class="value_field"><input class="input_field" type="text" name="static_route_destination_address" value=""></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="static_route_destination_mask">Destination Mask:</label></div>
              <div class="value_field"><input class="input_field" type="text" name="static_route_destination_mask" value=""></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="static_route_gateway_address">Gateway Address:</label></div>
              <div class="value_field"><input class="input_field" type="text" name="static_route_gateway_address" value=""> </div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_delete_static_route" class="button" type="submit" value="Delete" name="delete">
          </p>
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="static_route_gateway_metric">Gateway Metric:</label></div>
              <div class="value_field"><input class="input_field" type="text" name="static_route_gateway_metric" size="4" value=""> </div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_change_static_route" class="button" type="submit" value="Submit" name="submit">
          </p>
        </form>
      </div>
      <!-- REPEAT END -->

      <!-- ADD NEW STATIC ROUTE BEGIN -->
      <div class="config_form_box">
        <form id="add_static_route" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="new_state">Enabled:</label></div>
              <div class="value_field"><input id="new_state" type="checkbox" name="new_state"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_destination_address">Destination Address:</label></div>
              <div class="value_field"><input id="new_destination_address" class="input_field" type="text" name="new_destination_address" value=""></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_destination_mask">Destination Mask:</label></div>
              <div class="value_field"><input id="new_destination_mask" class="input_field" type="text" name="new_destination_mask" value=""></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_gateway_address">Gateway Address:</label></div>
              <div class="value_field"><input id="new_gateway_address" class="input_field" type="text" name="new_gateway_address" value=""> </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_gateway_metric">Gateway Metric:</label></div>
              <div class="value_field"><input id="new_gateway_metric" class="input_field" type="text" name="new_gateway_metric" size="4" value=""> </div>
            </div>
          </div>
      
          <p class="button_area">
            <input id="add_static_route" class="button" type="submit" value="Add" name="submit">
          </p>
        </form>
        
        <!-- ADD NEW STATIC ROUTE END -->

      </div>

    </div>

    <h3>Dynamic Routes</h3>
    <div id="dynamic_routes_area">

      <!-- IF NO DYNAMIC ROUTES ARE AVAILABLE -->
      <div id="no_dynamic_routes_info" class="config_form_box" style="display: none;">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">
              <span>(no dynamic routes)</span>
            </div>
          </div>
        </div>
      </div>
      <!-- IF END -->

      <!-- REPEAT BEGIN  -  for every dynamic route -->
      <div class="config_form_box group" style="display: none;">
        <form id="template_dynamic_route" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field">Destination Address:</div>
              <div class="value_field">
                <span class="dynamic_route_destination_address">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Destination Mask:</div>
              <div class="value_field">
                <span class="dynamic_route_destination_mask">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Gateway Address:</div>
              <div class="value_field">
                <span class="dynamic_route_gateway_address">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Gateway Metric:</div>
              <div class="value_field">
                <span class="dynamic_route_gateway_metric">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Source:</div>
              <div class="value_field">
                <span class="dynamic_route_source">-</span>
              </div>
            </div>
          </div>
        </form>
      </div>
      <!-- REPEAT END -->

    </div>


    <h3>IP Masquerading</h3>
    <div id="masquerading_area">

      <!-- REPEAT BEGIN  -  for every masquerading entry -->
      <div class="config_form_box group" style="display: none;">
        <form id="template_masquerading" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="masquerading_enabled">Enabled:</label></div>
              <div class="value_field"><input id="masquerading_enabled" type="checkbox" name="masquerading_enabled"></div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_delete_masquerading" class="button" type="submit" value="Delete" name="delete">
          </p>
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field">Interface:</div>
              <div class="value_field">
                <span class="masquerading_interface">-</span>
              </div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_change_masquerading" class="button" type="submit" value="Submit" name="submit">
          </p>
        </form>
      </div>
      <!-- REPEAT END -->

      <!-- ADD NEW MASQUERADING BEGIN -->
      <div class="config_form_box">
        <form id="add_masquerading" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="new_masquerading_enabled">Enabled:</label></div>
              <div class="value_field"><input id="new_masquerading_enabled" type="checkbox" name="new_masquerading_enabled"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_masquerading_interface">Interface:</label></div>
              <div class="value_field">
                <ul class="check_list_important">
                  <li><input id="routing_add_masquerading_interface_X1" type="radio" name="new_masquerading_interface" value="X1" checked="checked">
                    <label for="routing_add_masquerading_interface_X1">X1</label>
                  </li>
                  <li><input id="routing_add_masquerading_interface_X2" type="radio" name="new_masquerading_interface" value="X2">
                    <label for="routing_add_masquerading_interface_X2">X2</label>
                  </li>
                  <li><input id="routing_add_masquerading_interface_other" type="radio" name="new_masquerading_interface" value="other">
                    <label for="routing_add_masquerading_interface_other">other: </label>
                    <input class="input_field" name="new_masquerading_interface_other" type="text" size=4>
                  </li>
                </ul>
              </div>
            </div>
          </div>
          <p class="button_area">
            <input id="add_masquerading_button" class="button" type="submit" value="Add" name="submit">
          </p>
        </form>
      </div>
      <!-- ADD NEW MASQUERADING END -->

    </div>

  
    <h3>Port Forwarding</h3>
    <div id="forwarding_area">

      <!-- REPEAT BEGIN  -  for every forwarding entry -->
      <div class="config_form_box group" style="display: none;">
        <form id="template_forwarding" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="forwarding_enabled">Enabled:</label></div>
              <div class="value_field"><input id="forwarding_enabled" type="checkbox" name="forwarding_enabled"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Interface:</div>
              <div class="value_field">
                <span class="forwarding_interface">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Port:</div>
              <div class="value_field">
                <span class="forwarding_port">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Protocol:</div>
              <div class="value_field">
                <span class="forwarding_protocol">-</span>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Destination Address:</div>
              <div class="value_field">
                <span class="forwarding_destination_address">-</span>
              </div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_delete_forwarding" class="button" type="submit" value="Delete" name="delete">
          </p>
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field">Destination Port:</div>
              <div class="value_field">
                <span class="forwarding_destination_port">-</span>
              </div>
            </div>
          </div>
          <p class="button_area">
            <input id="template_change_forwarding" class="button" type="submit" value="Submit" name="submit">
          </p>
        </form>
      </div>
      <!-- REPEAT END -->

      <!-- ADD NEW FORWARDING BEGIN -->
      <div class="config_form_box">
        <form id="add_forwarding" action="javascript_requested.php" method="POST">
          <div class="config_data_list">
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_enabled">Enabled:</label></div>
              <div class="value_field"><input id="new_forwarding_enabled" type="checkbox" name="new_forwarding_enabled"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_enabled">Interface:</label></div>
              <div class="value_field">
                <ul class="check_list_important">
                  <li><input id="routing_add_forwarding_interface_X1" type="radio" name="new_forwarding_interface" value="X1" checked="checked">
                    <label for="routing_add_forwarding_interface_X1">X1</label>
                  </li>
                  <li><input id="routing_add_forwarding_interface_X2" type="radio" name="new_forwarding_interface" value="X2">
                    <label for="routing_add_forwarding_interface_X2">X2</label>
                  </li>
                  <li><input id="routing_add_forwarding_interface_other" type="radio" name="new_forwarding_interface" value="other">
                    <label for="routing_add_forwarding_interface_other">other: </label>
                    <input class="input_field" name="new_forwarding_interface_other" type="text" size=4>
                  </li>
                </ul>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_port">Port:</label></div>
              <div class="value_field"><input id="new_forwarding_port" class="input_field" type="text" size="4" name="new_forwarding_port"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_enabled">Protocol:</label></div>
              <div class="value_field">
                <ul class="check_list_important">
                  <li>
                    <input id="new_forwarding_protocol_tcp" type="checkbox" name="new_forwarding_protocol_tcp">
                    <label for="new_forwarding_protocol_tcp">TCP</label>
                  </li>
                  <li>
                    <input id="new_forwarding_protocol_udp" type="checkbox" name="new_forwarding_protocol_udp">
                    <label for="new_forwarding_protocol_udp">UDP</label>
                  </li>
                </ul>
              </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_destination_address">Destination Address:</label></div>
              <div class="value_field"><input id="new_forwarding_destination_address" class="input_field" type="text" name="new_forwarding_destination_address"></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field"><label for="new_forwarding_destination_port">Destination Port:</label></div>
              <div class="value_field"><input id="new_forwarding_destination_port" class="input_field" type="text" size="4" name="new_forwarding_destination_port"></div>
            </div>
          </div>
          <p class="button_area">
            <input id="add_forwarding_button" class="button" type="submit" value="Add" name="submit">
          </p>
        </form>
      </div>
      <!-- ADD NEW FORWARDING END -->

    </div>
