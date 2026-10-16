/**
  FileSystem Handler for the WebServer
*/

#include "esp_partition.h"  // to check existing data partitions in Flash memory

#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // Use LittleFSThis file system is used.
#include <FFat.h>      // or.. FAT


// getFS() : identifies which FS is on the system
// returns either littlefs or fatfs, or null
// if partition allows it, formats the fs and reboots
fs::FS* getFS()
{
  TRACE("Starting WebServer example...\n");

  fs::FS *fsys = nullptr;
  // ----- check partitions for finding the filesystem type -----
  esp_partition_iterator_t i;

  i = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_FAT, nullptr);
  if (i) {
    TRACE("FAT partition found.");
    fsys = &FFat;
  } else {
    i = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
    if (i) {
      TRACE("SPIFFS partition found.");
      fsys = &LittleFS;
    } else {
      TRACE("no partition found.");
    }
  }
  esp_partition_iterator_release(i);

  // mount and format as needed
  TRACE("Mounting the filesystem...\n");
  if (!fsys) {
    TRACE("need to change the board configuration to include a partition for files.\n");
    delay(30000);
  } else if ((fsys == (fs::FS *)&FFat) && (!FFat.begin())) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    TRACE("formatting FAT...\n");
    FFat.format();
    delay(2000);
    TRACE("restarting...\n");
    delay(2000);
    ESP.restart();
  } else if ((fsys == (fs::FS *)&LittleFS) && (!LittleFS.begin())) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    TRACE("formatting LittleFS...\n");
    LittleFS.format();
    delay(2000);
    TRACE("restarting...\n");
    delay(2000);
    ESP.restart();
  }
  return fsys;
}

// The FileServerHandler is registered to the web server to support DELETE and UPLOAD of files into the filesystem.
FileServerHandler::FileServerHandler(WebServer &server) {
  this->fsys = getFS();
 // this->server = server;
  if (this->fsys != nullptr) {
    // FS enpoints:
    server.on("/$upload.htm", [&server]() {
      server.send(200, "text/html", FPSTR(uploadContent));
    });

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
    server.serveStatic("/", *this->fsys, "/");

    // UPLOAD and DELETE of files in the file system using a request handler.
    server.addHandler(this);
    TRACE("FileServerHandler is registered\n");
  } else {
    TRACE("No filesystem detected, content will be static\n");
  }
}

// @brief check incoming request. Can handle POST for uploads and DELETE.
// @param requestMethod method of the http request line.
// @param requestUri request resource from the http request line.
// @return true when method can be handled.
bool FileServerHandler::canHandle(WebServer &server, HTTPMethod requestMethod, const String &uri) {
  // register some REST services
  if (requestMethod == HTTP_GET)
  {
    return (uri.startsWith("/$fs") || uri.startsWith("$fs"));
  }
  return ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE));
}  // canHandle()

bool FileServerHandler::fileExists(const char *filename) {
  if (this->fsys != nullptr) return this->fsys->exists(filename);
  return false;
}

bool FileServerHandler::canUpload(WebServer &server, const String &uri) {
  // only allow upload on root fs level.
  return (uri == "/");
}  // canUpload()

bool FileServerHandler::handle(WebServer &server, HTTPMethod requestMethod, const String &requestUri) {
  // ensure that filename starts with '/'
  if (this->fsys == nullptr) {
    server.send(204);  // all done.
    return (true);
  } else {
    if (requestUri.startsWith("$fs")) {
      handleListFiles(server);
      return true;
    } else {

      String fName = requestUri;
      if (!fName.startsWith("/")) {
        fName = "/" + fName;
      }

      if (requestMethod == HTTP_POST) {
        // all done in upload. no other forms.
      } else if (requestMethod == HTTP_DELETE) {
        if (this->fsys->exists(fName)) {
          TRACE("DELETE %s\n", fName.c_str());
          this->fsys->remove(fName);
        }
      }  // if

      server.send(200);  // all done.
    }
    return (true);
  }
}  // handle()

// uploading process
void FileServerHandler::upload(WebServer UNUSED &server, const String &requestUri, HTTPUpload &upload) {
  // ensure that filename starts with '/'
  static size_t uploadSize;
/*
      if (upload.status == UPLOAD_FILE_START) {
        String fName = upload.filename;

        // Open the file for writing
        if (!fName.startsWith("/")) {
          fName = "/" + fName;
        }
        TRACE("start uploading file %s...\n", fName.c_str());

        if (fsys->exists(fName)) {
          fsys->remove(fName);
        }  // if
        _fsUploadFile = fsys->open(fName, "w");
        uploadSize = 0;

      } else if (upload.status == UPLOAD_FILE_WRITE) {
        // Write received bytes
        if (_fsUploadFile) {
          size_t written = _fsUploadFile.write(upload.buf, upload.currentSize);
          if (written < upload.currentSize) {
            // upload failed
            TRACE("  write error!\n");
            _fsUploadFile.close();

            // delete file to free up space in filesystem
            String fName = upload.filename;
            if (!fName.startsWith("/")) {
              fName = "/" + fName;
            }
            fsys->remove(fName);
          }
          uploadSize += upload.currentSize;
          // TRACE("free:: %d of %d\n", LittleFS.usedBytes(), LittleFS.totalBytes());
          // TRACE("written:: %d of %d\n", written, upload.currentSize);
          // TRACE("totalSize: %d\n", upload.currentSize + upload.totalSize);
        }  // if

      } else if (upload.status == UPLOAD_FILE_END) {
        TRACE("upload done.\n");
        // Close the file
        if (_fsUploadFile) {
          _fsUploadFile.close();
          TRACE(" %d bytes uploaded.\n", upload.totalSize);
        }
      }  // if
*/
}  // upload()

// This function is called when the WebServer was requested to list all existing files in the filesystem.
// a JSON array with file information is returned.
bool FileServerHandler::handleListFiles(WebServer &server) { //}, HTTPMethod requestMethod, const String &requestUri) {
  File dir = this->fsys->open("/", "r");
  
  String result;

  result += "[\n";
  while (File entry = dir.openNextFile()) {
    if (result.length() > 4) {
      result += ",\n";
    }
    result += "  {";
    result += "\"type\": \"file\", ";
    result += "\"name\": \"" + String(entry.name()) + "\", ";
    result += "\"size\": " + String(entry.size()) + ", ";
    result += "\"time\": " + String(entry.getLastWrite());
    result += "}";
  }  // while

  result += "\n]";
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/javascript; charset=utf-8", result);
}  // handleListFiles()
