<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of OpenVPN and IPsec</h2>

    <p>Changes will take effect after the next controller reboot.</p>
    
    <!-- OpenVPN ------------------------------------------------------------- -->
    <h3>OpenVPN</h3>
    
    <div class="config_form_box group">
      <div class="config_data_list">
        <div class="label_value_pair" id="vpn_openvpn_current_state" >
          <div class="label_field">Current State:</div>
          <div class="value_field"><span id="state_output_vpn_openvpn_state"></span></div>
        </div>
      </div>
    </div>
          
    <form id="vpn_openvpn_state_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_openvnp_enabled">OpenVPN enabled:</label></div>
            <div class="value_field">
              <input id="vpn_openvpn_enabled" type="checkbox" name="state" value="enabled" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_vpn_openvpn" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>

    <!-- OpenVPN upload ------------------------------------------------------------- -->
    <div class="config_form_box">
      <form id="vpn_openvpn_conf_upload_form" action="receive_upload.php" target="vpn_upload_iframe" method="post" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_openvpn_conf_upload_file_real_input">openvpn.conf:</label></div>
            <div class="value_field">
              <input id="vpn_openvpn_conf_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="openvpn_conf_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="upload_file" id="vpn_openvpn_conf_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="vpn_openvpn_conf_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_openvpn_conf_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
    
      <!-- OpenVPN download ------------------------------------------------------------- -->
      <form id="vpn_openvpn_conf_download_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
              <!-- 
            <div class="label_field"><label for="vpn_openvnp_upload">open.vpn.conf</label></div>
            <div class="value_field"></div>
             -->
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_openvpn_conf_download" class="button" type="submit" value="Start Download" name="download">
        </p>
      </form>
    </div>
        
    
    <!-- IPsec ------------------------------------------------------------- -->
    <h3>IPsec</h3>
    
    <div class="config_form_box group">
      <div class="config_data_list">
        <div class="label_value_pair" id="vpn_ipsec_current_state" >
          <div class="label_field">Current State:</div>
          <div class="value_field"><span id="state_output_vpn_ipsec_state"></span></div>
        </div>
      </div>
    </div>
          
    <form id="vpn_ipsec_state_form" action="javascript_requested.php" method="POST">
      <div class="config_form_box group">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_openvnp_enabled">IPsec enabled:</label></div>
            <div class="value_field">
              <input id="vpn_ipsec_enabled" type="checkbox" name="state" value="enabled" >
            </div>
          </div>
        </div>
        
        <p class="button_area">
          <input id="change_state_button_vpn_ipsec_button" class="button" type="submit" value="Submit" name="change_state">
        </p>
      </div>
    </form>

    <!-- IPsec.conf upload  ------------------------------------------------------------- -->
    <div class="config_form_box group">
      <form id="vpn_ipsec_conf_upload_form" action="receive_upload.php" target="vpn_upload_iframe" method="post" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_ipsec_conf_upload_file_real_input">ipsec.conf:</label></div>
            <div class="value_field">
              <input id="vpn_ipsec_conf_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="openvpn_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="upload_file" id="vpn_ipsec_conf_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="vpn_ipsec_conf_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_ipsec_conf_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
    
      <!-- ipsec.conf download  ------------------------------------------------------------- -->
      <form id="vpn_ipsec_conf_download_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_ipsec_conf_download_button" class="button" type="submit" value="Start Download" name="download">
        </p>
      </form>
    </div>
    
    <!-- ipsec.secrets upload  ------------------------------------------------------------- -->
    <div class="config_form_box">
      <form id="vpn_ipsec_secrets_upload_form" action="receive_upload.php" target="vpn_upload_iframe" method="post" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_ipsec_secrets_upload_file_real_input">ipsec.secrets:</label></div>
            <div class="value_field">
              <input id="vpn_ipsec_secrets_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="ipsec_secrets_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="upload_file" id="vpn_ipsec_secrets_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="vpn_ipsec_secrets_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_ipsec_secrets_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
    
      <!-- IPsec.secrets download ------------------------------------------------------------- -->
      <!-- 
      <form id="vpn_ipsec_secrets_download_form" action="javascript_requested.php" method="POST">
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_ipsec_secrets_download_button" class="button" type="submit" value="Start Download" name="download">
        </p>
      </form>
       -->
    </div>
    
    <!-- Certificates ------------------------------------------------------------- -->
    
    <h3>Certificate Upload</h3>
    <div class="config_form_box group">
      <form id="vpn_certificate_upload_form" action="receive_upload.php" target="vpn_upload_iframe" method="post" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">
      
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_certificate_upload_file_real_input">New Certificate:</label></div>
            <div class="value_field">
              <input id="vpn_certificate_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="certificate_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="upload_file" id="vpn_certificate_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="vpn_certificate_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_certificate_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
    </div>
    
    <div class="config_form_box">
      <form id="vpn_key_upload_form" action="receive_upload.php" target="vpn_upload_iframe" method="post" enctype="multipart/form-data">
        <input type="hidden" name="upload_type" value="default">
        <input type="hidden" name="csrf_id" value="">

        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field"><label for="vpn_key_upload_file_real_input">New Private Key:</label></div>
            <div class="value_field">
              <input id="vpn_key_upload_file_fake_input" size="30" class="input_field file_browse_fake_input" type="text" name="key_upload_file_fake_input" value="">
            </div>
          </div>
        </div>
        <p class="button_area">
          <span class="fileinputs">
            <input class="file" type="file" name="upload_file" id="vpn_key_upload_file_real_input" >
            <span class="fakefile">
              <button class="button file_browse_fake_button" id="vpn_key_upload_file_fake_button">Browse</button>
            </span>
          </span>
        </p>
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">&nbsp;</div>
          </div>
        </div>
        <p class="button_area">
          <input id="vpn_key_upload_submit_button" class="button" type="submit" value="Start Upload" name="submit" disabled >
        </p>
      </form>
    </div>
    
    <h3>Certificate List</h3>
    <div id="vpn_certificate_area">
    </div>
    
    <h3>Private Key List</h3>
    <div id="vpn_key_area">
    </div>
    
    <iframe id="vpn_upload_iframe" name="vpn_upload_iframe" class="upload_iframe"></iframe>
    <iframe id="vpn_download_iframe" name="vpn_download_iframe" class="upload_iframe" ></iframe>
    
    