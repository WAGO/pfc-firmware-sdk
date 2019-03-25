/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var ModemContent = function(id)
{
  var modemContent = this;

  modemContent.id = id || 'modem';

  modemContent.uploadDir    = UPLOAD_DIR + '/modem_software_upload';
  modemContent.logfilePath  = '/tmp/modem_software_upload/logfile'

  //------------------------------------------------------------------------------
  // define view elements
  //------------------------------------------------------------------------------
  var ModemCommonConfigParamView = (function()
  {
    modemContent.paramView = new DeviceParamView();

    modemContent.paramView.Add(
    {
      paramId         : 'modem_sim_config'
    });

    modemContent.paramView.Add(
    {
      paramId         : 'modem_network_config'
    });

    modemContent.paramView.Add(
    {
      paramId         : 'modem_gprs_config'
    });

    modemContent.paramView.Add(
    {
      paramId         : 'modem_info'
    });

  })();

  
  //------------------------------------------------------------------------------
  // first creation of content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(modemContent.id + '_create', function(event)
  {
    // prepare things for modem software upload
    //------------------------------------------------------------------------------
    var modemSoftwareUploadForm = $('#' + modemContent.id + '_content #modem_software_upload_form');

    // write upload directory on device in form member
    $(modemSoftwareUploadForm).append('<input type="hidden" name="upload_directory" value="' + modemContent.uploadDir + '">');

    // show iframe for test to see the upload results on screen
    //$('#' + modemContent.id + '_content #upload_modem_software_iframe').show();

    // Does not work with IE! After sending click event to browse button, file selection apparently is ok - but form can't be submitted
    // Take solution with transparent "browse" button over real button to click it directly...
    //pageElements.InitFakeFileInput('#' + softwareUploadsContent.id + '_content', 'software_upload');

    //------------------------------------------------------------------------------
    // add events for form processing
    //------------------------------------------------------------------------------

    // write sim and/or  puk
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #modem_sim_area').bind('submit', function(event)
    {
      event.preventDefault();
      modemContent.ProcessSimPinWrite(event.target);
    });

    // click on selection mode selection
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode]').bind('change', function(event)
    {
      //event.preventDefault();
      modemContent.ShowNetworkSelectionModeDependencies();
    });

    // click on refresh provider list button
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #modem_refresh_provider_list_button').bind('click', function(event)
    {
      event.preventDefault();
      modemContent.RefreshProviderList();
    });

    // submit network configuration
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #mobile_network_config_form').bind('submit', function(event)
    {
      event.preventDefault();

      var selectionMode = $(event.target).find('[name=selection_mode]:checked').val();
      if('MANUAL' === selectionMode)
      {
        var providerIndex = $(event.target).find('[name=provider]').val();
        modemContent.ProcessProviderChangeSubmit(providerIndex);
      }
      else
      {
        modemContent.ChangeSelectionMode(selectionMode);
      }
    });

    // submit network package service form
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #mobile_network_package_service_form').bind('submit', function(event)
    {
      event.preventDefault();
      modemContent.ProcessNetworkPackageServiceConfig(event.target);
    });

    // enable upload software button (only) when file was selected
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #modem_software_upload_file_real_input').bind('change', function(event)
    {
      // get filename of files array, because reading fileupload value ($(event.target).val()) is not the correct way 
      // and adds funny strings to filename ("fakepath etc.")
      // respect, that file array is not always existing (especcialy IE sometimes has an error here and files array is undefined)
      if($(event.target)[0].files && $(event.target)[0].files[0] && $(event.target)[0].files[0].name) {
        filename = $(event.target)[0].files[0].name;
      }

      // show filename in fake input field
      $('#' + modemContent.id + '_content #modem_software_upload_form').find('.file_browse_fake_input').val(filename);

      // enable update button if really one file was selected
      if(filename.length)
      {
        $('#'  + modemContent.id + '_content #modem_software_upload_submit_button').removeAttr('disabled');
      }
      else
      {
        $('#'  + modemContent.id + '_content #modem_software_upload_submit_button').prop('disabled', 'disabled');
      }
    });

    // submit form event - file upload
    //------------------------------------------------------------------------------
    $('#' + modemContent.id + '_content #modem_software_upload_form').bind('submit', function(event)
    {
      event.preventDefault();
      modemContent.UploadSoftware(this);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of complete content
  //------------------------------------------------------------------------------
  $('#content_area').bind(modemContent.id + '_refresh', function(event)
  {
    modemContent.Refresh();
  });

};


ModemContent.prototype.Refresh = function(cb)
{
  var modemContent = this;

  deviceParams.ReadValueGroup(modemContent.paramView.list, function()
  {
     //modemContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(modemContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(modemContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading modem configuration.', errorString ] );
    }

    modemContent.RefreshSimStateArea();
    modemContent.RefreshNetworkConfig();
    modemContent.RefreshGprsConfig();
    modemContent.RefreshModemInfo();

    cb && cb();
  });

};


ModemContent.prototype.RefreshSimStateArea = function()
{
  // states: UNKNOWN, SIM_PIN, SIM_PUK, READY, NOT_INSERTED, ERROR

  var modemContent  = this;
  var areaHtml      = '';
  var errorText     = '';

  var simConfig = deviceParams.list['modem_sim_config'].value;

  if(SUCCESS != deviceParams.list['modem_sim_config'].status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading SIM authentication data.', deviceParams.list['modem_sim_config'].errorText ]);
    areaHtml  = modemContent.SimAuthAreaErrorHtml();
  }
  else if(!simConfig || !simConfig.SimState || !simConfig.SimAttempts)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading SIM authentication data.', 'Parameter missing' ] );
    areaHtml = modemContent.SimAuthAreaErrorHtml();
  }

  else switch(simConfig.SimState)
  {
    case 'SIM_PIN': areaHtml = modemContent.SimAuthAreaPinHtml(simConfig.SimAttempts); break;
    case 'SIM_PUK': areaHtml = modemContent.SimAuthAreaPukHtml(simConfig.SimAttempts); break;
    case 'READY':   areaHtml = modemContent.SimAuthAreaReadyHtml(); break;
    case 'NOT_INSERTED': areaHtml = modemContent.SimAuthAreaErrorHtml('SIM card not inserted.'); break;
    default:        areaHtml = modemContent.SimAuthAreaErrorHtml(); break;
  }

  $('#' + modemContent.id + '_content #modem_sim_area').html(areaHtml);
};


ModemContent.prototype.SimAuthAreaPinHtml = function(simAttempts)
{
  var html = '';

  html += '<h3>SIM Authentication</h3>';
  html += '<form id="modem_sim_pin_enter_form" action="javascript_requested.php" method="POST">';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">State:</div>';
  html +=         '<div class="value_field">PIN requested (' + simAttempts + ' attempts left)</div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="modem_sim_pin">PIN:</label></div>';
  html +=         '<div class="value_field"><input id="modem_sim_pin" class="input_field" type="password" name="pin" value=""></div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Submit" name="submit">';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';

  return html;
};


ModemContent.prototype.SimAuthAreaPukHtml = function(simAttempts)
{
  var html = '';

  html += '<h3>SIM Authentication</h3>';

  html += '<form id="modem_sim_puk_enter_form" action="javascript_requested.php" method="POST">';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field">State:</div>';
  html +=         '<div class="value_field">PIN unsuccessfull, PUK requested (' + simAttempts + ' attempts left)</div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair"">';
  html +=         '<div class="label_field"><label for="modem_sim_puk">PUK:</label></div>';
  html +=         '<div class="value_field"><input id="modem_sim_puk" class="input_field" type="password" name="puk" value=""></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="modem_sim_pin">New PIN:</label></div>';
  html +=         '<div class="value_field"><input id="modem_sim_pin" class="input_field" type="password" name="pin" value=""></div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Submit" name="submit">';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';

  return html;
};


ModemContent.prototype.SimAuthAreaReadyHtml = function()
{
  var html = '';

  html += '<h3>SIM Authentication</h3>';

  html += '<div class="config_form_box">';
  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">State:</div>';
  html +=       '<div class="value_field">Ready</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';

  return html;
};

ModemContent.prototype.SimAuthAreaErrorHtml = function(optionalMessage)
{
  optionalMessage = optionalMessage || '';
  var html = '';

  html += '<h3>SIM Authentication</h3>';

  html += '<div class="config_form_box">';
  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Error while reading SIM data. ' + optionalMessage + '</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';

  return html;
};


ModemContent.prototype.RefreshGprsConfig = function()
{
  var modemContent  = this;

  var gprsConfig = deviceParams.list['modem_gprs_config'].value;
  
  if(SUCCESS != deviceParams.list['modem_gprs_config'].status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading packed date network access configuration.', deviceParams.list['modem_gprs_config'].errorText ]);
  }

  else if(   (undefined === gprsConfig)
          || (undefined === gprsConfig.GprsRegistrationState) || (undefined === gprsConfig.GprsAccessPointName)
          || (undefined === gprsConfig.GprsAuthType)          || (undefined === gprsConfig.GprsUserName)
          || (undefined === gprsConfig.GprsPassword)          || (undefined === gprsConfig.GprsConnectivity) )
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading packed date network access configuration.', 'Parameter missing' ]);
  }

  else
  {
    // show connectivity state
    $('#' + modemContent.id + '_content #modem_gprs_connectivity').removeAttr('checked');
    if('ENABLED' === gprsConfig.GprsConnectivity)
    {
      $('#' + modemContent.id + '_content #modem_gprs_connectivity').prop('checked', 'checked');
    }

    // show registration state
    var registrationState = modemContent.NetworkRegistrationOutputText(gprsConfig.GprsRegistrationState);
    $('#' + modemContent.id + '_content #modem_gprs_state').html(registrationState);

    // show access point name, user and password in input fields
    $('#' + modemContent.id + '_content #modem_gprs_apn').val(gprsConfig.GprsAccessPointName);
    $('#' + modemContent.id + '_content #modem_gprs_user').val(gprsConfig.GprsUserName);
    $('#' + modemContent.id + '_content #modem_gprs_password').val(gprsConfig.GprsPassword);

    // show auth type in checkbox selection
    $('#' + modemContent.id + '_content #mobile_network_package_service_form [name=auth_type]').removeAttr('checked');
    $('#' + modemContent.id + '_content #mobile_network_package_service_form [name=auth_type][value=' + gprsConfig.GprsAuthType + ']').prop('checked', 'checked');
  }
};

ModemContent.prototype.RefreshModemInfo = function()
{
  var modemContent  = this;

  var modemInfo = deviceParams.list['modem_info'].value;

  if(SUCCESS != deviceParams.list['modem_info'].status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading modem info.', deviceParams.list['modem_modem_info'].errorText ]);
  }

  else if((undefined === modemInfo) || (undefined === modemInfo.ModemFirmware) )
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading modem info.', 'Parameter missing' ]);
  }

  else
  {
    // show software version
    $('#' + modemContent.id + '_content #modem_software_version').html(modemInfo.ModemFirmware);
  }
};


ModemContent.prototype.RefreshNetworkConfig = function()
{
  var modemContent  = this;

  var networkConfig = deviceParams.list['modem_network_config'].value;

  if(SUCCESS != deviceParams.list['modem_network_config'].status)
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading network configuration.', deviceParams.list['modem_network_config'].errorText ]);
  }

  else if(   (undefined === networkConfig)
          || (undefined === networkConfig.NetworkSelection) || (undefined === networkConfig.NetworkRegistrationState)
          || (undefined === networkConfig.NetworkId)        || (undefined === networkConfig.NetworkName)
          || (undefined === networkConfig.NetworkType)      || (undefined === networkConfig.SignalStrength) )
  {
    $('body').trigger('event_errorOccured', [ 'Error while reading network configuration.', 'Parameter missing' ] );
  }

  else
  {
    // show network registration state
    var registrationState = modemContent.NetworkRegistrationOutputText(networkConfig.NetworkRegistrationState);
    $('#' + modemContent.id + '_content #state_output_modem_network_state').html(registrationState);

    // show signal strength
    modemContent.VisualizeSignalStrengthByImage(networkConfig.SignalStrength);
    $('#' + modemContent.id + '_content #modem_signal_strength').html(networkConfig.SignalStrength);

    // show operator
    var separator           = ((networkConfig.NetworkName.length > 0) && (networkConfig.NetworkType.length > 0)) ? ' | ' : '';
    //var opetatorOutputText  = networkConfig.NetworkName + ' | ' + networkConfig.NetworkType;
    $('#' + modemContent.id + '_content #modem_operator').html(networkConfig.NetworkName + separator + networkConfig.NetworkType);


    // show selection mode via checkbox
    $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode]').removeAttr('checked');
    $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode][value=' + networkConfig.NetworkSelection + ']').prop('checked', 'checked');

    modemContent.ShowNetworkSelectionMode(networkConfig.NetworkSelection);
  }

};


ModemContent.prototype.CombinedProviderTypeString = function(provider, type)
{
  var separator       = ((provider.length > 0) && (type.length > 0)) ? ' | ' : '';
  var combinedString  = (provider ? provider : '') + separator + (type ? type : '');

  return combinedString;
};


ModemContent.prototype.NetworkRegistrationOutputText = function(networkRegistrationState)
{
  var outputText = 'Unknown';

  if(networkRegistrationState) switch(networkRegistrationState)
  {
    case 'HOMENET':
    case 'ROAMING': outputText = 'Registered';      break;
    case 'STOPPED':
    case 'STARTED':
    case 'DENIED':  outputText = 'Not registered';  break;
    default:        outputText = 'Unknown';         break;
  }

  return outputText;
};


ModemContent.prototype.ProviderStateOutputText = function(providerState)
{
  var aStateOutputStrings =
  {
    'AVAILABLE':  'available',
    'FORBIDDEN':  'forbidden',
    'CURRENT'  :  'current'
  };

  var outputText = ('undefined' != aStateOutputStrings[providerState]) ? aStateOutputStrings[providerState] : 'unknown';

  return outputText;
};




ModemContent.prototype.VisualizeSignalStrengthByImage = function(signalStrength)
{
  var modemContent    = this;
  //var imgWidth        = (119 + 1) / 2;    // + 1 because of missing gap at the end of img, /2 because we have all blue and grey stripes in one image
  var imgWidth        = 60;
  var imgStripeWidth  = 6;
  var signalStrength  = parseInt(signalStrength) || 0;

  // adjust signal strength to size of colored bar in image
  //var imgWidth          = parseInt($('#' + modemContent.id + '_content #modem_signal_strength_img').width(), 10) + 1;  // + 1 because of missing gap at the end of img
  var imgSignalStrength = signalStrength * imgWidth / 100;

  // adjust signal strength accurate to width of single stripes in colored bar; consider 1px gap between stripes
  imgSignalStrength     = Math.round(imgSignalStrength / imgStripeWidth) * imgStripeWidth;// + 1;
  if(imgSignalStrength <= 0) imgSignalStrength = 1;

  imgSignalStrength = imgWidth - imgSignalStrength;

  // show image for signal strength by width of colored bar via shifted background image
  //$('#' + modemContent.id + '_content #modem_signal_strength_img').css('background-position', '-' + imgSignalStrength + 'px');
  $('#' + modemContent.id + '_content #modem_signal_strength_img_area').html('<img src="images/transparent.gif" width="' + imgWidth + '" height="10">');
  $('#' + modemContent.id + '_content #modem_signal_strength_img_area img').css(
  {
    'background':           'url(images/signal_strength.gif)',
    'background-position':  '-' + imgSignalStrength + 'px'
  });
};



ModemContent.prototype.ShowNetworkSelectionMode = function(networkSelectionMode)
{
  var modemContent  = this;

  if(networkSelectionMode)
  {
    // show selection mode via checkbox
    $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode]').removeAttr('checked');
    $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode][value=' + networkSelectionMode + ']').prop('checked', 'checked');

    modemContent.ShowNetworkSelectionModeDependencies();
  }

  return;
};





ModemContent.prototype.ShowNetworkSelectionModeDependencies = function(networkSelectionMode)
{
  var modemContent          = this;
  var networkSelectionMode  = networkSelectionMode || $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode]:checked').val();

  // selection mode is not manual - disable provider selection menu
  // selection mode is not manual - hide provider selection
  if('MANUAL' != networkSelectionMode)
  {
    //$('#' + modemContent.id + '_content #mobile_network_config_form select[name=provider]').prop('disabled', 'disabeld');
    $('#' + modemContent.id + '_content #mobile_network_config_form #modem_provider_selection_area').hide();
  }

  // selection is manual
  else
  {
    // show provider selection
    $('#' + modemContent.id + '_content #mobile_network_config_form #modem_provider_selection_area').show();

    // if provider list was not created by now (no option with a value in provider selection menu)
    if(   (SUCCESS != deviceParams.list['modem_providerlist'].status)
       || (0 === deviceParams.list['modem_providerlist'].value.length) )
    {
      modemContent.RefreshProviderList();
    }

    // provider list is available, enable selection menu
    else
    {
      $('#' + modemContent.id + '_content #mobile_network_config_form select[name=provider]').removeAttr('disabled');
    }
  }

  return;
};


ModemContent.prototype.RefreshProviderList = function()
{
  var modemContent  = this;

  pageElements.ShowBusyScreen('Creating provider list...');

  deviceParams.ReadValue({ id: 'modem_providerlist', variables: { } }, function()
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != deviceParams.list['modem_providerlist'].status)
    {
      var errorText = deviceParams.list['modem_providerlist'].errorText;
      $('body').trigger('event_errorOccured', [ 'Error while creating provider list.', errorText ]);
    }
    else if((SUCCESS != modemContent.ShowProviderList()) || (SUCCESS != modemContent.ShowProviderListSelection()))
    {
      $('body').trigger('event_errorOccured', [ 'Error while creating provider list.', 'Wrong data format' ]);
    }
  });

};



ModemContent.prototype.ShowProviderListSelection = function()
{
  var modemContent  = this;
  var areaHtml      = '';
  var status        = SUCCESS;
  var providerList  = deviceParams.list['modem_providerlist'].value;

  $('#' + modemContent.id + '_content #modem_provider_selection option').remove();

  // error while reading providerlist -> show according text and disable selection menu
  if(SUCCESS != deviceParams.list['modem_providerlist'].status)
  {
    status = ERROR;
    $('#' + modemContent.id + '_content #modem_provider_selection').prop('disabled', 'disabled');
    $('#' + modemContent.id + '_content #modem_provider_selection').append('<option>Unknown</option>');
  }
  else
  {
    // for each provider - create a selection menu line if data is ok and provider is not forbidden
    $.each(providerList, function(providerIndex, network)
    {
      if((undefined != network.name) && (undefined != network.state) && (undefined != network.id) && (undefined != network.type))
      {
        if(('AVAILABLE' === network.state) || ('CURRENT' === network.state))
        {
          var providerSelectionText = modemContent.CombinedProviderTypeString(network.name, network.type);
          var selectedText          =  ('CURRENT' === network.state) ? 'selected' : '';

          $('#' + modemContent.id + '_content #modem_provider_selection').append('<option value="' + providerIndex + '" ' + selectedText +'>' + providerSelectionText + '</option>');
        }
      }
    });

    // no available providers existing -> show according text and disable selection menu
    if(0 === $('#' + modemContent.id + '_content #modem_provider_selection option').length)
    {
      $('#' + modemContent.id + '_content #modem_provider_selection').prop('disabled', 'disabled');
      $('#' + modemContent.id + '_content #modem_provider_selection').append('<option>No provider found</option>');
    }

    // providers are existing and selection mode selection is set to "manual" -> enable provider selection
    else
    {
      var networkSelectionMode  = $('#' + modemContent.id + '_content #mobile_network_config_form input[name=selection_mode]:checked').val();
      if('MANUAL' === networkSelectionMode)
      {
        $('#' + modemContent.id + '_content #modem_provider_selection').removeAttr('disabled');
      }
    }
  }

  return status;
};



ModemContent.prototype.ShowProviderList = function()
{
  var modemContent  = this;
  var status        = SUCCESS;
  var html          = '';
  var providerList  = deviceParams.list['modem_providerlist'].value;

  $('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list').html('');

  if(SUCCESS != deviceParams.list['modem_providerlist'].status)
  {
    status = ERROR;
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">Unknown</div>';
    html +=     '</div>';

    $('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list').html(html);
  }

  else
  {
    $.each(providerList, function(networkIndex, network)
    {
      html = modemContent.ProviderListNetworkHtml(network.name, network.state, network.id, network.type);
      $('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list').append(html);
    });

    // if no useable provider list was created (no option with a value in provider selection menu)
    if(0 === $('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list .label_value_pair').length)
    {
      html +=     '<div class="label_value_pair">';
      html +=       '<div class="label_field">No provider found.</div>';
      html +=     '</div>';

      $('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list').html(html);
    }
  }

  /*
  if($('#' + modemContent.id + '_content #modem_providerlist_area .config_data_list)
  var now         = new Date();
  var timeString  = now.getHours() + ':' + now.getMinutes() + ':' + now.getSeconds();
  $('#' + modemContent.id + '_content form#mobile_network_refresh_provider_list_form .state').html('Last Update Time: ' + timeString);
  */
  return status;
};


ModemContent.prototype.ProviderListNetworkHtml = function(networkName, networkState, networkId, networkType)
{
  var modemContent  = this;

  var html = '';

  /*
  html += '<div class="config_form_box group">';
  html +=   '<div class="config_data_list">';
  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Network Name:</div>';
  html +=       '<div class="value_field">' + (networkName ? networkName : '') + '</div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Network State:</div>';
  html +=       '<div class="value_field">' + stateOutputString + '</div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Network ID:</div>';
  html +=       '<div class="value_field">' + (networkId ? networkId : '') + '</div>';
  html +=     '</div>';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Network Type:</div>';
  html +=       '<div class="value_field">' + (networkType ? networkType : '') + '</div>';
  html +=     '</div>';
  html +=   '</div>';
  */
  if((undefined != networkName) && (undefined != networkState) && (undefined != networkId) && (undefined != networkType))
  {
    html +=     '<div class="label_value_pair">';
    html +=       '<div class="label_field">' + modemContent.CombinedProviderTypeString(networkName, networkType) + '</div>';
    html +=       '<div class="value_field">'
    html +=         'ID: ' + networkId + ', ';
    html +=         modemContent.ProviderStateOutputText(networkState);
    html +=       '</div>';
    html +=     '</div>';
  }

  return html;
};


ModemContent.prototype.ProviderListUnknownHtml = function()
{
  var html = '';

  html += '<div class="config_form_box group">';
  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="label_field">Unknown</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';
  html += '<p class="button_area">';
  html +=   '<input id="modem_refresh_provider_list_button" class="button" type="submit" value="Refresh" name="submit">';
  html += '</p>';

  return html;
};

ModemContent.prototype.ProviderSelectionNetworkHtml = function(networkName, networkState, networkId, networkType)
{
  var html = '<option value="' + networkId + '">' + networkName + ' | ' + networkType + '</option>';

  return html;
};

ModemContent.prototype.RecursivlyRefresh = function() {
  var modemContent  = this;
  var networkConfig = deviceParams.list['modem_network_config'].value;
  
  // show actual configuration
  modemContent.Refresh(function() {

    // check state of network config
    var registrationState = modemContent.NetworkRegistrationOutputText(networkConfig.NetworkRegistrationState);
    if (registrationState === 'Unknown') {
      setTimeout(function() {
        modemContent.RecursivlyRefresh()
      }, 5000);
    }

  });
}

ModemContent.prototype.ProcessSimPinWrite = function(formObj)
{
  var modemContent  = this;
  var deviceParamId = 'modem_sim_pin';
  var pin           = $(formObj).find('input[name=pin]').val();
  var newValueList  = { pin: pin };

  if($(formObj).find('input[name=puk]').length)
  {
    deviceParamId       = 'modem_sim_puk';
    newValueList['puk'] = $(formObj).find('input[name=puk]').val();
  }

  pageElements.ShowBusyScreen('SIM authentication...');

  deviceParams.ChangeValue(deviceParamId, newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while SIM authentication.', errorText ]);
      modemContent.Refresh()
    }

    else {
      // refresh content until registration state is not unknown any more
      modemContent.RecursivlyRefresh()
    }
    
  });

};

ModemContent.prototype.ChangeSelectionMode = function(selectionMode)
{
  var modemContent  = this;

  pageElements.ShowBusyScreen('Change network configuration...');

  var newValueList  = { selectionMode: selectionMode };
  deviceParams.ChangeValue('modem_network_selection_mode', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while configuration of network.', errorText ]);
    }

    // show actual configuration
    modemContent.Refresh();
  });

  return;
};


ModemContent.prototype.ProcessProviderChangeSubmit = function(providerIndex)
{
  var modemContent  = this;
  var providerList  = deviceParams.list['modem_providerlist'].value;

  // get provider id and type directly from provider list in device params by its index
  if(   (SUCCESS != deviceParams.list['modem_providerlist'].status)
     || (undefined === providerList[providerIndex])
     || (undefined === (providerId    = providerList[providerIndex].id))
     || (undefined === (providerType  = providerList[providerIndex].type)) )
  {
    $('body').trigger('event_errorOccured', [ 'Invalid provider data.', errorText ]);
  }

  else
  {
    pageElements.ShowBusyScreen('Change network configuration...');

    var newValueList  = { networkId: providerId, networkType: providerType };
    deviceParams.ChangeValue('modem_network_provider', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while configuration of network.', errorText ]);
      }

      // show actual configuration
      modemContent.Refresh();
    });
  }
  return;
};


ModemContent.prototype.ProcessNetworkPackageServiceConfig = function(formObj)
{
  var modemContent  = this;

  // get parameters, mark empty strings by quotation marks
  var connectivity  = $(formObj).find('input[name=connectivity]:checked').length ? "ENABLED" : "DISABLED";
  var apn           = $(formObj).find('input[name=apn]').val()                || '""';
  var user          = $(formObj).find('input[name=user]').val()               || '""';
  var password      = $(formObj).find('input[name=password]').val()           || '""';
  var authType      = $(formObj).find('input[name=auth_type]:checked').val();

  var newValueList  = { apn: apn, authType: authType, userName: user, password: password, connectivity: connectivity };

  pageElements.ShowBusyScreen('Change network package service configuration...');

  deviceParams.ChangeValue('modem_gprs_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while network package service configuration.', errorText ]);
    }

    // show actual configuration
    modemContent.Refresh();
  });

};


ModemContent.prototype.UploadSoftware = function(formObj)
{
  var modemContent = this;

  pageElements.ShowBusyScreen('Upload software...');

  var fileInput = $(formObj).find('[id$=upload_file_real_input]')[0];
  var file = fileInput.files[0];
  
  Xhr.sessionToken = deviceCommunication.csrfToken;
  var upload = new Upload([
  {
    file: file,
    destination: modemContent.uploadDir + '/' + file.name
  }]);

  upload.execute(function(filepaths, error)
  {
    pageElements.RemoveBusyScreen();

    if (error)
    {
      $('body').trigger('event_errorOccured', [ 'File upload failed.', error.message ]);
    }
    else
    {
      var filepath = filepaths[0];
      
      $('#' + modemContent.id + '_content #modem_software_upload_file_real_input').val('');
      $('#' + modemContent.id + '_content #modem_software_upload_file_real_input').trigger('change');

      var newValueList  = { modemFirmwarePath: filepath, logfilePath: modemContent.logfilePath };

      pageElements.ShowBusyScreen('Activating new software...');

      deviceParams.ChangeValue('modem_firmware', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();

        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while activating new software.', errorText ]);
        }
        else
        {
          upload.cleanup();
          modemContent.Refresh();
        }
      });
    }

  });

};

