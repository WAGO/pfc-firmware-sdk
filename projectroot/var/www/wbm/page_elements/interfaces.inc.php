<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>TCP/IP Configuration</h2>
	  
    <p>Changes will take effect immediately.</p>
    <p>Changes to the settings can have an 
    impact on the DHCPD server. Please check that the DHCPD configuration is still correct 
    (see page "Ports and Services" - "DHCPD").</p>
    
    <div id="ip_address_config_area"></div>

    <div id="default_gateway_config_area">
        <h3>Gateway Configuration</h3> 
        <div class="config_form_box" >
            <div class="config_data_list">
            <div class="label_value_pair">
                <div class="value_field">
                <span>Note: The gateway configuration is performed via the 
                "<a class="content_area_link" href="index.php#routing">Networking - Routing</a>" 
                page under "Static Route". A route is a gateway.</span>
                </div>
            </div>
            </div>
        </div>
    </div>
    
    <div id="dns_server_config_area"></div>
    
    
