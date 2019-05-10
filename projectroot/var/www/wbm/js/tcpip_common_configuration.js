/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*-----------------------------------------------------------------
 * hostname configuration
 *-----------------------------------------------------------------
 */
HostnameConfig = function()
{
  var hostnameConfig = this;
  
  var CreateTcpipCommonConfigView = (function()
  {
    hostnameConfig.paramView = new DeviceParamView();
            
    hostnameConfig.paramView.Add(
    {
      paramId         : 'hostname',
      outputElementId : 'hostname'
    });

    hostnameConfig.paramView.Add(
    {
      paramId         : 'actual_hostname',
      outputElementId : 'actual_hostname'
    });
  })();

};


HostnameConfig.prototype.ChangeHostname = function()
{
  var hostnameContent = this;
  
  pageElements.ShowBusyScreen("Changing hostname...");
  
  var newValueList = { hostname: $('input[name=hostname]').val() };

  deviceParams.ChangeValue('hostname', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing hostname.', errorText ]);
    }
    
    hostnameContent.Refresh();
  });

};

HostnameConfig.prototype.Create = function()
{
  var hostnameConfig = this;

  // add events for form processing
  $("#hostname_form").bind('submit', function(event)
  {
    event.preventDefault();
    hostnameConfig.ChangeHostname();
  });

};

HostnameConfig.prototype.Refresh = function()
{
  var hostnameConfig = this;

  deviceParams.ReadValueGroup(hostnameConfig.paramView.list, function()
  {
    hostnameConfig.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(hostnameConfig.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(hostnameConfig.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading hostname data.', errorString ] );
    };
  });
};


/*-----------------------------------------------------------------
 * domain name configuration
 *-----------------------------------------------------------------
 */
DomainNameConfig = function()
{
  var domainNameConfig = this;
  
  var CreateDomainNameConfigView = (function()
  {
    domainNameConfig.paramView = new DeviceParamView();
                  
    domainNameConfig.paramView.Add(
    {
      paramId         : 'domain_name',
      outputElementId : 'domain-name'
    });
  
    domainNameConfig.paramView.Add(
    {
      paramId         : 'actual_domain_name',
      outputElementId : 'tcpip_common_actual_domainname'
    });

  })();
  
};


DomainNameConfig.prototype.ChangeDomainName = function()
{
  var domainNameConfig = this;
  
  pageElements.ShowBusyScreen("Changing domain name...");
  
  var newValueList = { domainName: $('input[name=domain-name]').val() };

  deviceParams.ChangeValue('domain_name', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing domain name.', errorText ]);
    }
    
    domainNameConfig.Refresh();
  });

};



DomainNameConfig.prototype.Create = function()
{
  var domainNameConfig = this;

  // add events for form processing
  $("#domain_name_form").bind('submit', function(event)
  {
    event.preventDefault();
    domainNameConfig.ChangeDomainName();
  });

};

DomainNameConfig.prototype.Refresh = function()
{
  var domainNameConfig = this;

  deviceParams.ReadValueGroup(domainNameConfig.paramView.list, function()
  {
    domainNameConfig.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(domainNameConfig.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(domainNameConfig.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading domain name.', errorString ] );
    };
  });
};




/*-----------------------------------------------------------------
 * content of page common TCP/IP configuration (hostname/domainname)
 *-----------------------------------------------------------------
 */
TcpIpCommonConfigurationContent = function(id)
{
  var tcpipCommonConfigContent = this;
  
  tcpipCommonConfigContent.id = id || 'tcpip_common_configuration';
  
  var hostnameConfig    = new HostnameConfig();
  var domainNameConfig  = new DomainNameConfig();
  
  /*
   * event - first creation of content area
   */
  $('#content_area').bind(tcpipCommonConfigContent.id + '_create', function(event)
  {
    //Log("Event - tcpipCommonConfig create");
    hostnameConfig.Create();
    domainNameConfig.Create();
  });

  /*
   * event - refresh of content area
   */
  $('#content_area').bind(tcpipCommonConfigContent.id + '_refresh', function(event)
  {
    //Log("Event - tcpipCommonConfig refresh");
    hostnameConfig.Refresh();
    domainNameConfig.Refresh();
  });

};


