/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var VpnContent = function(id)
{
  var vpnContent = this;
  
  vpnContent.id = id || 'vpn';
  
  // upload/download path (fix by vpn configtool)
  vpnContent.uploadDir    = UPLOAD_DIR + '/vpncfg';
  vpnContent.downloadDir  = UPLOAD_DIR + '/vpncfg-out';
  
  //------------------------------------------------------------------------------
  // define view elements 
  //------------------------------------------------------------------------------
  var VpnCommonConfigParamView = (function()
  {
    vpnContent.paramView = new DeviceParamView();
    
    vpnContent.paramView.Add(
    {
      paramId         : 'vpn_config'
    });

  })();

  
  //------------------------------------------------------------------------------
  // first creation of content area
  //------------------------------------------------------------------------------
  $('#content_area').bind(vpnContent.id + '_create', function(event)
  {
    // prepare things for vpn software upload
    //------------------------------------------------------------------------------

    // store upload directory in upload forms (actually the same for all forms on this page)
    //$(('#' + vpnContent.id + '_content #vpn_openvpn_conf_upload_form')).append('<input type="hidden" name="upload_directory" value="' + vpnContent.uploadDir + '">');
    $(('#' + vpnContent.id + '_content form[id$=upload_form]')).append('<input type="hidden" name="upload_directory" value="' + vpnContent.uploadDir + '">');

    // show iframes FOR TEST to see the upload or download results on screen
    //$('#' + vpnContent.id + '_content #vpn_upload_iframe').show();
    //$('#' + vpnContent.id + '_content #vpn_download_iframe').show();

    // Does not work with IE! After sending click event to browse button, file selection apparently is ok - but form can't be submitted
    // Take solution with transparent "browse" button over real button to click it directly... 
    //pageElements.InitFakeFileInput('#' + softwareUploadsContent.id + '_content', 'software_upload');
    
    //------------------------------------------------------------------------------
    // add events for form processing
    //------------------------------------------------------------------------------
    
    // change OpenVPN running state
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content #vpn_openvpn_state_form').bind('submit', function(event)
    {
      event.preventDefault();
      vpnContent.ProcessOpenVpnStateChange(event.target);
    });
    
    // change IPsec running state
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content #vpn_ipsec_state_form').bind('submit', function(event)
    {
      event.preventDefault();
      vpnContent.ProcessIpSecStateChange(event.target);
    });
    
    // delete certificate
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content #vpn_certificate_area').bind('submit', function(event)
    {
      event.preventDefault();
      vpnContent.ProcessDeleteCertificate(event);
    });
    
    // delete key
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content #vpn_key_area').bind('submit', function(event)
    {
      event.preventDefault();
      vpnContent.ProcessDeleteCertificate(event);
    });


    // catch uploads by form to prevent POST 
    // formerly uploads were done via multipart/form-data. this was changed 
    // to a chunked upload using the File object solely, which makes the form data 
    // obsolete to be sent to the backend.
    $('[id$=_upload_form]').bind('submit', function(event)
    {
      event.preventDefault();
    });
    

    // after user has selected file - show filename and enable file update buttons (only) when files were selected
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content [id$=_upload_file_real_input]').bind('change', function(event)
    {
      var uploadType  = $(event.target).attr('id').replace('_upload_file_real_input', '');
      // get filename of files array, because reading fileupload value is not the correct way and adds funny strings to filename ("fakepath etc.")
      // respect, that filearray is not always existing
      var filename = "";
      if($(event.target)[0].files[0] && $(event.target)[0].files[0].name) {
        filename = $(event.target)[0].files[0].name;
      }

      $('#' + vpnContent.id + '_content #' + uploadType + '_upload_form').find('.file_browse_fake_input').val(filename);

      if(filename.length)
      {
        $('#'  + vpnContent.id + '_content #' + uploadType + '_upload_submit_button').removeAttr('disabled');
      }
      else
      {
        $('#'  + vpnContent.id + '_content #' + uploadType + '_upload_submit_button').attr('disabled', 'disabled');
      }
    });

    // submit form event - file upload (openvpn.conf, ipsec.conf, ipsec.secrets, certificate, key)
    //------------------------------------------------------------------------------
    $('#' + vpnContent.id + '_content form[id$=upload_form]').bind('submit', function(event) 
    {
      // don't prevent default!!! File upload is handled in iframe, not via ajax
      vpnContent.Upload(this);
    });

    // submit form event - download of config files (ipsec.conf, ipsec.secrets, openvpn.conf)
    //------------------------------------------------------------------------------
    //$('#' + vpnContent.id + '_content #vpn_ipsec_conf_download_form').bind('submit', function(event) 
    $('#' + vpnContent.id + '_content form[id$=download_form]').bind('submit', function(event)
    {
      event.preventDefault();
      vpnContent.Download(this);
    });
    
  });


  //------------------------------------------------------------------------------
  // refresh of complete content
  //------------------------------------------------------------------------------
  $('#content_area').bind(vpnContent.id + '_refresh', function(event)
  {
    vpnContent.Refresh();
  });

};


VpnContent.prototype.Refresh = function()
{
  var vpnContent  = this;
  
  deviceParams.ReadValueGroup(vpnContent.paramView.list, function()
  {
    //vpnContent.paramView.ShowValues(); // actually, no values can be shown automatically
    
    if(deviceParams.ReadErrorOccured(vpnContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(vpnContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading configuration.', errorString ] );
    }

    var vpnConfig = deviceParams.list['vpn_config'].value;

    vpnContent.RefreshOpenVpnRunningState(vpnConfig);
    vpnContent.RefreshOpenVpnConfiguredState(vpnConfig);
    
    vpnContent.RefreshIpSecRunningState(vpnConfig);
    vpnContent.RefreshIpSecConfiguredState(vpnConfig);
    
    vpnContent.RefreshCertificateArea(vpnConfig);
    vpnContent.RefreshKeyArea(vpnConfig);
  });

};


VpnContent.prototype.RefreshOpenVpnRunningState = function(oVpnConfig)
{
  var vpnContent  = this;
  var html        = '';
  
  if(!oVpnConfig || !oVpnConfig.vpn || !oVpnConfig.vpn.ovpn || !oVpnConfig.vpn.ovpn.runtime)
  {
    status  = ERROR;
    html    = 'unknown';
  }
  else
  {
    switch(oVpnConfig.vpn.ovpn.runtime)
    {
      case 'stopped': html = '<img class="state" title="stopped" alt="stopped" src="images/kreuz_rot_10.gif">stopped'; break;
      case 'running': html = '<img class="state" title="running" alt="running" src="images/haken_gruen_10.gif">running'; break;
      default:        html = oVpnConfig.vpn.ovpn.runtime; break;
    }
  }
  $('#' + vpnContent.id + '_content #state_output_vpn_openvpn_state').html(html);
  
  return status;
};


VpnContent.prototype.RefreshOpenVpnConfiguredState = function(oVpnConfig)
{
  var vpnContent = this;
  
  if((SUCCESS != status) || !oVpnConfig || !oVpnConfig.vpn || !oVpnConfig.vpn.ovpn || !oVpnConfig.vpn.ovpn.status)
  {
    status  = ERROR;
  }
  else switch(oVpnConfig.vpn.ovpn.status)
  {
    case 'enabled':   $('#' + vpnContent.id + '_content #vpn_openvpn_state_form [name=state]').prop('checked', 'checked'); break;
    case 'disabled':  $('#' + vpnContent.id + '_content #vpn_openvpn_state_form [name=state]').removeAttr('checked'); break;
    default:          $('#' + vpnContent.id + '_content #vpn_openvpn_state_form [name=state]').removeAttr('checked'); break;
  }

  return status;
};


VpnContent.prototype.RefreshIpSecRunningState = function(oVpnConfig)
{
  var vpnContent = this;
  var html        = '';
  
  if(!oVpnConfig || !oVpnConfig.vpn || !oVpnConfig.vpn.ipsec || !oVpnConfig.vpn.ipsec.runtime)
  {
    status  = ERROR;
    html    = 'unknown';
  }
  else
  {
    switch(oVpnConfig.vpn.ipsec.runtime)
    {
      case 'stopped': html = '<img class="state" title="stopped" alt="stopped" src="images/kreuz_rot_10.gif">stopped'; break;
      case 'running': html = '<img class="state" title="running" alt="running" src="images/haken_gruen_10.gif">running'; break;
      default:        html = oVpnConfig.vpn.ipsec.runtime; break;
    }
  }
  $('#' + vpnContent.id + '_content #state_output_vpn_ipsec_state').html(html);
  
  return status;
};


VpnContent.prototype.RefreshIpSecConfiguredState = function(oVpnConfig)
{
  var vpnContent = this;
  
  if((SUCCESS != status) || !oVpnConfig || !oVpnConfig.vpn || !oVpnConfig.vpn.ipsec || !oVpnConfig.vpn.ipsec.status)
  {
    status  = ERROR;
  }
  else switch(oVpnConfig.vpn.ipsec.status)
  {
    case 'enabled':   $('#' + vpnContent.id + '_content #vpn_ipsec_state_form [name=state]').prop('checked', 'checked'); break;
    case 'disabled':  $('#' + vpnContent.id + '_content #vpn_ipsec_state_form [name=state]').removeAttr('checked'); break;
    default:          $('#' + vpnContent.id + '_content #vpn_ipsec_state_form [name=state]').removeAttr('checked'); break;
  }

  return status;
};


VpnContent.prototype.RefreshCertificateArea = function(oVpnConfig)
{
  var vpnContent = this;

  // remove old content
  $('#' + vpnContent.id + '_content #vpn_certificate_area').html('');

  if(oVpnConfig && oVpnConfig.vpn && oVpnConfig.vpn.certs)
  {
    var certificateIndex = 0;
    $.each(oVpnConfig.vpn.certs, function(certificateName, oCertificate)
    {
      var html = vpnContent.GetCertificateHtml(certificateIndex, certificateName, oCertificate);
      $('#' + vpnContent.id + '_content #vpn_certificate_area').append(html);
      ++certificateIndex;
    });
  }
  
  if(0 === $('#' + vpnContent.id + '_content #vpn_certificate_area form[id^=vpn_certificate]').length)
  {
    var html = vpnContent.GetNoCertificateHtml('certificates');
    $('#' + vpnContent.id + '_content #vpn_certificate_area').append(html);
  }

  // set margin after last entry (group ends here)
  $('#' + vpnContent.id + '_content #vpn_certificate_area div.config_form_box').last().removeClass('group');
};


VpnContent.prototype.RefreshKeyArea = function(oVpnConfig)
{
  var vpnContent = this;

  // remove old content
  $('#' + vpnContent.id + '_content #vpn_key_area').html('');

  if(oVpnConfig && oVpnConfig.vpn && oVpnConfig.vpn.keys)
  {
    var keyIndex = 0;
    $.each(oVpnConfig.vpn.keys, function(keyIndex, keyName)
    {
      var html = vpnContent.GetKeyHtml(keyIndex, keyName);
      $('#' + vpnContent.id + '_content #vpn_key_area').append(html);
      ++keyIndex;
    });
  }
  
  if(0 === $('#' + vpnContent.id + '_content #vpn_key_area form[id^=vpn_key]').length)
  {
    var html = vpnContent.GetNoCertificateHtml('keys');
    $('#' + vpnContent.id + '_content #vpn_key_area').append(html);
  }
  
  // set margin after last entry (group ends here)
  $('#' + vpnContent.id + '_content #vpn_key_area div.config_form_box').last().removeClass('group');
};


VpnContent.prototype.GetCertificateHtml = function(certificateIndex, certificateName, oCertificate)
{
  var vpnContent  = this;
  var html        = '';
  
  if(certificateName && oCertificate && oCertificate.notBefore)
  {
    var timeString = vpnContent.ConvertCertificateTime(oCertificate.notBefore);
    
    html += '<div class="config_form_box group">';
    html +=   '<form id="vpn_certificate_' + certificateIndex + '" action="javascript_requested.php" method="POST">';
    html +=     '<div class="config_data_list">';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="value_field"><span class="filename">' + certificateName + '</span> (' + timeString + ')</div>';
    html +=       '</div>';
    html +=     '</div>';
    html +=     '<p class="button_area">';
    html +=       '<input class="button" type="submit" value="Delete" name="delete_certificate">';
    html +=     '</p>';
    html +=   '</form>';
    html += '</div>';
  }

  return html;
};


VpnContent.prototype.ConvertCertificateTime = function(certTime)
{
  var timeString = certTime;
  
  // change timestamp to dd.mm.yyyy, hh:mm:ss
  /*
  var date        = new Date(parseInt(oCertificate.notBefore, 10) * 1000);
  var minutes     = "0" + date.getMinutes();
  var seconds     = "0" + date.getSeconds();
  var timeString = date.getDate() + '.' + (date.getMonth() + 1) + '.' + date.getFullYear() + ', ' + date.getHours() + ':' + minutes.substr(-2) + ':' + seconds.substr(-2);
  */
  
  // convert time string
  //  150622095134Z -> 2015.06.22, 09.51.34 Z (Z == Zulu == GMT == UTC)
  var year      = '20' + certTime.substr(0, 2);
  var month     = certTime.substr(2, 2);
  var day       = certTime.substr(4, 2);
  var hours     = certTime.substr(6, 2);
  var minutes   = certTime.substr(8, 2);
  var seconds   = certTime.substr(10,2);
  var timezone  = certTime.substr(12, 1);
  
  timeString = day + '.' + month + '.' + year + ', ' + hours + ':' + minutes + ':' + seconds + ', ' + timezone;
  
  return timeString;
};


VpnContent.prototype.GetKeyHtml = function(keyIndex, keyName)
{
  var vpnContent  = this;
  var html        = '';
  
  if(keyName)
  {
    html += '<div class="config_form_box group">';
    html +=   '<form id="vpn_key_' + keyIndex + '" action="javascript_requested.php" method="POST">';
    html +=     '<div class="config_data_list">';
    html +=       '<div class="label_value_pair">';
    html +=         '<div class="value_field"><span class="filename">' + keyName + '</span></div>';
    html +=       '</div>';
    html +=     '</div>';
    html +=     '<p class="button_area">';
    html +=       '<input class="button" type="submit" value="Delete" name="delete_key">';
    html +=     '</p>';
    html +=   '</form>';
    html += '</div>';
  }

  return html;
};


VpnContent.prototype.GetNoCertificateHtml = function(certificateText)
{
  var vpnContent      = this;
  var certificateText = certificateText || 'entry';
  var html            = '';
  
  html += '<div class="config_form_box group">';
  html +=   '<div class="config_data_list">';
  html +=     '<div class="label_value_pair">';
  html +=       '<div class="value_field">No ' + certificateText + ' existing.</div>';
  html +=     '</div>';
  html +=   '</div>';
  html += '</div>';

  return html;
};


VpnContent.prototype.ProcessOpenVpnStateChange = function(formObj)
{
  var vpnContent  = this;
  var test        = $(formObj).find('input[name=state]:checked').length;
  var newState    = $(formObj).find('input[name=state]:checked').length ? '--enable' : '--disable';
  
  var newValueList  = { state: newState };

  pageElements.ShowBusyScreen('Change OpenVPN state...');

  deviceParams.ChangeValue('vpn_openvpn_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing OpenVPN state.', errorText ]);
    }

    // show actual configuration
    vpnContent.Refresh();
  });

};


VpnContent.prototype.ProcessIpSecStateChange = function(formObj)
{
  var vpnContent  = this;
  var newState    = $(formObj).find('input[name=state]:checked').length ? '--enable' : '--disable';
  
  var newValueList  = { state: newState };

  pageElements.ShowBusyScreen('Change IPsec state...');

  deviceParams.ChangeValue('vpn_ipsec_state', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();

    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while changing IPsec state.', errorText ]);
    }

    // show actual configuration
    vpnContent.Refresh();
  });

};


VpnContent.prototype.ProcessDeleteCertificate = function(formObj)
{
  var vpnContent  = this;
  var filename    = $(formObj.target).find('span.filename').html();
  var fileType    = '';
  
  if($(formObj.target).attr('id').match('certificate')) fileType = 'cert';
  else if($(formObj.target).attr('id').match('key')) fileType = 'key';
  
  if((0 === filename.length) || (0 === fileType.length))
  {
    $('body').trigger('event_errorOccured', [ 'Software error - invalid filename or type.' ]);
  }
  else
  {
    var newValueList  = { fileType: fileType, filename: filename };
    
    pageElements.ShowBusyScreen('Deleting file ' + filename + '...');

    deviceParams.ChangeValue('vpn_delete_certificate', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();

      if(SUCCESS != status)
      {
        $('body').trigger('event_errorOccured', [ 'Error while deleting file.', errorText ]);
      }

      // show actual configuration
      vpnContent.Refresh();
    });
  }
};


VpnContent.prototype.Upload = function(formObj)
{
  var vpnContent      = this;
  var uploadType      = $(formObj).attr('id').replace('_upload_form', '');
  var uploadFileType  = '';

  switch(uploadType)
  {
    case 'vpn_openvpn_conf':  uploadFileType = 'openvpn.conf';  break;
    case 'vpn_ipsec_conf':    uploadFileType = 'ipsec.conf';    break;
    case 'vpn_ipsec_secrets': uploadFileType = 'ipsec.secrets'; break;
    case 'vpn_certificate'  : uploadFileType = 'cert';          break;
    case 'vpn_key'          : uploadFileType = 'key';           break;
    default: break;
  }

  if(0 === uploadFileType.length)
  {
    $('body').trigger('event_errorOccured', [ 'Software error: wrong input parameter.' ]);
  }
  else
  {
    pageElements.ShowBusyScreen('Upload new ' + uploadFileType + '...');

    var fileInput = $(formObj).find('[id$=upload_file_real_input]')[0];
    var file = fileInput.files[0];

    Xhr.sessionToken = deviceCommunication.csrfToken;
    var upload = new Upload([{
      file: file,
      destination: vpnContent.uploadDir + '/' + file.name
    }]);

    upload.execute(function(filepaths, error) {

      pageElements.RemoveBusyScreen();

      if (error) {
        $('body').trigger('event_errorOccured', [ 'File upload failed.', error.message ]);
      } else {
      
        var filepath = filepaths[0];
        
        $(formObj).find('[id$=upload_file_real_input]').val('');
        $(formObj).find('[id$=upload_file_real_input]').trigger('change');

        var newValueList  = { 
          configFileType: uploadFileType, 
          filename: file.name 
        };
        
        pageElements.ShowBusyScreen('Activating new ' + uploadFileType + '...');

        deviceParams.ChangeValue('vpn_store_config_file', newValueList, function(status, errorText)
        {
          pageElements.RemoveBusyScreen();
        
          if(SUCCESS != status)
          { 
            $('body').trigger('event_errorOccured', [ 'Error while activating new ' + uploadFileType + '.', errorText ]);
          }
          upload.cleanup();
          vpnContent.Refresh();
        });
      }
    });
  }

};

VpnContent.prototype.Download = function(formObj)
{
  var vpnContent      = this;
  var downloadType    = $(formObj).attr('id').replace('_download_form', '');
  var configFileType  = '';

  switch(downloadType)
  {
    case 'vpn_openvpn_conf':  configFileType = 'openvpn.conf';  break;
    case 'vpn_ipsec_conf':    configFileType = 'ipsec.conf';    break;
    case 'vpn_ipsec_secrets': configFileType = 'ipsec.secrets'; break;
    case 'vpn_certificate'  : uploadFileType = 'cert';          break;
    case 'vpn_key'          : uploadFileType = 'key';           break;
    default: break;
  }

  if(0 === configFileType.length)
  {
    $('body').trigger('event_errorOccured', [ 'Software error: wrong input parameter.', errorText ]);
  }
  else
  {
    pageElements.ShowBusyScreen('Create copy of ' + configFileType + '...');
    
    //var newValueList  = { configFileType: configFileType, filePath: vpnContent.downloadDir + '/' + configFileType };
    var newValueList  = { configFileType: configFileType };

    deviceParams.ChangeValue('vpn_copy_config_file', newValueList, function(status, errorText)
    {
      pageElements.RemoveBusyScreen();
        
      if(SUCCESS != status)
      { 
        $('body').trigger('event_errorOccured', [ 'Error while download of ' + configFileType + '.', errorText ]);
      }
      else
      {
        // provide download file via iframe, transfair csrf id also to ensure access rights
        var downloadFrame = $('#vpn_download_iframe');
        $(downloadFrame).attr('src', 'offer_download.php?download_dir=' + vpnContent.downloadDir + '&csrf_id=' + deviceCommunication.csrfToken);

        // evaluate result when iframe content is ready
        $(downloadFrame).bind('load', function()
        {
          // status and error only are available in case of errors (otherwise we only got the download file)
          pageElements.RemoveBusyScreen();
          var status  = downloadFrame.contents().find('.status').html();
          if(status && (SUCCESS != status))
          {
            var errorText   = downloadFrame.contents().find('.error_text').html();
            $('body').trigger('event_errorOccured', [ 'Error while download of ' + configFileType, errorText ]);
          }
          $(this).unbind();
        });
      }
    });
  }
};


