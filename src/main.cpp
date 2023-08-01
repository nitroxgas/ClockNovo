#include <Arduino.h>
//#include "DisplayManager.h"
//#include "PeripheryManager.h"
//#include "MQTTManager.h"
#include "ServerManager.h"
#include "Globals.h"
//#include "UpdateManager.h"

TaskHandle_t taskHandle;
volatile bool StopTask = false;


void setup()
{
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
  delay(2000);
  loadSettings();
  Serial.begin(115200);
  //PeripheryManager.setup();
  ServerManager.loadSettings();
  delay(500);
  ServerManager.setup();
  // PeripheryManager.playBootSound();
  if (ServerManager.isConnected)
  {    
    float x = 4;
    while (x >= -85)
    {
      x -= 0.18;
    }
  }
  else
  {
    AP_MODE = true;
    StopTask = true;
  }
  delay(200);  
}

void loop()
{
   ServerManager.tick();
}