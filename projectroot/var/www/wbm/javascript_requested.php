<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

include("wbm_lib.inc.php");

?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>WAGO Ethernet Web-Based Management</title>
<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1">
<meta name="author" content="WAGO Kontakttechnik GmbH & Co. KG">

<link rel="stylesheet" type="text/css" href="css/styles.css">


<!------------------------------------------------------------
< JAVA SCRIPT FUNCTIONS
<------------------------------------------------------------->
<script type="text/javascript">

</script>

</head>

<!------------------------------------------------------------
< HTML BODY
<------------------------------------------------------------->

<body>

<div id="page">

  <!-------------------------------------------------------------------
  < HEAD AREA
  < ------------------------------------------------------------------->
  <?php WBM_PrintHeadArea(); ?>


  <!-------------------------------------------------------------------
  < CONTENT AREA
  < ------------------------------------------------------------------->
  <div id="content_area">

    <p>Wago Web Based Management requires JavaScript.</p>
    <p>Please enable JavaScript functionality in your browser and
    <a href="../index.html">load start page</a>.</p>

  </div>
</div>

<?php system("cat footer.html"); ?>

</body>
</html>