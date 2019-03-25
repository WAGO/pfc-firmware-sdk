<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of internal 3G Modem</h2>
	  
    <p>Changes will take effect immediately.</p>
    
    <!-- 
    <div id="modem_information_area">
      <h3>Modem State</h3>
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair" id="modem_state"" >
            <div class="label_field">Modem State:</div>
            <div class="value_field"><span id="state_output_modem_state">-</span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Manufacturor:</div>
            <div class="value_field"><span id="modem_manufacturor"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Model:</div>
            <div class="value_field"><span id="modem_model"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Revision:</div>
            <div class="value_field"><span id="modem_revision"></span></div>
          </div>
        </div>
      </div>
    <div>
    -->
    
    <div id="modem_sim_area"></div>
    
    <div id="modem_mobile_network_area">
      <h3>Mobile Network Configuration</h3>
      <form id="mobile_network_config_form" action="javascript_requested.php" method="post">
        <div class="config_form_box">
          <div class="config_data_list">
            <div class="label_value_pair" id="modem_network_state" >
              <div class="label_field">State:</div>
              <div class="value_field"><span id="state_output_modem_network_state">-</span><span id="modem_network_state_text"></span></div>
            </div>
        
            <div class="label_value_pair">
              <div class="label_field">Signal Quality (%):</div>
              <div class="value_field">
                <span id="modem_signal_strength_img_area"></span>
                <span id="modem_signal_strength"></span>
                </div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Operator:</div>
              <div class="value_field"><span id="modem_operator"></span></div>
            </div>
            <div class="label_value_pair">
              <div class="label_field">Selection Mode:</div>
              <div class="value_field">
                <ul class="check_list_important">
                  <li><input type="radio" id="modem_selection_mode_automatic" name="selection_mode" value="AUTOMATIC" >
                    <label for="modem_selection_mode_automatic">Automatic</label> 
                  </li>
                  <li><input type="radio" id="modem_selection_mode_automatic_umts_preferred" name="selection_mode" value="PREFER_UMTS" >
                    <label for="modem_selection_mode_automatic_umts_preferred">Automatic - UMTS preferred</label> 
                  </li>
                  <li><input type="radio" id="modem_selection_mode_automatic_gsm_preferred" name="selection_mode" value="PREFER_GSM" >
                    <label for="modem_selection_mode_automatic_gsm_preferred">Automatic - GSM preferred</label> 
                  </li>
                  <li><input type="radio" id="modem_selection_mode_automatic_umts_only" name="selection_mode" value="ONLY_UMTS" >
                    <label for="modem_selection_mode_automatic_umts_only">Automatic - UMTS only</label> 
                  </li>
                  <li><input type="radio" id="modem_selection_mode_automatic_gsm_only" name="selection_mode" value="ONLY_GSM" >
                    <label for="modem_selection_mode_automatic_gsm_only">Automatic - GSM only</label> 
                  </li>
                  <li><input type="radio" id="modem_selection_mode_manual" name="selection_mode" value="MANUAL" >
                    <label for="modem_selection_mode_manual">Manual</label>
                  </li>
                  </ul>
              </div>
            </div>
        
            <div class="label_value_pair" id="modem_provider_selection_area">
              <div class="label_field">Provider:</div>
              <div class="value_field">
                <select id="modem_provider_selection" name="provider">
                  <option value="unknown">Unknown</option>
                </select>
              </div>
            </div> 
          
          </div>
        
          <p class="button_area">
            <input class="button" type="submit" value="Submit" name="submit">
          </p>
        </div>
      </form>
    </div>
    
    <h3>Provider List</h3>
    <div id="modem_providerlist_area">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair" >
            <div class="label_field"><span class="state">Unknown</span></div>
          </div>
        </div>

        <form id="mobile_network_refresh_provider_list_form" action="javascript_requested.php" method="post">
          <p class="button_area">
            <input id="modem_refresh_provider_list_button" class="button" type="submit" value="Refresh" name="submit">
          </p>
        </form>
      </div>
    </div>
    
    
    <h3>Network Package Service</h3>
    <form id="mobile_network_package_service_form" action="javascript_requested.php" method="post">
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair" >
            <div class="label_field">Enabled:</div>
            <div class="value_field"><input id="modem_gprs_connectivity" type="checkbox" name="connectivity" ></div>
          </div>
          <div class="label_value_pair" >
            <div class="label_field">State:</div>
            <div class="value_field"><span id="modem_gprs_state"></span></div>
          </div>
          <div class="label_value_pair" >
            <div class="label_field">APN:</div>
            <div class="value_field"><input id="modem_gprs_apn" class="input_field" name="apn"></div>
          </div>
          <div class="label_value_pair" >
            <div class="label_field">User:</div>
            <div class="value_field"><input id="modem_gprs_user" class="input_field" name="user"></div>
          </div>
          <div class="label_value_pair" >
            <div class="label_field">Password:</div>
            <div class="value_field"><input id="modem_gprs_password" class="input_field" name="password"></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Authentication Type:</div>
            <div class="value_field">
              <ul class="check_list_important">
                <li>
                  <input id="modem_grps_auth_type_none" type="radio" name="auth_type" value="NONE" >
                  <label for="modem_grps_auth_type_none">None</label>
                </li>
                <li>
                  <input id="modem_grps_auth_type_pap" type="radio" name="auth_type" value="PAP" >
                  <label for="modem_grps_auth_type_pap">PAP</label>
                </li>
                <li>
                  <input id="modem_grps_auth_type_chap" type="radio" name="auth_type" value="CHAP" >
                  <label for="modem_grps_auth_type_chap">CHAP</label>
                </li>
                <li>
                  <input id="modem_grps_auth_type_pap_or_chap" type="radio" name="auth_type" value="PAP_OR_CHAP" >
                  <label for="modem_grps_auth_type_pap_or_chap">PAP or CHAP</label>
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
    
    <h3>Upload and activate new Modem Software</h3>
    <form id="modem_software_upload_form" action="receive_upload.php" target="upload_modem_software_iframe" method="post" enctype="multipart/form-data">
      <input type="hidden" name="upload_type" value="modem_software_upload">
      <input type="hidden" name="csrf_id" value="">
      
      <div class="config_form_box">
        <div class="config_data_list">
          <div class="label_value_pair" >
            <div class="label_field">Currently used:</div>
            <div class="value_field"><span id="modem_software_version"></span></div>
          </div>
          <div class="label_value_pair" >
            <div class="label_field">New Software:</div>
            <div class="value_field">
              <input id="software_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="software_upload_file_fake_input" value="">
            </div>
          </div>
        </div>    
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="modem_software_upload_file" id="modem_software_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="modem_software_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="modem_software_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </div>
    </form>
    
    <iframe id="upload_modem_software_iframe" name="upload_modem_software_iframe" class="upload_iframe"></iframe>
    
      
