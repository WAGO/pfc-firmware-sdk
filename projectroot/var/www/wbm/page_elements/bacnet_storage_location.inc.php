<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of BACnet Storage Location parameters</h2>

    <p>Changes will take effect immediately.</p>     
    
    <h3>BACnet Persistence</h3>
    
    <div class="config_form_box">
      <form id="bacnet_persist_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field"><label for="persist_storage">Storage location:</label></div>
            <div class="value_field">
               <ul id="bacnet_persist_ul" name="bacnet_persist_ul" class="check_list_important">
                <li>
                  <input id="bacnet_persist_internal_flash" type="radio" name="persist_location" value="internal-flash" >
                  <label for="internal_flash">Internal-Flash</label>
                </li>
                <li>
                  <input id="bacnet_persist_sd_card" type="radio" name="persist_location" value="sd-card">
                  <label for="sd_card">SD-Card</label>
                </li>
              </ul>
             </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_persist_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>      
      </form>
    </div>
    
    
    <h3>BACnet Trendlog</h3>
    
    <div class="config_form_box">
      <form id="bacnet_trend_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field"><label for="trend_storage">Storage location:</label></div>
            <div class="value_field">
               <ul id="bacnet_trend_ul" name="bacnet_trend_ul" class="check_list_important">
                <li>
                  <input id="bacnet_trend_internal_flash" type="radio" name="trend_location" value="internal-flash" >
                  <label for="internal_flash">Internal-Flash</label>
                </li>
                <li>
                  <input id="bacnet_trend_sd_card" type="radio" name="trend_location" value="sd-card">
                  <label for="sd_card">SD-Card</label>
                </li>
              </ul>
             </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_trend_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>      
      </form>
    </div>
    
    
    <h3>BACnet Eventlog</h3>
    
    <div class="config_form_box">
      <form id="bacnet_event_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list"> 
          <div class="label_value_pair">
            <div class="label_field"><label for="event_storage">Storage location:</label></div>
            <div class="value_field">
               <ul id="bacnet_event_ul" name="bacnet_event_ul" class="check_list_important">
                <li>
                  <input id="bacnet_event_internal_flash" type="radio" name="event_location" value="internal-flash" >
                  <label for="internal_flash">Internal-Flash</label>
                </li>
                <li>
                  <input id="bacnet_event_sd_card" type="radio" name="event_location" value="sd-card">
                  <label for="sd_card">SD-Card</label>
                </li>
              </ul>
             </div>
          </div>
        </div>
        <p class="button_area">
          <input class="button bacnet_event_submit_button" type="SUBMIT" value="Submit" name="Change">
        </p>      
      </form>
    </div>
    
