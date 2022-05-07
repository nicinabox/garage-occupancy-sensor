#include <WiFi.h>
#include <HCSR04.h>
#include <PubSubClient.h>
#include "config.h"
#include "secrets.h"

const byte trigPin = 5;
const byte echoPin = 18;

WiFiClient wifiClient;
PubSubClient pubclient(MQTT_HOST, MQTT_PORT, wifiClient);
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

float distanceCm, prevDistanceCm;

void awaitWifiConnected() {
  Serial.setDebugOutput(true);
  Serial.println("Trying to connect " + String(WIFI_SSID));

  IPAddress deviceIp DEVICE_IP;
  IPAddress gatewayIp GATEWAY_IP;
  IPAddress subnetMask SUBNET_MASK;

  WiFi.mode(WIFI_STA);
  WiFi.config(deviceIp, gatewayIp, subnetMask);
  WiFi.hostname(HOSTNAME);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 9);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!pubclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubclient.connect(HOSTNAME)) {
      Serial.println("connected");
      pubclient.publish(MQTT_TOPIC_CONNECTED, "true");
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); // Starts the serial communication

  awaitWifiConnected();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {
  if (!pubclient.connected()) {
    reconnect();
  }
  pubclient.loop();

  distanceCm = distanceSensor.measureDistanceCm();

  bool present = distanceCm <= MAX_PRESENCE_DISTANCE_CM;
  bool prevPresent = prevDistanceCm <= MAX_PRESENCE_DISTANCE_CM;

  if (present && !prevPresent) {
    pubclient.publish(MQTT_TOPIC_OCCUPANCY, OCCUPANCY_DETECTED_VALUE);
    Serial.println(OCCUPANCY_DETECTED_VALUE);
  }

  if (prevPresent && !present) {
    pubclient.publish(MQTT_TOPIC_OCCUPANCY, OCCUPANCY_NOT_DETECTED_VALUE);
    Serial.println(OCCUPANCY_NOT_DETECTED_VALUE);
  }

  prevDistanceCm = distanceCm;

  delay(500);
}
