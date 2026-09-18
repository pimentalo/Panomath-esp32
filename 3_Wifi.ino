/**
  Largely inspired from https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/WebServer/

  Endpoints:
  - / -> /index.html
  - /index.html: home page
  - /status: 
  - /lcd: 
  - /sound:
  - /fs: GET: list FS files
  - /fs/[name] GET: Download file
  - /fs/[name] PUT: Upload file
*/
#include <Arduino.h>
#include <WebServer.h>
#include <WiFi.h>
#include "HtmlPages.h"

const char *ssid = "PanoMath";
const char *passPhrase = "PI=3.14159";

// name of the server. You reach it using http://webserver
#define HOSTNAME "panomath"

// local time zone definition (Berlin)
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// need a WebServer for http access on port 80.
WebServer server(80);

// ===== Request Handler class used to answer more complex requests =====


// mark parameters not used in example
#define UNUSED __attribute__((unused))



WifiServer::WifiServer()
{

}


void WifiServer::setWaveHeader(const char* data, int length) 
{

}  
void WifiServer::setWavedata(const char* data, int length) {

}

// enable the CUSTOM_ETAG_CALC to enable calculation of ETags by a custom function
#define CUSTOM_ETAG_CALC

// ===== Simple functions used to answer simple GET requests =====

// This function is called when the WebServer was requested without giving a filename.
// This will redirect to the file index.htm when it is existing otherwise to the built-in $upload.htm page
void handleRedirect() {
  TRACE("Redirect...\n");
  String url = "/index.htm";

/*  if (!fsys->exists(url)) {
    url = "/$upload.htm";
  }*/

  server.sendHeader("Location", url, true);
  server.send(302);
}  // handleRedirect()


void handleGetWavedata() {
  TRACE("WAVEDATA...\n");

  byte result [64000];
  int length = 64000;
  MemoryStream stream(result);
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "application/octet-stream", stream, length);
}

// This function is called when the sysInfo service was requested.
void handleSysInfo() {
  String result;

  result += "{\n";
  result += "  \"Chip Model\": " + String(ESP.getChipModel()) + ",\n";
  result += "  \"Chip Cores\": " + String(ESP.getChipCores()) + ",\n";
  result += "  \"Chip Revision\": " + String(ESP.getChipRevision()) + ",\n";
  result += "  \"flashSize\": " + String(ESP.getFlashChipSize()) + ",\n";
  result += "  \"freeHeap\": " + String(ESP.getFreeHeap()) + ",\n";

/*  if (fsys == (fs::FS *)&FFat) {
    result += "  \"fsTotalBytes\": " + String(FFat.totalBytes()) + ",\n";
    result += "  \"fsUsedBytes\": " + String(FFat.usedBytes()) + ",\n";
  } else {
    result += "  \"fsTotalBytes\": " + String(LittleFS.totalBytes()) + ",\n";
    result += "  \"fsUsedBytes\": " + String(LittleFS.usedBytes()) + ",\n";
  }*/

  result += "}";

  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}  // handleSysInfo()

void WifiServer::setup()
{
  // allow to address the device by the given name e.g. http://webserver
  WiFi.setHostname(HOSTNAME);

  // start WiFI
  WiFi.softAP(ssid, passPhrase);
  IPAddress IP = WiFi.softAPIP();

  TRACE("Wifi démarré %s", IP);
  
  // Initialise le FS
  this->fileHandler = new FileServerHandler(server);

  // // Ask for the current time using NTP request builtin into ESP firmware.
  //TRACE("Setup ntp...\n");
  //configTzTime(TIMEZONE, "pool.ntp.org");

  TRACE("Register redirect...\n");

  // register a redirect handler when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);
  server.on("/wavedata", HTTP_GET, handleGetWavedata);

  TRACE("Register service handlers...\n");

  
  // enable CORS header in webserver results
  server.enableCORS(true);

  // enable ETAG header in webserver results (used by serveStatic handler)
#if defined(CUSTOM_ETAG_CALC)
  // This is a fast custom eTag generator. It returns a value based on the time the file was updated like
  // ETag: 63bbceb5
  server.enableETag(true, [](FS &fs, const String &path) -> String {
    File f = fs.open(path, "r");
    String eTag = String(f.getLastWrite(), 16);  // use file modification timestamp to create ETag
    f.close();
    return (eTag);
  });

#else
  // enable standard ETAG calculation using md5 checksum of file content.
  server.enableETag(true);
#endif

  // serve all static files
  server.serveStatic("/", *fsys, "/");

  TRACE("Register default (not found) answer...\n");

  // handle cases when file is not found
  server.onNotFound([]() {
    // standard not found in browser.
    server.send(404, "text/html", FPSTR(notFoundContent));
  });

  server.begin();

  TRACE("open <http://%s> or <http://%s>\n", WiFi.getHostname(), WiFi.localIP().toString().c_str());
}

// run the server...
void WifiServer::HandleClient() {
  IPAddress IP = WiFi.softAPIP();

  Serial.println(IP);
//  TRACE("Wifi démarré %d", IP);

  server.handleClient();
}  // loop()