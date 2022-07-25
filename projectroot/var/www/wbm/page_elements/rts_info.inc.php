<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>PLC Runtime Information</h2>
    
    <p>The PLC runtime ports can be configured on the "Ports and Services - PLC Runtime Services" page.</p>

    <div id="codesys_info_version_2_area">
    
      <h3>PLC Runtime</h3>
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Version:</div>
            <div class="value_field"><span id="codesys_info_version_2"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Webserver Version:</div>
            <div class="value_field"><span id="codesys_webserver_version"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">State:</div>
            <div class="value_field"><span id="rts_state"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Number of Tasks:</div>
            <div class="value_field"><span id="number_of_tasks"></span></div>
          </div>
        </div>
      </div>


      <h3>Project Details</h3>
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Date:</div>
            <div class="value_field"><span id="project_date"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Title:</div>
            <div class="value_field"><span id="project_title"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Version:</div>
            <div class="value_field"><div class="rts_project_version"><span id="project_version"></span></div></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Author:</div>
            <div class="value_field"><span id="project_author"></span></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Description:</div>
            <div class="value_field"><div id="project_description" class="rts_project_description"></div></div>
          </div>
          <div class="label_value_pair">
            <div class="label_field">Checksum:</div>
            <div class="value_field"><span id="project_checksum"></span></div>
          </div>
        </div>
      </div>
    
      <div id="rts_info_no_task_area">
        <h3>Task Information</h3>
        <div class="config_form_box" >
          <div class="config_data_list">
            <div class="value_field"><span id="rts_info_no_task_info_text">Unknown</span></div>
          </div>
        </div>
      </div>
    
    
    <?php
    $maxTaskCount = 15;
    $taskNo       = 0;

    // construct static html sceleton for task output for every possible task 
    for($taskNo = 0; $taskNo < $maxTaskCount; ++$taskNo)
    {
      printf("\n    <div class=\"rts_task_table\" id=\"rts-task-table-%d\" >\n", $taskNo);
      printf("        <h3>\n");
      printf("          <div class=\"accordeon_control\">\n");
      printf("            <a id=\"toggle_task_info\" title=\"Show task informations\" href=\"toggle_task_info_%d\">\n", $taskNo);
      printf("              <img alt=\"open\" src=\"images/plus_green_18x18.png\">\n");
      printf("            </a>\n");
      printf("          </div>\n");
      printf("          Task %d: <span id=\"task_name_%d\"></span>, ID: <span id=\"task_id_%d\"></span>\n", $taskNo, $taskNo, $taskNo);
      printf("        </h3>\n");
      printf("        <div class=\"config_form_box initial_invisible\" >\n");
      printf("          <div class=\"config_data_list\">\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Cycle count:</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_cycle_count_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Cycle time (usec):</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_cycletime_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Cycle time min (usec):</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_cycletime_min_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Cycle time max (usec):</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_cycletime_max_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Cycle time avg (usec):</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_cycleletime_acc_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">State:</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_status_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Mode:</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_mode_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Priority:</div>\n");
      printf("              <div class=\"value_field\"><span id=\"task_priority_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("            <div class=\"label_value_pair\">\n");
      printf("              <div class=\"label_field\">Interval (<span id=\"task_interval_unit_%d\"></span>):</div>\n", $taskNo);
      printf("              <div class=\"value_field\"><span id=\"task_interval_%d\"></span></div>\n", $taskNo);
      printf("            </div>\n");
      printf("          </div>\n");
      printf("        </div>\n");
      printf("      </div>\n");
    }
    ?>
    
    </div>
    
    <div id="codesys_info_version_3_area" >
      <h3>PLC Runtime</h3>
      <div class="config_form_box" >
        <div class="config_data_list">
          <div class="label_value_pair">
            <div class="label_field">Version:</div>
            <div class="value_field"><span id="codesys_info_version_3"></span></div>
          </div>
        </div>
      </div>
    </div>

    