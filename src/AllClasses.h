
#define AFFICHAGE_NB_LIGNES 3 // nombre de lignes
#define AFFICHAGE_NB_CHARS 8 // nombre de caractères par ligne
#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define LIGHTGREY 7
#define GREY 8
#define LIGHTBLUE 9
#define LIGHTGREEN 10
#define LIGHTCYAN 11
#define LIGHTRED 12
#define LIGHTMAGENTA 13
#define YELLOW 14
#define WHITE 15

class Affichage
{
  public:
    Affichage();
    
    // Clear display
    void clear();
    // C line background
    void setLineBackground(int line, int type, void* content);
    // Change line content
    void setLineText(int line, char* content, int foreGround = 15, int backGround = 0);
    // Show content
    void show();
    
    // Nothing to do for now ...
    void setup();
  private:
    char* textContent[3];

};

/*
  Un compte à rebours.

  setTime() initialise la durée
  start() commence à décompter
  suspend() met en pause
*/
enum CompteARebours_Status {
  STOPPED = 0,
  RUNNING,
  PAUSED  
};

class CompteARebours
{
  public:
    CompteARebours();
    void start();
    void pause();
    void resume();
    void setTime(int minutes, int seconds);
    void getString(char* buffer, int length);
  private:
    long _totalTime_s; // temps total en s
    long _startTime; // compteur1 en millisecondes
    CompteARebours_Status _status; 
    long _pauseTime; // heure de début de pause
    long _pauseDuration; // temps de pause
    char _buffer[10]; // la chaine d'affichage
};


/**
  FileSystem Handler for the WebServer
*/
#include <WebServer.h>

// The FileServerHandler is registered to the web server to support DELETE and UPLOAD of files into the filesystem.
class FileServerHandler : public RequestHandler {
  public:
    FileServerHandler(WebServer &server);
    bool fileExists(const char *filename);
    bool canHandle(WebServer &server, HTTPMethod requestMethod, const String &uri) override;
    bool canUpload(WebServer &server, const String &uri) override;
    bool handle(WebServer &server, HTTPMethod requestMethod, const String &requestUri) override;
    // uploading process
    void upload(WebServer &server, const String &requestUri, HTTPUpload &upload) override;
  protected:
    fs::FS *fsys = nullptr;
    File _fsUploadFile;
    //WebServer *server;
    bool handleListFiles(WebServer &server);
};

class WifiServer
{
  public:
    WifiServer();
    void setup();
    void HandleClient(); // To be called from loop
   // void setWaveHeader(const char* data, int length); //  
    void setWaveData(const void* data, int length, int start = 0); // 
  private:
    FileServerHandler *fileHandler;
};

class Keyboard
{

};

class Micro
{
  public:
    Micro(int pinData, int pinClk, int pinLRClk);
    
    // Read data from DMA buffer
    void Read();

    // Get last data
    void *GetSoundData(int& length);
    // Get data as wave file
    //void GetWave();
  private:
    int currentPosition; // start position (in bytes) to store next data
    size_t bufferLength; // buffer length in bytes
    void* samples;  // Samples data  
};