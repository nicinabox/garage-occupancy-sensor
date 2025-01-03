#include <WiFi.h>
#include <WiFiMulti.h>
#include <ArduinoOTA.h>
#include <HCSR04.h>
#include <PubSubClient.h>
#include <RunningMedian.h>
#include <esp_task_wdt.h>
#include "config.h"

WiFiMulti wifiMulti;
WiFiClient wifiClient;
PubSubClient client(MQTT_HOST, MQTT_PORT, wifiClient);
UltraSonicDistanceSensor distanceSensor(TRIG_PIN, ECHO_PIN);
RunningMedian samples = RunningMedian(6);

float prevDistance;

void connectWiFi()
{
	Serial.setDebugOutput(ENABLE_DEBUG);
	Serial.print("Trying to connect");

	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true);
	WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
	WiFi.setHostname(HOSTNAME);
	ArduinoOTA.setHostname(HOSTNAME);

	while (wifiMulti.run() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(1000);
	}

	Serial.println("connected to " + WiFi.SSID());
	Serial.println(WiFi.localIP());
	Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
}

void connectMQTT()
{
	while (!client.connected())
	{
		Serial.print("Trying MQTT connection");
		Serial.print(".");

		bool connected = client.connect(
			MQTT_CLIENT_ID,
			MQTT_USER,
			MQTT_PASS,
			MQTT_LAST_WILL_TOPIC,
			MQTT_LAST_WILL_QOS,
			MQTT_LAST_WILL_RETAIN,
			MQTT_LAST_WILL_MESSAGE);

		if (connected)
		{
			Serial.println("connected");
			client.publish(MQTT_TOPIC_CONNECTED, "true");
		}
		else
		{
			Serial.print("failed, rc=");
			Serial.print(client.state());
			delay(1000);
		}
	}
}

void occupancyEffect(bool isPresent)
{
	const char *value = isPresent ? OCCUPANCY_DETECTED_VALUE : OCCUPANCY_NOT_DETECTED_VALUE;

	digitalWrite(LED_BUILTIN, isPresent ? HIGH : LOW);

	client.publish(MQTT_TOPIC_OCCUPANCY, value);

	Serial.print(MQTT_TOPIC_OCCUPANCY);
	Serial.print(" - ");
	Serial.println(value);
}

bool isPresent(float distanceValue)
{
	return distanceValue > 0 && distanceValue <= MAX_PRESENCE_DISTANCE_CM;
}

void setup()
{
	Serial.begin(BAUD_RATE);

	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(TRIG_PIN, OUTPUT);
	pinMode(ECHO_PIN, INPUT);

	for (size_t i = 0; i < WIFI_MULTI_SIZE; i++)
	{
		wifiMulti.addAP(WIFI_MULTI[i][0], WIFI_MULTI[i][1]);
	}

	connectWiFi();
	ArduinoOTA.begin();
	connectMQTT();

	esp_task_wdt_init(WDT_TIMEOUT, true);
	esp_task_wdt_add(NULL);
}

void loop()
{
	client.loop();
	ArduinoOTA.handle();

	if (wifiMulti.run() != WL_CONNECTED)
	{
		connectWiFi();
	}

	connectMQTT();

	float rawDistance = distanceSensor.measureDistanceCm();
	Serial.println(rawDistance);

	// Limit bad readings
	if (rawDistance > 0) {
		samples.add(rawDistance);
	}

	float distance = samples.getMedian();

	bool present = isPresent(distance);
	bool prevPresent = isPresent(prevDistance);

	if (present != prevPresent)
	{
		occupancyEffect(present);

		Serial.printf("distance: %f | prevDistance %f", distance, prevDistance);
		Serial.println();
	}

	prevDistance = distance;

	esp_task_wdt_reset();
	delay(1000);
}
