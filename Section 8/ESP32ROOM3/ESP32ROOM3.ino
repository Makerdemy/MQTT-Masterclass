#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 
#include "DHT.h"

const char* ssid = "MQTTBROKER";
const char* password = "mqttpass";

#define DHTPIN 4 
#define DHTTYPE DHT11
#define ORG "j5sxd2"
#define DEVICE_TYPE "esp32room3"
#define DEVICE_ID "esp32r3"
#define TOKEN ""

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/status7/fmt/json";
char pubTopic2[] = "iot-2/evt/status8/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

char willTopic[]="iot-2/evt/status9/fmt/json";
int willQos=0;
boolean willRetain=false;   
char willMessage[]= "{\"sensor\":\"dht11\",\"disabledsensorid\":3,\"status\":0}";
WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);
DHT dht(DHTPIN, DHTTYPE);


void setup() {
    Serial.begin(115200);
    dht.begin();
    Serial.println();
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

    if (!client.connected()) {
        Serial.print("Reconnecting client to ");
        Serial.println(server);
        while (!client.connect(clientId, authMethod, token, willTopic,willQos,willRetain,willMessage)) {
            Serial.print(".");
            delay(500);
        }
        Serial.println("Bluemix connected");
    }
}

long lastMsg = 0;
void loop() {
    client.loop();
    long now = millis();
    if (now - lastMsg > 3000) {
        lastMsg = now;
         float humidity = dht.readHumidity();
         float temperature = dht.readTemperature();
        String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload += ",\"temperature\":";
              payload += temperature;
              payload += "}}";
       
        Serial.print("Sending payload: ");
        Serial.println(payload);

        if (client.publish(pubTopic1, (char*) payload.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
        String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload1 += ",\"humidity\":";
              payload1 += humidity;
              payload1 += "}}";
              
        Serial.print("Sending payload: ");
        Serial.println(payload1);
       
         if (client.publish(pubTopic2, (char*) payload1.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
    }
}
