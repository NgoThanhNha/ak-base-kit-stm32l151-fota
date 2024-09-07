/**
 ******************************************************************************
 * @file: w5500_port.c
 * @brief: Paho MQTT to WIZnet Chip interface source file. The process of porting an interface to use paho MQTT.
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
#include "mqtt_port.h"
#include "w5500_port.h"
#include "socket.h"

unsigned long mqtt_millis;

/*
 * @brief MQTT mqtt_millis handler
 * @note MUST BE register to your system 1m Tick timer handler.
 */
void mqtt_tick(void) {
	mqtt_millis++;
}

/*
 * @brief MQTT timer initialize
 * @param  timer : pointer to a timer structure
 *         that contains the configuration information for the Timer.
 */
void mqtt_timer_init(mqtt_timer* timer) {
	timer->end_time = 0;
}

/*
 * @brief MQTT check expired timer
 * @param  timer : pointer to a Timer structure
 *         that contains the configuration information for the Timer.
 */
char mqtt_timer_is_expired(mqtt_timer* timer) {
	long left = timer->end_time - mqtt_millis;
	return (left < 0);
}

/*
 * @brief countdown millisecond timer
 * @param  timer : pointer to a timer structure
 *         that contains the configuration information for the Timer.
 *         timeout : setting timeout millisecond.
 */
void mqtt_timer_countdown_ms(mqtt_timer* timer, unsigned int timeout) {
	timer->end_time = mqtt_millis + timeout;
}

/*
 * @brief countdown second timer
 * @param  timer : pointer to a Timer structure
 *         that contains the configuration information for the Timer.
 *         timeout : setting timeout millisecond.
 */
void mqtt_timer_countdown(mqtt_timer* timer, unsigned int timeout) {
	timer->end_time = mqtt_millis + (timeout * 1000);
}

/*
 * @brief left millisecond timer
 * @param  timer : pointer to a MQTT timer structure
 *         that contains the configuration information for the Timer.
 */
int mqtt_timer_left_ms(mqtt_timer* timer) {
	long left = timer->end_time - mqtt_millis;
	return (left < 0) ? 0 : left;
}

/*
 * @brief new network setting
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         sn : socket number where x can be (0..7).
 * @retval None
 */
void mqtt_new_network(Network* n, int sn) {
	n->my_socket = sn;
	n->mqttread = w5x00_read;
	n->mqttwrite = w5x00_write;
	n->disconnect = w5x00_disconnect;
}

/*
 * @brief read function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         buffer : pointer to a read buffer.
 *         len : buffer length.
 * @retval received data length or SOCKERR code
 */
int w5x00_read(Network* n, unsigned char* buffer, int len, long time) {

	if ((getSn_SR(n->my_socket) == SOCK_ESTABLISHED) && (getSn_RX_RSR(n->my_socket)>0)) {
		return recv(n->my_socket, buffer, len);
	}
	return SOCK_ERROR;

}

/*
 * @brief write function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         buffer : pointer to a read buffer.
 *         len : buffer length.
 * @retval length of data sent or SOCKERR code
 */
int w5x00_write(Network* n, unsigned char* buffer, int len, long time)
{
	if(getSn_SR(n->my_socket) == SOCK_ESTABLISHED) {
		return send(n->my_socket, buffer, len);
	}

	return SOCK_ERROR;
}

/*
 * @brief disconnect function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 */
void w5x00_disconnect(Network* n) {
	disconnect(n->my_socket);
}

/*
 * @brief connect network function
 * @param  n : pointer to a Network structure
 *         that contains the configuration information for the Network.
 *         ip : server iP.
 *         port : server port.
 * @retval SOCKOK code or SOCKERR code
 */
int mqtt_connect_network(Network* n, uint8_t* ip, uint16_t port) {
	uint16_t myport = 12345;

	if(socket(n->my_socket, Sn_MR_TCP, myport, 0) != n->my_socket) {
		return SOCK_ERROR;
	}
		
	if(connect(n->my_socket, ip, port) != SOCK_OK) {
		return SOCK_ERROR;
	}
	return SOCK_OK;
}