<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("wbm_lib.inc.php");
include("session_lifetime.inc.php");

session_start();

if(isset($_SESSION["ip_client"]))
{
  if(!Session_CheckClientIP())
    Session_DestroySession();
}

if(isset($_SESSION["userAgent"]))
{
  if(!Session_CheckUserAgent())
    Session_DestroySession();
}

$csrfToken = $_SESSION["csrf_token"];
$username   = isset($_SESSION["username"]) ? $_SESSION["username"] : "";


//echo "username:".$username."<br>csrfToken:".$csrfToken;

?>
<!DOCTYPE html>

<html>
<head>
<title>WAGO Ethernet Web-based Management</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta name="author" content="WAGO Kontakttechnik GmbH & Co. KG">


<link rel="stylesheet" type="text/css" href="css/styles.css">

</head>

<!------------------------------------------------------------
< HTML BODY
<------------------------------------------------------------->

<body>

<div id="page">

  <!-------------------------------------------------------------------
  < HEAD AREA
  < ------------------------------------------------------------------->
  <div id="head_area">

    <h1>Web-based Management</h1>
    <div class="user_management_area"></div>

    <?php
    // include a link to the new wbm, if it is installed
    if (file_exists('/var/www/wbm-ng')) {
      printf("<div id=\"new_wbm_area\"><a href=\"/wbm-ng\">Open New WBM</a></div>");
    }
    ?>
    
    <p id="value_coupler_info_text"></p>

  </div>
  <div class="horizontal-line"></div>

  <!-------------------------------------------------------------------
  < STATUS AREA
  < ------------------------------------------------------------------->
  <?php WBM_PrintStatusFrame();?>

  <!-------------------------------------------------------------------
  < MAIN NAVIGATION AREA
  < ------------------------------------------------------------------->
  <?php WBM_PrintNavFrame(); ?>

  <!-------------------------------------------------------------------
  < CONTENT AREA
  < ------------------------------------------------------------------->
  <div id="content_area">

    <div class='single_content_page' id='error_content'>
      <h2>Error</h2>
      <p>Requested content is not available.</p>
      <p class="content_page_error"></p>
    </div>

  </div>
</div>

<?php WBM_PrintFooter(); ?>

</body>


<!------------------------------------------------------------
< JAVA SCRIPT FUNCTIONS
<------------------------------------------------------------->
<script type="text/javascript">

var wbmWorking = true;

if(("Microsoft Internet Explorer" === window.navigator.appName) && (9 >= parseInt(document.documentMode, 10)))
{
  wbmWorking = false;
}

// polyfill for indexOf, which is not supported by IE8
// Production steps of ECMA-262, Edition 5, 15.4.4.14
// Reference: http://es5.github.io/#x15.4.4.14
if (!Array.prototype.indexOf) {
  Array.prototype.indexOf = function(searchElement, fromIndex) {
    for (var i = fromIndex; i < this.length; i++) {
      if (this[i] === searchElement) return i;
    }
    return -1; // not found
  };
}

</script>

<script type="text/javascript" src="js/jquery-1.12.4.min.js"></script>
<script type="text/javascript" src="js/iplib.js"></script>
<script type="text/javascript" src="js/jquery.xml2json.js"></script>

<script type="text/javascript" src="js/file-iterator.js"></script>
<script type="text/javascript" src="js/xhr.js"></script>
<script type="text/javascript" src="js/file-transfer.js"></script>
<script type="text/javascript" src="js/upload.js"></script>

<script type="text/javascript" src="js/general_definitions.js"></script>
<script type="text/javascript" src="js/page_buildup.js"></script>
<script type="text/javascript" src="js/tooltip.js"></script>
<script type='text/javascript' src="js/device_communication.js"></script>
<script type="text/javascript" src="js/device_params.js"></script>
<script type="text/javascript" src="js/device_param_list.js.php"></script>
<script type='text/javascript' src="js/state_area.js"></script>
<script type="text/javascript" src="js/head_station_info.js"></script>
<script type="text/javascript" src="js/user_management.js"></script>

<script type='text/javascript' src="js/interface_config.js"></script>
<script type='text/javascript' src="js/information.js"></script>
<script type='text/javascript' src="js/rts_info.js"></script>
<script type='text/javascript' src="js/rts_configuration.js"></script>
<script type='text/javascript' src="js/webvisu.js"></script>
<script type='text/javascript' src="js/tcpip_common_configuration.js"></script>
<script type='text/javascript' src="js/interfaces.js"></script>
<script type='text/javascript' src="js/default_gateway.js"></script>
<script type='text/javascript' src="js/dns_server.js"></script>
<script type='text/javascript' src="js/ethernet.js"></script>
<script type='text/javascript' src="js/firewall_general_configuration.js"></script>
<script type='text/javascript' src="js/firewall_mac_address_filter.js"></script>
<script type='text/javascript' src="js/firewall_user_filter.js"></script>
<script type='text/javascript' src="js/firewall_service_configuration.js"></script>
<script type='text/javascript' src="js/clock.js"></script>
<script type='text/javascript' src="js/users.js"></script>
<script type='text/javascript' src="js/create_image.js"></script>
<script type='text/javascript' src="js/rs232.js"></script>
<script type='text/javascript' src="js/service_interface.js"></script>
<script type='text/javascript' src="js/start_reboot.js"></script>
<script type='text/javascript' src="js/firmware_backup.js"></script>
<script type='text/javascript' src="js/firmware_restore.js"></script>
<script type='text/javascript' src="js/system_partition.js"></script>
<script type='text/javascript' src="js/mass_storage.js"></script>
<script type='text/javascript' src="js/software_uploads.js"></script>
<script type='text/javascript' src="js/network_services.js"></script>
<script type='text/javascript' src="js/ntp.js"></script>
<script type='text/javascript' src="js/codesys_ports.js"></script>
<script type='text/javascript' src="js/ssh.js"></script>
<script type='text/javascript' src="js/tftp.js"></script>
<script type='text/javascript' src="js/dhcpd.js"></script>
<script type='text/javascript' src="js/dns_service.js"></script>
<script type='text/javascript' src="js/modbus_services.js"></script>
<script type='text/javascript' src="js/snmp_general.js"></script>
<script type='text/javascript' src="js/snmp_v1_v2c.js"></script>
<script type='text/javascript' src="js/snmp_v3.js"></script>
<script type='text/javascript' src="js/diagnostic.js"></script>
<script type='text/javascript' src="js/profibus.js"></script>
<script type='text/javascript' src="js/modem.js"></script>
<script type='text/javascript' src="js/vpn.js"></script>
<script type='text/javascript' src="js/tls.js"></script>
<script type='text/javascript' src="js/integrity.js"></script>
<script type='text/javascript' src="js/opcua.js"></script>
<script type='text/javascript' src="js/cloud_connectivity.js?20190325"></script>
<script type='text/javascript' src="js/routing.js"></script>
<script type='text/javascript' src="js/bacnet_diagnostic.js"></script>
<script type='text/javascript' src="js/bacnet_general_configuration.js"></script>
<script type='text/javascript' src="js/bacnet_storage_location.js"></script>

<?php //include("test/test.inc.php"); ?>

<script type="text/javascript">

//------------------------------------------------------------------------------
//document ready
//------------------------------------------------------------------------------
$(document).ready(function()
{
  // get our own MAC - available only via PHP and important
  var clientMacAddress = '<?php printf("%s", GetClientMacAddress()); ?>';

  //------------------------------------------------------------------------------
  // start working
  //------------------------------------------------------------------------------
  pageElements  = new PageElements();

  var content = new Array(
      new InformationContent('information'),
      new RtsInfoContent('rts_info'),
      new DiagnosticContent('diagnostic'),
      new RtsConfigurationContent('rts_configuration'),
      new WebvisuContent('webvisu'),
      new TcpIpCommonConfigurationContent('tcpip_common_configuration'),
      new InterfacesContent('interfaces'),
      new EthernetContent('ethernet'),
      new FirewallGeneralConfigContent('firewall_general_configuration'),
      new FirewallMacAddressFilterContent('firewall_mac_address_filter', clientMacAddress),
      new FirewallUserFilterContent('firewall_user_filter'),
      new ClockContent('clock'),
      new UsersContent('users'),
      new CreateImageContent('create_image'),
      new RS232Content('rs232'),
      new ServiceInterfaceContent('service_interface'),
      new StartRebootContent('start_reboot'),
      new FirmwareBackupContent('firmware_backup'),
      new FirmwareRestoreContent('firmware_restore'),
      new SystemPartitionContent('system_partition'),
      new MassStorageContent('mass_storage'),
      new SoftwareUploadsContent('software_uploads'),
      new NetworkServicesContent('network_services'),
      new NtpClientContent('ntp'),
      new CodesysPortsContent('codesys_ports'),
      new SshContent('ssh'),
      new TftpContent('tftp'),
      new DhcpdContent('dhcpd'),
      new DnsServiceContent('dns_service'),
      new ModbusServicesContent('modbus_services'),
      new SnmpGeneralContent('snmp_general'),
      new SnmpV1V2cContent('snmp_v1_v2c'),
      new SnmpV3Content('snmp_v3'),
      new ProfibusContent('profibus'),      
      new ModemContent('modem'),
      new VpnContent('vpn'),
      new TlsContent('tls'),
      new IntegrityContent('integrity'),
      new OpcuaContent('opcua'),
      new DataAgentPageContent('cloud_connectivity'),
      new RoutingContent('routing'),
      new BacnetDiagnosticContent('bacnet_diagnostic'),
      new BacnetGeneralContent('bacnet_general_configuration'),
      new BacnetStorageContent('bacnet_storage_location')
  );

  var actualContentId  = document.location.hash.replace("#", "") || 'information';

  if(!wbmWorking)
  {
    $('#status_area').hide();
    $('#main_nav').hide();
    $('.single_content_page[id=error_content]').show();

    var errorText = 'WBM cannot be displayed, because you are using an old Internet Explorer Version (< IE10), ';
    errorText    += 'or compability mode is switched on (maybe for intranet sides - please check your IE settings).';
    $('.single_content_page[id=error_content] p.content_page_error').text(errorText);
  }
  else
  {

    pageBuildup          = new PageBuildup(actualContentId, function(status)
    {
      var userManagement   = new UserManagement('<?php echo $username?>', '<?php echo $csrfToken; ?>');

      pageBuildup.UpdateFeatureDependantEntries(function() {

        // update main menu links
        pageBuildup.ShowNavEntrySelected(pageBuildup.actualPageId);
        pageBuildup.AddNavigationListEvents();

        // update main menu to user level
        pageBuildup.SetCurrentUserlevel(userManagement.GetCurrentUserlevel());

        // if a special initial content is requested, load it - and do it via click to navigation, to check access rights before
        if(actualContentId.length)
        {
          // only in case of login page, go directly to it
          if('login' === actualContentId)
          {
            pageBuildup.ProcessContentChangeRequest(actualContentId);
          }
          else
          {
            $('#main_nav a#' + actualContentId).trigger('click');
          }
        }

        $('.content_page[id=error_content] .content_page_error').text('Error while creating page.');
        $('.content_page[id=error_content]').show();

        stateArea.Create(function(status)
        {
          if(status === SUCCESS)
            // disabled for for full authentication
            stateArea.StartCyclicReading();
      });

      // disabled for for full authentication
        headStationInfoContent.Init();
      });
    });
  };

});



</script>

</html>
