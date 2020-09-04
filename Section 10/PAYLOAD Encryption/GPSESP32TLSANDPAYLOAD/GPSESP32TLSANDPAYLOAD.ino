#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h> 
#include "mbedtls/aes.h"

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

mbedtls_aes_context aes;
 
char * key = "abcdefghijklmnop";
unsigned char output[16];

const char* test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\n" \
     "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
     "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
     "QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\n" \
     "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\n" \
     "U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\n" \
     "nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\n" \
     "KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\n" \
     "/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\n" \
     "kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\n" \
     "/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\n" \
     "AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\n" \
     "aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\n" \
     "Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\n" \
     "oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\n" \
     "QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\n" \
     "d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\n" \
     "xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\n" \
     "CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\n" \
     "5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\n" \
     "8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\n" \
     "2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\n" \
     "c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\n" \
     "j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\n" \
     "-----END CERTIFICATE-----\n";


     
WiFiClientSecure wifiClient;
PubSubClient client(server, 8883, NULL, wifiClient);
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
   wifiClient.setCACert(test_root_ca);
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
    char* input = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
              payload += ",\"Latitude\":";
              payload += flat;
              payload += "}}";
   
     mbedtls_aes_init( &aes );
     mbedtls_aes_setkey_enc( &aes, (const unsigned char*) key, strlen(key) * 8 );
     mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*)input, output);
     mbedtls_aes_free( &aes );
     for (int i = 0; i < 16; i++) {
 
       char str[3];
 
      sprintf(str, "%02x", (int)output[i]);
      Serial.print(str);
     }
     
   
      
     if (client.publish(pubTopic1, (char*) output)) {
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
