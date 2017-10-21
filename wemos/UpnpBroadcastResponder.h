#ifndef UPNPBROADCASTRESPONDER_H
#define UPNPBROADCASTRESPONDER_H
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Switch.h"


#define MAX_SWITCHES 14

class UpnpBroadcastResponder {
private:
        WiFiUDP UDP;  
        Switch switches[MAX_SWITCHES];
        int numOfSwitchs ;
public:
        UpnpBroadcastResponder();
        ~UpnpBroadcastResponder();
        bool beginUdpMulticast();
        void serverLoop();
        void addDevice(Switch& device);
};
 
#endif
