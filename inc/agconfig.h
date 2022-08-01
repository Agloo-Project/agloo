/*
 * Copyright (c) 2006-2020, DreamGrow Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-07-07     luhuadong    the first version
 */

#ifndef __AG_CONFIG_H__
#define __AG_CONFIG_H__

#define AG_USING_LIBC

/*
 * Choose MQTT or ZeroMQ message queue
 * If choose MQTT, there are two mode: MQTTClient and MQTTAsync 
 */
#define AG_LIBS_USING_MQTT
#define AG_LIBS_USING_MQTT_CLIENT
//#define AG_LIBS_USING_MQTT_ASYNC
//#define AG_LIBS_USING_MQTT_MOSQUITTO
//#define AG_LIBS_USING_ZEROMQ

#define AG_LIBS_USING_ZLOG
#define LOG_FILE    "/etc/agloo/zlog.conf"

#endif /* __AG_CONFIG_H__ */