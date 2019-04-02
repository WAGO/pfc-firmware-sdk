/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var StartRebootContent = function(id)
{
  var startRebootContent = this;
  
  startRebootContent.id = id || 'start_reboot';
  
  startRebootContent.rebootFinishedTimeout  = (1000 * 50); // 50 sec
  

  /*
   * event - first creation of area
   */
  $('#content_area').bind(startRebootContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + startRebootContent.id + '_content #reboot_button').bind('click', function(event)
    {
      event.preventDefault();
      startRebootContent.StartReboot()
    });
  });


  /*
   * event - refresh content area
   */
  $('#content_area').bind(startRebootContent.id + '_refresh', function(event)
  {
    // no actions necessary, data not changes
  });
};


//------------------------------------------------------------------------------
// reboot coupler
// event: click to reboot button
//------------------------------------------------------------------------------
StartRebootContent.prototype.StartReboot = function()
{
  var startRebootContent = this;

  // as a precaution recheck
  if(confirm('Really want to reboot controller?'))
  {
    pageElements.ShowBusyScreen('Reboot controller...');
 
    var newValueList = { };

    deviceParams.ChangeValue('start_reboot', newValueList, function(status, errorText)
    {
      if(SUCCESS != status)
      {
        pageElements.RemoveBusyScreen();
        $('body').trigger('event_errorOccured', [ 'Error while reboot.', errorText ]);
      }
      
      else deviceCommunication.WaitForConnection(startRebootContent.rebootFinishedTimeout, function(status)
      {
        pageElements.RemoveBusyScreen();
        if(SUCCESS != status)
        {
          $('body').trigger('event_errorOccured', [ 'Error while reboot.', 'Timeout' ]);
        }
        else
        {
          window.location.reload();
        }
      });
    });
  }
};




