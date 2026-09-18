#define LOG_ERROR(...) Serial.print("ERROR "); Serial.printf(__VA_ARGS__);
#define TRACE(...) Serial.print("ERROR "); Serial.printf(__VA_ARGS__)

#include <Arduino.h>
#include "AllClasses.h"
#include "AllPins.h"

Affichage _affichage;
CompteARebours _compteur1; 
CompteARebours _compteur2; 
WifiServer _server;

//Micro _Micro(PIN_MIC_DOUT, PIN_MIC_BCLK, PIN_MIC_LRCLK);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  delay(500);

  Serial.println("Panomath starting");
  _affichage.setup();
  Serial.println("Panomath affichage prêt");
  
  _compteur1.setTime(5,0);
  _compteur2.setTime(6,0);
  Serial.println("Panomath compteurs prêts");
  
  _server.setup();
  Serial.println("Panomath serveur démarré");
  
  Serial.println("Panomath configuré!");
}

void loop() {
  Serial.println();

  // put your main code here, to run repeatedly:
  char compteur1[10], compteur2[10];
  _compteur1.getString(compteur1, 10);
  _compteur2.getString(compteur2, 10);
  _affichage.clear();
  Serial.println("+------+");
  _affichage.setLineText(0, compteur1, GREEN);
  Serial.println(compteur1);
  _affichage.setLineText(1, compteur2, MAGENTA);
  Serial.println(compteur2);
  /*  _affichage.setLineText(0, "Hello!", GREEN);
  _affichage.setLineText(1, "G----", RED);
  _affichage.setLineText(2, "----D");*/
  Serial.println();
  _affichage.show();
  Serial.println("+------+");

  _server.HandleClient();
  delay(100);
}