<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

<h2>Configuration of Cloud Connectivity</h2>
<p id="rebootHint">Changes will take effect after next reboot.</p>
<input type="hidden" name="signOfLifeHidden" id="theSignOfLifeHidden" value="" />
<input type="hidden" name="theMassStorageIsReady" id="theMassStorageIsReady" value="" />
<form id="iec60870_104_form" action="javascript_requested.php" method="POST">

<div class="sparkplug_activated">
  <p>Note: The data protocol Sparkplug payload B is only available on 750-821x and Touch Panel 600 devices. Please check that the Sparkplug license
  is added to the device.</p>
</div>

<h3>Software version</h3>
<div class="config_form_box">
  <div class="config_data_list"
  <div class="label_value_pair">
    <div class="label_field"><label for="attention">1.7</label></div>
  </div>
</div>

<h3>Status</h3>
<div class="config_form_box">
  <div class="config_data_list">
    <div class="label_value_pair">
	  <div class="label_field">Service operation:</div>
	  <div class="value_field"><span id="spanOperation">updating... please wait</span></div>
	</div>
	<div id="divDataCollections" class="label_value_pair">
	  <div class="label_field">Data from PLC runtime:</div>
	  <div class="value_field"><span id="spanCollectionsCount">updating... please wait</span></div>
	</div>
	<div class="label_value_pair">
	<div class="label_field">Cloud connection:</div>
	<div class="value_field"><span id="spanConnection">updating... please wait</span></div>
	</div>
	<div class="label_value_pair">
	<div class="label_field">Heartbeat:</div>
	<div class="value_field"><span id="spanHeartbeat">updating... please wait</span></div>
	</div>
	<div id="divTelemetryTransmission" class="label_value_pair">
	<div class="label_field">Telemetry data transmission:</div>
	<div class="value_field"><span id="spanTelemetryTransmission">updating... please wait</span></div>
	</div>
	<div class="label_value_pair">
	<div class="label_field">Cache fill level (QoS 1 and 2):</div>
	<div class="value_field"><span id="spanFillLevel">updating... please wait</span></div>
	</div>
	<div id="divSparkplugLicenseCovered" class="label_value_pair">
	<div class="label_field">Sparkplug License Covered:</div>
	<div class="value_field"><span id="spanSparkplugLicenseCovered">updating... please wait</span></div>
	</div>
  </div>
</div>

    <h3>Settings</h3>
	<div class="config_form_box">

      <div class="config_data_list">
	    <div class="label_value_pair">
		  <div class="label_field"><label for="chkCloudConnectivityEnabled">Service enabled:</label></div>
		  <div class="value_field"><input id="chkCloudConnectivityEnabled" type="checkbox" name="chkCloudConnectivityEnabled" value="disabled" ></div>
		</div>
		</div>
	<div class="config_data_list">
	<div class="label_value_pair">
	  <div class="label_field"><label for="selectIotPlatform">Cloud platform:</label></div>
	  <div class="value_field">
	    <select id="selectIotPlatform" class="wide" name="selectIotPlatform" size="1">
		<option value="ECloud">Wago Cloud</option>
		<option value="AzureCloud">Azure</option>
		<option value="MQTT">MQTT AnyCloud</option>
		<option value="IBM">IBM Cloud</option>
		<option value="AWS">Amazon Web Services (AWS)</option>
		<option value="SAP">SAP IoT Services</option>
		</select>
		</div>
	</div>
	</div>

    <div class="config_data_list" id="AzureIoTconfigUI" style="display: none;">
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputHostName">Hostname:</label></div>
	  <div class="value_field"><input id="inputHostName" class="input_field" type="text" name="inputHostName" size="50" maxlength="250" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputDeviceId">Device ID:</label></div>
	  <div class="value_field"><input id="inputDeviceId" class="input_field" type="text" name="DeviceId" size="50" maxlength="250" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputSharedAccessKey">Activation Key:</label></div>
	  <div class="value_field"><input id="inputSharedAccessKey" class="input_field" type="text" name="inputSharedAccessKey" size="50" maxlength="250" value="" ></div>
	</div>
	</div>

    <div class="config_data_list" id="mqttIoTconfigUI" style="display: none;">
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttHostName">Hostname:</label></div>
	  <div class="value_field"><input id="inputMqttHostName" class="input_field" type="text" name="inputMqttHostName" size="50" maxlength="250" value="" ></div>
	</div>
	            <div class="label_value_pair" id="divNodeGroupId">
				  <div class="label_field"><label for="inputMqttGroupId">Group ID:</label></div>
				  <div class="value_field"><input id="inputMqttGroupId" class="input_field" type="text" name="inputMqttGroupId" size="50" maxlength="300" value="" ></div>
				</div>
				<div class="label_value_pair">
	              <div class="label_field"><label for="inputMqttClientId" id="labelDeviceOrEdgeNodeId">Device ID:</label></div>
				<div class="value_field"><input id="inputMqttClientId" class="input_field" type="text" name="inputMqttClientId" size="50" maxlength="300" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="chkMqttDoCleanSession">Clean session:</label></div>
	  <div class="value_field"><input id="chkMqttDoCleanSession" type="checkbox" name="chkMqttDoCleanSession"></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="chkMqttSecurityEnabled">TLS:</label></div>
	  <div class="value_field"><input id="chkMqttSecurityEnabled" type="checkbox" name="chkMqttSecurityEnabled" size="50" maxlength="300" value="enabled" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttPort">Port:</label></div>
	  <div class="value_field"><input id="inputMqttPort" class="input_field" type="text" name="inputMqttPort" size="50" maxlength="300" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttCaFile">CA file:</label></div>
	  <div class="value_field"><input id="inputMqttCaFile" class="input_field" type="text" size="50" maxlength="250" name="inputMqttCaFile" value="" ></div>
	</div>
	</div>

    <div class="config_data_list" id="IBMConfigUI" style="display: none;">
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttUserName">User:</label></div>
	  <div class="value_field"><input id="inputMqttUserName" class="input_field" type="text" name="inputMqttUserName" size="50" maxlength="250" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttUserPassword">Password:</label></div>
	  <div class="value_field"><input id="inputMqttUserPassword" class="input_field" type="password" name="inputMqttUserPassword" size="50" maxlength="250" value="" ></div>
	</div>
	</div>

    <div class="config_data_list" id="AWSConfigUI" style="display: none;">
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttCertFile">Certification file:</label></div>
	  <div class="value_field"><input id="inputMqttCertFile" class="input_field" type="text" size="50" maxlength="250" name="inputMqttCertFile" value="" ></div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="inputMqttKeyFile">Key file:</label></div>
	  <div class="value_field"><input id="inputMqttKeyFile" class="input_field" type="text" size="50" maxlength="250" name="inputMqttKeyFile" value="" ></div>
	</div>
	</div>

    <div class="config_data_list" id="WebswocketsConfigUI" style="display: none;">
	  <div class="label_value_pair">
	    <div class="label_field"><label for="chkWebsocketsEnabled">Use websockets:</label></div>
		<div class="value_field"><input id="chkWebsocketsEnabled" type="checkbox" name="chkWebsocketsEnabled" value="disabled" ></div>
	  </div>
	</div>
	
    <div class="config_data_list" id="CompressionConfigUI" style="display: none;">
	  <div class="label_value_pair">
	    <div class="label_field"><label for="chkCompressionEnabled">Use compression:</label></div>
		<div class="value_field"><input id="chkCompressionEnabled" type="checkbox" name="chkCompressionEnabled" value="disabled" ></div>
		</div>
	</div>

    <div class="config_data_list" id="divApplicationBehavior">
	<div class="label_value_pair" id="divDataProtocol" style="display: none;">
	  <div class="label_field"><label for="selectDataProtocol">Data protocol:</label></div>
	  <div class="value_field">
	    <select name="selectDataProtocol" id="selectDataProtocol" class="wide" size="1">
		  <option value="NormalizedJSON">Wago Protocol</option>
		  <option value="PlcProgSpecific">Native MQTT</option>
		  <option value="SparkplugB">Sparkplug payload B</option>
		</select>
		</div>
	</div>
	<div class="label_value_pair">
	  <div class="label_field"><label for="selectCacheMode">Cache mode:</label></div>
	  <div class="value_field">
	    <select id="selectCacheMode" class="wide" name="selectCacheMode" size="1">
		  <option value="RAM">RAM (volatile)</option>
		  <option value="SDCard" >SD Card (non-volatile)</option>
		</select>
		</div>
	</div>
	<div class="label_value_pair" id="divLastWillEnabled">
	  <div class="label_field"><label for="chkMqttLastWillEnabled">Last Will:</label></div>
	  <div class="value_field"><input id="chkMqttLastWillEnabled" type="checkbox" name="chkMqttLastWillEnabled" value="disabled" ></div>
	</div>
	<div class="label_value_pair" id="divLastWillTopic" style="display: none;">
	  <div class="label_field"><label for="inputMqttLastWillTopic">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Topic:</label></div>
	  <div class="value_field"><input name="inputMqttLastWillTopic" id="inputMqttLastWillTopic" class="input_field" type="text" size="50" maxlength="250" value=""></div>
	</div>
	<div class="label_value_pair" id="divLastWillPayload" style="display: none;">
	  <div class="label_field"><label for="inputMqttLastWillPayload">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Message:</label></div>
	  <div class="value_field"><input name="inputMqttLastWillPayload" id="inputMqttLastWillPayload" class="input_field" type="text" size="50" maxlength="250" value=""></div>
	</div>
	<div class="label_value_pair" id="divLastWillQoS" style="display: none;">
	  <div class="label_field"><label for="selectMqttLastWillQoS">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;QoS:</label></div>
	  <div class="value_field">
	    <select name="selectMqttLastWillQoS" id="selectMqttLastWillQoS" class="wide" size="1">
		  <option value="0">0 - At most once</option>
		  <option value="1">1 - At least once</option>
		  <option value="2">2 - Exactly once</option>
		</select>
		</div>
	</div>
	<div class="label_value_pair" id="divLastWillRetain" style="display: none;">
	  <div class="label_field"><label for="chkMqttLastWillRetain">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Retain:</label></div>
	  <div class="value_field"><input id="chkMqttLastWillRetain" type="checkbox" name="chkMqttLastWillRetain" value="disabled" ></div>
	</div>
	  </div>

      <div class="config_data_list" id="DeviceConfigUI" style="display: none;">
	    <div class="label_value_pair">
		  <div class="label_field"><label for="chkDeviceInfoEnabled">Device info:</label></div>
		  <div class="value_field"><input id="chkDeviceInfoEnabled" type="checkbox" name="chkDeviceInfoEnabled" value="enabled" ></div>
		</div>
		<div class="label_value_pair">
		  <div class="label_field"><label for="chkDeviceStatusEnabled">Device status:</label></div>
		  <div class="value_field"><input id="chkDeviceStatusEnabled" type="checkbox" name="chkDeviceStatusEnabled" value="enabled" ></div>
		</div>
	  </div>

    <div class="config_data_list" id="CommandsConfigUI" style="display: none;">
	<div class="label_value_pair">
	  <div class="label_field"><label for="chkStandardCommandsEnabled">Standard commands:</label></div>
	  <div class="value_field"><input id="chkStandardCommandsEnabled" type="checkbox" name="chkStandardCommandsEnabled" value="enabled" ></div>
	</div>
	</div>

      <p class="button_area">
	    <input class="button" type="SUBMIT" value="Submit" name="Submit">
		</p>

    </div>
</form>

<?php include("page_element_general_content.inc.php"); ?>
