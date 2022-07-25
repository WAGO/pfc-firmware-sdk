/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*------------------------------------------------------------------------------
 * Object DsaConfiguration
 *
 *------------------------------------------------------------------------------
 */
function DsaConfiguration()
{
  var dsaConfig = this;

};


DsaConfiguration.prototype.Create = function()
{
  var dsaConfig = this;

  // add event for submit of form
  $('#dsa_configuration_area #dsa_state_form').bind('submit', function(event)
  {
    event.preventDefault();
    dsaConfig.Change(event.target);
  });

};


/*
 * Request actual values AND refresh display
 */
DsaConfiguration.prototype.Refresh = function()
{
  var dsaConfig = this;

  deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
  {
    dsaConfig.RefreshDisplay();
  });
};


/*
 * Refresh only display of values - without requesting data
 */
DsaConfiguration.prototype.RefreshDisplay = function()
{
  var dsaConfig       = this;
  var interfaceConfig = deviceParams.list['interface_config_xml'].value;

  if((SUCCESS != deviceParams.list['interface_config_xml'].status) || !interfaceConfig.dsaMode)
  {
    var errorString = deviceParams.list['interface_config_xml'].errorText;
    $('body').trigger('event_errorOccured', [ 'Error while reading switch configuration.', errorString ] );
  }

  else
  {
    $('#dsa_state_form').find("[name='switchconfig'][value='" + interfaceConfig.dsaMode + "']").prop('checked', 'checked');
  }
};



/**
 * click event for changing DSA State button
 *
 */
DsaConfiguration.prototype.Change = function(formObj)
{
  var dsaConfig   = this;

  pageElements.ShowBusyScreen('Changing switch state...');

  var newDsaMode  = $("input[name='switchconfig']:checked").val();
  var newValueList  = { dsaMode: newDsaMode };

  deviceParams.ChangeValue('dsa_mode', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    // error while processing - evaluate if problems with connection are probable
    if(SUCCESS != status)
    {
      // dirty hack to avoid a lost connection error during changing switch state.
      if ("error" !== errorText )
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing switch state.', errorText ]);
      }
    }

    // refresh whole page, because most of the entries are depending on switch configuration
    $('#content_area').trigger('interfaces_refresh');
    $('#content_area').trigger('switch_configuration_refresh');
  });


};



/*
 * ------------------------------------------------------------------------------
 * SwitchConfiguration
 *------------------------------------------------------------------------------
 */
function SwitchConfiguration()
{
  var switchConfig = this;

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var ShowBroadcastProtection = function(paramStatus, paramValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      $('#' + outputElementId).find('option[value=' + paramValue + ']').prop('selected', 'selected');
    }
  };

  var ShowPortMirror = function(paramStatus, paramValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      $('#switch_configuration_area input[name=port_mirror]').removeAttr('checked');
      $('#switch_configuration_area input[name=port_mirror][value= ' + paramValue + ']').prop('checked', 'checked');
    }
  };

  var ShowRateLimit = function(paramStatus, paramValue, outputElementId)
  {
    if(SUCCESS === paramStatus)
    {
      if('disabled' === paramValue) paramValue = 'off';
      var test = $('#' + outputElementId).find('option[value="64.kbps"]');
      $('#' + outputElementId).find('option[value="' + paramValue + '"]').prop('selected', 'selected');
    }
  };

  var CreateRateLimitSelection = function(paramStatus, validRateLimitList, outputElementId)
  {
    // menu not created by now (we do it only once)
    if((SUCCESS === paramStatus) && (0 === $('#' + outputElementId).find('option').length))
    {
      $('#' + outputElementId).append('<option value="off">Disabled</option>');
      $.each(validRateLimitList, function(rateLimitIndex, rateLimitValue)
      {
        if('off' != rateLimitValue)
        {
          $('#' + outputElementId).append('<option value="' + rateLimitValue + '">' + rateLimitValue.replace('.', ' ') + '</option>');
        }
      });
    }
  };

  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var switchConfigParamView = (function()
  {
    switchConfig.paramView = new DeviceParamView();

    switchConfig.paramView.Add(
    {
      paramId         : 'switch_valid_rate_limits',
      outputElementId : 'switch_configuration_area #switch_config_rate_limit',
      outputFkt       : CreateRateLimitSelection
    });

    switchConfig.paramView.Add(
    {
      paramId         : 'switch_broadcast_protection',
      outputElementId : 'switch_configuration_area #switch_config_broadcast_protection',
      outputFkt       : ShowBroadcastProtection
    });

    switchConfig.paramView.Add(
    {
      paramId         : 'switch_port_mirror',
      outputFkt       : ShowPortMirror
    });

    switchConfig.paramView.Add(
    {
      paramId         : 'switch_fast_aging_state',
      outputElementId : 'switch_configuration_area #switch_config_fast_aging_enabled'
    });

    switchConfig.paramView.Add(
    {
      paramId         : 'switch_rate_limit',
      outputElementId : 'switch_configuration_area #switch_config_rate_limit',
      outputFkt       : ShowRateLimit
    });

  })();

};


SwitchConfiguration.prototype.Create = function()
{
  var switchConfig = this;

  // add event for submit of form
  $('#switch_config_form').bind('submit', function(event)
  {
    event.preventDefault();
    switchConfig.Change(event.target);
  });

  //------------------------------------------------------------------------------
  // refresh of area (needed, if only switch config area should be refreshed, not the whole page
  //------------------------------------------------------------------------------
  $('#content_area').bind('switch_configuration_refresh', function(event)
  {
    switchConfig.Refresh();
  });
  
};


/*
 * Request actual values AND refresh display
 */
SwitchConfiguration.prototype.Refresh = function()
{
  var switchConfiguration = this;

  deviceParams.ReadValueGroup(switchConfiguration.paramView.list, function()
  {
    switchConfiguration.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(switchConfiguration.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(switchConfiguration.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading switch configuration.', errorString ] );
    }
  });

};


SwitchConfiguration.prototype.Change = function(formObj)
{
  var switchConfiguration = this;
  var portMirror          = $(formObj).find('[name=port_mirror]:checked').val();
  var fastAging           = $(formObj).find('[name=fast_aging_enabled]:checked').length ? '1' : '0';
  var broadcastProtection = $(formObj).find('[name=broadcast_protection]').val();
  var rateLimit           = $(formObj).find('[name=rate_limit]').val();

  pageElements.ShowBusyScreen('Changing switch configuration...');

  var newValueList  = { broadcastProtection: broadcastProtection, rateLimit: rateLimit, portMirror: portMirror, fastAging: fastAging };

  deviceParams.ChangeValue('switch_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing switch configuration.', errorText ]);
    }

    switchConfiguration.Refresh();
  });

};



function EthernetContent(id)
{
  var ethernetContent = this;

  ethernetContent.id = id || 'ethernet';

  ethernetContent.dsaConfig                   = new DsaConfiguration;
  ethernetContent.switchConfiguration         = new SwitchConfiguration;

  ethernetContent.compatibilityMode = false;

  

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(ethernetContent.id + '_create', function(event)
  {
	  ethernetContent.Create();
	  ethernetContent.dsaConfig.Create();
	  
      var paramId = 'order_number';
      deviceParams.ReadValue({ id: paramId}, function() {
        var param = deviceParams.list[paramId];
        if (SUCCESS == param.status)
        {
          if (param.value.includes('750-8210') ||
              param.value.includes('750-8211') ||
              param.value.includes('750-8215') ) {
        	$('#switch_configuration_area').remove();
            $('#dsa_configuration_area .config_form_box.group').removeClass('group');
          }
          else{
            ethernetContent.switchConfiguration.Create();
          }
        } else {
          var errorText = param.errorText;
          $('body').trigger('event_errorOccured', [ 'Error while reading order number.', errorText ]);
        }
      });
  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(ethernetContent.id + '_refresh', function(event)
  {
    ethernetContent.Refresh();

    ethernetContent.dsaConfig.Refresh();
    
    var paramId = 'order_number';
    deviceParams.ReadValue({ id: paramId}, function() {
      var param = deviceParams.list[paramId];
      if (SUCCESS == param.status)
      {
        if (!param.value.includes('750-8210') &&
            !param.value.includes('750-8211') &&
            !param.value.includes('750-8215') ) {
          ethernetContent.switchConfiguration.Refresh();
        }
      } else {
        var errorText = param.errorText;
        $('body').trigger('event_errorOccured', [ 'Error while reading order number.', errorText ]);
      }
    });
  });
};



//------------------------------------------------------------------------------
// first creation of area
//------------------------------------------------------------------------------
EthernetContent.prototype.Create = function()
{
  var ethernetContent = this;

  // create and show html
  // - we can do it here at creation as long as we always show exact these two interface blocks (X1 and X2)
  $('#' + ethernetContent.id + '_content #eth_config_ifaces_area').append(ethernetContent.IFaceHtml('X1'));
  $('#' + ethernetContent.id + '_content #eth_config_ifaces_area').append(ethernetContent.IFaceHtml('X2'));

  // add event - click to single checkbox (enable/disable states must be adapted)
  $('#' + ethernetContent.id + '_content input[type=checkbox]').bind('click', function(event)
  {
    var portName = this.id.match(/X./);
    ethernetContent.VisualizeDependencies(portName);
  });

  // click to config form submit
  $('#' + ethernetContent.id + '_content #eth_config_ifaces_area').bind('submit', function(event)
  {
    event.preventDefault();
    ethernetContent.ChangeConfig(event.target);
  });

};



EthernetContent.prototype.Refresh = function()
{
  var ethernetContent = this;

  deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
  {
    //Log('defaultGatewayConfig - refresh by xml struct');
    ethernetContent.RefreshDisplay();
  });
};



//------------------------------------------------------------------------------
// refresh ethernet values via xml structure (already read)
// as long as we always show exact two ethernet interfaces (X1 and X2), we create
// html content diretly at page creation and are not in need to repeat it here at any refresh
//------------------------------------------------------------------------------
EthernetContent.prototype.RefreshDisplay = function()
{
  var ethernetContent = this;
  var interfaceConfig = deviceParams.list['interface_config_xml'].value;
  var iFaceCount      = 0;

  if((SUCCESS != deviceParams.list['interface_config_xml'].status) || !interfaceConfig.ifaces)
  {
    var errorString = deviceParams.list['interface_config_xml'].errorText;
    $('body').trigger('event_errorOccured', [ 'Error while reading ethernet configuration.', errorString ] );
  }

  else
  {
    // get values all configured ethernet interfaces
    $.each(interfaceConfig.ifaces, function(iFaceId, iFace)
    {
      if(iFace.ethernetSettings)
      {
        var portName  = iFace.ethernetSettings.portName;
        var iFaceArea = $('#eth_config_ifaces_area #ethernet_config_' + portName);

        // (re)set state checkbox
        if('enabled'  === iFace.state) $(iFaceArea).find('[id^=eth_enabled]').prop('checked', 'checked');
        if('disabled' === iFace.state) $(iFaceArea).find('[id^=eth_enabled]').removeAttr('checked');

        // (re)set autoneg checkbox
        if('enabled'  === iFace.ethernetSettings.autoneg) $(iFaceArea).find('[id^=eth_autoneg]').prop('checked', 'checked');
        if('disabled' === iFace.ethernetSettings.autoneg) $(iFaceArea).find('[id^=eth_autoneg]').removeAttr('checked');

        // create value string for selection menu of single speed and duplex value
        var speedDuplexValueString = iFace.ethernetSettings.speed.replace('M', '') + '-' + iFace.ethernetSettings.duplex;

        // Not working with jQuery 1.7 in IE: $(iFaceArea).find('[id^=eth_speed_duplex]').val(speedDuplexValueString);
        $(iFaceArea).find('[id^=eth_speed_duplex] option[value="' + speedDuplexValueString + '"]').prop('selected', 'selected');

        // check if speed/duplex value was existing in selection menu at all and otherwise show extra point "unknown"
        if(speedDuplexValueString !== $(iFaceArea).find('[id^=eth_speed_duplex]').val())
        {
          $(iFaceArea).find('[id^=eth_speed_duplex]').append('<option value="unknown" selected >Unknown</option>');
        }
        else
        {
          $(iFaceArea).find('[id^=eth_speed_duplex][value=unknown]').remove();
        }

        ethernetContent.VisualizeDependencies(portName);
      }
    });
  }

};



EthernetContent.prototype.IFaceHtml = function(portName)
{
  var ethernetContent = this;

  var html = '';
  html += '<h3>Interface ' + portName + '</h3>';
  html += '<form id="ethernet_config_' + portName + '" action="javascript_requested.php" >';
  html +=   '<div class="config_form_box">';
  html +=     '<div class="config_data_list">';

  html +=       '<div class="label_value_pair one_interfaces">';
  html +=         '<div class="label_field"><label for="eth_enabled_' + portName + '">Enabled:</label></div>';
  html +=         '<div class="value_field"><input id="eth_enabled_' + portName + '" type="checkbox" name="eth_state_' + portName + '" value="enabled" ></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="eth_autoneg_' + portName + '">Autonegotiation on:</label></div>';
  html +=         '<div class="value_field"><input id="eth_autoneg_' + portName + '" type="checkbox" name="eth_autoneg_' + portName + '"></div>';
  html +=       '</div>';
  html +=       '<div class="label_value_pair">';
  html +=         '<div class="label_field"><label for="eth_speed_duplex_eth1">Speed/Duplex:</label></div>';
  html +=         '<div class="value_field">';
  html +=           '<select id="eth_speed_duplex_' + portName + '" class="input_field" name="eth_speed_duplex_' + portName + '">';
  html +=             '<option value="10-half">10 MBit half duplex</option>';
  html +=             '<option value="10-full">10 MBit full duplex</option>';
  html +=             '<option value="100-half">100 MBit half duplex</option>';
  html +=             '<option value="100-full">100 MBit full duplex</option>';
  html +=           '</select>';
  html +=         '</div>';
  html +=       '</div>';
  html +=     '</div>';
  html +=     '<p class="button_area">';
  html +=       '<input class="button" type="submit" value="Submit" >';
  html +=     '</p>';
  html +=   '</div>';
  html += '</form>';

  return html;
};


//------------------------------------------------------------------------------
// enable/disable form fields depending on values of other fields (read or set by user)
//------------------------------------------------------------------------------
EthernetContent.prototype.VisualizeDependencies = function(portName)
{
  var ethernetContent = this;

  // disable autoneg field
  $('#' + ethernetContent.id + '_content [name=eth_autoneg_' + portName + ']').attr('disabled', true);

  // disable speed/duplex field
  $('#' + ethernetContent.id + '_content [name=eth_speed_duplex_' + portName + ']').attr('disabled', true);

  // state enabled?
  if($('#' + ethernetContent.id + '_content [name=eth_state_' + portName + ']').prop('checked'))
  {
    // enable autoneg field
    $('#' + ethernetContent.id + '_content [name=eth_autoneg_' + portName + ']').removeAttr('disabled');

    // autoneg enabled?
    if(!$('#' + ethernetContent.id + '_content [name=eth_autoneg_' + portName + ']').prop('checked'))
    {
      // enable speed/duplex field
      $('#' + ethernetContent.id + '_content [name=eth_speed_duplex_' + portName + ']').removeAttr('disabled');
    }
  }
};


//------------------------------------------------------------------------------
// process change of configuration by form submit
//------------------------------------------------------------------------------
EthernetContent.prototype.ChangeConfig = function(formObj)
{
  var ethernetContent = this;
  var portName        = $(formObj).attr('id').match(/X./);

  pageElements.ShowBusyScreen('Changing ethernet configuration...');

  var state   = ($(formObj).find('[name^=eth_state]').prop('checked'))   ? 'on' : 'off';
  var autoneg = ($(formObj).find('[name^=eth_autoneg]').prop('checked')) ? 'on' : 'off';

  var speedDuplexString = $(formObj).find('[name^=eth_speed_duplex]').val();
  var duplex            = speedDuplexString.match('full') ? 'full' : 'half';
  var speed             = speedDuplexString.replace('-' + duplex, '') + 'M';

  var newValueList  = { portName: portName, state: state, autoneg: autoneg, speed: speed, duplex: duplex };

  deviceParams.ChangeValue('config_ethernet', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing ethernet configuration.', errorText ]);
    }

    // show actual configuration
    ethernetContent.Refresh();
  });

};
