/**
 * @file builtinfiles.h
 * @brief This file is part of the WebServer example for the ESP8266WebServer.
 *
 * This file contains long, multiline text variables for  all builtin resources.
 */

// used for $upload.htm
static const char index_html[] PROGMEM =
  R"==(
<!doctype html>
<html lang='en'>

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Panomath</title>
</head>

<body style="width:300px">
  <h1>Panomath</h1>
  <nav>
    <ul>
      <li>Compteurs</li>
    </ul>
  </nav>
  <hr>
  <article id='panAffichage' url='affichage'>
    <fieldset>
      <legend>Modifiez ici les compteurs</legend>
      <label for="compteur1"></label>
      <input id="compteur1" type="text" />
      <label for="compteur2"></label>
      <input id="compteur2" type="text" />
      <div>
        <button type="button" method="get">Rafraichir</button>
        <button type="button" method="post">Enregistrer</button>
      </div>
    </fieldset>
  </article>

  <article id="panSon" url="son">
    <fieldset>
      <legend>Choisisser et testez ici le son</legend>
      <div>
        <button type="button" method="get">Rafraichir</button>
        <button type="button" method="post">Enregistrer</button>
      </div>
    </fieldset>
  </article>

  <article id="panFS">
    <fieldset>
      <legend>Afficher les fichiers</legend>
      <div id="files">
      </div>
      <div>
        <button type="button">Enregistrer</button>
      </div>
    </fieldset>
  </article>

  <script>
  </script>
</body>
)==";



// used for $upload.htm
static const char uploadContent[] PROGMEM =
  R"==(
<!doctype html>
<html lang='en'>

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Upload</title>
</head>

<body style="width:300px">
  <h1>Upload</h1>
  <div><a href="/">Home</a></div>
  <hr>
  <div id='zone' style='width:16em;height:12em;padding:10px;background-color:#ddd'>Drop files here...</div>

  <script>
    // allow drag&drop of file objects
    function dragHelper(e) {
      e.stopPropagation();
      e.preventDefault();
    }

    // allow drag&drop of file objects
    function dropped(e) {
      dragHelper(e);
      var fls = e.dataTransfer.files;
      var formData = new FormData();
      for (var i = 0; i < fls.length; i++) {
        formData.append('file', fls[i], '/' + fls[i].name);
      }
      fetch('/', { method: 'POST', body: formData }).then(function () {
        window.alert('done.');
      });
    }
    var z = document.getElementById('zone');
    z.addEventListener('dragenter', dragHelper, false);
    z.addEventListener('dragover', dragHelper, false);
    z.addEventListener('drop', dropped, false);
  </script>
</body>
)==";

// used for $upload.htm
static const char notFoundContent[] PROGMEM = R"==(
<html>
<head>
  <title>Resource not found</title>
</head>
<body>
  <p>The resource was not found.</p>
  <p><a href="/">Start again</a></p>
</body>
)==";
