#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define AWS_IOT_SUBSCRIBE_TOPIC "ESP8266/Sub"
 
#define relay1 23
#define relay2 24  // Define the second relay pin
 
WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);
 
void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}
 
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  Serial.print("incoming: ");
  Serial.println(topic);
 
  StaticJsonDocument<200> doc;
  deserializeJson(doc, payload);
  const char* message = doc["message"];
  Serial.println();
 
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]); // Print payload content
  }
 
  char relay1Command = (char)payload[62]; // Extracting the controlling command for relay1 from the payload
  char relay2Command = (char)payload[63]; // Extracting the controlling command for relay2 from the payload
  Serial.print("Relay1 Command: ");
  Serial.println(relay1Command);
  Serial.print("Relay2 Command: ");
  Serial.println(relay2Command);
 
  if (relay1Command == '1')
  {
    digitalWrite(relay1, HIGH);
    Serial.println("Relay1 State changed to HIGH");
  }
  else if (relay1Command == '0')
  {
    digitalWrite(relay1, LOW);
    Serial.println("Relay1 State changed to LOW");
  }

  if (relay2Command == '1')
  {
    digitalWrite(relay2, HIGH);
    Serial.println("Relay2 State changed to HIGH");
  }
  else if (relay2Command == '0')
  {
    digitalWrite(relay2, LOW);
    Serial.println("Relay2 State changed to LOW");
  }
  
  Serial.println();
}
 
void setup()
{
  Serial.begin(115200);
  connectAWS();
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
}
 
void loop()
{
  client.loop();
  delay(1000);
}
