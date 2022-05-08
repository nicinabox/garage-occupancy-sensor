#include <WiFi.h>
#include <HCSR04.h>
#include <PubSubClient.h>
#include <RunningMedian.h>
#include "config.h"
#include "secrets.h"

const byte trigPin = 5;
const byte echoPin = 18;

WiFiClient wifiClient;
PubSubClient pubclient(MQTT_HOST, MQTT_PORT, wifiClient);
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);
RunningMedian samples = RunningMedian(5);

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
    Serial.print(".");
    delay(1000);
  }

  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we"re reconnected
  while (!pubclient.connected()) {
    Serial.print("Trying MQTT connection...");
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

void occupancyEffect(bool isPresent) {
    const char* value = isPresent ? OCCUPANCY_DETECTED_VALUE : OCCUPANCY_NOT_DETECTED_VALUE;

    digitalWrite(LED_BUILTIN, isPresent ? HIGH : LOW);

    pubclient.publish(MQTT_TOPIC_OCCUPANCY, value);

    Serial.print(MQTT_TOPIC_OCCUPANCY);
    Serial.print(" - ");
    Serial.println(value);
}

bool isPresent(float distanceValue) {
  return distanceValue > 0 && distanceValue <= MAX_PRESENCE_DISTANCE_CM;
}

void setup() {
  Serial.begin(115200);

  awaitWifiConnected();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  if (!pubclient.connected()) {
    reconnect();
  }
  pubclient.loop();

  float rawDistance = distanceSensor.measureDistanceCm();
  samples.add(rawDistance);

  distanceCm = samples.getMedian();

  bool present = isPresent(distanceCm);
  bool prevPresent = isPresent(prevDistanceCm);

  if (present != prevPresent) {
      occupancyEffect(present);

      Serial.print("distanceCm/prevDistanceCm - ");
      Serial.print(distanceCm);
      Serial.print("/");
      Serial.print(prevDistanceCm);
      Serial.println();
  }

  prevDistanceCm = distanceCm;

  delay(500);
}
