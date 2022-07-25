<?php include("page_element_general_content.inc.php");

/*
 * Copyright (c) 2018-2022 WAGO GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

?>

    <h2>Open Source Licenses</h2>

<?php

  $fileContent = file_get_contents('../licenses/oss.txt');
  print("<div class=\"licence-block\">".$fileContent."</div>");

  $ossFiles = array_merge(
    glob('../licenses/oss/*.txt'),
    glob('../licenses/oss/generic/*.txt')
  );

  foreach ($ossFiles as $filePath) {
    $fileContent = htmlspecialchars(file_get_contents($filePath));
    print("<br>");
    $fileName = pathinfo($filePath, PATHINFO_FILENAME);
    $fileNameParts = explode('.', $fileName, 2);
    $fileName = str_replace('_', ' ', $fileName);
    $fileName = str_replace('license.', '', $fileName);
    $fileName = str_replace('license-extension.', '', $fileName);

    print("<h3>".$fileName."</h3>");
    print("<div class=\"licence-block\">".$fileContent."</div>");
  }

 ?>
