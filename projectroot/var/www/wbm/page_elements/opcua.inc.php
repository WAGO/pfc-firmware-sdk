<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

<h2>OPC UA</h2>

<p>Changes will take effect immediately.
<br><br>
Note: If changes are made, the OPC UA server is restarted after pressing any of the Submit buttons. Existing connections will be lost.</p>

<h3>OPC UA Server</h3>
<div id="opcua_info" class="config_form_box">
  <div class="config_data_list" >
    <div class="label_value_pair">
      <div class="label_field">Version:</div>
      <div class="value_field"><span id="opcua_version"></span></div>
    </div>
  </div>
</div>


<h3>General OPC UA Server Configuration</h3>
<form id="config_opcua" action="javascript_requested.php" method="POST">
  <div class="config_form_box">
    <div class="config_data_list" >
      <div class="label_value_pair">
        <div class="label_field"><label for="opcua_state">Server enabled:</label></div>
        <div class="value_field"><input id="opcua_state" type="checkbox" name="opcua_state" ></div>
      </div>
      <div class="label_value_pair">
        <div class="label_field"><label for="opcua_port">Port number:</label></div>
        <div class="value_field"><input id="opcua_port" type="text" name="opcua_port" ></div>
      </div>
      <div class="label_value_pair">
        <div class="label_field"><label for="opcua_log_level">Log level:</label></div>
        <div class="value_field"><select id="opcua_log_level" name="opcua_log_level">
          <option value="info">info</option>
          <option value="debug">debug</option>
          <option value="warning">warning</option>
          <option value="error">error</option>
        </select></div>
      </div>
      <div class="label_value_pair">
        <div class="label_field"><label for="opcua_anonymous_access">Unlimited anonymous access:</label></div>
        <div class="value_field"><input id="opcua_anonymous_access" type="checkbox" name="opcua_anonymous_access" ></div>
      </div>
    </div>
    <p class="button_area">
      <input class="button" type="submit" value="Submit" name="submit">
    </p>
  </div>
</form>


<h3>OPC UA Security Settings</h3>
<form id="config_opcua_trust_all_clients" action="javascript_requested.php" method="POST">
  <div class="config_form_box group">
    <div class="config_data_list" >
      <div class="label_value_pair">
        <div class="label_field"><label for="opcua_trust_all_clients">Trust all clients:</label></div>
        <div class="value_field"><input id="opcua_trust_all_clients" type="checkbox" name="opcua_trust_all_clients" ></div>
      </div>
    </div>
    <p class="button_area">
      <input class="button" type="submit" value="Submit" name="submit">
    </p>
  </div>
</form><form id="config_opcua_security" action="javascript_requested.php" method="POST">
  <div class="config_form_box">
    <div class="config_data_list" >
      <?php
        $arr = array(
          'opcua_application_uri_check' => 'Application URI Check',
          'opcua_user_token_policy_id_check' => 'User Token Policy ID Check',
          'opcua_nonce_length_check' => 'Nonce Length Check',
          'opcua_error_cert_time' => 'Error Certificate Time',
          'opcua_cert_issuer_time_invalid' => 'Certificate Issuer Time Invalid',
          'opcua_cert_revocation_unknown' => 'Certificate Revocation Unknown',
          'opcua_certificate_issuer_revocation_unknown' => 'Certificate Issuer Revocation Unknown'
        );
        foreach ($arr as $value => $label) { ?>
          <div class="label_value_pair">
            <div class="label_field"><label for="<?= $value ?>"><?= $label ?>:</label></div>
            <div class="value_field"><input id="<?= $value ?>" type="checkbox" name="<?= $value ?>" ></div>
          </div>
        <?php }
      ?>
    </div>
    <p class="button_area">
      <input class="button" type="submit" value="Submit" name="submit">
    </p>
  </div>
</form>
