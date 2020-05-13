/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var OpcuaContent = function(id)
{
  var opcuaContent = this;

  opcuaContent.id = id || 'opcua';

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------


  var OpcuaParamView = (function()
  {
    opcuaContent.paramView = new DeviceParamView();

    opcuaContent.paramView.Add(
    {
      paramId         : 'opcua_config'
    });

  })();


  $('#content_area').bind(opcuaContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + opcuaContent.id + '_content #config_opcua').bind('submit', function(event)
    {
      event.preventDefault();
      opcuaContent.ProcessConfigChangeSubmit(event.target);
    });
    // add events for form processing
    $('#' + opcuaContent.id + '_content #config_opcua_trust_all_clients').bind('submit', function(event)
    {
      event.preventDefault();
      opcuaContent.ProcessTrustAllClientsConfigChangeSubmit(event.target);
    });
    // add events for form processing
    $('#' + opcuaContent.id + '_content #config_opcua_security').bind('submit', function(event)
    {
      event.preventDefault();
      opcuaContent.ProcessSecurityConfigChangeSubmit(event.target);
    });

  });

  $('#content_area').bind(opcuaContent.id + '_refresh', function(event)
  {
    opcuaContent.Refresh();
  });

};


OpcuaContent.prototype.Refresh = function()
{
  var opcuaContent = this;

  deviceParams.ReadValueGroup(opcuaContent.paramView.list, function()
  {
    if(   deviceParams.ReadErrorOccured(opcuaContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(opcuaContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading OPC UA server configuration.', errorString ] );
    }
    else
    {
      opcuaContent.ShowConfigFromObject();
    }
  });

};

OpcuaContent.prototype.ShowConfigFromObject = function()
{
  var opcuaContent  = this;

  if(SUCCESS === deviceParams.list['opcua_config'].status)
  {
    var opcuaConfig = deviceParams.list['opcua_config'].value;

    var opcuaInfoArea   = $('#' + opcuaContent.id + '_content #opcua_info');
    $(opcuaInfoArea).find('#opcua_version').html(opcuaConfig.serverVersion || '---');

    var opcuaGeneralArea   = $('#' + opcuaContent.id + '_content #config_opcua');
    $(opcuaGeneralArea).find('#opcua_state').removeAttr('checked');
    if (opcuaConfig.state === 'enable') {
      $(opcuaGeneralArea).find('#opcua_state').prop('checked', 'checked');
    }
    $(opcuaGeneralArea).find('#opcua_port').val(opcuaConfig.port || '');
    $(opcuaGeneralArea).find('#opcua_log_level option[value="'+(opcuaConfig.logLevel || '')+'"]').prop('selected', 'selected');

    $(opcuaGeneralArea).find('#opcua_anonymous_access').removeAttr('checked');
    if (opcuaConfig.unlimitedAccess === 'enable') {
      $(opcuaGeneralArea).find('#opcua_anonymous_access').prop('checked', 'checked');
    }

    $('#opcua_trust_all_clients').removeAttr('checked');
    if (opcuaConfig.trustAllClients === 'enable') {
      $('#opcua_trust_all_clients').prop('checked', 'checked');
    }

    var opcuaSecurityArea   = $('#' + opcuaContent.id + '_content #config_opcua_security');

    var ids = [
      'opcua_application_uri_check',
      'opcua_error_cert_time',
      'opcua_cert_issuer_time_invalid',
      'opcua_cert_revocation_unknown',
      'opcua_certificate_issuer_revocation_unknown'
    ];
    var keys = [
      'applicationUriCheck',
      'userTokenPolicyIdCheck',
      'noneLengthCheck',
      'errorCertificateTime',
      'certificateIssuerTimeInvalid',
      'certificateRevocationUnknown',
      'certificateIssuerRevocationUnknown'
    ];
    for (var i = 0; i < ids.length; i++) {
      $(opcuaSecurityArea).find('#' + ids[i]).removeAttr('checked');
      if (opcuaConfig[keys[i]] === 'enable') {
        $(opcuaSecurityArea).find('#' + ids[i]).prop('checked', 'checked');
      }
    }
  }
};



OpcuaContent.prototype.ProcessConfigChangeSubmit = function(formObj)
{
  var opcuaContent = this;

  pageElements.ShowBusyScreen('Changing OPC UA configuration...');


  var newValueList  = { configOpcua: JSON.stringify({
    state:           $(formObj).find('[name=opcua_state]:checked').length ? 'enable' : 'disable',
    port:            parseInt($(formObj).find('[name=opcua_port]').val(), 10),
    logLevel:        $(formObj).find('[name=opcua_log_level]').val(),
    unlimitedAccess: $(formObj).find('[name=opcua_anonymous_access]:checked').length ? 'enable' : 'disable'
  })};

  deviceParams.ChangeValue('opcua_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing OPC UA configuration.', errorText ]);
    }

    opcuaContent.Refresh();
  });

};



OpcuaContent.prototype.ProcessTrustAllClientsConfigChangeSubmit = function(formObj)
{
    var opcuaContent = this;

    pageElements.ShowBusyScreen('Changing OPC UA configuration...');

    var newValueList  = {configOpcua: JSON.stringify({
      trustAllClients:                    $(formObj).find('[name=opcua_trust_all_clients]:checked').length ? 'enable' : 'disable'
    })};

    deviceParams.ChangeValue('opcua_config', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing OPC UA configuration.', errorText ]);
      }

      opcuaContent.Refresh();
    });

};

OpcuaContent.prototype.ProcessSecurityConfigChangeSubmit = function(formObj)
{
    var opcuaContent = this;

    pageElements.ShowBusyScreen('Changing OPC UA configuration...');

    var newValueList  = {configOpcua: JSON.stringify({
      applicationUriCheck:                $(formObj).find('[name=opcua_application_uri_check]:checked').length ? 'enable' : 'disable',
      errorCertificateTime:               $(formObj).find('[name=opcua_error_cert_time]:checked').length ? 'enable' : 'disable',
      certificateIssuerTimeInvalid:       $(formObj).find('[name=opcua_cert_issuer_time_invalid]:checked').length ? 'enable' : 'disable',
      certificateRevocationUnknown:       $(formObj).find('[name=opcua_cert_revocation_unknown]:checked').length ? 'enable' : 'disable',
      certificateIssuerRevocationUnknown: $(formObj).find('[name=opcua_certificate_issuer_revocation_unknown]:checked').length ? 'enable' : 'disable'
    })};

    deviceParams.ChangeValue('opcua_config', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing OPC UA configuration.', errorText ]);
      }

      opcuaContent.Refresh();
    });

};
