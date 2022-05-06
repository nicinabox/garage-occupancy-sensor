#include <WiFi.h>
#include <HCSR04.h>
#include "config.h"
#include "secrets.h"

const byte trigPin = 5;
const byte echoPin = 18;
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);

float distanceCm, prevDistanceCm;

void awaitWifiConnected() {
  Serial.setDebugOutput(true);
  Serial.println("Trying to connect " + String(WIFI_SSID));

  IPAddress device_ip DEVICE_IP;
  IPAddress gateway_ip GATEWAY_IP;
  IPAddress subnet_mask SUBNET_MASK;

  WiFi.mode(WIFI_STA);
  WiFi.config(device_ip, gateway_ip, subnet_mask);
  WiFi.hostname(HOSTNAME);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 9);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(1000);
  }

  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200); // Starts the serial communication

  awaitWifiConnected();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void loop() {
  distanceCm = distanceSensor.measureDistanceCm();

  if (distanceCm < MAX_PRESENCE_DISTANCE_CM && prevDistanceCm >= MAX_PRESENCE_DISTANCE_CM){
    Serial.println("PRESENT");
  }

  if (distanceCm >= MAX_PRESENCE_DISTANCE_CM && prevDistanceCm < MAX_PRESENCE_DISTANCE_CM){
    Serial.println("VACANT");
  }

  prevDistanceCm = distanceCm;

  delay(500);
}
