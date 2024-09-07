/**
 ******************************************************************************
 * @file: w5500_port.c
 * @brief: Paho MQTT to WIZnet Chip interface header file. The process of porting an interface to use paho MQTT.
 * @date: 2016/12/06
 * @version: 1.0.0
 * @par: Revision history
		<2016/12/06> 1st Release
 * @author: Peter Bang & Justin Kim
 * @copyright: Copyright (c)  2016, WIZnet Co., LTD.
 * All rights reserved.
 * 
 * [Update]
 * @author: Nark
 * @date:   23/08/2024
 * @brief:  code refactor
 ******************************************************************************
**/

#ifndef __MQTT_PORT_H__
#define __MQTT_PORT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief mqtt_tick
 * @note MUST BE register to your system with system tick timer or other timer
 */
void mqtt_tick();

/*
 * @brief timer structure
 */
typedef struct mqtt_timer mqtt_timer;
struct mqtt_timer {
	unsigned long system_tick_period;
	unsigned long end_time;
};

/*
 * @brief network structure
 */
typedef struct Network Network;
struct Network {
	int my_socket;
	int (*mqttread) (Network*, unsigned char*, int, long);
	int (*mqttwrite) (Network*, unsigned char*, int, long);
	void (*disconnect) (Network*);
};

/*
 * @brief timer function
 */
void mqtt_timer_init(mqtt_timer*);
char mqtt_timer_is_expired(mqtt_timer*);
void mqtt_timer_countdown_ms(mqtt_timer*, unsigned int);
void mqtt_timer_countdown(mqtt_timer*, unsigned int);
int mqtt_timer_left_ms(mqtt_timer*);

/*
 * @brief network interface porting
 */
int w5x00_read(Network*, unsigned char*, int, long);
int w5x00_write(Network*, unsigned char*, int, long);
void w5x00_disconnect(Network*);
void mqtt_new_network(Network* n, int sn);
int mqtt_connect_network(Network* n, uint8_t* ip, uint16_t port);

#ifdef __cplusplus
}
#endif

#endif /* __MQTT_PORT_H__ */
