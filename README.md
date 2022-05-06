# garage-occupancy-sensor

## Configuration

Define `MAX_PRESENCE_DISTANCE_CM` in `config.h`

Define WiFi, MQTT connections in `secrets.h`

## Development

### Setup

```
cp secrets.example.h secrets.h
```

### MQTT

Use `mosquitto` to test MQTT locally.

Run the server

```
mosquitto -c mosquitto.conf
```

Listen for the topic

```
mosquitto_sub -h localhost -t MQTT_TOPIC
```
