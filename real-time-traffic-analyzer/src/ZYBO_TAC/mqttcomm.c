#include "mqttcomm.h"

int MQTT_disconnect() {
    int ret = MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    return ret;
}

void MQTT_init() {
    int ec;

    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_create(&client, MQTT_BROKER, MQTT_CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    if ((ec = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect, error code %d\n", ec);
        exit(1);
    } else
        printf("MQTT successful connected!\n");
}

int MQTT_Send(char * message, int len) {
    return MQTTClient_publish(client, MQTT_TOPIC, len, message, MQTT_QOS, 0, NULL);
}
