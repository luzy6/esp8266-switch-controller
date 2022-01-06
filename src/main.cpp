#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DebounceEvent.h>
#include <ESPAsyncUDP.h>
#include <Output.h>
#include <ArduinoOTA.h>

#ifndef OUTPUT_GPIO
#define OUTPUT_GPIO 4
#define BUTTON_PIN 12

#define ON "0"
#define OFF "1"
#define REVERSE "2"
#define BEGIN_OTA "3"
#define END_OTA "4"

#define PORT 8266

#define DEBUG false
#endif

const char* WIFI_SSID = "ChinaNet-5Tme";
const char* WIFI_PSW  = "tnscntnt";

bool is_OTA = false;

Output relay = Output(OUTPUT_GPIO);
DebounceEvent button = DebounceEvent(BUTTON_PIN, BUTTON_SWITCH | BUTTON_SET_PULLUP);
AsyncUDP udp;

void smartConfig() {
  WiFi.mode(WIFI_STA);
  if (DEBUG)
  {
    Serial.println("Debug mode");
    WiFi.begin(WIFI_SSID, WIFI_PSW);
  }
  else
  {
    WiFi.begin();
  }

  for (int i = 0; i < 10; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("AutoConfig Success");
      Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
      // Serial.println(INADDR_ANY.toString().c_str());
      // Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
      // Serial.printf("Subnet mask: ");
      // Serial.println(WiFi.subnetMask());
      // Serial.printf("Gatway IP:%s\r\n", WiFi.gatewayIP().toString().c_str());
      // WiFi.printDiag(Serial); //打印关键的Wi-Fi诊断信息，信息比较多
      break;
    }
    else
    {
      Serial.println("AutoConfig Waiting...");
      // Serial.println(WiFi.status());
      delay(1000);
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("AutoConfig Faild!" );
    WiFi.mode(WIFI_STA);
    Serial.println("\r\nWait for Smartconfig");
    WiFi.beginSmartConfig();
    while (1)
    {
      Serial.print(".");
      if (WiFi.smartConfigDone())
      {
        Serial.println("SmartConfig Success");
        Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
        Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
        WiFi.setAutoConnect(true);  // 设置自动连接
        break;
      }
      delay(3000);
    }
  }
}

void handleRec(AsyncUDPPacket packet) {
  char *data = (char*)packet.data();
  size_t plen = packet.length();
  // String data = (char*)packet.data();
  Serial.print("Length: ");
  Serial.print(plen);
  Serial.print("Data: ");
  Serial.write(data, plen);
  Serial.println();
  // Serial.println(data);
  // Serial.println(sizeof(data));
  // Serial.println(sizeof(ON));
  // Serial.println(strcmp(ON, data));
  // Serial.println(strcmp(data, OFF));
  // Serial.println(strcmp(data, REVERSE));
  // Serial.println(memcmp(data, ON, 1));
  // Serial.println(memcmp(data, OFF, 1));
  // Serial.println(memcmp(data, REVERSE, 1));

  // Serial.println(packet.isBroadcast()?"Broadcast":packet.isMulticast()?"Multicast":"Unicast");
  if (strncmp(data, ON, plen) == 0) {
    Serial.println("output on");
    relay.on();
  }
  else if (strncmp(data, OFF, plen) == 0) {
    Serial.println("output off");
    relay.off();
  }
  else if (strncmp(data, REVERSE, plen) == 0) {
    Serial.println("output switched");
    relay.reverse();
  }
  else if (strncmp(data, BEGIN_OTA, plen) == 0) {
    Serial.println("begin OTA");
    if (!is_OTA){
      ArduinoOTA.begin();
    }
    is_OTA = true;
  }
  else if (strncmp(data, END_OTA, plen) == 0) {
    Serial.println("end OTA");
    is_OTA = false;
  }
}

void setOTA() {
  ArduinoOTA.setHostname("esp8266");
  ArduinoOTA.setPassword("123456");
  ArduinoOTA.setPort(18266);
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  Serial.println("OTA setted");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);  
  smartConfig();
  setOTA();
  if(udp.listen(PORT)) {
  // if(udp.listen(PORT)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket(handleRec);
  }
}

void loop() {
  if (is_OTA) {
    ArduinoOTA.handle();
  }
  // put your main code here, to run repeatedly:
  if (button.loop() != EVENT_NONE) {
        Serial.println("button switched");
        relay.reverse();
    }
  // delay(1000);
  // udp.broadcast("here?");
  // Serial.println("haha");
}
