/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

var SoftwareUploadsContent = function(id)
{
  var softwareUploadsContent = this;
  
  softwareUploadsContent.id = id || 'software_upload';
  

  //------------------------------------------------------------------------------
  // internal functions
  //------------------------------------------------------------------------------
  var ShowActualUpdatePackageCallback = function(paramStatus, updatePackageFilename, formId)
  {
    if((SUCCESS === paramStatus) && (updatePackageFilename.length > 0))
    {
      softwareUploadsContent.uploadExists = true;
      $('#' + formId + ' .actual_update_package').html(updatePackageFilename);
      $('#' + formId + ' .upload_handling_area').show();
    }
    else
    {
      softwareUploadsContent.uploadExists = false;
      $('#' + formId + ' .actual_update_package').html('No upload file existing.');
      $('#' + formId + ' .upload_handling_area').hide();
    }
  };

  //------------------------------------------------------------------------------
  // initialize view elements
  //------------------------------------------------------------------------------
  var SoftwareUploadsParamView = (function()
  {
    softwareUploadsContent.paramView = new DeviceParamView();
                
    softwareUploadsContent.paramView.Add(
    {
      paramId         : 'software_upload_update_script',
      outputElementId : softwareUploadsContent.id + '_content #package_update_activate_form',
      outputFkt       : ShowActualUpdatePackageCallback
    });
            
  })();
      

  //------------------------------------------------------------------------------
  // first creation of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(softwareUploadsContent.id + '_create', function(event)
  {
    // show iframe for test to see the upload results on screen
    //$('#' + softwareUploadsContent.id + '_content #upload_package_update_iframe').show();

    // Does not work with IE! After sending click event to browse button, file selection apparently is ok - but form can't be submitted
    // Take solution with transparent "browse" button over real button to click it directly... 
    //pageElements.InitFakeFileInput('#' + softwareUploadsContent.id + '_content', 'software_upload');
    
    // event: user has selected a file
    $('#' + softwareUploadsContent.id + '_content #software_upload_file_real_input').bind('change', function(event)
    {
      // get filename of files array, because reading fileupload value is not the correct way and adds funny strings to filename ("fakepath etc.")
      // respect, that filearray is not always existing
      var filename = "";
      if($(event.target)[0].files[0] && $(event.target)[0].files[0].name) {
        filename = $(event.target)[0].files[0].name;
      }
      // show filename in fake input field
      $('#' + softwareUploadsContent.id + '_content #package_update_upload_form').find('.file_browse_fake_input').val(filename);

      // enable update button if really one file was selected
      if(filename.length)
      {
        $('#'  + softwareUploadsContent.id + '_content #software_upload_submit_button').removeAttr('disabled');
      }
      else
      {
        $('#'  + softwareUploadsContent.id + '_content #software_upload_submit_button').attr('disabled', 'disabled');
      }
    });
    
    // submit form event - file upload
    $('#' + softwareUploadsContent.id + '_content #package_update_upload_form').bind('submit', function(event) 
    {
      // don't prevent default!!! File upload is handled in iframe, not via ajax
      softwareUploadsContent.UploadPackage(this);
    });

    // submit form event - activate or delete uploaded file
    $('#' + softwareUploadsContent.id + '_content #package_update_activate_form').bind('submit', function(event) 
    {
      event.preventDefault();
      var action = $(this).find('[name=package_update_handling]:checked').val();
      if('activate' === action)
      {
        softwareUploadsContent.ActivatePackage(this);
      }
      else if ('force' === action)
      {
        softwareUploadsContent.ForcePackage(this);
      }
      else
      {
        softwareUploadsContent.DeletePackage(this);
      }
    });

    // catch uploads by form to prevent POST 
    // formerly uploads were done via multipart/form-data. this was changed 
    // to a chunked upload using the File object solely, which makes the form data 
    // obsolete to be sent to the backend.
    $('[id$=_upload_form]').bind('submit', function(event)
    {
      event.preventDefault();
    });

  });


  //------------------------------------------------------------------------------
  // refresh of area
  //------------------------------------------------------------------------------
  $('#content_area').bind(softwareUploadsContent.id + '_refresh', function(event)
  {
    softwareUploadsContent.Refresh();
  });

};


SoftwareUploadsContent.prototype.Refresh = function()
{
  var softwareUploadsContent = this;
  
  deviceParams.ReadValueGroup(softwareUploadsContent.paramView.list, function()
  {
    softwareUploadsContent.paramView.ShowValues();
    if(deviceParams.ReadErrorOccured(softwareUploadsContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(softwareUploadsContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading software packages.', errorString ] );
    }
  });

};



SoftwareUploadsContent.prototype.UploadPackage = function(formObj)
{
  var softwareUploadsContent = this;

  pageElements.ShowBusyScreen('Upload software...');
  
  if (softwareUploadsContent.uploadExists) {
    var discardUploadForm = $('#package_update_activate_form')[0];
    softwareUploadsContent.DeletePackage(discardUploadForm, doUpload);
  } else {
    doUpload();
  }

  function doUpload () {

    var fileInput = $(formObj).find('[id$=upload_file_real_input]')[0];
    var file = fileInput.files[0];

    Xhr.sessionToken = deviceCommunication.csrfToken;
    var upload = new Upload([{
      file: file, 
      destination: '/var/downloads/update-script' + '/' + file.name
    }]);

    upload.execute(function(_, error) {
      pageElements.RemoveBusyScreen();

      if (error) {
        $('body').trigger('event_errorOccured', [ 'File upload failed.', error.message ]);
      } else {
        upload.cleanup();

        $('#' + softwareUploadsContent.id + '_content #software_upload_file_real_input').val('');
        $('#' + softwareUploadsContent.id + '_content #software_upload_file_real_input').trigger('change');

        softwareUploadsContent.Refresh();
      }
    });
  };

};


SoftwareUploadsContent.prototype.ActivatePackage = function(formObj)
{
  var softwareUploadsContent = this;
  
  var newValueList  = { packageType: 'update-script' };
  
  pageElements.ShowBusyScreen('Activating new software...');

  deviceParams.ChangeValue('activate_software_package', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while activating new software.', errorText ]);
    }
    
    softwareUploadsContent.Refresh();
  });

};


SoftwareUploadsContent.prototype.ForcePackage = function(formObj)
{
  var softwareUploadsContent = this;
  
  var newValueList  = { packageType: 'update-script' };
  
  pageElements.ShowBusyScreen('Activating new software...');

  deviceParams.ChangeValue('force_software_package', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while activating new software.', errorText ]);
    }
    
    softwareUploadsContent.Refresh();
  });

};


SoftwareUploadsContent.prototype.DeletePackage = function(formObj, callback)
{
  var softwareUploadsContent = this;
  callback = callback || function() { 
    softwareUploadsContent.Refresh(); 
  };
  
  var newValueList  = { packageType: 'update-script', action: 'delete' };
  
  pageElements.ShowBusyScreen('Delete upload file...');

  deviceParams.ChangeValue('delete_software_package', newValueList, function(status, errorText)
  {
    pageElements.RemoveBusyScreen();
    
    if(SUCCESS != status)
    {
      $('body').trigger('event_errorOccured', [ 'Error while deleting upload file.', errorText ]);
    }
    
    callback();
  });

};

