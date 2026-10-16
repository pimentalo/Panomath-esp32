/**
  Largely inspired from https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/examples/WebServer/

  Endpoints:
  - / -> /index.html
  - /index.html: home page
  - /status: 
  - /lcd: 
  - /sound:
  - /wavedata: send wave data
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

const void* _waveData;
int _waveDataLength;
int _waveDataStart;

class MemStream: public Stream {
  public:
    MemStream(const void* data, int length, int start = 0)
    {
      this->_data = (uint8_t *)data;
      this->_length = length;
      this->_start = start;
      this->_position = start;
      this->_available = length;
    }

    int available() {
      return this->_available;
    }

    int read() {
      if (this->_available > 0)
      {
        this->_available--;
        int result = (int) (this->_data[this->_position++]); 
        if (this->_position > this->_length) this->_position = 0;
        return result;
      }
      return -1;
    }

    int peek() {
      if (this->_available > 0)
        return (int) (this->_data[this->_position]);
      return -1;
    }

    size_t readBytes(char* buffer, int length)
    {
//      TRACE("Memstream::readBytes() %x %d\n", buffer, length);

      if (this->_available < 0) return -1;

      // force à travailler plus localement
      int available = this->_available;

      // transférer le maximum
      int remainingLength = length < available ? length : available;

      int position = this->_position;
      int totalLength = this->_length;
      const uint8_t* data = this->_data;

      int copied = 0;

      while (available > 0 && remainingLength > 0) {
        // we will copy till the end and beyond... We must do 2 transfer
        int toCopy = remainingLength;
        if (position + remainingLength > totalLength)
        {
          toCopy = totalLength - position;  
        }
        memcpy(buffer, data + position, toCopy);
        copied += toCopy;
        available -= toCopy;
        position += toCopy;
        if (position >= totalLength) position = 0;
        remainingLength -= toCopy;
      }

      this->_available = available;
      this->_position = position;
      return copied;
    }

    size_t write(uint8_t) {
      return 0;
    }

  private:
    const uint8_t* _data;
    int _length;
    int _start;
    int _available;
    int _position;
};

WifiServer::WifiServer()
{
  _waveData = NULL;
  _waveDataLength = 0;
  _waveDataStart = 0;
}



void WifiServer::setWaveData(const void* data, int length, int start ) {
  _waveData = data;
  _waveDataLength = length;
  _waveDataStart = 0;
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

  if(_waveData == NULL) {
    LOG_ERROR("handleGetWaveData(): wave data uninitialized")
  } else {

    server.sendHeader("Cache-Control", "no-cache");
/*    server.sendHeader("Content-Length", String(_waveDataLength));
    server.send(200, "application/octet-stream", ""); // Empty string to complete headers

    size_t chunkSize = 1024;
    size_t remainingBytes = _waveDataLength;
    const char* data = (const char *)_waveData;
    while (remainingBytes > 0) {
      if (chunkSize > remainingBytes) chunkSize = remainingBytes;
      server.sendContent((const char *)data, chunkSize);
      data += chunkSize;
    }
  }*/
    MemStream *ms = new MemStream(_waveData, _waveDataLength, _waveDataStart);
    server.send(200, "application/octet-stream", *ms, _waveDataLength);
  }
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

  // register a redirect handler when only domain name is given.
  server.on("/", HTTP_GET, handleRedirect);
  server.on("/wavedata", HTTP_GET, handleGetWavedata);
  server.on("/sysinfo", HTTP_GET, handleSysInfo);

  // Initialise le FS
  this->fileHandler = new FileServerHandler(server);

  // // Ask for the current time using NTP request builtin into ESP firmware.
  //TRACE("Setup ntp...\n");
  //configTzTime(TIMEZONE, "pool.ntp.org");

  TRACE("Register redirect...\n");


  TRACE("Register service handlers...\n");

  
  // enable CORS header in webserver results
//  server.enableCORS(true);


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
//  IPAddress IP = WiFi.softAPIP();

//  Serial.println(IP);
//  TRACE("Wifi démarré %d", IP);

  server.handleClient();
} 