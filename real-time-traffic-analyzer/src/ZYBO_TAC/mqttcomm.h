/* 
 * File:   mqttcomm.h
 * Author: misi
 *
 */

#ifndef MQTTCOMM_H
#define	MQTTCOMM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define MQTT_BROKER        "tcp://test.mosquitto.org:1883"
#define MQTT_CLIENTID      "RTTA001"
#define MQTT_TOPIC         "traffic/Debrecen"
#define MQTT_QOS           1
#define MQTT_TIMEOUTMEOU   10000L

    static MQTTClient client;

    int MQTT_disconnect();

    void MQTT_init();

    int MQTT_Send(char * message, int len);


#ifdef	__cplusplus
}
#endif

#endif	/* MQTTCOMM_H */

