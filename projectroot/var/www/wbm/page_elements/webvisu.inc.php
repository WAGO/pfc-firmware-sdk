<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>PLC WebVisu</h2>
    <p>Changes will take effect immediately.</p>

    <p>Note: The WebVisu is not available, if the corresponding webserver is disabled. 
    The state can be configured on "Ports and Services - PLC Runtime Services" page.</p>

    <p><a href="../webvisu/webvisu.htm" id="webvisu-link" target="_blank" >Open WebVisu in new window</a></p>
    
    <form id="webvisu_default_webserver_form" action="javascript_requested.php" method="POST">
    
      <h3>Webserver Configuration</h3>
      <div class="config_form_box">
        <div class="config_data_list">

          <div class="label_value_pair" id="webvisu_codesys_webserver_state" >
            <div class="label_field">CODESYS 2 Webserver State:</div>
            <div class="value_field"><span id="state_output_webvisu_codesys_webserver">-</span></div>
          </div>
          
          <div class="label_value_pair" id="webvisu_codesys_3_webserver_state" >
            <div class="label_field">e!RUNTIME Webserver State:</div>
            <div class="value_field"><span id="state_output_webvisu_codesys_3_webserver">-</span></div>
          </div>
          
          <div class="label_value_pair">
            <div class="label_field">Default Webserver:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="webvisu_default_webserver_wbm" type="radio" name="default-webserver" value="wbm" >
                  <label for="webvisu_default_webserver_wbm">Web-based Management</label>
                </li>
                <li>
                  <input id="webvisu_default_webserver_codesys" type="radio" name="default-webserver" value="webvisu" >
                  <label for="webvisu_default_webserver_codesys">WebVisu</label>
                </li>
              </ul>
            </div>
          </div>
        </div>
      
        <p class="button_area">
          <input class="button" type="submit" value="Submit" name="submit">
        </p>
      </div>
      
    </form>

