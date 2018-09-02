#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <time.h>
#include <EEPROM.h>

char timeZone[6] = "1";
int i=0;
int8_t tz = 1;
WiFiManagerParameter custom_timeZone("timeZone", "UTC+", timeZone, 6);
WiFiManager wifiManager;

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
    EEPROM.begin(1);
    tz=EEPROM.read(0);
    sprintf(timeZone,"%d",tz);
    Serial.printf("tz_eeprom: %d\r\n",tz);
    custom_timeZone.setValue(timeZone, 6);
    WiFi.persistent(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin();
    wifiManager.setDebugOutput(1);
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.addParameter(&custom_timeZone);
    wifiManager.setTimeout(30);
    wifiManager.setConfigPortalBlocking(false);
    Serial.println("Going to open a WIFI access point for user configuration, closing in 30s! SSID:NixieClockConfig");
    wifiManager.startConfigPortal("NixieClockConfig");
}

void saveConfigCallback () {
  Serial.println("Save new TimeZone setting");
  String inString = custom_timeZone.getValue();
  tz = inString.toInt();
  EEPROM.write(0,tz);
  EEPROM.commit();
}

unsigned char decToBCD(unsigned char dec) {
    return (dec / 10 << 4) + dec%10;  
}

void loop() {
    time_t now = time(nullptr);
    //Serial.println(ctime(&now));
    tm* time_tm = localtime(&now);
    if (i%10 == 0 || time_tm->tm_year < 100) {
      //Serial.println("Update time via NTP");
      configTime(tz * 3600, 0, "pool.ntp.org", "time.nist.gov");
    }
    wifiManager.process();
    Serial.write(decToBCD(time_tm->tm_hour));
    Serial1.write(decToBCD(time_tm->tm_hour));
    delay(10);
    Serial.write(decToBCD(time_tm->tm_min));
    Serial1.write(decToBCD(time_tm->tm_min));
    delay(10);
    Serial.write(0xff); //end of packet
    Serial1.write(0xff); //end of packet
    delay(500);
    i++;
}
