/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function InterfaceConfig()
{
  var interfaceConfig = this;
  
  //interfaceConfig.configfile  = 'network-interfaces.xml';
  interfaceConfig.status      = ERROR;
  interfaceConfig.ifaces      = {};
  
  interfaceConfig.compatibilityModePossible  = false;
};



InterfaceConfig.prototype.DeleteConfig = function()
{
  var interfaceConfig = this;
  
  $.each(interfaceConfig.ifaces, function(iFaceId, iFace)
  {
    delete interfaceConfig.ifaces[iFaceId];
  });
};


InterfaceConfig.prototype.CreateConfig = function()
{
  var interfaceConfig = this;
};
  

InterfaceConfig.prototype.Refresh = function(callback)
{
  var interfaceConfig = this;

  interfaceConfig.DeleteConfig();
  
  // get inferface xml confix via configtool
  deviceParams.ReadValue({ id: 'interface_config_xml' }, function()
  {
    if(SUCCESS != deviceParams.list['interface_config_xml'].status)
    {
      Log('failed getting interface config file');
    
      interfaceConfig.status  = NOT_FOUND;
      $('body').trigger('event_errorOccured', [ 'Error while reading interface configuration.', deviceParams.list['interface_config_xml'].errorText ]);
      if(callback) callback(NOT_FOUND);
      //else $('#content_area').trigger('event_interfaceConfig_updated', [ NOT_FOUND ]);
    }
    else
    {
      //Log('interfaceConfig - received XML');
      // shifted xml convertation to central deviceParams object
      //interfaceConfig.ConvertXML(deviceParams.list['interface_config_xml'].value);
      $.each(deviceParams.list['interface_config_xml'].value.ifaces, function(iFaceId, iFace)
      {
        interfaceConfig.ifaces[iFaceId] = $.extend(true, {}, iFace);
      });
    
      if(callback) callback(SUCCESS);
      else 
      {
        $('#content_area').trigger('event_interfaceConfig_updated', [ SUCCESS ]);
      }
    }
  });

  //get interfaces config file direcly (old way)
  /*
  var xmlFileArray = $.ajax({ type: "GET", url: interfaceConfig.configfile, async: false, cache: false });

  var xmlResponse = null;
  if(xmlFileArray.status === 200)
  {
    // especially for IE - use active X to parse XML document really as XML structure...
    if(window.ActiveXObject)
    {
      var xml = new ActiveXObject("Microsoft.XMLDOM");
      xml.loadXML(xmlFileArray.responseText);
      xmlResponse = xml;
    }
  
    // ... and for any other ordinary browser - just get automatically generated xml
    else
    {
      xmlResponse = xmlFileArray.responseXML;
    }
  }

  // can't get xml config file?
  if(!xmlResponse)
  {
    Log('failed getting interface config file');
    
    interfaceConfig.status  = NOT_FOUND;
    if(callback) callback(NOT_FOUND);
    //else $('#content_area').trigger('event_interfaceConfig_updated', [ NOT_FOUND ]);

    //$('body').trigger('event_errorOccured', [ 'Can\'t get interface configuration - expect default.', 
    //                                          'File ' + interfaceConfig.configfile + ' missing or incorrect' ]);
  }
  
  // got XML file
  else
  {
    Log('received interface config file');
    interfaceConfig.ConvertXML(xmlResponse);
    //interfaceConfig.xml     = xmlResponse;
    interfaceConfig.status  = SUCCESS;
    
    if(callback) callback(SUCCESS);
    else 
    {
      $('#content_area').trigger('event_interfaceConfig_updated', [ SUCCESS ]);
    }
  }
  */
  
};


/*
 * extract data from xml config
 */
InterfaceConfig.prototype.ConvertXML = function(xmlConfig)
{
  var interfaceConfig = this;

  interfaceConfig.dsaMode = $(xmlConfig).find('dsa_mode').text();
  
  $(xmlConfig).find('iface').each(function(index, iFace)
  {
    var iFaceId = $(iFace).find('> device_name').text();

    if($(iFace).find('> ip_settings').length && ('1' === $(iFace).find('ip_settings > show_in_wbm').text()))
    {
      interfaceConfig.ifaces[iFaceId] =
      //interfaceConfig.aIfaces.push(
      {
        deviceName:       $(iFace).find('> device_name').text(),
        state:            $(iFace).find('> state').text(),
        ipSettings:
        {
          portName:         $(iFace).find('> ip_settings > port_name').text(),
          type:             $(iFace).find('> ip_settings > type').text(),
          staticIpAddr:     $(iFace).find('> ip_settings > static_ipaddr').text(),
          staticNetmask:    $(iFace).find('> ip_settings > static_netmask').text(),
          staticBroadcast:  $(iFace).find('> ip_settings > static_broadcast').text(),
          gateway:
          {
            ip:             $(iFace).find('> ip_settings > gw > ip').text(),
            state:          $(iFace).find('> ip_settings > gw > state').text(),
          }
        }
      };
    }
      
    else if($(iFace).find('> ethernet_settings').length)
    {
      interfaceConfig.ifaces[iFaceId] =
      //interfaceConfig.aIfaces.push(
      {
        deviceName:       $(iFace).find('> device_name').text(),
        state:            $(iFace).find('> state').text(),
        ethernetSettings:
        {
          portName:       $(iFace).find('> ethernet_settings > port_name').text(),
          autoneg:        $(iFace).find('> ethernet_settings > autoneg').text(),
          speed:          $(iFace).find('> ethernet_settings > speed').text(),
          duplex:         $(iFace).find('> ethernet_settings > duplex').text(),
          mac:            $(iFace).find('> ethernet_settings > mac').text(),
        }
      };
    }
  });

};


InterfaceConfig.prototype.IpAddressOk = function(ipAddress)
{
  var ipAddressOk = false;
  var pattern     = /^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/
 
  if(ipAddress && ipAddress.match(pattern)) ipAddressOk = true;
  
  return ipAddressOk;
};

var interfaceConfig = new InterfaceConfig();

