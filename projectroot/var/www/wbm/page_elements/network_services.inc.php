<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Configuration of Network Services</h2>

    <p>Changes will take effect immediately.</p>
    <p>Note: After disabling HTTP or HTTPS service, Web-based Management will possibly lose connection to the controller.</p>

    <div id="network_services_interface_state_area">

      <h3>FTP</h3>
      <div id="ftp_service_config_area"></div>

      <h3>FTPS</h3>
      <div id="ftps_service_config_area"></div>

      <h3>HTTP</h3>
      <div id="http_service_config_area"></div>

      <h3>HTTPS</h3>
      <div id="https_service_config_area"></div>

      <h3>I/O-Check</h3>
      <div id="iocheck_port_config_area"></div>

      <div class="initial_invisible 3s_opcua">
        <p>Changing the OPC UA server state will take effect after next controller reboot.</p>
  
        <h3>OPC UA</h3>
        <div id="opcua_config_area"></div>
      </div>

    </div>
