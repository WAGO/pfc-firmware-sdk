/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SnmpGeneralContent = function(id)
{
  var snmpGeneralContent = this;
  
  snmpGeneralContent.id = id || 'snmp_general';
  
  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var SnmpGeneraContentlView = (function()
  {
    snmpGeneralContent.paramView = new DeviceParamView();
                
    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_state',
      outputElementId : snmpGeneralContent.id + '_content #snmp_active_state'
    });

    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_device_name',
      outputElementId : snmpGeneralContent.id + '_content #device-name'
    });
    
    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_description',
      outputElementId : snmpGeneralContent.id + '_content #description'
    });
        
    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_physical_location',
      outputElementId : snmpGeneralContent.id + '_content #physical-location'
    });
        
    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_contact',
      outputElementId : snmpGeneralContent.id + '_content #contact'
    });

    snmpGeneralContent.paramView.Add(
    {
      paramId         : 'snmp_objectID',
      outputElementId : snmpGeneralContent.id + '_content #sysObjectID'
    });

  })();
  
  
  //------------------------------------------------------------------------------
  // event - first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpGeneralContent.id + '_create', function(event)
  {
    // add events for form processing
    $('#' + snmpGeneralContent.id + '_content #general_snmp_config').bind('submit', function(event)
    {
      event.preventDefault();
      snmpGeneralContent.ChangeConfig(this);
    });

  });


  //------------------------------------------------------------------------------
  // event - refresh content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(snmpGeneralContent.id + '_refresh', function(event)
  {
    snmpGeneralContent.Refresh();
  });

};


SnmpGeneralContent.prototype.Refresh = function()
{
  var snmpGeneralContent = this;
  
  deviceParams.ReadValueGroup(snmpGeneralContent.paramView.list, function()
  {
    snmpGeneralContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(snmpGeneralContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(snmpGeneralContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading SNMP configuration.', errorString ] );
    }
  });

};


SnmpGeneralContent.prototype.ChangeConfig = function(formObj)
{
  var snmpGeneralContent = this;
  
  pageElements.ShowBusyScreen('Changing general SNMP parameter...');

  // get script parameter from form data - given, but empty strings must be converted to "" to let the script
  // know, that this paramter value should be deleted. 
  var state             = ($(formObj).find('[name=service_active_state]:checked').length) ? 'enabled' : 'disabled';
  
  var deviceName        = $(formObj).find('input#device-name').val()        || "\"\"";
  var description       = $(formObj).find('input#description').val()        || "\"\"";
  var physicalLocation  = $(formObj).find('input#physical-location').val()  || "\"\"";
  var contact           = $(formObj).find('input#contact').val()            || "\"\"";
  var objectID          = $(formObj).find('input#sysObjectID').val()        || "\"\"";
  
  var newValueList  = { state: state, deviceName: deviceName, description: description, physicalLocation: physicalLocation, contact: contact, objectID: objectID };

  deviceParams.ChangeValue('snmp_general_config', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing SNMP parameter.', errorText ]);
    }

    snmpGeneralContent.Refresh();
  });

};


