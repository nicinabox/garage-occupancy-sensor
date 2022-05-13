#include <WiFi.h>
#include <HCSR04.h>
#include <PubSubClient.h>
#include <RunningMedian.h>
#include "config.h"

const byte trigPin = 5;
const byte echoPin = 18;

WiFiClient wifiClient;
PubSubClient client(MQTT_HOST, MQTT_PORT, wifiClient);
UltraSonicDistanceSensor distanceSensor(trigPin, echoPin);
RunningMedian samples = RunningMedian(5);

float prevDistance;

void connectWifi()
{
	Serial.setDebugOutput(true);
	Serial.println("Trying to connect " + String(WIFI_SSID));

	WiFi.mode(WIFI_STA);
	WiFi.hostname(HOSTNAME);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	while (WiFi.status() != WL_CONNECTED)
	{
		Serial.print(".");
		delay(1000);
	}

	Serial.println(WiFi.localIP());
}

void onWifiDisconnect(WiFiEvent_t event, WiFiEventInfo_t info)
{
	connectWifi();
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
			occupancyEffect(isPresent(prevDistance));
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
	Serial.begin(115200);

	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);

	pinMode(LED_BUILTIN, OUTPUT);

	connectWifi();
	connectMQTT();

	WiFi.onEvent(onWifiDisconnect, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);
}

void loop()
{
	connectMQTT();
	client.loop();

	float rawDistance = distanceSensor.measureDistanceCm();
	samples.add(rawDistance);

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

	delay(500);
}
