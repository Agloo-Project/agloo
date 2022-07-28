/*
 * Copyright (c) 2006-2020, DreamGrow Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-07     luhuadong    the first version
 */

#include "agloo.h"
#ifdef AG_LIBS_USING_MQTT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "MQTTClient.h"
#include "msg_wrapper.h"

#define ADDRESS     "tcp://localhost:1883"
#define QOS         1
#define TIMEOUT     10000L

#define LOG_I(...)               printf(__VA_ARGS__);
#define LOG_D(...)               printf(__VA_ARGS__);
#define LOG_W(...)               printf(__VA_ARGS__);
#define LOG_E(...)               printf(__VA_ARGS__);

#ifdef __cplusplus
extern "C" {
#endif

static const char *username = "asensing";
static const char *password = "p@ssw0rd";

static void delivered(void *context, MQTTClient_deliveryToken dt)
{
    //printf("\nMessage with token value %d delivery confirmed\n", dt);
}

static int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    msg_arrived_cb_t *user_cb = (msg_arrived_cb_t *)context;

    //printf("\nMessage arrived\n");
    //printf("     topic: %s\n", topicName);
    //printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);

    user_cb(topicName, message->payload, message->payloadlen);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

static void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

int msg_bus_init(msg_node_t *handle, const char *name, char *address, msg_arrived_cb_t *cb)
{
    int rc;

    // 初始化 MQTT Client 选项
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = username;
    conn_opts.password = password;

    //#define MQTTClient_message_initializer { {'M', 'Q', 'T', 'M'}, 0, 0, NULL, 0, 0, 0, 0 }
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    // 声明消息 token
    MQTTClient_deliveryToken token;

    if (!address) address = ADDRESS;

    rc = MQTTClient_create(handle, address, name, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to create client, return code %d\n", rc);
        return -AG_ERROR;
    }

    rc = MQTTClient_setCallbacks(*handle, (void *)cb, connlost, msgarrvd, delivered);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to set callbacks, return code %d\n", rc);
        MQTTClient_destroy(handle);
        return -AG_ERROR;
    }

    //使用MQTTClient_connect将client连接到服务器，使用指定的连接选项。成功则返回MQTTCLIENT_SUCCESS
    rc = MQTTClient_connect(*handle, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to connect, return code %d\n", rc);
        MQTTClient_destroy(handle);
        return -AG_ERROR;
    }

    return AG_EOK;
}

int msg_bus_deinit(msg_node_t handle)
{
    MQTTClient_destroy(&handle);
    return AG_EOK;
}

int msg_bus_publish(msg_node_t handle, const char *topic, const char *payload)
{
    int rc;
    MQTTClient_deliveryToken token;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    pubmsg.payload = (void *)payload;
    pubmsg.payloadlen = strlen(payload);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(handle, topic, &pubmsg, &token);
    rc = MQTTClient_waitForCompletion(handle, token, TIMEOUT);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to publish, return code %d\n", rc);
        return -AG_ERROR;
    }

    return AG_EOK;
}

int msg_bus_publish_raw(msg_node_t handle, const char *topic, const void *payload, const int payloadlen)
{
    int rc;
    MQTTClient_deliveryToken token;

    MQTTClient_publish(handle, topic, payloadlen, payload, QOS, 0, &token);
    rc = MQTTClient_waitForCompletion(handle, token, TIMEOUT);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to publish, return code %d\n", rc);
        return -AG_ERROR;
    }

    return AG_EOK;
}

int msg_bus_subscribe(msg_node_t handle, const char *topic)
{
    int rc;
    rc = MQTTClient_subscribe(handle, topic, QOS);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to subscribe, return code %d\n", rc);
        return -AG_ERROR;
    }
    return AG_EOK;
}

int msg_bus_unsubscribe(msg_node_t handle, const char *topic)
{
    int rc;
    rc = MQTTClient_unsubscribe(handle, topic);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to unsubscribe, return code %d\n", rc);
        return -AG_ERROR;
    }
    return AG_EOK;
}

int msg_bus_set_callback(msg_node_t handle, msg_arrived_cb_t *cb)
{
    int rc;

    rc = MQTTClient_setCallbacks(handle, (void *)cb, connlost, msgarrvd, delivered);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to set callbacks, return code %d\n", rc);
        return -AG_ERROR;
    }
    return AG_EOK;
}

int msg_bus_connect(msg_node_t handle)
{
    int rc;

    // 初始化 MQTT Client 选项
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = username;
    conn_opts.password = password;

    rc = MQTTClient_connect(handle, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) {
        LOG_E("Failed to connect, return code %d\n", rc);
        return -AG_ERROR;
    }
    return AG_EOK;
}

int msg_bus_disconnect(msg_node_t handle)
{
    MQTTClient_disconnect(handle, 10000);
    return AG_EOK;
}

int msg_bus_is_connected(msg_node_t handle)
{
    if (MQTTClient_isConnected(handle)) return AG_TRUE;
    else return AG_FALSE;
}

#ifdef __cplusplus
}
#endif

#endif /* AG_LIBS_USING_MQTT */