/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var NetworkServicesContent = function(id)
{
  var networkServicesContent  = this;

  networkServicesContent.id   = id || 'network_services';


  var CreateNetworkServicesContentParamView = (function()
  {
    networkServicesContent.paramView = new DeviceParamView();

    networkServicesContent.paramView.Add(
    {
      paramId         : 'telnet_state',
      outputElementId : networkServicesContent.id + '_content #telnet_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'ftp_state',
      outputElementId : networkServicesContent.id + '_content #ftp_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'ftps_state',
      outputElementId : networkServicesContent.id + '_content #ftps_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'http_state',
      outputElementId : networkServicesContent.id + '_content #http_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'https_state',
      outputElementId : networkServicesContent.id + '_content #https_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'iocheck_state',
      outputElementId : networkServicesContent.id + '_content #iocheck_active_state'
    });

    networkServicesContent.paramView.Add(
    {
      paramId         : 'opcua_state',
      outputElementId : networkServicesContent.id + '_content #opcua_active_state'
    });

  })();



  $('#content_area').bind(networkServicesContent.id + '_create', function(event)
  {
    // create several areas for configuration of service active state
    $('#network_services_interface_state_area #telnet_service_config_area').html(networkServicesContent.ServiceConfigHtml('telnet', 'Telnet'));
    $('#network_services_interface_state_area #ftp_service_config_area').html(networkServicesContent.ServiceConfigHtml('ftp', 'FTP'));
    $('#network_services_interface_state_area #ftps_service_config_area').html(networkServicesContent.ServiceConfigHtml('ftps', 'FTPS'));
    $('#network_services_interface_state_area #http_service_config_area').html(networkServicesContent.ServiceConfigHtml('http', 'HTTP'));
    $('#network_services_interface_state_area #https_service_config_area').html(networkServicesContent.ServiceConfigHtml('https', 'HTTPS'));
    $('#network_services_interface_state_area #iocheck_port_config_area').html(networkServicesContent.ServiceConfigHtml('iocheck', 'I/O-Check'));
    $('#network_services_interface_state_area #opcua_config_area').html(networkServicesContent.ServiceConfigHtml('opcua', 'OPC UA'));

    // click for enable/disable services active states directly
    $('#' + networkServicesContent.id + '_content form[id$=service_active_state_form]').bind('submit', function(event)
    {
      event.preventDefault();

      var service  = $(event.target).attr('id').replace('_service_active_state_form', '');
      var newState = $(event.target).find('input[name=service_active_state]:checked').length ? 'enabled' : 'disabled';

      networkServicesContent.ChangeServiceState(service, newState);
    });

  });


  $('#content_area').bind(networkServicesContent.id + '_refresh', function(event)
  {
    networkServicesContent.Refresh();
  });

};

NetworkServicesContent.prototype.Refresh = function()
{
  var networkServicesContent = this;

  networkServicesContent.RefreshServicesActiveState();
};


NetworkServicesContent.prototype.RefreshServicesActiveState = function()
{
  var networkServicesContent = this;

  deviceParams.ReadValueGroup(networkServicesContent.paramView.list, function()
  {
    networkServicesContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(networkServicesContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(networkServicesContent.paramView.list);

      if('' === errorString)
      {
        errorString = deviceParams.CollectedErrorNumbers(networkServicesContent.paramView.list);
      }

      $('body').trigger('event_errorOccured', [ 'Error while reading service states.', errorString ] );
    }
  });
};



NetworkServicesContent.prototype.ChangeServiceState = function(service, newState)
{
  var networkServicesContent = this;

  pageElements.ShowBusyScreen('Changing port state...');

  var deviceParamId = service + '_state';
  var newValueList  = { state: newState };

  deviceParams.ChangeValue(deviceParamId, newValueList, function(status, errorText)
  {
    // special case - our actually used http or https protocol was turned of 
    // - try to redirect over the other protocoll if possible
    if( (service === 'http' || service === 'https') && newValueList.state === 'disabled' ) {

      // Check, if the deactivated service is the one currently in use
      if (location.protocol === (service + ':')) {
          var otherService = service === 'http' ? 'https' : 'http';

          // Look availability of the alternative service
          if (deviceParams.list[otherService + '_state'].value === 'enabled') {

              // reload URL with other protocol
              // wait 1 second. the webserver may be unavailable due to a server restart
              setTimeout(function() {
                location.href = otherService + location.href.substr(service.length);
              }, 1000);

              // dont do anything else, as the page will be reloaded in a sec
              return;
          }
      }
    }

    if(SUCCESS != status)
    {
      // if http or https and error text that indicates a lost connection - normal behaviour, discard error
      if(("error" === errorText) && (('http' === service) || ('https' === service)))
      {
        // do nothing
      }
      else
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port state.', errorText ]);
      }
    }

    // always refresh page, because it is important to get the acutal values in device parameter list
    // wait 1 second before reading the new service states. the webserver may be unavailable due to a server restart
    setTimeout(function() {
        pageElements.RemoveBusyScreen();
        networkServicesContent.RefreshServicesActiveState();
      }, 2000);

  });

};



NetworkServicesContent.prototype.ServiceConfigHtml = function(serviceId, serviceText)
{
  html = '';

  html += '<div id="' + serviceId + '_state_area">';
  html +=   '<form id="' + serviceId + '_service_active_state_form" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="service" value="' + serviceId + '">';
  //html +=     '<div class="config_form_box group">';
  html +=     '<div class="config_form_box">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field"><label for="">Service active:</label></div>';
  html +=           '<div class="value_field">';
  html +=             '<input id="' + serviceId + '_active_state" type="checkbox" value="change_state" name="service_active_state" >';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Submit" name="change_service_active_state">';
  html +=       '</p>';
  html +=     '</div>';
  html +=   '</form>';
  html += '</div>';

  return html;
};
