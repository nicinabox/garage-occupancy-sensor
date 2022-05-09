# garage-occupancy-sensor

## Configuration

### Setup

```
cp src/config.example.h src/config.h
```

Adjust config as needed in `src/config.h`.

## Development

### Flashing

1. Plugin USB
2. In Arduino editor press Upload
3. When "Connecting..." hold down BOOT button until flashing begins

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
