<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>WAGO Licenses</h2>

<?php

  $fileContent = file_get_contents('../licenses/wago.txt');
  print("<div class=\"licence-block\">".$fileContent."</div>");

 ?>
