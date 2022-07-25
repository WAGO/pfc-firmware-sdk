/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var TlsContent = function(id)
{
  var tlsContent = this;

  tlsContent.id = id || 'tls';

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------


  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var TlsParamView = (function()
  {
    tlsContent.paramView = new DeviceParamView();

    tlsContent.paramView.Add(
    {
      paramId         : 'tls_cipher_list_class',
    });

  })();


  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(tlsContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + tlsContent.id + '_content #tls_config_form').bind('submit', function(event)
    {
      event.preventDefault();
      var newCipherListClass =   $('input[name=tls-cipher-list-class]:checked').val();
      tlsContent.ChangetlsCipherListClass(newCipherListClass);
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(tlsContent.id + '_refresh', function(event)
  {
    tlsContent.Refresh();
  });

};


TlsContent.prototype.Refresh = function()
{
  var tlsContent = this;

  deviceParams.ReadValueGroup(tlsContent.paramView.list, function()
  {
    if(deviceParams.ReadErrorOccured(tlsContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(tlsContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading TLS configuration.', errorString ] );
    }
    else
    {
      switch(deviceParams.list['tls_cipher_list_class'].value)
      {
        case 'standard':  $("input[name=tls-cipher-list-class][value=standard]").prop("checked", "checked"); break;
        case 'strong':    $("input[name=tls-cipher-list-class][value=strong]").prop("checked", "checked"); break;
        default:          break;
      }
    }
  });

};

TlsContent.prototype.ChangetlsCipherListClass = function(newCipherListClass)
{
  var tlsContent = this;
  var newValueList;

  if(newCipherListClass === 'standard')
  {
    newValueList = { cipherListClass: 'standard' };
  }
  else
  {
    newValueList = { cipherListClass: 'strong' };
  }

  pageElements.ShowBusyScreen('Changing TLS version...');


  deviceParams.ChangeValue('tls_cipher_list_class', newValueList, function(status, errorText)
  {
    if(SUCCESS != status)
    {
      pageElements.RemoveBusyScreen();
      $('body').trigger('event_errorOccured', [ 'Error while changing TLS version.', errorText ]);
    } else {
      // after a successfull change, a restart of the webserver is required for the changes to take effect
      // then initiate restart of webserver
      deviceParams.ChangeValue('restart_webserver', { }, function(status, errorText)
      {
        // webserver restart will always return with an error -
        //  do nothing, but hide the busy-box and re-read the values
        
        // wait about 1 second (reading takes about that amount of time)
        setTimeout(function() {
          // show current configuration
          tlsContent.Refresh();
          pageElements.RemoveBusyScreen();
        }, 1000);

      });
    }
  });
};
