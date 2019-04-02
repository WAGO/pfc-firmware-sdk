/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

$(document).ready(function()
{
  // mouseover tooltip for links
  var tooltipSelector = 'a.tooltip_link';
  var tooltip     = {};
  var linkObj     = {};
  
  function GetAdjustedLeftPosition(leftPosition)
  {
    var invisibleWidth = leftPosition + tooltip.outerWidth() - document.body.offsetWidth;
    if(invisibleWidth > 0) 
    {
      leftPosition -= invisibleWidth;
    }
    
    return leftPosition;
  }
  
  // event - mouseenter
  $(document).on('mouseenter', tooltipSelector, function(event) 
  {
    //console.log("tooltip mouseover");
    linkObj = $(this);
    
    // save link title text and delete it from link to avoid normal display by browser
    var title = linkObj.attr('title');
    linkObj.attr('title', '');

    // add div container for tooltip to DOM and memorize
    $('body').append('<div id="tooltip" class="tooltip" />');
    tooltip = $('div#tooltip');

    // add tooltip text to box and locate it on screen depending on mouse position
    tooltip.html(title);
    tooltip.css(
    {
      'left'    : GetAdjustedLeftPosition(event.pageX + 10) + 'px',
      'top'     : (event.pageY + 10) + 'px',
      'font'    : '11px Verdana, sans-serif'
      //'width'   : '100px',
      //'height'  : '50px',
    });
    
    tooltip.fadeIn('slow');
    
    // add handler for mousemove - change position of tooltip box depending on mouse position
    $(document).on('mousemove', tooltipSelector, function(event) 
    {
      //console.log("tooltip mousemove");
      tooltip.css(
      {
        'left'  : GetAdjustedLeftPosition(event.pageX + 10) + 'px',
        'top'   : (event.pageY + 10) + 'px'
      });
    });
  });

  
  // event - mouseclick
  $(document).on('click', tooltipSelector, function(event) 
  {
    // do nothing, especially don't follow link!
    event.preventDefault();
    event.stopPropagation();
    return false;
  });
    

  // event - content change. Can only be triggered by another script (eg. content change after ajax request)
  $(document).on('change', tooltipSelector, function(event) 
  {
    //console.log("toolObj.change");
    var clickLinkObj = $(this);
    // if tooltip is actually existing and the changed object is the one the tooltip showes
    if(tooltip.length && ($(linkObj).attr('href') == $(clickLinkObj).attr('href')) )
    {
      // remove title text from link and put it into tooltip again
      var title = linkObj.attr('title');
      linkObj.attr('title', '');
      tooltip.html(title);
    }
  });
  
  
  // event - mouseleave
  $(document).on('mouseleave', tooltipSelector, function(event) 
  {
    //console.log("tooltip leave");
    // put tooltip text back to href title element, if title value has not explicit been changed meanwhile (e.g. by ajax) 
    linkObj.attr("title", tooltip.html());
    
    // remove complete tooltip object and memorize 
    tooltip.remove();
    tooltip = $('div#tooltip');
  });

});


