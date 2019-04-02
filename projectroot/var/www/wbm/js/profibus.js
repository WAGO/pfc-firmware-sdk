/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var ProfibusContent = function(id)
{
  var profibusContent = this;
  
  profibusContent.id    = id || 'profibus';
  profibusContent.area  = null; 
  

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  // show state of flag "change of slave address allowed" by image and text
  //------------------------------------------------------------------------------
  ShowSsaReleasedState = function(paramStatus, paramValue, outputElementId)
  {
    if((SUCCESS === paramStatus) && ("true" === paramValue))
    {
      $('#user_ssa_released').html('<img class="state" src="images/haken_gruen_10.gif" alt="enabled" title="enabled">enabled');
    }
    
    else if((SUCCESS === paramStatus) && ("false" === paramValue))
    {
      $('#user_ssa_released').html('<img class="state" src="images/kreuz_rot_10.gif" alt="disabled" title="disabled">disabled');
    }
    
    else 
    {
      $('#user_ssa_released').html('Error');
    }
  };

  //------------------------------------------------------------------------------
  // reference profibus content device parameter
  //------------------------------------------------------------------------------
  var CreateProfibusParamView = (function()
  {
    profibusContent.paramView = new DeviceParamView();
    
    profibusContent.paramView.Add(
    {
      paramId         : 'pbdp_ssa_user_slave_address'
    });

    profibusContent.paramView.Add(
    {
      paramId         : 'pbdp_ssa_user_slave_address_by_plc'
    });

    profibusContent.paramView.Add(
    {
      paramId         : 'pbdp_ssa_user_released',
      outputElementId : 'factory_ssa_released',
      outputFkt       : ShowSsaReleasedState
    });

    profibusContent.paramView.Add(
    {
      paramId         : 'pbdp_ssa_factory_slave_address',
      outputElementId : 'factory_slave_address'
    });

    profibusContent.paramView.Add(
    {
      paramId         : 'pbdp_ssa_factory_released',
      outputElementId : 'factory_ssa_released'
    });
    
  })();


  //------------------------------------------------------------------------------
  // event - first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(profibusContent.id + '_create', function(event)
  {
    profibusContent.area = '#' + profibusContent.id + '_content';
    
    // add events for form processing
    $(profibusContent.area + ' #ssa_reset_form').bind('submit', function(event)
    {
      event.preventDefault();
      profibusContent.ResetSsaValues();
    });

  });


  //------------------------------------------------------------------------------
  // event - refresh content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(profibusContent.id + '_refresh', function(event)
  {
    profibusContent.Refresh();
  });

};


//------------------------------------------------------------------------------
// read profibus parameter from device (again)
//------------------------------------------------------------------------------
ProfibusContent.prototype.Refresh = function()
{
  var profibusContent = this;

  $(profibusContent.area + ' #user_slave_address').html('unknown'); 
  $(profibusContent.area + ' #user_ssa_released').html('unknown'); 

  deviceParams.ReadValueGroup(profibusContent.paramView.list, function()
  {
    profibusContent.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(profibusContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(profibusContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading values.', errorString ] );
    }
    else
    {
      var slaveAddressByProfibus  = deviceParams.list['pbdp_ssa_user_slave_address'].value;
      var slaveAddressByPlc       = deviceParams.list['pbdp_ssa_user_slave_address_by_plc'].value;
      
      var slaveAddress            = slaveAddressByProfibus || slaveAddressByPlc;
      var slaveAddressNote        = slaveAddressByProfibus ? 'Set-Slave-Address Service (SSA)' : 'PLC';
      
      $('#' + profibusContent.id + '_content #user_slave_address').html(slaveAddress);
      $('#' + profibusContent.id + '_content #profibus_ssa_slave_address_source_note').html(slaveAddressNote);
    }
  });
};


//------------------------------------------------------------------------------
// process form - reset SSA values
//------------------------------------------------------------------------------
ProfibusContent.prototype.ResetSsaValues = function()
{
  var profibusContent = this;

  // get stored factory values from hidden input fields
  var factorySlaveAddress = $(profibusContent.area + ' #factory_slave_address').val();
  var factorySsaReleased  = $(profibusContent.area + ' #factory_ssa_released').val();
  
  if(   (isNaN === parseInt(factorySlaveAddress, 10)) 
     || ('true' != factorySsaReleased) && ('false' != factorySsaReleased))
  {
    $('body').trigger('event_errorOccured', [ 'Error: No factory values available. Try reload of WBM.' ] );
  }
  
  else
  {
    pageElements.ShowBusyScreen("Reset SSA values...");

    var newValueList = { ssaReleased: factorySsaReleased };

    deviceParams.ChangeValue('pbdp_ssa_user_released', newValueList, function(ssaReleasedStatus, errorText)
    {
      //var ssaReleasedStatus = status; 

      var newValueList = { slaveAddress: factorySlaveAddress };

      deviceParams.ChangeValue('pbdp_ssa_user_slave_address', newValueList, function(status, errorText)
      {
        pageElements.RemoveBusyScreen();
        profibusContent.Refresh();
    
        if((SUCCESS != ssaReleasedStatus) || (SUCCESS != status))
        {
          $('body').trigger('event_errorOccured', [ 'Error while reset.', errorText ]);
        }
      });
    });
  }
};




  