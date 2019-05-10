/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var TftpContent = function(id)
{
  var tftpContent = this;
  
  tftpContent.id = id || 'tftp';
  
  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var TftpParamView = (function()
  {
    tftpContent.paramView = new DeviceParamView();
                
    tftpContent.paramView.Add(
    {
      paramId         : 'tftp_state',
      outputElementId : tftpContent.id + '_content #tftp_active_state'
    });
            
    tftpContent.paramView.Add(
    {
      paramId         : 'tftp_download_dir',
      outputElementId : tftpContent.id + '_content #tftp_download_dir'
    });
    
  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(tftpContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + tftpContent.id + '_content #tftp_config_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var newState    = $(event.target).find('[name=service_active_state]:checked').length ? 'enabled' : 'disabled';
      var downloadDir = $(event.target).find('input[name=download_dir]').val();
      tftpContent.ChangeConfig(newState, downloadDir);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(tftpContent.id + '_refresh', function(event)
  {
    tftpContent.Refresh();
  });
  
};


TftpContent.prototype.Refresh = function()
{
  var tftpContent = this;
  
  deviceParams.ReadValueGroup(tftpContent.paramView.list, function()
  {
    tftpContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(tftpContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(tftpContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading TFTP configuration.', errorString ] );
    }
  });

};


TftpContent.prototype.ChangeState = function(buttonObj)
{
  var tftpContent = this;
  
  var newState  = ('Enable' === buttonObj.value) ? 'enabled' : 'disabled';

  pageElements.ShowBusyScreen('Changing TFTP state...');

  var newValueList  = { state: newState };

  deviceParams.ChangeValue('tftp_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing TFTP state.', errorText ]);
    }
    else
    {
      // show actual configuration
      //tftpContent.Refresh();
      pageElements.VisualizeStateByTextAndButton(SUCCESS, newState, tftpContent.id + '_content #tftp_state_form');
    }
  });

};


TftpContent.prototype.ChangeDownloadDir = function(downloadDir)
{
  var tftpContent = this;

  pageElements.ShowBusyScreen('Changing download directory...');

  var newValueList  = { downloadDir: downloadDir };

  deviceParams.ChangeValue('tftp_download_dir', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
  
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing download directory.', errorText ]);
    }
  
    // show actual configuration
    tftpContent.Refresh();
  });
};


TftpContent.prototype.ChangeConfig = function(state, downloadDir)
{
  var tftpContent = this;

  // check input parameter
  if(('enabled' != state) && ('disabled' != state))
  {
    $('body').trigger('event_errorOccured', [ 'Software Error: wrong parameter for port state.', state ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen('Changing TFTP configuration...');

    var newValueList  = { state: state, downloadDir: downloadDir };

    deviceParams.ChangeValue('tftp_config', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
  
      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while changing TFTP configuration.', errorText ]);
      }
  
      // show actual configuration
      tftpContent.Refresh();
    });
  }
};

