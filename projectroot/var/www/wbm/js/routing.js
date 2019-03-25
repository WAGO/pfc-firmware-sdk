/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var RoutingContent = function(id)
{
  var routingContent = this;

  routingContent.id = id || 'routing';

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------


  var RoutingParamView = (function()
  {
    routingContent.paramView = new DeviceParamView();

    routingContent.paramView.Add(
    {
      paramId         : 'routing_config'
    });

  })();


  $('#content_area').bind(routingContent.id + '_create', function(event)
  {
    // add event general config submit
    $('#' + routingContent.id + '_content #config_routing').bind('submit', function(event)
    {
      event.preventDefault();
      routingContent.ProcessConfigChangeSubmit(event.target);
    });

    // add event for add static route
    $('#' + routingContent.id + '_content #add_static_route').bind('submit', function(event)
    {
      event.preventDefault();
      routingContent.ProcessAddStaticRoute(event.target);
    });

    // add event for add masquerading
    $('#' + routingContent.id + '_content #add_masquerading').bind('submit', function(event)
    {
      event.preventDefault();
      routingContent.ProcessAddMasquerading(event.target);
    });

    // add event for add port forwarding
    $('#' + routingContent.id + '_content #add_forwarding').bind('submit', function(event)
    {
      event.preventDefault();
      routingContent.ProcessAddForwarding(event.target);
    });

  });

  $('#content_area').bind(routingContent.id + '_refresh', function(event)
  {
    routingContent.Refresh();
  });

};


RoutingContent.prototype.Refresh = function()
{
  var routingContent = this;
  pageElements.ShowBusyScreen('Reading Routing configuration...');  

  deviceParams.ReadValueGroup(routingContent.paramView.list, function()
  {
    if(   deviceParams.ReadErrorOccured(routingContent.paramView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(routingContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading Routing configuration.', errorString ] );
    }
    else
    {
      routingContent.ShowConfigFromObject();
    }
    pageElements.RemoveBusyScreen();    
  });

};

RoutingContent.prototype.ShowConfigFromObject = function()
{
  var routingContent  = this;

  if(SUCCESS === deviceParams.list['routing_config'].status)
  {
    // contains the whole routing configuration 
    var routingConfig = JSON.parse(deviceParams.list['routing_config'].value);


    // general configuration box
    var routingGeneralArea   = $('#' + routingContent.id + '_content #config_routing');
    $(routingGeneralArea).find('#routing_state').prop('checked', false);
    var generalConfig = routingConfig.generalRoutingConfig;
    if (generalConfig.routingEnabled == 'enabled') {
      $(routingGeneralArea).find('#routing_state').prop('checked', true);
    }


    // static routes box
    var routingStaticRoutes   = $('#' + routingContent.id + '_content #static_routes_area');
    var addStaticRoute        = $('#add_static_route').parent();
    var oldStaticRoutes       = $('[id^="static_route_"').parent();
    oldStaticRoutes.remove();
    var staticRouteTemplate   = $('#template_static_route');
    
    var self = this;
    $.each(routingConfig["staticRoutes"], function(index, staticRoute) {
      var staticRouteElement = staticRouteTemplate.clone(true);
      staticRouteElement.attr('id', 'static_route_' + index);
      
      var deleteButton = staticRouteElement.find('#template_delete_static_route');
      deleteButton.attr('id', 'delete_static_route_' + index);

      // hide delete button, if this static route is of type 'fixed'
      if (staticRoute['routingType'] == 'fixed') {
        deleteButton.remove();
      }

      var submitButton = staticRouteElement.find('#template_change_static_route');
      submitButton.attr('id', 'change_static_route_' + index);

      // append new box
      addStaticRoute.before(staticRouteElement);
      staticRouteElement.wrap('<div class="config_form_box group"></div>');

      // set values
      staticRouteElement.find('[name="static_route_state"]').prop('checked', false);
      if (staticRoute['state'] == 'enabled') {
        staticRouteElement.find('[name="static_route_state"]').prop('checked', true);
      }
      staticRouteElement.find('[name="static_route_destination_address"]').val(staticRoute['destination']);
      staticRouteElement.find('[name="static_route_destination_mask"]').val(staticRoute['destinationMask']);
      staticRouteElement.find('[name="static_route_gateway_address"]').val(staticRoute['gateway']);
      staticRouteElement.find('[name="static_route_gateway_metric"]').val(staticRoute['metric']);

      // bind to submit and delete button events
      submitButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessChangeStaticRoute(index, staticRouteElement);
      });
      deleteButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessDeleteStaticRoute(index);
      });
    });



    // dynamic routes box
    var routingDynamicRoutes   = $('#' + routingContent.id + '_content #dynamic_routes_area');
    var oldDynamicRoutes       = $('[id^="dynamic_route_"').parent();
    oldDynamicRoutes.remove();
    var dynamicRouteTemplate   = $('#template_dynamic_route');

    /*
    // add some dynamic routes for testing
    routingConfig["dynamicRoutes"].push({ 
      destination:'default',
      destinationMask: '0.0.0.0',
      gateway: '192.168.1.1',
      metric: '20',
      source: 'route'
    });
    routingConfig["dynamicRoutes"].push({ 
      destination:'default',
      destinationMask: '0.0.0.0',
      gateway: '192.168.1.2',
      metric: '20',
      source: 'route'
    });
    */

    var numberOfDynamicRoutes = routingConfig["dynamicRoutes"] && routingConfig["dynamicRoutes"].length || 0;
    if (numberOfDynamicRoutes > 0) {

      $(routingContent).find('#no_dynamic_routes_info').hide();

      var self = this;
      $.each(routingConfig["dynamicRoutes"], function(index, dynamicRoute) {
        var dynamicRouteElement = dynamicRouteTemplate.clone(true);
        dynamicRouteElement.attr('id', 'dynamic_route_' + index);

        // append new box (all boxes are grouped together one after the other, so add "group" class)
        routingDynamicRoutes.append(dynamicRouteElement);
        dynamicRouteElement.wrap('<div class="config_form_box group"></div>');

        // set values
        dynamicRouteElement.find('.dynamic_route_destination_address').html(dynamicRoute['destination']);
        dynamicRouteElement.find('.dynamic_route_destination_mask').html(dynamicRoute['destinationMask']);
        dynamicRouteElement.find('.dynamic_route_gateway_address').html(dynamicRoute['gateway']);
        dynamicRouteElement.find('.dynamic_route_gateway_metric').html(dynamicRoute['metric']);
        dynamicRouteElement.find('.dynamic_route_source').html(dynamicRoute['source']);
      });

      // last dynamic route box must have correct distance to next element - remove "group" class from it 
      routingDynamicRoutes.find('.config_form_box').last().removeClass('group');

    } else {
      routingDynamicRoutes.find('#no_dynamic_routes_info').show();
    }


    // masquerading box
    var routingMasquerading   = $('#' + routingContent.id + '_content #masquerading_area');
    var addMasquerading        = $('#add_masquerading').parent();
    var oldMasquerading       = $('[id^="masquerading_entry_"]').parent();
    oldMasquerading.remove();
    var masqueradingTemplate   = $('#template_masquerading');
    
    var self = this;
    $.each(routingConfig["ipMasquerading"], function(index, masquerading) {
      var masqueradingElement = masqueradingTemplate.clone(true);
      masqueradingElement.attr('id', 'masquerading_entry_' + index);
      
      var deleteButton = masqueradingElement.find('#template_delete_masquerading');
      deleteButton.attr('id', 'delete_masquerading_' + index);

      var submitButton = masqueradingElement.find('#template_change_masquerading');
      submitButton.attr('id', 'change_masquerading_' + index);

      // append new box
      addMasquerading.before(masqueradingElement);
      masqueradingElement.wrap('<div class="config_form_box group"></div>');

      // set values
      masqueradingElement.find('.masquerading_interface').html(masquerading['interface']);
      if (masquerading['state'] == 'enabled') {
        masqueradingElement.find('[name="masquerading_enabled"]').prop('checked', true);
      } else {
        masqueradingElement.find('[name="masquerading_enabled"]').prop('checked', false);
      }


      // bind to submit and delete button events
      submitButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessChangeMasquerading(index, masqueradingElement);
      });
      deleteButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessDeleteMasquerading(index);
      });

    });




    // port forwarding box
    var routingForwarding   = $('#' + routingContent.id + '_content #forwarding_area');
    var addForwarding       = $('#add_forwarding').parent();
    var oldForwarding       = $('[id^="forwarding_entry_"]').parent();
    oldForwarding.remove();
    var forwardingTemplate   = $('#template_forwarding');
    
    var self = this;
    $.each(routingConfig["portForwarding"], function(index, forwarding) {
      var forwardingElement = forwardingTemplate.clone(true);
      forwardingElement.attr('id', 'forwarding_entry_' + index);
      
      var deleteButton = forwardingElement.find('#template_delete_forwarding');
      deleteButton.attr('id', 'delete_forwarding_' + index);

      var submitButton = forwardingElement.find('#template_change_forwarding');
      submitButton.attr('id', 'change_forwarding_' + index);

      // append new box
      addForwarding.before(forwardingElement);
      forwardingElement.wrap('<div class="config_form_box group"></div>');

      // set values
      if (forwarding['state'] == 'enabled') {
        forwardingElement.find('[name="forwarding_enabled"]').prop('checked', true);
      } else {
        forwardingElement.find('[name="forwarding_enabled"]').prop('checked', false);
      }
      forwardingElement.find('.forwarding_interface').html(forwarding['interface']);
      forwardingElement.find('.forwarding_port').html(forwarding['port']);
      var protocol = forwarding['protocol'] == 'tcpudp' ? 
        'TCP/UDP' : 
        forwarding['protocol'] == 'tcp' ? 
        'TCP' :
        'UDP' ;

      forwardingElement.find('.forwarding_protocol').html(protocol);
      forwardingElement.find('.forwarding_destination_address').html(forwarding['forwardIp']);
      forwardingElement.find('.forwarding_destination_port').html(forwarding['forwardPort']);
      
      // bind to submit and delete button events
      submitButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessChangeForwarding(index, forwardingElement);
      });
      deleteButton.bind('click', function(event)
      {
        event.preventDefault();
        routingContent.ProcessDeleteForwarding(index);
      });

    });


  }
};

RoutingContent.prototype.ProcessConfigChangeSubmit = function(formObj)
{
  var routingContent = this;

  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    state: $(formObj).find('[name=routing_state]').is(':checked') ? 'enabled' : 'disabled'
  };

  deviceParams.ChangeValue('routing_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();
    }
  });

};

RoutingContent.prototype.ProcessAddStaticRoute = function(formObj) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    state:           $(formObj).find('[name=new_state]').is(':checked') ? 'enabled' : 'disabled',
    destination:     $(formObj).find('[name=new_destination_address]').val(),
    destinationMask: $(formObj).find('[name=new_destination_mask]').val(),
    gateway:         $(formObj).find('[name=new_gateway_address]').val(),
    gatewayMetric:   $(formObj).find('[name=new_gateway_metric]').val()
  };

  deviceParams.ChangeValue('routing_add_static_route', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    } 
    else 
    {
      routingContent.Refresh();

      // set form members to default, e.g. remove input strings
      $(formObj).find('[name=new_state]').prop('checked', false);
      $(formObj).find('[name=new_destination_address]').val('');
      $(formObj).find('[name=new_destination_mask]').val('');
      $(formObj).find('[name=new_gateway_address]').val('');
      $(formObj).find('[name=new_gateway_metric]').val('');
    }
  });
}

RoutingContent.prototype.ProcessChangeStaticRoute = function(index, formObj) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index:           '' + index,
    state:           $(formObj).find('[name=static_route_state]').is(':checked') ? 'enabled' : 'disabled',
    destination:     $(formObj).find('[name=static_route_destination_address]').val(),
    destinationMask: $(formObj).find('[name=static_route_destination_mask]').val(),
    gateway:         $(formObj).find('[name=static_route_gateway_address]').val(),
    gatewayMetric:   $(formObj).find('[name=static_route_gateway_metric]').val()
  };

  deviceParams.ChangeValue('routing_change_static_route', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    } 
    else 
    {
      routingContent.Refresh();
    }
  });
}


RoutingContent.prototype.ProcessDeleteStaticRoute = function(index) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index:     '' + index
  };

  deviceParams.ChangeValue('routing_delete_static_route', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    } 
    else 
    {
      routingContent.Refresh();
    }
  });
}


RoutingContent.prototype.ProcessAddMasquerading = function(formObj) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');

  var newInterface = $(formObj).find('[name=new_masquerading_interface]:checked').val();
  
  if(newInterface == 'other') {
    newInterface = $(formObj).find('[name=new_masquerading_interface_other]').val();
  }

  if(!newInterface) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'No interface selected.' 
    ]);    
    return;
  }

  var newValueList = { 
    'interface': newInterface,
    'state':     $(formObj).find('[name=new_masquerading_enabled]').is(':checked') ? 'enabled' : 'disabled'
  };

  deviceParams.ChangeValue('routing_add_masquerading', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();

      // set form members to default, e.g. remove input strings
      $(formObj).find('[name=new_masquerading_enabled]').prop('checked', false);
      $(formObj).find('[name=new_masquerading_interface]').prop('checked', false);
      $(formObj).find('[name=new_masquerading_interface]#routing_add_masquerading_interface_X1').prop('checked', true);
      $(formObj).find('[name=new_masquerading_interface_other]').val('');
    }
  });
}





RoutingContent.prototype.ProcessDeleteMasquerading = function(index) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index:     '' + index
  };

  deviceParams.ChangeValue('routing_delete_masquerading', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();
    }
  });
}


RoutingContent.prototype.ProcessChangeMasquerading = function(index, formObj) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index: '' + index,
    state: $(formObj).find('[name=masquerading_enabled]').is(':checked') ? 'enabled' : 'disabled'
  };

  deviceParams.ChangeValue('routing_change_masquerading', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();
    }
  });
}



RoutingContent.prototype.ProcessAddForwarding = function(formObj) {
  var routingContent = this;



  var protocol = '';
  if ($(formObj).find('[name=new_forwarding_protocol_tcp]').is(':checked') ) {
    protocol += 'tcp';
  }
  if ($(formObj).find('[name=new_forwarding_protocol_udp]').is(':checked') ) {
    protocol += 'udp';
  }
  
  if (!protocol) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'At least one protocol has to be selected.' 
    ]);    
    return;
  }

  var interface = $(formObj).find('[name=new_forwarding_interface]:checked').val();
  
  if (interface == 'other') {
    interface = $(formObj).find('[name=new_forwarding_interface_other]').val();
  }

  if (!interface) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'No interface selected.' 
    ]);    
    return;
  }

  var newValueList = { 
    state: $(formObj).find('[name=new_forwarding_enabled]').is(':checked') ? 'enabled' : 'disabled',
    interface: interface,
    port: $(formObj).find('[name=new_forwarding_port]').val(),
    protocol: protocol,
    forwardIp: $(formObj).find('[name=new_forwarding_destination_address]').val(),
    forwardPort: $(formObj).find('[name=new_forwarding_destination_port]').val(),
  };

  if (!newValueList.port) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'Please provide a port value.' 
    ]);    
    return;
  }
  if (!newValueList.forwardIp) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'Please provide a destination address.' 
    ]);    
    return;
  }
  if (!newValueList.forwardPort) {
    $('body').trigger('event_errorOccured', [ 
      'Error while changing Routing configuration.', 
      'Please provide a destination port.' 
    ]);    
    return;
  }

  pageElements.ShowBusyScreen('Changing Routing configuration...');  

  deviceParams.ChangeValue('routing_add_forwarding', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();

      // set form members to default, e.g. remove input strings
      $(formObj).find('[name=new_forwarding_enabled]').removeAttr('checked');
      $(formObj).find('[name=new_forwarding_interface]').prop('checked', false);
      $(formObj).find('[name=new_forwarding_interface]#routing_add_forwarding_interface_X1').prop('checked', true);
      $(formObj).find('[name=new_forwarding_interface_other]').val('');
      $(formObj).find('[name=new_forwarding_port]').val('');
      $(formObj).find('[name=new_forwarding_protocol_tcp]').removeAttr('checked');
      $(formObj).find('[name=new_forwarding_protocol_udp]').removeAttr('checked');
      $(formObj).find('[name=new_forwarding_destination_address]').val('');
      $(formObj).find('[name=new_forwarding_destination_port]').val('');
    }
    
  });
}





RoutingContent.prototype.ProcessDeleteForwarding = function(index) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index:     '' + index
  };

  deviceParams.ChangeValue('routing_delete_forwarding', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();
    }
  });
}


RoutingContent.prototype.ProcessChangeForwarding = function(index, formObj) {
  var routingContent = this;
  
  pageElements.ShowBusyScreen('Changing Routing configuration...');


  var newValueList = { 
    index: '' + index,
    state: $(formObj).find('[name=forwarding_enabled]').is(':checked') ? 'enabled' : 'disabled'
  };

  deviceParams.ChangeValue('routing_change_forwarding', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing Routing configuration.', errorText ]);
    }
    else 
    {
      routingContent.Refresh();
    }
  });
}