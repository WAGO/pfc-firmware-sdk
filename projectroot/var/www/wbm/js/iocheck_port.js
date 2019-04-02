/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var IocheckPortContent = function(id)
{
  var iocheckPortContent = this;
  
  iocheckPortContent.id = id || 'iocheck_port';
  
  iocheckPortContent.comPortFirewallConfig = new FirewallServiceConfiguration('iocheck', '#iocheck_firewall_service_config_area', 'group', function() { iocheckPortContent.Refresh(); });

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var IocheckPortParamView = (function()
  {
    iocheckPortContent.paramView = new DeviceParamView();
                
    iocheckPortContent.paramView.Add(
    {
      paramId         : 'iocheck_port_state',
      outputElementId : iocheckPortContent.id + '_content #iocheck_port_active_state'
    });
            
    iocheckPortContent.comPortFirewallConfig.FillParamView(iocheckPortContent.paramView);
        
  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(iocheckPortContent.id + '_create', function(event)
  {
    iocheckPortContent.comPortFirewallConfig.Create();
    
    // add events for form processing
    $('#' + iocheckPortContent.id + '_content #iocheck_port_config_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var newState    = $(event.target).find('[name=service_active_state]:checked').length ? 'enabled' : 'disabled';
      iocheckPortContent.ChangeConfig(newState);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(iocheckPortContent.id + '_refresh', function(event)
  {
    iocheckPortContent.Refresh();
  });
  
};


IocheckPortContent.prototype.Refresh = function()
{
  var iocheckPortContent = this;
  
  deviceParams.ReadValueGroup(iocheckPortContent.paramView.list, function()
  {
    iocheckPortContent.comPortFirewallConfig.RefreshDisplay();
    iocheckPortContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(iocheckPortContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(iocheckPortContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading TFTP configuration.', errorString ] );
    }
  });

};



IocheckPortContent.prototype.ChangeConfig = function(state)
{
  var iocheckPortContent = this;

  // check input parameter
  if(('enabled' != state) && ('disabled' != state))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for port state.', state ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing I/O-Check port configuration...');

    var newValueList  = { state: state };

    deviceParams.ChangeValue('iocheck_port_state', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
  
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing I/O-Check port configuration.', errorText ]);
      }
  
      // show actual configuration
      iocheckPortContent.Refresh();
    });
  }
};

