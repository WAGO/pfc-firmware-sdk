/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

function HeadStationInfoContent()
{
  var headStationInfo = this;

  var CreateHeadStationParamViewList = (function()
  {
    headStationInfo.paramView = new DeviceParamView();
    
    headStationInfo.paramView.Add(
    {
      paramId         : 'product_description',
      outputElementId : 'value_coupler_info_text'
    });
  })();
  
};

HeadStationInfoContent.prototype.Refresh = function()
{
  var headStationInfo = this;

  deviceParams.ReadValueGroup(headStationInfo.paramView.list, function()
  {
    headStationInfo.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(headStationInfo.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(headStationInfo.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading head station info.', errorString ] );
    };
  });

};

  
HeadStationInfoContent.prototype.Init = function()
{
  this.Refresh();
};

var headStationInfoContent = new HeadStationInfoContent();
