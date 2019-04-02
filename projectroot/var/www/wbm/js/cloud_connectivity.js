/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

/*
 * functions for read and write settings of DataAgent
 */

 
 
/*---------------------------------------------------------------------------
* define parameters
* ---------------------------------------------------------------------------
*/
var CreateIEC60870Params = (function()
{
    deviceParams.Add(
    {
      id                    : 'dataagent_operation_status',
      exampleValue          : '{...}' ,
      configtoolReadParams  :
      {
        name      : 'get_dataagent_config',
        parameter : [ 'dataagent-operation-status' ]
      }
    });

    deviceParams.Add(
    {
      id                    : 'dataagent_config_params',
      exampleValue          : '{...}' ,
      configtoolReadParams  :
      {
        name      : 'get_dataagent_config',
        parameter : [ 'dataagent-config-params' ]
      }
    });

	deviceParams.Add(
	{
	  id                    : 'dataagent_feature',
	  invariant             : true,
	  configtoolReadParams:
	  {
	   name      : 'detectfeature',
	   parameter : [ 'feature_dataagent' ]
	  }
	});

	deviceParams.Add(
	{
		id                    : 'iec60870_',
		configtoolWriteParams :
		{
			name      : 'config_dataagent',
			parameter : [
				'theNewIoTPlatform=$theNewIoTPlatform',
				'theNewHostName=$theNewHostName',
				'theNewDeviceId=$theNewDeviceId',
				'theNewSharedAccessKey=$theNewSharedAccessKey',
				'theMqttHost=$theMqttHost',
				'theMqttUser=$theMqttUser',
				'theMqttPassword=$theMqttPassword',
				'theMqttClientId=$theMqttClientId',
				'theMqttTLSOn=$theMqttTLSOn',
				'theMqttPort=$theMqttPort',
				'theMqttCaFile=$theMqttCaFile',
				'theMqttCertFile=$theMqttCertFile',
				'theMqttKeyFile=$theMqttKeyFile',
				'theDataProtocol=$theDataProtocol',
				'theCacheMode=$theCacheMode',
				'theDeviceInfoEnabled=$theDeviceInfoEnabled',
				'theDeviceStatusEnabled=$theDeviceStatusEnabled',
				'theStandardCommandsEnabled=$theStandardCommandsEnabled',
				'theCloudConnectivityEnabled=$theCloudConnectivityEnabled',
				'theGroupId=$theGroupId',
				'theLastWillEnabled=$theLastWillEnabled',
				'theLastWillTopic=$theLastWillTopic',
				'theLastWillPayload=$theLastWillPayload',
				'theLastWillQos=$theLastWillQos',
				'theLastWillRetain=$theLastWillRetain',
				'theMqttCleanSession=$theMqttCleanSession',
			],
			sudo      : true
		}
	});

})();

  
function DataAgentPageContent(id)
{
	var theSparkplugLicenseCovered = 0;
	
	var thePageContent = this;
	thePageContent.id = id;

	var RefreshVisibilityMqttLastWill = function()
	{
		if($("#divLastWillEnabled").is(":visible") && $("#chkMqttLastWillEnabled").is(':checked'))
		{
			$('#divLastWillTopic').show();
			$('#divLastWillPayload').show();

			if (iotplatformValue = "AWS")
			{
				$('#divLastWillQoS').show();
				var LastWillQoS = $("#divLastWillQoS").val();
				$("#divLastWillQoS option[value='2']").attr('disabled','disabled');
				if (LastWillQoS == "2")
				{
					$('#divLastWillQoS').prop('value', '1');
				}
				
				$('#divLastWillRetain').prop('checked', true).attr('disabled','disabled');
			}
			else
			{
				$('#divLastWillRetain').show();
			}
		}
		else
		{
			$('#divLastWillTopic').hide();
			$('#divLastWillPayload').hide();
			$('#divLastWillQoS').hide();
			$('#divLastWillRetain').hide();
		}
	}

	var RefreshVisibility = function()
	{
		var iotplatformValue = $("#selectIotPlatform").val();
		//console.log(iotplatformValue);
		var textLabel1 = "Client ID:";
		var textLabel2 = "Edge node ID:";
		switch (iotplatformValue)
		{
			case "ECloud":
				$("#AzureIoTconfigUI").show();
				$('#mqttIoTconfigUI').hide();
				$('#IBMConfigUI').hide();
				$('#AWSConfigUI').hide();
				$('#DeviceConfigUI').hide();
				$('#CommandsConfigUI').hide();
				$("#chkDeviceInfoEnabled").prop('checked', true);
				$("#chkDeviceStatusEnabled").prop('checked', true);
				$("#chkStandardCommandsEnabled").prop('checked', true);
				$("#chkMqttSecurityEnabled").removeAttr('disabled');
				$("#inputMqttPort").removeAttr('disabled');
				var varHostName = $('#inputHostName').val();
				if (!varHostName)
				{
					$("#inputHostName").val('wagocloud.azure-devices.net');
				}
				$('#divLastWillEnabled').hide();
				RefreshVisibilityMqttLastWill();
				$('#selectDataProtocol').prop('value', 'NormalizedJSON');
				$('#divDataProtocol').hide();
				$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
				break;

			case "AzureCloud":
				$("#AzureIoTconfigUI").show();
				$("#mqttIoTconfigUI").hide();
				$("#divNodeGroupId").hide();
				$('#IBMConfigUI').hide();
				$('#AWSConfigUI').hide();
				$('#DeviceConfigUI').show();
				$('#CommandsConfigUI').show();
				$("#chkMqttSecurityEnabled").removeAttr('disabled');
				$("#inputMqttPort").removeAttr('disabled');
				$('#divLastWillEnabled').hide();
				RefreshVisibilityMqttLastWill();
				$('#selectDataProtocol').prop('value', 'NormalizedJSON');
				$('#divDataProtocol').hide();
				$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
				break;

			case "IBM":
				$("#AzureIoTconfigUI").hide();
				$("#mqttIoTconfigUI").show();
				$("#labelDeviceOrEdgeNodeId").text(textLabel1);
				$("#divNodeGroupId").hide();
				$('#IBMConfigUI').show();
				$('#AWSConfigUI').hide();

				var dataProtVal = $("#selectDataProtocol").val();
				$("#selectDataProtocol option[value='SparkplugB']").attr('disabled','disabled');
				if (dataProtVal == "SparkplugB")
				{
					$('#selectDataProtocol').prop('value', 'NormalizedJSON');
				}

				dataProtVal = $("#selectDataProtocol").val();
				if (dataProtVal == "NormalizedJSON")
				{
					$('#DeviceConfigUI').show();
				}
				else
				{
					$('#DeviceConfigUI').hide();
				}

				$('#CommandsConfigUI').hide();
				$("#chkStandardCommandsEnabled").prop('checked', false);
				$("#chkMqttSecurityEnabled").removeAttr('disabled');
				$("#chkMqttDoCleanSession").prop('checked', false).attr('disabled','disabled');
				$("#inputMqttPort").removeAttr('disabled');
				$('#divLastWillEnabled').show();
				RefreshVisibilityMqttLastWill();
				$('#divDataProtocol').show();
				$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
				break;

			case "AWS":
				$("#AzureIoTconfigUI").hide();
				$("#mqttIoTconfigUI").show();
				$("#labelDeviceOrEdgeNodeId").text(textLabel1);
				$('#IBMConfigUI').hide();
				$('#AWSConfigUI').show();
				var dataProtVal = $("#selectDataProtocol").val();
				if (dataProtVal == "NormalizedJSON")
				{
					$('#DeviceConfigUI').show();
					$('#CommandsConfigUI').show();
				}
				else
				{
					$('#DeviceConfigUI').hide();
					$('#CommandsConfigUI').hide();
				}
/*				if (theSparkplugLicenseCovered)
				{
					$("#selectDataProtocol option[value='SparkplugB']").removeAttr('disabled');
				}
				else
				{
					$("#selectDataProtocol option[value='SparkplugB']").attr('disabled','disabled');
					if (dataProtVal == "SparkplugB")
					{
						$('#selectDataProtocol').prop('value', 'NormalizedJSON');
					}
				}
*/				$("#chkMqttSecurityEnabled").prop('checked', true);
				$("#chkMqttSecurityEnabled").attr('disabled', 'true');
				$("#chkMqttDoCleanSession").prop('checked', true).attr('disabled','disabled');
				$("#inputMqttPort").val('8883');
				$("#inputMqttPort").attr('disabled','true');
				if (dataProtVal == "SparkplugB")
				{
					$("#labelDeviceOrEdgeNodeId").text(textLabel2);
					$("#divNodeGroupId").show();
					$('#divLastWillEnabled').hide();
					$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').show();
				}
				else
				{
					$("#divNodeGroupId").hide();
					$('#divLastWillEnabled').show();	
					$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
				}
				RefreshVisibilityMqttLastWill();
				$('#divDataProtocol').show();
				break;

			case "MQTT":
				$("#AzureIoTconfigUI").hide();
				$("#mqttIoTconfigUI").show();
				$("#labelDeviceOrEdgeNodeId").text(textLabel1);
				$('#IBMConfigUI').show();
				$('#AWSConfigUI').show();
				var dataProtVal = $("#selectDataProtocol").val();
				if (dataProtVal == "NormalizedJSON")
				{
					$('#DeviceConfigUI').show();
					$('#CommandsConfigUI').show();
				}
				else
				{
					$('#DeviceConfigUI').hide();
					$('#CommandsConfigUI').hide();
				}
/*				if (theSparkplugLicenseCovered)
				{
					$("#selectDataProtocol option[value='SparkplugB']").removeAttr('disabled');
				}
				else
				{
					$("#selectDataProtocol option[value='SparkplugB']").attr('disabled','disabled');
					if (dataProtVal == "SparkplugB")
					{
						$('#selectDataProtocol').prop('value', 'NormalizedJSON');
					}
				}
*/				$("#chkMqttSecurityEnabled").removeAttr('disabled');
				$("#chkMqttDoCleanSession").removeAttr('disabled');
				$("#inputMqttPort").removeAttr('disabled');
				if (dataProtVal == "SparkplugB")
				{
					$("#labelDeviceOrEdgeNodeId").text(textLabel2);
					$("#divNodeGroupId").show();
					$('#divLastWillEnabled').hide();
					$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').show();
				}
				else
				{
					$("#divNodeGroupId").hide();
					$('#divLastWillEnabled').show();
					$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();	
				}
				RefreshVisibilityMqttLastWill();
				$('#divDataProtocol').show();
				break;

			default:
				$("#AzureIoTconfigUI").hide();
				$("#mqttIoTconfigUI").hide();
				$('#IBMConfigUI').hide();
				$('#AWSConfigUI').hide();
				$('#divApplicationBehavior').hide();
				$('#DeviceConfigUI').hide();
				$('#CommandsConfigUI').hide();
				$('#divLastWillEnabled').hide();
				$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
				RefreshVisibilityMqttLastWill();
		}

		var massStorageVal = $("#theMassStorageIsReady").val();
		var dataProtVal = $("#selectDataProtocol").val();
		if (massStorageVal == "false" || dataProtVal == "PlcProgSpecific" || dataProtVal == "SparkplugB")
		{
			$("#selectCacheMode option[value='SDCard']").attr('disabled','disabled');
			$("#selectCacheMode").prop('value', 'RAM');
		}
		else
		{
			$("#selectCacheMode option[value='SDCard']").removeAttr('disabled');
			$("#selectCacheMode").prop('value', configParams.CacheMode); // IE-8 compatible syntax
		}
	};

    var DisplayConfigParams = function(paramStatus, paramValue, outputElementId)
    {
        //console.log('' + outputElementId  + ' ' + paramStatus + ' ' + paramValue);
        try
        {
            configParams = jQuery.parseJSON(paramValue);
            var parentElemIdPrefix = "#" + thePageContent.id + "_content ";
            $(parentElemIdPrefix + "#chkCloudConnectivityEnabled").prop('checked', configParams.CloudConnectivityEnabled);
            $(parentElemIdPrefix + "#selectIotPlatform").prop('value', configParams.IoTPlatform); // IE-8 compatible syntax

            $(parentElemIdPrefix + "#inputHostName").val(configParams.HostName);
            $(parentElemIdPrefix + "#inputDeviceId").val(configParams.DeviceId);
            $(parentElemIdPrefix + "#inputSharedAccessKey").val(configParams.SharedAccessKey);

            $(parentElemIdPrefix + "#inputMqttHostName").val(configParams.MQTTHost);
            $(parentElemIdPrefix + "#inputMqttUserName").val(configParams.MQTTUser);
            $(parentElemIdPrefix + "#inputMqttUserPassword").val(configParams.MQTTPassword);
            $(parentElemIdPrefix + "#inputMqttGroupId").val(configParams.GroupId);
			$(parentElemIdPrefix + "#inputMqttClientId").val(configParams.MQTTClientId);
			$(parentElemIdPrefix + "#chkMqttDoCleanSession").prop('checked', configParams.MQTTDoCleanSession);
            $(parentElemIdPrefix + "#chkMqttSecurityEnabled").prop('checked', configParams.MQTTTLSOn);
            $(parentElemIdPrefix + "#inputMqttPort").val(configParams.MQTTPort);
            $(parentElemIdPrefix + "#inputMqttCaFile").val(configParams.MQTTCaFile);
            $(parentElemIdPrefix + "#inputMqttCertFile").val(configParams.MQTTCertFile);
            $(parentElemIdPrefix + "#inputMqttKeyFile").val(configParams.MQTTKeyFile);
			$(parentElemIdPrefix + "#chkMqttLastWillEnabled").prop('checked', configParams.LastWillEnabled);
			$(parentElemIdPrefix + "#inputMqttLastWillTopic").val(configParams.LastWillTopic);
			$(parentElemIdPrefix + "#inputMqttLastWillPayload").val(configParams.LastWillPayload);
			$(parentElemIdPrefix + "#selectMqttLastWillQoS").prop('value', configParams.LastWillQoS);
			$(parentElemIdPrefix + "#chkMqttLastWillRetain").prop('checked', configParams.LastWillRetain);
			$(parentElemIdPrefix + "#selectDataProtocol").prop('value', configParams.DataProtocol);

            $(parentElemIdPrefix + "#chkDeviceInfoEnabled").prop('checked', configParams.DeviceInfo);
            $(parentElemIdPrefix + "#chkDeviceStatusEnabled").prop('checked', configParams.DeviceStatus);
            $(parentElemIdPrefix + "#chkStandardCommandsEnabled").prop('checked', configParams.StandardCommandsEnabled);
			$(parentElemIdPrefix + "#theMassStorageIsReady").val(configParams.NonVolatileStorageIsAvailable);

            RefreshVisibility();
        }
        catch(e)
        {
            console.log('Error while parsing DataAgent configuration from a JSON string!');
        }
    }

	var DisplayDataAgentStopped = function()
	{
		var parentElemIdPrefix = "#" + thePageContent.id + "_content ";
		
		var operationSpanElem = $(parentElemIdPrefix + "#spanOperation");
		operationSpanElem.empty();
		operationSpanElem.text("stopped");
		var redCrossImage = $('<img class="state" src="images/kreuz_rot_10.gif" />', {
								  title: "Application not running",
								  alt: "stopped"
							  });
		redCrossImage.prependTo(operationSpanElem);
		
		$(parentElemIdPrefix + "#spanCollectionsCount").text("");
		$(parentElemIdPrefix + "#spanConnection").text("");
		$(parentElemIdPrefix + "#spanHeartbeat").text("");
		$(parentElemIdPrefix + "#spanTelemetryTransmission").text("");
		$(parentElemIdPrefix + "#spanFillLevel").text("");
		
		$('#divSparkplugLicenseCovered').hide();
		if (value.DataProtocolLive == "SparkplugB")
		{
			$('#divSparkplugLicenseCovered').show();
		}

//		$('#'  + thePageContent.id + '_content' + ' .sparkplug_activated').hide();
	}

    var DisplayLiveStatus = function(paramStatus, paramValue, outputElementId)
    {
        //console.log('' + outputElementId + ' ' + paramValue + ' ' + paramStatus);
        try
        {
            value = jQuery.parseJSON(paramValue);

            var parentElemIdPrefix = "#" + thePageContent.id + "_content ";
            var previosValue = "" + $(parentElemIdPrefix + "#theSignOfLifeHidden").val();

            // We can reliebly compare because javascript timer interval is greater than interval producing the status
            if (previosValue != value.SignOfLife)
            {
                // Sign of life changed, so we know DataAgent is alive
                $(parentElemIdPrefix + "#theSignOfLifeHidden").val(value.SignOfLife);
				
				var greenCheckImage = $('<img class="state" src="images/haken_gruen_10.gif" />');
				var redCrossImage = $('<img class="state" src="images/kreuz_rot_10.gif" />');
				
				
				var operationSpanElem = $(parentElemIdPrefix + "#spanOperation");
				operationSpanElem.empty();
				operationSpanElem.text("running");
				greenCheckImage.clone().prop('title', 'running').prop('alt', 'running').prependTo(operationSpanElem);
				
				if (value.DataProtocolLive == "PlcProgSpecific")
				{
					$('#divDataCollections').hide();
					$('#divTelemetryTransmission').hide();
				}
				else
				{
					$('#divDataCollections').show();
					$(parentElemIdPrefix + "#spanCollectionsCount").text('' + value.CollectionsCountFromPlc);

					$('#divTelemetryTransmission').show();
					var transmissionSpanElem = $(parentElemIdPrefix + "#spanTelemetryTransmission");
					transmissionSpanElem.empty();
					if (value.TelemetryDataTransmission)
					{
						transmissionSpanElem.text("enabled");
						greenCheckImage.clone().prop('title', 'enabled').prop('alt', 'enabled').prependTo(transmissionSpanElem);
					}
					else
					{
						transmissionSpanElem.text("disabled");
						redCrossImage.clone().prop('title', 'disabled').prop('alt', 'disabled').prependTo(transmissionSpanElem);
					}
				}
				
				$(parentElemIdPrefix + "#spanHeartbeat").text('' + value.Heartbeat + ' seconds');

				var connectionSpanElem = $(parentElemIdPrefix + "#spanConnection");
				connectionSpanElem.empty();
                if (value.Connected)
                {
					var labelConnected = "connected";
					if (value.DataProtocolLive == "NormalizedJSON" && value.WagoProtocolVersion != "0")
					{
						labelConnected = labelConnected.concat(' (protocol ', value.WagoProtocolVersion, ')');
                }

					connectionSpanElem.text(labelConnected);
					greenCheckImage.clone().prop('title', labelConnected).prop('alt', labelConnected).prependTo(connectionSpanElem);
                }
                else
                {
					connectionSpanElem.text("disconnected");
                    redCrossImage.clone().prop('title', 'disconnected').prop('alt', 'disconnected').prependTo(connectionSpanElem);
                }


                var fillLevelAsFloat = parseFloat(value.FillLevel);
                var fillLevelRoundedNumber = (Number(fillLevelAsFloat)).toFixed(2);
				$(parentElemIdPrefix + "#spanFillLevel").text('' + fillLevelRoundedNumber + ' % (' + value.OutgoingDataBlocks + ' data blocks)');
				if (value.DataProtocolLive == "SparkplugB")
				{
					$('#divSparkplugLicenseCovered').show();

					var sparkplugLicenseCoveredSpanElem = $(parentElemIdPrefix + "#spanSparkplugLicenseCovered");
					if (value.SparkplugLicenseCovered)
					{
						sparkplugLicenseCoveredSpanElem.text("License existing");
						greenCheckImage.clone().prop('title', 'License existing').prop('alt', 'License existing').prependTo(sparkplugLicenseCoveredSpanElem );
					}
					else
					{
						sparkplugLicenseCoveredSpanElem .text("License not existing");
						redCrossImage.clone().prop('title', 'License not existing').prop('alt', 'License not existing').prependTo(sparkplugLicenseCoveredSpanElem );
					}
				}
				else
				{
					$('#divSparkplugLicenseCovered').hide();
				}
				
				if (theSparkplugLicenseCovered != value.SparkplugLicenseCovered)
				{
					theSparkplugLicenseCovered = value.SparkplugLicenseCovered;
					RefreshVisibility();
				}
            }
            else
            {
                DisplayDataAgentStopped();
            }
        }
        catch(e)
        {
            //console.log('Error while parsing live status of DataAgent from a JSON string!');
            DisplayDataAgentStopped();
        }
    };


    var RefreshStatusDataAgent = function()
    {
        //console.log('DataAgentPageContent.prototype.RefreshStatusDataAgent');
        deviceParams.ReadValueGroup(thePageContent.StatusParamView.list, true, function()
        {
            thePageContent.StatusParamView.ShowValues();
            if(deviceParams.ReadErrorOccured(thePageContent.StatusParamView.list))
            {
                var errorString = deviceParams.CollectedErrorStrings(thePageContent.StatusParamView.list);
                $('body').trigger('event_errorOccured', [ 'Error while reading DataAgent status.', errorString ] );
            };
        });
    }


  /*
   * definitions for display of parameters
   */
  var CreateIEC60870ParamView = (function()
  {
    thePageContent.paramView = new DeviceParamView();

    thePageContent.paramView.Add(
    {
        paramId         : 'dataagent_config_params',
        outputElementId : thePageContent.id + '_content #dom_dataagent_config_params',
        outputFkt       : DisplayConfigParams
    });

    var dataAgentStatusParameter = {
        paramId         : 'dataagent_operation_status',
        outputElementId : thePageContent.id + '_content #dom_dataagent_operation_status',
        outputFkt       : DisplayLiveStatus
    };

    thePageContent.paramView.Add(dataAgentStatusParameter);

    // View instance for status of DataAgent
    thePageContent.StatusParamView = new DeviceParamView();
    thePageContent.StatusParamView.Add(dataAgentStatusParameter);

	//console.log('CreateIEC60870ParamView end');
  })();
 
 
    // Register for create event: It will be fired after user clicked IoT Connectivity the very first time within the navigation pane
    $('#content_area').bind(thePageContent.id + '_create', function(event)
    {
        $( "#selectIotPlatform" ).change(function() {
			var dataProtVal = $("#selectDataProtocol").val();
			if (dataProtVal == "SparkplugB")
			{
			    $('body').trigger('event_errorOccured', [ 'Attention\n The data protocol Sparkplug payload B is only available on 750-821x devices. Please check that the Sparkplug license is added to the device.' ] );
			}
            RefreshVisibility();
        });

		$( "#selectDataProtocol" ).change(function() {
			RefreshVisibility();
			var dataProtVal = $("#selectDataProtocol").val();
			if (dataProtVal  == "SparkplugB")
			{
			    $('body').trigger('event_errorOccured', [ 'Attention\n The data protocol Sparkplug payload B is only available on 750-821x devices. Please check that the Sparkplug license is added to the device.' ] );
			}
		});

		$( "#chkMqttLastWillEnabled" ).change(function() {
			RefreshVisibilityMqttLastWill();
		});

        $("#iec60870_104_form").bind('submit', function(event)
        {
			event.preventDefault();

			var doSubmit = true;
			var varCloudConnectivityEnabled = $('#chkCloudConnectivityEnabled').is(':checked');
			var varCacheMode = $('#selectCacheMode').val()  || "\"\"";
			if (varCloudConnectivityEnabled == false && varCacheMode == "SDCard")
			{
				if(!confirm('Deactivating service will detele all related data from mass storage. Do you want to continue?'))
				{
					doSubmit = false;
				}
			}

			if (doSubmit)
			{
				thePageContent.ChangeSettingsofDataAgent(this);
			}
        });
    });

  
    // Register for refresh event: It will be fired after user clicked IoT Connectivity within the navigation pane
    $('#content_area').bind(thePageContent.id + '_refresh', function(event)
    {
        thePageContent.myIntervalTimerId = window.setInterval(RefreshStatusDataAgent, 31 * 1000);
        thePageContent.Refresh();
    });

    // Register for sleep event: It will be fired after user clicked other pane than IoT Connectivity within the navigation pane
    $('#content_area').bind('event_' + thePageContent.id + '_sleep', function(event)
    {
        window.clearInterval(thePageContent.myIntervalTimerId);
    });

	//console.log('DataAgentPageContent end');
};


  
/*
 * read and show parameters necessary for DataAgent content
 */
DataAgentPageContent.prototype.Refresh = function()
{
  var pageContent = this;

  deviceParams.ReadValueGroup(pageContent.paramView.list, function()
  {
    pageContent.paramView.ShowValues();

    if(deviceParams.ReadErrorOccured(pageContent.paramView.list)) 
    {
      var errorString = deviceParams.CollectedErrorStrings(pageContent.paramView.list);
      $('body').trigger('event_errorOccured', [ 'Error while reading DataAgent settings.', errorString ] );
    };
  });
  
  //console.log('DataAgentPageContent.prototype.Refresh end');
};


/*
 * functions for changing DataAgent configuration
 */
DataAgentPageContent.prototype.ChangeSettingsofDataAgent = function(formObj)
{
	pageElements.ShowBusyScreen("Changing DataAgent settings ...");

	// get script parameter from form data - given, but empty strings must be converted to "" to let the script
	// know, that this parameter value should be deleted.
	var varCloudConnectivityEnabled = $(formObj).find('#chkCloudConnectivityEnabled').is(':checked');
	var platform = $(formObj).find('#selectIotPlatform').val()  || "\"\"";
	var varHostName = $(formObj).find('input#inputHostName').val()  || "\"\"";
	var varDeviceId = $(formObj).find('input#inputDeviceId').val()  || "\"\"";
	var varSharedAccessKey = $(formObj).find('input#inputSharedAccessKey').val()  || "\"\"";

	var varMqttHost = $(formObj).find('#inputMqttHostName').val()  || "\"\"";
	var varMqttUser = $(formObj).find('#inputMqttUserName').val()  || "\"\"";
	var varMqttPassword = $(formObj).find('#inputMqttUserPassword').val()  || "\"\"";
	var varGroupId = $(formObj).find('input#inputMqttGroupId').val()  || "\"\"";
	var varMqttClientId = $(formObj).find('#inputMqttClientId').val()  || "\"\"";
	var varMqttDoCleanSession = $(formObj).find('#chkMqttDoCleanSession').is(':checked')  || "\"\"";
	var varMqttSecurityEnabled = $(formObj).find('#chkMqttSecurityEnabled').is(':checked')  || "\"\"";
	var varMqttPort = $(formObj).find('#inputMqttPort').val()  || "\"\"";
	var varMqttCaFile = $(formObj).find('#inputMqttCaFile').val()  || "\"\"";
	var varMqttCertFile = $(formObj).find('#inputMqttCertFile').val()  || "\"\"";
	var varMqttKeyFile = $(formObj).find('#inputMqttKeyFile').val()  || "\"\"";

	var varDataProtocol = $(formObj).find('#selectDataProtocol').val()  || "\"\"";
	var varCacheMode = $(formObj).find('#selectCacheMode').val()  || "\"\"";
	var varMqttLastWillEnabled = $(formObj).find('#chkMqttLastWillEnabled').is(':checked');
	var varLastWillTopic = $(formObj).find('#inputMqttLastWillTopic').val() || "\"\"";
	var varLastWillPayload = $(formObj).find('#inputMqttLastWillPayload').val() || "\"\"";
	var varLastWillQos = $(formObj).find('#selectMqttLastWillQoS').val() || "\"\"";
	var varLastWillRetain = $(formObj).find('#chkMqttLastWillRetain').is(':checked');

	var varDeviceInfoEnabled = $(formObj).find('#chkDeviceInfoEnabled').is(':checked');
	var varDeviceStatusEnabled = $(formObj).find('#chkDeviceStatusEnabled').is(':checked');
	var varStandardCommandsEnabled = $(formObj).find('#chkStandardCommandsEnabled').is(':checked');
	
	var newValueList  =
			{
		theNewIoTPlatform: platform,
		theNewHostName: varHostName,
		theNewDeviceId: varDeviceId,
		theNewSharedAccessKey: varSharedAccessKey,
		theMqttHost: varMqttHost,
		theMqttUser: varMqttUser,
		theMqttPassword: varMqttPassword,
		theMqttClientId: varMqttClientId,
		theMqttTLSOn: varMqttSecurityEnabled,
		theMqttPort: varMqttPort,
		theMqttCaFile: varMqttCaFile,
		theMqttCertFile: varMqttCertFile,
		theMqttKeyFile: varMqttKeyFile,
		theCacheMode: varCacheMode,
		theDeviceInfoEnabled: varDeviceInfoEnabled,
		theDeviceStatusEnabled: varDeviceStatusEnabled,
		theStandardCommandsEnabled: varStandardCommandsEnabled,
		theCloudConnectivityEnabled: varCloudConnectivityEnabled,
		theGroupId: varGroupId,
		theLastWillEnabled: varMqttLastWillEnabled,
		theLastWillTopic: varLastWillTopic,
		theLastWillPayload: varLastWillPayload,
		theLastWillQos: varLastWillQos,
		theLastWillRetain: varLastWillRetain,
		theDataProtocol: varDataProtocol,
		theMqttCleanSession: varMqttDoCleanSession
	};

	deviceParams.ChangeValue('iec60870_', newValueList, function(status, errorText)
	{
		pageElements.RemoveBusyScreen();
		if(SUCCESS != status)
		{
			$('body').trigger('event_errorOccured', [ 'Error while changing DataAgent settings.', errorText ]);
		}
		else
		{
			$('#rebootHint').css('color','orangered').css('font-weight', 'bold');
		}
	});
};

