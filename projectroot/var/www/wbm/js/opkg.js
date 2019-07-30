
function OpkgContent(id)
{
  var opkgContent = this;
  opkgContent.id = id || 'opkg';

  var ShowOpkgList = function(status, opkgList, outputElementId)
  {
    var outputOpkgList = opkgList;

    if(SUCCESS !== status)
    {
      outputOpkgList = "No opkg information available";
    }
    else if (!opkgList.length || !opkgList)
    {
      outputOpkgList = "No packages available";
    }

    outputOpkgList = outputOpkgList.replace(/ /g, "&nbsp;");
    outputOpkgList = outputOpkgList.replace(/</g, "&lt;");
    outputOpkgList = outputOpkgList.replace(/>/g, "&gt;");
    outputOpkgList = outputOpkgList.replace(/"/g, "&quot;");
    outputOpkgList = outputOpkgList.replace(/\r\n/g, "</td></tr><tr><td>");
    outputOpkgList = outputOpkgList.replace(/\n/g, "</td></tr><tr><td>");

    $('table#' + outputElementId).html('<tr><td>'+ outputOpkgList + '</td></tr>');
  };

  var ShowPkgInfo = function(status, opkgList, outputElementId)
  {
    var outputOpkgList = opkgList;

    if(SUCCESS !== status)
    {
      outputOpkgList = "No opkg information available";
    }
    else if (!opkgList.length || !opkgList)
    {
      outputOpkgList = "No packages available";
    }


    outputOpkgList = outputOpkgList.replace(/&nbsp;/g, " ");
    outputOpkgList = outputOpkgList.replace(/&lt;/g, "<");
    outputOpkgList = outputOpkgList.replace(/&gt;/g, ">");
    outputOpkgList = outputOpkgList.replace(/&quot;/g,'"');
    outputOpkgList = outputOpkgList.replace(/&amp;/g, "&");


    opkgContent.ProcessRefreshSubmit(this);
    window.alert(outputOpkgList);
    
  };

  var CreateOpkgParamView = (function()
  {
    opkgContent.list_paramView = new DeviceParamView();
    opkgContent.pkg_paramView = new DeviceParamView();

    opkgContent.list_paramView.Add(
    {
      paramId         : 'opkglist',
      outputElementId : 'opkg_list_table',
      outputFkt       : ShowOpkgList
    });
    opkgContent.pkg_paramView.Add(
    {
      paramId         : 'opkgfkt',
      outputElementId : 'refresh_opkg_list',
      outputFkt       : ShowPkgInfo
    });

  })();

  var AddEvents = (function()
  {
    // fist creation of opkg content
    $('#content_area').bind(opkgContent.id + '_create', function(event)
    {
      opkgContent.Create();
    });

    // refresh of opkg content area
    $('#content_area').bind(opkgContent.id + '_refresh', function(event)
    { //Log("Event - opkg refresh");

      $('#' + opkgContent.id + '_content #refresh_opkg_list').trigger('submit');
    });

  })();
}


OpkgContent.prototype.Create = function()
{
  var opkgContent = this;

  var defaultOpkgSelectVal = "0";

  $('#'+opkgContent.id + '_content select#opkg_sel').val(defaultOpkgSelectVal);
  $('#'+opkgContent.id + '_content select#opkg_sel').on('change',function(event)
  {
    opkgContent.ProcessRefreshSubmit(this);
  });
  $('#'+opkgContent.id + '_content input[name=refresh]').bind('click',function(event)
  {
    event.preventDefault();
    opkgContent.ProcessRefreshSubmit(this);
  });
  $('#'+opkgContent.id + '_content input[name=pkgcontrol]').bind('click',function(event)
  {
    event.preventDefault();
    opkgContent.ProcessInfoSubmit(this);
  });
 
  $('#'+opkgContent.id + '_content #opkg_list_table').on('click','tr',function()
  {
    $(this).addClass('selected').siblings().removeClass('selected');       
  });

  // events for form processing
  $('#' + opkgContent.id + '_content #refresh_opkg_list').bind('submit', function(event)
  {
    $('#' + opkgContent.id + '_content #refresh_opkg_list').trigger('submit');
  });

  opkgContent.ProcessRefreshSubmit(this);
}


OpkgContent.prototype.RefreshOpkgList = function(opkSelPar)
{
  var opkgContent = this;

  pageElements.ShowBusyScreen("Reading opkg information...");
  deviceParams.ReadValue({id:'opkglist', variables : {opkg_list_val:opkSelPar}}, function()
  {
    
    pageElements.RemoveBusyScreen();
    if(SUCCESS != deviceParams.list['opkglist'].status)
    {
      $('body').trigger('event_errorOccured',  'Error while changing opkg information.');
    }

    opkgContent.list_paramView.ShowValues();
  });
}


OpkgContent.prototype.ProcessRefreshSubmit = function(formObj)
{
  var opkgContent = this;

  var opkgSelVal = $('#opkg_sel').val();
  var opkgSelPar = "-all";

  if(opkgSelVal == "0"){
    $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val("Info");
    opkgSelPar = "-all";}
  else if(opkgSelVal == "1"){
    $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val("Remove");
    opkgSelPar = "-installed";}
  else if(opkgSelVal == "2"){
     $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val("Remove");
    opkgSelPar = "-userinstalled";}
  else if(opkgSelVal == "3"){
    $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val("Upgrade");	
    opkgSelPar = "-upgradable";}
  else if(opkgSelVal == "4"){
    $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val("Install");	
    opkgSelPar = "-notinstalled";}

  opkgContent.RefreshOpkgList(opkgSelPar);

}


OpkgContent.prototype.ProcessInfoSubmit = function(formObj)
{
  var opkgContent = this;

  var sel_fkt= $('#'+opkgContent.id + '_content input[name=pkgcontrol]').val();
  
  var sel_pkg_html = $('#'+opkgContent.id + '_content #opkg_list_table tr.selected').html();
  var sel_pkg;

  if(!sel_pkg_html)
  {
    window.alert("Please select Package first");
    return 0;
  }
  else if(sel_pkg_html == "<td>No&nbsp;packages&nbsp;available</td>")
  {
    window.alert("No Packages available");
    return 0;
  }
  sel_pkg_html = sel_pkg_html.replace('<td>','');
  sel_pkg_html = sel_pkg_html.replace('</td>','');

  sel_pkg = sel_pkg_html.split("&nbsp;")[0];

  if(sel_fkt == "Install")
  {
    sel_pkg=sel_pkg+"="+sel_pkg_html.split("&nbsp;")[2];
    if(!window.confirm("Are you sure to install " + sel_pkg))
    {
      return 0;
    }
    pageElements.ShowBusyScreen("Installing " + sel_pkg);
  }
  else if(sel_fkt == "Remove")
  {
    if(!window.confirm("Are you sure to remove " + sel_pkg))
    {
      return 0;
    }
    pageElements.ShowBusyScreen("Removing " + sel_pkg);
  }
  else if(sel_fkt == "Upgrade")
  {
    if(!window.confirm("Are you sure to upgrade " + sel_pkg))
    {
      return 0;
    }
    pageElements.ShowBusyScreen("Upgrading " + sel_pkg);
  }
  else if(sel_fkt == "Info")
  {
    pageElements.ShowBusyScreen("Getting " + sel_pkg + " information");
  }
  else
  {
    if(!window.confirm("Sure to "+ sel_fkt.toLowerCase() + " " + sel_pkg))
    {
      return 0;
    }
    pageElements.ShowBusyScreen("Reading " + sel_pkg + " package information...");
  }

  sel_fkt= "-"+sel_fkt.toLowerCase();

  deviceParams.ReadValue({id:'opkgfkt', variables : {opkg_fkt_val:sel_fkt,opkg_pkg_val:sel_pkg}}, function()
  {
    pageElements.RemoveBusyScreen();
    if(SUCCESS != deviceParams.list['opkgfkt'].status)
    {
      $('body').trigger('event_errorOccured',  'Error while changing pkg information.');
    }
    opkgContent.pkg_paramView.ShowValues();
  });
  
}

