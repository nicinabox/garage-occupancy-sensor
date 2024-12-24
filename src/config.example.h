#define WDT_TIMEOUT 120

#define TRIG_PIN 5
#define ECHO_PIN 18

#define HOSTNAME ""

#define ENABLE_DEBUG true
#define BAUD_RATE 115200

#define WIFI_MULTI_SIZE 1

const char * WIFI_MULTI[WIFI_MULTI_SIZE][2] = {
    { "Name", "Password" },
};

#define MQTT_HOST "0.0.0.0"
#define MQTT_PORT 1883
#define MQTT_NAME ""
#define MQTT_PASS ""
#define MQTT_KEEP_ALIVE 15
#define MQTT_CLIENT_ID HOSTNAME

#define MQTT_TOPIC_CONNECTED "stat/connected"
#define MQTT_TOPIC_OCCUPANCY "stat/occupancy"

#define MQTT_LAST_WILL_TOPIC "stat/connected"
#define MQTT_LAST_WILL_QOS 0
#define MQTT_LAST_WILL_RETAIN true
#define MQTT_LAST_WILL_MESSAGE "false"

#define MAX_PRESENCE_DISTANCE_CM 160.00 // 5.2 feet

#define OCCUPANCY_DETECTED_VALUE "true"
#define OCCUPANCY_NOT_DETECTED_VALUE "false"
