/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var FirewallServiceConfiguration = function(service, outputAreaId, additionalBoxClass, reloadCallback)
{
  firewallServiceConfiguration            = this;
  
  if(!service || !outputAreaId) return false;
  
  firewallServiceConfiguration.service            = service;
  firewallServiceConfiguration.outputAreaId       = outputAreaId;
  firewallServiceConfiguration.additionalBoxClass = additionalBoxClass || '';
  firewallServiceConfiguration.reloadCallback     = reloadCallback;

};



FirewallServiceConfiguration.prototype.FillParamView = function(paramView)
{
  var firewallServiceConfiguration = this;
  
  paramView.Add(
  {
    paramId         : 'interface_config_xml'
  });

  paramView.Add(
  {
    paramId         : 'ebtables_config'
  });
      
  paramView.Add(
  {
    paramId         : firewallServiceConfiguration.service + '_firewall_config'
  });
};


FirewallServiceConfiguration.prototype.Create = function()
{
  var firewallServiceConfiguration  = this;

  $(firewallServiceConfiguration.outputAreaId).bind('submit', function(event)
  {
    firewallServiceConfiguration.ProcessChangeServiceInterfaceState(event);
  });
  
};



FirewallServiceConfiguration.prototype.RefreshDisplay = function()
{
  var firewallServiceConfiguration  = this;

  $(firewallServiceConfiguration.outputAreaId).html('');
  $(firewallServiceConfiguration.outputAreaId).html(firewallServiceConfiguration.CreateServiceAreaHtmlFrame());
  firewallServiceConfiguration.CreateInterfaceSelection();
};


FirewallServiceConfiguration.prototype.CreateInterfaceSelection = function()
{
  var firewallServiceConfiguration  = this;
  var service                       = firewallServiceConfiguration.service;
  var iFaceConfig                   = deviceParams.list['interface_config_xml'].value;
  var ebtables                      = deviceParams.list['ebtables_config'].value;
  var iFaceFirewallConfig           = deviceParams.list[service + '_firewall_config'].value;
  
  var ifListObj = $('#' + service + '_firewall_service_config_form .interface_selection');
  $(ifListObj).html('');
  
  if(   (SUCCESS === deviceParams.list['interface_config_xml'].status)
     && (SUCCESS === deviceParams.list['ebtables_config'].status)
     && (SUCCESS === deviceParams.list[service + '_firewall_config'].status))
  {
    $.each(iFaceConfig.ifaces, function(iFaceId, iFace)
    {
      if(iFace.ipSettings) // && iFace.ipSettings.showInWbm)
      {
        var iFaceId       = iFace.ipSettings.portName;

        // if interfaces are switched - conform output port term
        var iFaceLabel = (0 == iFaceConfig.dsaMode) ? 'X1/X2' : iFaceId;
        
        var html    = firewallServiceConfiguration.InterfaceHtmlCheckboxSelection(iFaceId, service, iFaceLabel);
        $(ifListObj).append(html);
      
        // interface is totally open (firewall is disabled) - checkbox is checked and disabled
        if(ebtables.ethernet.interfaces[iFaceId].state && ('open' === ebtables.ethernet.interfaces[iFaceId].state))
        {
          $(ifListObj).find('#' + service + '_firewall_state_' + iFaceId).attr('disabled', 'disabled');
          $(ifListObj).find('#' + service + '_firewall_state_' + iFaceId).attr('checked', 'checked');
        }
      
        else if(iFaceFirewallConfig.iFace[iFaceId] && ('on' === iFaceFirewallConfig.iFace[iFaceId].state))
        {
          $(ifListObj).find('#' + service + '_firewall_state_' + iFaceId).attr('checked', 'checked');
        }
      }
    });
  }
  
};


FirewallServiceConfiguration.prototype.CreateServiceAreaHtmlFrame = function(service)
{
  var firewallServiceConfiguration  = this;
  var service                       = firewallServiceConfiguration.service;
  var html                          = '';
  
  html += '<div id="' + service + '_interface_state_area">';
  html +=   '<form id="' + service + '_firewall_service_config_form" action="javascript_requested.php" method="POST">';
  html +=     '<input type="hidden" name="service" value="' + service + '">';
  html +=     '<div class="config_form_box ' + firewallServiceConfiguration.additionalBoxClass + '">';
  html +=       '<div class="config_data_list">';
  html +=         '<div class="label_value_pair">';
  html +=           '<div class="label_field">Enabled on interface:</div>';
  html +=           '<div class="value_field">';
  html +=             '<ul class="check_list_important interface_selection">';
  html +=             '</ul>';
  html +=           '</div>';
  html +=         '</div>';
  html +=       '</div>';
  html +=       '<p class="button_area">';
  html +=         '<input class="button" type="submit" value="Submit" name="change_service_state">';
  html +=       '</p>';
  html +=     '</div>';
  html +=   '</form>';
  html += '</div>';

  return html;
};




FirewallServiceConfiguration.prototype.InterfaceHtmlCheckboxSelection = function(iFaceId, service, iFaceLabel)
{
  var iFaceLabel  = iFaceLabel || iFaceId;
  var html        = '';
  
  html +=             '<li><input id="' + service + '_firewall_state_' + iFaceId + '" type="checkbox" name="firewall_state" value=' + iFaceId + '>';
  html +=                '<label for="' + service + '_firewall_state_' + iFaceId + '">' + iFaceLabel + '</label></li>';

  return html;
};

/*
FirewallServiceConfiguration.prototype.InterfaceHtml = function(iFaceId, service)
{
  var html = '';
  
  html += '<form id="' + service + '_state_form_iface_' + iFaceId + '" action="javascript_requested.php" method="POST">';
  //html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';

  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">Interface ' + iFaceId + ':</div>';
  html +=         '<div class="value_field id="state_output_telnet_iface_' + iFaceId + '">-</div>';
  html +=       '</div>';
  html +=     '</div>';
  
  html +=     '<p class="button_area">';
  html +=       '<input id="change_state_button_telnet_iface_' + iFaceId + '" class="button" type="submit" value="-" name="change_state">';
  html +=     '</p>';
  //html +=   '</div>';
  html += '</form>';
  
  return html;
};
*/

/*
FirewallServiceConfiguration.prototype.InterfaceHtml = function(iFaceId, service)
{
  var html = '';
  
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="' + service + '_firewall_state_' + iFaceId + '">Interface ' + iFaceId + ':</label></div>';
  html +=         '<div class="value_field">';
  html +=           '<input id="' + service + '_firewall_state_' + iFaceId + '" type="checkbox" value="change_state" name="firewall_state" >';
  html +=         '</div>';
  html +=       '</div>';
  
  return html;
};
*/


FirewallServiceConfiguration.prototype.ProcessChangeServiceInterfaceState = function(event)
{
  event.preventDefault();
  
  var firewallServiceConfiguration  = this;
  var service                       = firewallServiceConfiguration.service;
  var changeValueGroupData          = [];
  var changeValueIndex              = 0;

  $(event.target).find('input[name=firewall_state][disabled!=disabled]').each(function(iFaceIndex)
  {
    var checkboxObj = this;
    var iFaceId     = $(checkboxObj).val();
    
    changeValueGroupData[changeValueIndex]            = {};
    changeValueGroupData[changeValueIndex].paramId    = 'set_service_firewall_interface_state';
    changeValueGroupData[changeValueIndex].valueList  = { service: service, state: 'off', iFaceId: iFaceId };
      
    if('checked' === $(checkboxObj).attr('checked'))
    {
      changeValueGroupData[changeValueIndex].valueList.state = 'on';
    }
    
    ++changeValueIndex;
  });
  
  if(changeValueGroupData.length)
  {
    pageElements.ShowBusyScreen('Changing port state...');

    deviceParams.ChangeValueGroup(changeValueGroupData, function(status, errorText)
    {
      //Log('Change firewall service configuration | ' + service);
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing port state.', errorText ]);
      }

      if(firewallServiceConfiguration.reloadCallback) firewallServiceConfiguration.reloadCallback();
    });
  }
  
};
