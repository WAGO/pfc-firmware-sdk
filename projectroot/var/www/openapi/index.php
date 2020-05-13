<!DOCTYPE html>
<html>
  <head>
    <title>WDX REST-API</title>
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="swagger-ui.css">
  </head>
  <body>

    <select id="config" onchange="refresh(this.value)">

<?php
// insert filenames of OpenAPI specs found in this directory (JSON only).
foreach( glob('./*.json') as $json_file) {

    $json_content = json_decode( file_get_contents($json_file) );
    $title = $json_content->info->title;
    $version = $json_content->info->version;

    echo "<option value=\"$json_file\">$title - $version</option>";
}

?>
    </select>
    
    <div id="swagger-ui"></div>
    <script src="swagger-ui-bundle.js"> </script>
    <script>
        function refresh(config) {
          const ui = SwaggerUIBundle({
            url: config,
            "urls.primaryName": "0.1.0",
            dom_id: "#swagger-ui",
            presets: [
              SwaggerUIBundle.presets.apis,
            ],
            plugins: [
              SwaggerUIBundle.plugins.DownloadUrl
            ]
          });
          window.ui = ui;
        }
        refresh(document.querySelector('select > option').value);
    </script>
  </body>
</html>