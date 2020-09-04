#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 


#include <SoftwareSerial.h>
#include <TinyGPS.h>
#define ORG "j5sxd2"
#define DEVICE_TYPE "esp32gps"
#define DEVICE_ID "espgps"
#define TOKEN "uPeI@xT@6Iia-Ppi(!"

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/status1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char* ssid = "MQTTBROKER";
const char* password = "mqttpass";


WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);
TinyGPS gps;
SoftwareSerial ss(27, 26);

void setup()
{
  Serial.begin(115200);
  ss.begin(9600);

  Serial.print("Connecting to "); 
  Serial.print(ssid);
  WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    } 
    Serial.println("");
    
    Serial.print("WiFi connected, IP address: "); 
    Serial.println(WiFi.localIP());

}
void reconnect()
{
   if (!client.connected()) {
        Serial.print("Reconnecting client to ");
        Serial.println(server);
        while (!client.connect(clientId, authMethod, token)) {
            Serial.print(".");
            delay(500);
        }
        Serial.println("Bluemix connected");
    }
    bool newData = false;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
       //Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
 
  if (newData)
  { 
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
    Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload += ",\"Latitude\":";
              payload += flat;
              payload += "}}";
     if (client.publish(pubTopic1, (char*) payload.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }

     String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload1 += ",\"Longitude\":";
              payload1 += flon;
              payload1 += "}}";
     if (client.publish(pubTopic2, (char*) payload1.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
  }
}

void loop()
{
 
if (!client.connected()) 
{
 reconnect();
}
 client.loop();
 
 delay(1000);
}
