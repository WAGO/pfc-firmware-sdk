<!DOCTYPE html>
<html>
  <head>
    <title>WAGO Device Access</title>
    <!-- needed for adaptive design -->
    <meta charset="utf-8"/>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <!-- custom style for the nav bar -->
    <style>
      body {
        margin: 0;
        padding-top: 40px;
      }

      nav {
        position: fixed;
        top: 0;
        width: 100%;
        z-index: 100;
      }
      #links_container {
          margin: 0;
          padding: 0;
          background-color: #0033a0;
      }

      #links_container li {
          display: inline-block;
          padding: 10px;
          color: white;
          cursor: pointer;
      }
    </style>
  </head>
  <body>
    <nav>
      <ul id="links_container">
      </ul>
    </nav>
    <redoc scroll-y-offset="body > nav">
    </redoc>
    <script src="redoc.standalone.js"> </script>

    <script>
      // list of APIS
      var apis = 
<?php
      // insert filenames of OpenAPI specs found in this directory (JSON only).
      $apis = array();
      foreach( glob('./*.json') as $json_file) {

          $json_content = json_decode( file_get_contents($json_file) );
          $title = $json_content->info->title;
          $version = $json_content->info->version;
          array_push($apis, array(
            "name" => $title." ".$version,
            "url"  => "$json_file"
          ));
          
      }
      echo json_encode($apis);
?>
      ;

      // see theming options here: https://redoc.ly/docs/api-reference-docs/configuration/theming/
      let theme = {
        colors: {
          primary: {
            main: "#000000"
          }
        }
      }

      // initially render first API
      Redoc.init(apis[0].url, {
          theme: theme
        });

      function onClick() {
        var url = this.getAttribute('data-link');
        Redoc.init(url, {
          theme: theme
        });
      }

      // dynamically building navigation items
      var navigationBar = document.getElementById('links_container');
      apis.forEach(function(api) {
        var listItem = document.createElement('li');
        listItem.setAttribute('data-link', api.url);
        listItem.innerText = api.name;
        listItem.addEventListener('click', onClick);
        navigationBar.appendChild(listItem);
      });
    </script>
  </body>
</html>