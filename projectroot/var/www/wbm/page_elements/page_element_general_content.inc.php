<?php

/*
 * Copyright (c) 2018 WAGO Kontakttechnik GmbH & Co. KG
 *
 * PROPRIETARY RIGHTS are involved in the subject matter of this material.
 */

// content that must be included in every wbm page element


// write requesting user to html - use css inline style, in order that information is invisible anyway
/*
 * not needed, since we use PHP login
if(isset($_SERVER["REMOTE_USER"]))
{
  printf("\n      <div class=\"remote_user\" style=\"display: none\">%s</div>\n", $_SERVER["REMOTE_USER"]);
}
*/

require_once('../page_auth.inc.php');

?>