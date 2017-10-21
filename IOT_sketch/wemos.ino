#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <functional>
#include <DNSServer.h>
#include "Switch.h"
#include "UpnpBroadcastResponder.h"
#include "CallbackFunction.h"
#include <EEPROM.h>

// prototypes
boolean connectWifi();



/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "ConfigAP";
/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

// DNS server
const byte DNS_PORT = 53;
DNSServer dnsServer;

// Change this before you flash
//#######################################
char ssid[32] = {'\0'};
char password[32] ={'\0'};
// change gpio pins as you need it.
//I am using ESP8266 EPS-12E GPIO16 and GPIO14
const int relayPin1 = D4;
const int relayPin2 = 14;

//#######################################
boolean wifiConnected = false;

   bool canConnect = false;

UpnpBroadcastResponder upnpBroadcastResponder;

Switch *bedroom = NULL;
Switch *fan = NULL;

// Web server
ESP8266WebServer server(80);


void setup()
{
  Serial.begin(115200);
  // Initialise wifi connection

  loadCredentials();
  canConnect = strlen(ssid) > 0; // Request WLAN connect if there is a SSID
  
  if(canConnect)
  {
    wifiConnected = connectWifi();
    if(wifiConnected)
    {
      upnpBroadcastResponder.beginUdpMulticast();
      
      // Define your switches here. Max 14
      // Format: Alexa invocation name, local port no, on callback, off callback
      bedroom = new Switch("bedroom light", 90, relayPin1);
      //kitchen = new Switch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);
      fan = new Switch("fan", 91, relayPin2);
  
      Serial.println("Adding switches upnp broadcast responder");
      upnpBroadcastResponder.addDevice(*bedroom);
      upnpBroadcastResponder.addDevice(*fan);
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
    /* Setup the DNS server redirecting all the domains to the apIP */  
//  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
//  dnsServer.start(DNS_PORT, "*", apIP);
  
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
      
      bedroom->serverLoop();
      fan->serverLoop();
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
    if (i > 100){
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
