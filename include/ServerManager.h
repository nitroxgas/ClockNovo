#ifndef ServerManager_h
#define ServerManager_h

#include <Arduino.h>

class ServerManager_
{
private:
    ServerManager_() = default;

public:
    static ServerManager_ &getInstance();
    void setup();
    void tick();
    void loadSettings();
    bool isConnected;
    IPAddress myIP;
};

extern ServerManager_ &ServerManager;
 
#endif
