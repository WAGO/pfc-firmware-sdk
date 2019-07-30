/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/**
 * building up page - navigation and content - by instructions from xml file "frontend_config_wbm.xml"
 * @param {string} id of actually displayed page (to find it e.g. in navigation list)
 * @param {function} callback
 */
function PageBuildup(actualPageId, callback)
{
  var pageBuildup = this;

  if(!xmlFilename) var xmlFilename = "frontend_config_wbm.xml";

  pageBuildup.actualPageId      = actualPageId || '';
  pageBuildup.mainNav           = $('#main_nav');
  pageBuildup.status            = ERROR;
  pageBuildup.xmlWbmArray       = null;
  pageBuildup.currentUserlevel  = 0;
  pageBuildup.referrer          = '';
  pageBuildup.contentRequestPending = false;

  // ADD ANY FEATURE DEPENDENT MENU ENTRIES HERE:
  pageBuildup.featureDependentEntries = [
    {
      title: 'OPC UA', //< GUI text
      id: 'opcua', //< unique menu entry identifier
      userLevel: 3, // 3: admin, 2: user, 0: guest
      dependsOnFeature: 'opcua_feature', //< needs to be configured in device_params_list.js
      inMenu: 'port'      //< optional -> will be appended inside
      //afterEntry: 'port'  //< optional -> will be appended after
    },
    {
        title: 'Cloud Connectivity', //< GUI text
        id: 'cloud_connectivity', //< unique menu entry identifier
        userLevel: 3, // 3: admin
        dependsOnFeature: 'dataagent_feature', //< needs to be configured in device_params_list.js
        //inMenu: 'port'      //< optional -> will be appended inside
        afterEntry: 'port'  //< optional -> will be appended after
    },
    {
	title: 'OPKG', //< GUI text
	id: 'opkg', //< unique menu entry identifier
	userLevel: 3, // 3: admin
	dependsOnFeature: 'opkg_feature', //< needs to be configured in device_params_list.js
	afterEntry:  'software_uploads'  //< optional -> will be appended after
    }//,...
  ];


  // navigation already existing -> nothing to do
  if(0 < $("#main_nav ul li").length)
  {
    pageBuildup.status     = SUCCESS;
    pageBuildup.navCreated = true;
  }

  else
  {
    deviceCommunication.GetRessource(
    {
      ressource:  xmlFilename,
      callback:   function(status, navStructFile, textStatus, navXmlXhr)
      {
        var navStruct = false;

        try { navStruct = $.xml2json(navXmlXhr.responseText); }
        catch(e)
        {
          pageBuildup.status = ERROR;
        }

        if(navStruct)
        {
          if(pageBuildup.CreateNavigationViaJson(navStruct) !== false)
          {
            pageBuildup.status = SUCCESS;
            pageBuildup.ProcessUserlevelAdjustment();
          }
        }

        if(pageBuildup.status != SUCCESS)
        {
          $('body').trigger('event_errorOccured', [ 'Can\'t read navigation structure', textStatus ]);
        }

        callback && callback();
      }
    });
  }
}

PageBuildup.prototype.AppendEntryToMenu = function (title, userLevel, id, inMenuId) {
  // get UL elemenet by menu id
  // this is either the root menu element or a nested one.
  var existingMenu = inMenuId ?
    $('#' + inMenuId + ' + ul.nav_submenu') : $('#main_nav > ul');

  // there may be not found any matching UL element for a given menuId.
  // this is an error case, as any containing menu is expected to exist.
  if (!existingMenu || existingMenu.length == 0) {
    console.error('Failed to add a feature dependent menu entry, because the containing menu entry was not found. (' + entryId + ')');
    return;
  }

  var navElement = document.createElement('nav');
  $(navElement).attr('text', title);
  $(navElement).attr('ressource', 'index.php');
  $(navElement).attr('user_level', userLevel);
  $(navElement).attr('fragment', id);

  var navLink = CreateNavLink($(navElement), id);
  existingMenu.append(navLink);
}

PageBuildup.prototype.AppendEntryAfterEntry = function (title, userLevel, id, afterEntryId) {

  // either get the entry to append the new one after...
  var existingEntry;
  if (afterEntryId) {
    var existingEntry = $('#' + afterEntryId);

    // did not find an existing entry to append to. in this case show a developer warning
    // however, instead append the new entry in the end
    if (!existingEntry || existingEntry.length == 0) {
      console.error('Failed to add a feature dependent menu entry, because the sibling menu entry was not found. (' + entryId + ')');
      return;
    }
  } else {
    throw new Error('AppendEntryAfterEntry failed. \'afterEntry\' argument was not specified or empty.');
    return;
  }

  var navElement = document.createElement('nav');
  $(navElement).attr('text', title);
  $(navElement).attr('ressource', 'index.php');
  $(navElement).attr('user_level', userLevel);
  $(navElement).attr('fragment', id);

  var navLink = CreateNavLink($(navElement), id);
  existingEntry.parent().after(navLink);
}

// insert special pages, that are not included in the XML menu configuration file
PageBuildup.prototype.UpdateFeatureDependantEntries = function (callback) {
  var pageBuildup = this;

  var entries = pageBuildup.featureDependentEntries;

  var featureParamView = new DeviceParamView();

  for (var i = 0; i < entries.length; i++) {
    featureParamView.Add({ paramId: entries[i].dependsOnFeature });
  }

  deviceParams.ReadValueGroup(featureParamView.list, function()
  {
    if (deviceParams.ReadErrorOccured(featureParamView.list))
    {
      var errorString = deviceParams.CollectedErrorStrings(featureParamView.list);
      console.warn('Feature Detection not available: ', errorString);
    }
    else
    {
      for (var i = 0; i < entries.length; i++) {
        var entry = entries[i];
        if (deviceParams.list[entry.dependsOnFeature].value === 'true') {

          if (entry.afterEntry){
            pageBuildup.AppendEntryAfterEntry(entry.title, entry.userLevel, entry.id, entry.afterEntry);
          } else {
            pageBuildup.AppendEntryToMenu(entry.title, entry.userLevel, entry.id, entry.inMenu);
          }
        }
      }
    }

    callback && callback();
  });
}

PageBuildup.prototype.AddNavigationListEvents = function()
{
  /*
   * event: mouseenter on menu line
   */
  $("#main_nav a").mouseenter(function(event)
  {
    //Log("mouseenter");
    var linkObj = this;

    // if link is not placed in an "flyed out" submenu (then it must still stay visible)
    // - hide the (possible) "flyout" menu
    if(!$(linkObj).parents('ul').hasClass('nav_flyout_submenu'))
    {
      $('.nav_flyout_submenu').hide();
    }

    // check if link (and therefore its possibly belonging submenu) is not already shown as selected
    if(!$(linkObj).hasClass('selected'))
    {
      // if a hidden submenu is existing - configure and show it
      subMenu = $(linkObj).siblings('ul .nav_submenu:hidden');
      if(subMenu.length)
      {
        // define position of flyout submenu
        subMenu.css(
        {
          //'width'       : (linkObj.offsetWidth - 30) + 'px',
          'left'        : (linkObj.offsetLeft + linkObj.offsetWidth - 3) + 'px',
          'top'         : ($(linkObj).offset().top - 5) + 'px'
        });

        subMenu.addClass('nav_flyout_submenu');
        //subMenu.slideDown();
        subMenu.show();
      }
    }

    // if mouse leaves navigation in the whole, close possible flyout menu anyway
    $('#main_nav').mouseleave(function(event)
    {
      $('.nav_flyout_submenu').hide();
    });
  });

  /*
   * event - click in navigation list link
   */
  $(pageBuildup.mainNav).find('a').bind('click', function(event)
  {
    //Log('main nav - click');
    event.preventDefault();
    event.stopPropagation();

    var newUrl = $(this).attr('href');
    pageBuildup.referrer = location.pathname.replace('/wbm/', '') + location.hash;
    pageBuildup.FollowNavLink(newUrl);
    return false;
  });
};

/* -------------------------------------------------------
 * Functions for changing content by clicks in navigation
 * -------------------------------------------------------
 */

/*
PageBuildup.prototype.Reload = function()
{
  var pageBuildup = this;
  var actualUrl   = location.pathname.replace('/wbm/', '') + '#' + location.hash;

  pageBuildup.FollowNavLink(actualUrl);
};
*/


PageBuildup.prototype.FollowNavLink = function(newUrl)
{
  var pageBuildup     = this;
  var newContentId    = '';
  var newPath         = newUrl;
  var actualPath      = location.pathname.replace('/wbm/', '');

  if(newUrl.match('#'))
  {
    newContentId    = newUrl.replace(/.*#/, '');
    newPath         = newUrl.replace(/#.*/, '');
  }

  // new URL is given - load complete ressource
  if(newPath != actualPath)
  {
    var contentIdString = newContentId.length ? ('#' + newContentId) : '';
    location.href = newPath + contentIdString;
  }

  // same URL - just change content
  else
  {
    var navLink = $(pageBuildup.mainNav).find('a[id=' + newContentId + ']');

    if(navLink && $(navLink).hasClass('forbidden'))
    {
      pageBuildup.referrer = newUrl;
      newContentId         = 'login';
    }

    pageBuildup.ProcessContentChangeRequest(newContentId);
  }
};


PageBuildup.prototype.LoadContent = function(newContentId)
{
  var pageBuildup = this;

  // try to load content from device
  //Log('Get content - ' + newContentId);
  if(!pageBuildup.contentRequestPending)
  {
    pageBuildup.contentRequestPending = true;

    deviceCommunication.GetRessource(
    {
      ressource: 'page_elements/' + newContentId + '.inc.php',
      callback:   function(status, contentHtml, textStatus)
      {
        pageBuildup.contentRequestPending = false;

        if(SUCCESS === status)
        {
          $('#content_area').append('<div class="single_content_page" id="' + newContentId + '_content" >' + contentHtml + '</div>');
          $('#content_area').trigger(newContentId + '_create');
          pageBuildup.ChangeContent(newContentId);
        }
        else
        {
          var errorText = '';
          if('Unauthorized' === textStatus)   errorText = "You are not authorized to access.";
          else if('Timeout' === textStatus)   errorText = "Webserver is unavailable.";
          else if('Not Found' === textStatus) errorText = "Ressource is not existing on device.";

          $('.single_content_page[id=error_content] .content_page_error').text(errorText);
          pageBuildup.ChangeContent(newContentId, ERROR);
        }
      }
    });
  }
};


PageBuildup.prototype.ChangeContent = function(newContentId, status)
{
  var pageBuildup = this;

  var status = status || SUCCESS;

  // hide old content
  var actualContentAreaId   = $('#content_area .single_content_page:visible').attr('id');
  if(actualContentAreaId)
  {
    var actualContentId       = actualContentAreaId.replace('_content', '');

    $('.single_content_page[id=' + actualContentAreaId + ']').hide();
    $('#content_area').trigger('event_' + actualContentId + '_sleep');
  }

  // do changes in navigation list and address line
  pageBuildup.ShowNavEntrySelected(newContentId);
  document.location.hash    = newContentId;
  pageBuildup.actualPageId  = newContentId;

  // show new content - or error page, if requested
  if(SUCCESS === status)
  {
    var newContentArea = $('.single_content_page[id=' + newContentId + '_content]');
    $(newContentArea).show();
    $('#content_area').trigger(newContentId + '_refresh');
  }
  else
  {
    $('.single_content_page[id=error_content]').show();
  }

  $('body').trigger('event_pageBuildup_contentChange');
};



PageBuildup.prototype.SetCurrentUserlevel = function(userlevel)
{
  var pageBuildup = this;

  pageBuildup.currentUserlevel  = parseInt(userlevel, 10) || 0;

  if(pageBuildup.navCreated)
  {
    pageBuildup.ProcessUserlevelAdjustment();
  }
};



PageBuildup.prototype.ProcessUserlevelAdjustment = function()
{
  var pageBuildup = this;
  //var contentLink = $('#main_nav a#' + newContentId);
  //var userlevel   = $(contentLink).find('[class^=userlevel]');

  $('#main_nav a').addClass('forbidden');

  var userlevel = pageBuildup.currentUserlevel;
  for(var userlevel = pageBuildup.currentUserlevel; userlevel >= 0; --userlevel)
  {
    var fittingLinks = $('#main_nav a.userlevel_' + userlevel);
    $(fittingLinks).removeClass('forbidden');
  }

};


PageBuildup.prototype.ProcessContentChangeRequest = function(newContentId)
{
  var pageBuildup = this;

  // search new content are in page
  var newContentArea = $('.single_content_page[id=' + newContentId + '_content]');
  if(!newContentArea.length)
  {
    pageBuildup.LoadContent(newContentId);
  }
  else
  {
    pageBuildup.ChangeContent(newContentId);
  }
};



//--------------------------------------------------------------------------------------
//  create a new navigation list element for webpage from specification of xml file
//
//  input:  navNode: node in xml config file with the requested information for link
//          idString: additionally added id string for link (e.g. for access to its css style)
//--------------------------------------------------------------------------------------
function CreateNavLink(navNode, idString)
{
  if(!navNode.attr('text') || !navNode.attr('ressource') || !idString)
  {
    return false;
  }

  var name      = navNode.attr('text');
  var url       = navNode.attr('ressource');
  var params    = navNode.attr('params');
  var fragment  = navNode.attr('fragment');
  var userlevel = navNode.attr('user_level')  || '3';

  var url = navNode.attr('ressource');
  if(params)    url = url + "?" + params;
  if(fragment)  url = url + "#" + fragment;


  var html = '';
  html += '<li><a id="' + idString + '" class="userlevel_' + userlevel + '" href="' + url + '" >' + name + '</a></li>';

  return $(html);
};


/**
 * Create a new navigation list element for WBM via specification of json file (extracted of xml frontend config file)
 * 
 * @param object navStruct whole navigation entry structure for this navigation link
 * @param string id
 * @return string html code for menu entry 
 *         boolean false in case of failure
 */
function CreateNavigationEntryViaJson(navStruct, id)
{
  var html = '';
  var url = '';
  
  if(!navStruct || !id || !navStruct.text || !navStruct.text[0] || !navStruct.user_level)
  {
    console.log('navigation structure format error');
    return false;
  }

  // no specified ressource give - take the current one
  url = navStruct.ressource || window.location.pathname;

  if(navStruct.params) url = url + '?' + navStruct.params;
  if(navStruct.fragment) url = url + '#' + navStruct.fragment;

  html += '<li>';
  html +=   '<a id="' + id + '" class="userlevel_' + navStruct.user_level + '" href="' + url + '" >' + navStruct.text[0] + '</a>';
  html += '</li>';

  return html;
}


//--------------------------------------------------------------------------------------
//  initially creation of navigation list by specifications of xml file (configured in constructor)
//--------------------------------------------------------------------------------------
PageBuildup.prototype.CreateNavigation = function(additionalFeatures)
{
  var pageBuildup = this;

  if(SUCCESS === pageBuildup.status)
  {
    $(pageBuildup.xmlWbmArray).find("> level").each(function()
    {
      var levelNode = this;

      listElement = CreateNavLink($(levelNode).find("nav"), $(levelNode).attr("id"));
      $("#main_nav > ul").append(listElement);

      // if necessary, create submenu - and hide it
      if($(levelNode).find("> level").length > 0)
      {
        $(listElement).find("> a").prepend('<img class="submenu_hint" src="../wbm/images/arrow_right_green.gif">');
        var subNavigation = document.createElement("ul");
        $(subNavigation).hide();
        $(subNavigation).addClass("nav_submenu");
        $("#main_nav > ul li:last").append(subNavigation);
        $(levelNode).find("> level").each(function()
        {
          subLevelNode = this;
          subListElement = CreateNavLink($(subLevelNode).find("nav"), $(subLevelNode).attr("id"));
          $(subNavigation).append(subListElement);
        });
      }
    });

    pageBuildup.navCreated = true;
    pageBuildup.ProcessUserlevelAdjustment();
  }

};

/**
 * Create WBM navigation object.
 * 
 * @param navStruct navigation structure in json
 * @return boolean true in case of success, false in case failure (format errors)
 */
PageBuildup.prototype.CreateNavigationViaJson = function(navStruct)
{
  var self = this;
  var listElement;
  var navEntry;
  var subNavigation;
  var subNavEntryHtml;

  if(!navStruct.frontend || !navStruct.frontend.id === 'wbm')
  {
    return false;
  }

  $.each(navStruct.frontend.level, function(mainMenuItemIndex, mainMenuItem)
  {
    navEntryHtml = CreateNavigationEntryViaJson(mainMenuItem.nav, mainMenuItem.id);
    $('#main_nav > ul').append(navEntryHtml);

    // if necessary, create submenu - and hide it
    if(mainMenuItem.level)
    {
      navEntry = $('#main_nav > ul > li:last-child');

      // add image (arrow) to main menu entry to show that sub menu is existing
      navEntry.find('> a').prepend('<img class="submenu_hint" src="../wbm/images/arrow_right_green.gif">');

      navEntry.append('<ul class="nav_submenu">');
      subNavigation = navEntry.find('ul');
      subNavigation.hide();

      $.each(mainMenuItem.level, function(subMenuItemIndex, subMenuItem)
      {
        subNavEntryHtml = CreateNavigationEntryViaJson(subMenuItem.nav, subMenuItem.id);
        $(subNavigation).append(subNavEntryHtml);
      });
    }
  });

  self.navCreated = true;
  return true;
}


//--------------------------------------------------------------------------------------
//  show specified entry in navigation list selected
//--------------------------------------------------------------------------------------
PageBuildup.prototype.ShowNavEntrySelected = function(selectedEntryId)
{
  var pageBuildup = this;

  //if(!selectedEntryId) selectedEntryId = pageBuildup.actualPageId;

  if((SUCCESS == pageBuildup.status) && pageBuildup.navCreated)
  {
    // remove existing select link classes hide possibly visible sub menus and show submenu hint image again
    $("#main_nav > ul a").removeClass("selected");
    $("#main_nav ul.nav_submenu").hide();
    $("#main_nav img.submenu_hint").show();

    if(selectedEntryId)
    {
      // add select class to actual menu entry
      var selectedNavLink = $("#main_nav a#" + selectedEntryId);
      $(selectedNavLink).addClass("selected");

      // if entry belongs to or has submenu - make it visible and hide image for advise to submenu
      /*
      $(selectedNavLink).parent().parent(".nav_submenu").slideDown('slow');
      $(selectedNavLink).siblings(".nav_submenu").slideDown('slow');
       */
      $(selectedNavLink).parent().parent(".nav_submenu").removeClass("nav_flyout_submenu");
      $(selectedNavLink).parent().parent(".nav_submenu").show();
      $(selectedNavLink).siblings(".nav_submenu").show();
      $(selectedNavLink).parent().parent(".nav_submenu").siblings().find("img.submenu_hint").hide();
    }
  }
};


//--------------------------------------------------------------------------------------
//  Global elements of webpage used in connection with javaScript and AJAX functions:
//  display infobox, hide screen while working process
//--------------------------------------------------------------------------------------
function PageElements()
{
  var pageElements = this;

  $("body").prepend('<div id="busy_state_infobox"></div>');
  $("body").prepend('<div id="infobox"></div>');
  $("body").prepend('<div id="page_overlay"></div>');

  pageElements.busyStateInfobox = $('#busy_state_infobox')
  pageElements.pageOverlay      = $('#page_overlay');
  pageElements.wbmState         = $('#value_wbm_state');

  pageElements.wbmCommunicationActive = false;
  pageElements.wbmConnectionLost      = false;

  var SetEventListner = (function()
  {
    $('body').on('event_deviceCommunicationStart', function(event)
    {
      pageElements.wbmCommunicationActive = true;
      pageElements.ShowPendingRequest();
    });

    $('body').on('event_deviceCommunicationResting', function(event)
    {
      pageElements.wbmCommunicationActive = false;
      pageElements.ShowNoPendingRequest();
    });

    $('body').on('event_deviceConnectionOk', function(event)
    {
      pageElements.wbmConnectionLost  = false;
      pageElements.ShowConnectionOk();
    });

    $('body').on('event_deviceConnectionLost', function(event)
    {
      pageElements.wbmConnectionLost  = true;
      pageElements.ShowConnectionLost();
    });

    $('body').on('event_elementContentChange', function(event, outputDivId, content)
    {
      pageElements.WriteElementContent(outputDivId, content);
    });

    $('body').on('event_errorOccured', function(event, text, additionalText)
    {
      pageElements.ShowConfirmScreen(text, additionalText);
    });

  })();

}


PageElements.prototype.ShowBusyScreen = function(text)
{
  if($("#page_overlay").length && $("#busy_state_infobox").length)
  {
    // disable file input fields (page overlay has no influence)
    $('input[type=file]').prop('disabled', true);
    $('button.file_browse_fake_button').addClass('fake_fileupload_grayed');
    //$('button.file_browse_fake_button').css('background', 'red');
    
    // gray out whole page, actually invisible parts too
    $("#page_overlay").height(document.body.scrollHeight + 400);
    $("#page_overlay").width(document.body.scrollWidth);
    $("#page_overlay").css('display', 'block');
    $("#page_overlay").show();

    if(!text) text = "Please wait...";
    $("#busy_state_infobox").html("<p>" + text + "</p>");
    $("#busy_state_infobox").css('display', 'block');
    $("#busy_state_infobox").show();

    // load background image new - with random number in url - to to make IE to show the animation in gif
    // (otherwise IE takes it from cache and does not show the animation from second display of box)
    $("#busy_state_infobox").css('background', 'white url(images/wait_1.gif?' + Math.random() + ') no-repeat center 80px block');
  }
};

PageElements.prototype.ChangeBusyScreenText = function(text)
{
  if($("#busy_state_infobox:visible").length)
  {
    $("#busy_state_infobox").html("<p>" + text + "</p>");
  }
}


PageElements.prototype.RemoveBusyScreen = function()
{
  if($("#page_overlay").length && $("#busy_state_infobox").length)
  {
    /*
    $("#busy_state_infobox").fadeOut(200);
    $("#page_overlay").fadeOut(200);
    */
    $("#busy_state_infobox").hide();
    $("#page_overlay").hide();

    // activate file input fields again
    $('input[type=file]').prop('disabled', false);
    $('button.file_browse_fake_button').removeClass('fake_fileupload_grayed');
  };

  if($('#infobox').length)
  {
    $('#infobox').hide();
  };

};



PageElements.prototype.ShowConfirmScreen = function(text, additionalText, callback)
{
  var pageOverlay = $('#page_overlay');
  var infoBox     = $('#infobox');

  if(pageOverlay.length && infoBox.length)
  {
    // gray out whole page, actually invisible parts too
    $(pageOverlay).height(document.body.scrollHeight + 400);
    $(pageOverlay).width(document.body.scrollWidth);
    $(pageOverlay).css('display', 'block');
    $(pageOverlay).show();

    if(!text) text = "...";
    $(infoBox).html("<p>" + text.replace('\n', '<br>') + "</p>");

    if(additionalText && additionalText.length)
    {
      //$('#info_box').append('<p class="additional_infos">...weitere Infos</p>');
      $(infoBox).append('<p class="additional_infos_hint">...more information</p>');
      $(infoBox).append('<p class="additional_infos">' + additionalText.replace('\n', '<br>') + '</p>');
    }

    $(infoBox).append('<div class="button_area">');
    $(infoBox).find('.button_area').append('<input type="button" name="ok" value="OK">');

    $(infoBox).removeClass();
    $(infoBox).css('wait_box');
    $(infoBox).css('display', 'block');
    $(infoBox).show();

    $(infoBox).find('input[name=ok]').click(function(event)
    {
      pageElements.RemoveBusyScreen();
      //$('body').off('keydown', ProcessKeyPress);   // modern version
      $('body').unbind('keydown');

      if(callback) callback();
    });

    // Reaktion auf Tastendruck - RETURN, ESC
    ProcessKeyPress = function(event)
    {
      event.preventDefault();
      event.stopPropagation();

      if(13 == event.keyCode) $(infoBox).find('input[name=ok]').trigger('click');
      else if(27 == event.keyCode) $(infoBox).find('input[name=ok]').trigger('click');
    }

    //$('body').on('keydown', ProcessKeyPress); // modern version
    $('body').bind('keydown', ProcessKeyPress);

    $(infoBox).find('p.additional_infos_hint').on('click', function(event)
    {
      $(infoBox).find('p.additional_infos').toggle();
    });

  }
};




PageElements.prototype.ShowPendingRequest = function()
{
  //$(this.wbmState).html('<img class="wbm_state" src="images/wait_2.gif" alt="active" title="active" width="16" >');

  if(pageElements.wbmConnectionLost)
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_red.gif" alt="connection lost" title="connection lost" width="16" >');
  }
  else
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_2.gif" alt="active" title="active" width="16" >');
  }
};


PageElements.prototype.ShowNoPendingRequest = function()
{
  //$(this.wbmState).html('<img class="wbm_state" src="images/wait_frame_green.gif" alt="not active" title="not active" width="16" >');
  if(pageElements.wbmConnectionLost)
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_frame_red.gif" alt="connection lost" title="connection lost" width="16" >');
  }
  else
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_frame_green.gif" alt="not active" title="not active" width="16" >');
  }
};


PageElements.prototype.ShowConnectionOk = function()
{
  if(pageElements.wbmCommunicationActive)
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_2.gif" alt="active" title="active" width="16" >');
  }
  else
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_frame_green.gif" alt="not active" title="not active" width="16" >');
  }
};

PageElements.prototype.ShowConnectionLost = function()
{
  if(pageElements.wbmCommunicationActive)
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_red.gif" alt="connection lost" title="connection lost" width="16" >');
  }
  else
  {
    $(this.wbmState).html('<img class="wbm_state" src="images/wait_frame_red.gif" alt="not active" title="Connection lost" width="16" >');
  }
};



PageElements.prototype.WriteElementContent = function(outputElementId, content)
{
  // evaluate how result must be written into dom - depends on kind of tag
  var outputElement = $('#' + outputElementId);

  if(outputElement && outputElement.length)
  {
    var tagName = $(outputElement).prop('tagName');
    var type    = $(outputElement).prop('type');

    //output element is checkbox - set box "checked", if response value is "1"
    if(('INPUT' === tagName) && ('checkbox' === type))
    {
      $(outputElement).removeAttr('checked');
      if(('1' === content) || ('enabled' === content) || ('true' === content)) $(outputElement).prop('checked', 'checked');
    }

    // output element is part of a radio button group - set box "checked", if response value is "1"
    else if(('INPUT' === tagName) && ('radio' === type))
    {
      // get button group name
        var name = $(outputElement).attr("name");
        $("[name=" + name + "]").removeAttr("checked");
        $("[name=" + name + "][value=" + content + "]").attr("checked", "checked");
    }

    // output element is checkbox - set box "checked", if response value is "1"
    else if('SELECT' === tagName)
    {
        // if response string is existing in selection menu, show according line selected
        if($("#" + callParams.outputDivId + " option[value='" + content + "']").length)
        {
          $("#" + callParams.outputDivId).val(content);
        }

        // else add an extra line in menu for an "unknown" option
        else
        {
          $("#" + callParams.outputDivId).append("<option selected value=\"unknown\" >Unknown</option>");
        }
    }

    // output element is input field - write response into "value"
    else if('INPUT' === tagName)
    {
      $(outputElement).val(content);
    }

    // output element is tab cell, span, p, ... - write response as "html" content
    else
    {
      $(outputElement).html(content);
    }
  }
};



//------------------------------------------------------------------------------
// Show state of a parameter (port, service, etc.) by image and text and fitting text on according action button.
// State elements are included in html form - the output element must start with id "state-output-"
// button element with id "change-state-button"
//
// parameter: paramStatus:      SUCCESS: no error while reading state
//            paramState:       state value that should be visualized (enabled/disabled)
//            outputElementId:  id of encycling form element
//------------------------------------------------------------------------------
PageElements.prototype.VisualizeStateByTextAndButton = function(paramStatus, paramState, outputElementId)
{
  var stateElement  = $('#' + outputElementId + ' [id^=state_output]');
  var changeButton  = $('#' + outputElementId + ' [id^=change_state_button]');

  if((SUCCESS === paramStatus) && ("enabled" === paramState))
  {
    $(stateElement).html('<img class="state" src="images/haken_gruen_10.gif" alt="enabled" title="enabled">enabled');
    $(changeButton).val('Disable');
  }
  else if((SUCCESS === paramStatus) && ("disabled" === paramState))
  {
    $(stateElement).html('<img class="state" src="images/kreuz_rot_10.gif" alt="disabled" title="disabled">disabled');
    $(changeButton).val('Enable');
  }
  else
  {
    $(stateElement).html('Unknown');
    $(changeButton).val('-');
  }
};


// complement to function VisualizeStateByTextAndButton
PageElements.prototype.ResetStateTextAndButton = function(outputElementId)
{
  $('#' + outputElementId + ' [id^=state_output]').html('Unknown');
  $('#' + outputElementId + ' [id^=change_state_button]').val('-');
};



PageElements.prototype.InitFakeFileInput = function(areaId, fileId)
{
  // get specified id for area including the fields, or search in whole body
  var areaId                    = areaId || 'body';

  // create ids of belonging fields and buttons
  var fakeFilenameInputFieldId  = '#' + fileId + '_file_fake_input';
  var fakeFilnameButtonId       = '#' + fileId + '_file_fake_button';
  var realFileInputFieldId      = '#' + fileId + '_file_real_input';

  // click on (fake) button, start (real) file selection
  $(areaId).find(fakeFilnameButtonId).bind('click', function(event)
  {
    event.preventDefault();
    $(areaId).find(realFileInputFieldId).trigger('click');
  });

  // click on (fake) input field, start (real) file selection
  $(areaId).find(fakeFilenameInputFieldId).bind('click', function(event)
  {
    $(areaId).find(realFileInputFieldId).trigger('click');
  });

  // real file has been selected - show name in (fake) input field
  $(areaId).find(realFileInputFieldId).bind('change', function(event)
  {
    var filename = $(this).val();
    $(areaId).find(fakeFilenameInputFieldId).val(filename);
  });

};


PageBuildup.prototype.AddContentArea = function(contentId, contentHtml)
{
  // input values missing
  if(!contentId || !contentHtml) return false;

  // content id already existing
  if($('#content_area .single_content_page #' + contentId + '_content').length)
  {
    //Log('Content area already existing');
    return false;
  }

  $('#content_area').append('<div class="single_content_page" id="' + contentId + '_content">' + contentHtml + '</div>');
  $('#content_area').trigger(contentId + '_create');
};
