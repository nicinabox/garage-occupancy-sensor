#include <WiFi.h>
#include "config.h"
#include "secrets.h"

const int trigPin = 5;
const int echoPin = 18;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

long duration;
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
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;

  if (distanceCm < MAX_PRESENCE_DISTANCE_CM && prevDistanceCm >= MAX_PRESENCE_DISTANCE_CM){
    Serial.println("PRESENT");
  }

  if (distanceCm >= MAX_PRESENCE_DISTANCE_CM && prevDistanceCm < MAX_PRESENCE_DISTANCE_CM){
    Serial.println("VACANT");
  }

  prevDistanceCm = distanceCm;

  delay(500);
}
