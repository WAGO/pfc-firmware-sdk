<?php include("page_element_general_content.inc.php"); ?>

<h2>OPKG</h2>
  <form id="refresh_opkg_list" action="javascript_requested.php" method="POST">
    <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="value_field">
              <label for="opkg_sel">List Opks:</label>
        		  <select id="opkg_sel" class="input_field" name="opkg_sel">
        			  <option value="0">All</option>
        			  <option value="1">Installed</option>
				  <option value="2">User-Installed</option>
				  <option value="3">Newer-Versions</option>
				  <option value="4">Not installed</option>
        		  </select>
            </div>
          </div>
        </div>
        <p class="button_area">
            <input class="button" type="submit" value="Refresh" name="refresh">
	    <input class="button" type="button" value="Info" name="pkgcontrol">
        </p>
      </div>
  </form>
  <div id="opkg_list">
    <table id="opkg_list_table"></table>
  </div>

