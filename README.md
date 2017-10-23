# HomeAutomation-With-Alexa with ESP8266

This project supports emulates belkin wemo switches using 1 ESP 8266 chip. 
This Project is based on [arduino-esp8266-alexa-multiple-wemo-switch](https://github.com/kakopappa/arduino-esp8266-alexa-multiple-wemo-switch) by kakopappa with certain modifications like easy configuration of switches and wifi configs.

## How to use:
    1. Download the code
    2. Open wemos.ino in the Arduino editor.
    3. Define switches 
    4. Flash


## How to define switches:
To add a new switch to your project , populate below array with your invocation name and ESP8266's GPIO pin number

          /*add your switch details here*/
          /*Format {< invocation name >,< GPIO pin number of ESP8266 >} . Add more switches separeated by commas(',')*/
          DeviceDetail deviceArray[] = {{"room lights",D4},{"fan",14}};


## How to configure Wifi:
After flashing the firmware the device will automcatically comeup as a SoftAP when it detects that no wifi is configured .
1. Connect your smart phone or laptop to "ConfigAP" ssid and enter "192.168.4.1" in your browser, this will open up a config webpage.
2. enter your ssid and password. Once configured the device will turn off SoftAP .

The Wifi credetntials are stored on the Flash , so you dont need to configure each time. If the device doesnt detects previously configured Wifi , the device will again come up in softAP mode , repeat the above steps mentioned.
