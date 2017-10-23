#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <EEPROM.h>

struct DeviceDetail
{
  String invocationName;
  const int pin;
};

// #######################################################################

/*add your switch details here*/
/*Format {< invocation name >,< GPIO pin number of ESP8266 >} . Add more switches separeated by commas(',')*/
DeviceDetail deviceArray[] = {{"room lights",D4},{"fan",D3}};

// #######################################################################

// prototypes
boolean connectWifi();


/* Set these to your desired config AP credentials. They are not configurable at runtime */
const char *softAP_ssid = "ConfigAP";
/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);


// Glocal variables
char ssid[32] = {'\0'};
char password[32] ={'\0'};

boolean wifiConnected = false;
bool canConnect = false;

const int DEVICE_PORT = 90;

int numOfSwitches = 0;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *switches[MAX_SWITCHES];


// Web server
ESP8266WebServer server(80);


void setup()
{
  Serial.begin(115200);
  // Initialise wifi connection

  loadCredentials();
  canConnect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID

  numOfSwitches = sizeof(deviceArray)/sizeof(DeviceDetail);
  Serial.print("numOfSwitches");
  Serial.print(numOfSwitches);
  
  if(canConnect)
  {
    wifiConnected = connectWifi();
    if(wifiConnected)
    {
      upnpBroadcastResponder.beginUdpMulticast();
      
      // Define your switches here. Max 14
      // Format: Alexa invocation name, local port no, on callback, off callback
      for(int i=0; i<numOfSwitches; i++)
      {
        Switch *deviceswitch = new Switch(deviceArray[i].invocationName, DEVICE_PORT+i, deviceArray[i].pin);
        switches[i] = deviceswitch;
        upnpBroadcastResponder.addDevice(*deviceswitch);
      }
      
      return;
    }
  }
  
  startSoftAP();
}


void startSoftAP()
{
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid);
  delay(500); // Without delay I've seen the IP address blank
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  startConfigServer();
  
}

void stopSoftAP()
{
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
}

void startConfigServer()
{  
  server.on("/", handleWifi);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.onNotFound ( handleNotFound );
  server.begin(); // Web server start
  Serial.println("HTTP server started");
}
 
void loop()
{
	 if(wifiConnected){
      upnpBroadcastResponder.serverLoop();

      for(int i=0 ;i < numOfSwitches ; i++)
      {
        switches[i]->serverLoop();
      }
	 }
   server.handleClient();
}


// connect to wifi – returns true if successful or false if not
boolean connectWifi(){
  stopSoftAP();
  boolean state = true;
  int i = 0;
  
 // WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  Serial.println("ssid");
  Serial.println(ssid);
  Serial.println("password");
  Serial.println(password);

  // Wait for connection
  Serial.print("Connecting ...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 50){
      state = false;
      break;
    }
    i++;
  }
  
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    startConfigServer();
  }
  else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}
