/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "app.h"

/* kernel includes */
#include "task.h"
#include "message.h"
#include "timer.h"

/* peripheral includes */
#include "io_cfg.h"

/* driver includes */
#include "led.h"
#include "button.h"

/* system includes */
#include "sys_boot.h"
#include "sys_cfg.h"

/* network includes */
#include "w5500.h"
#include "w5500_port.h"
#include "MQTTClient.h"
#include "mqtt_port.h"
#include "dns.h"
#include "dhcp.h"

/* task includes */
#include "bsp.h"
#include "task_list.h"
#include "task_life.h"
#include "task_shell.h"
#include "task_cloud.h"
#include "task_fw.h"

/* app initial functions */
static void app_start_timer();
static void app_task_init();

int main_app() {
    /******************************************************************************
    * init kernel
    *******************************************************************************/
    msg_init();
    task_create((task_t*)&app_task_table);
    task_polling_create((task_polling_t*)&app_task_polling_table);
    timer_init();

    /* independent watchdog timer init */
    sys_cfg_independent_watchdog_init();
    
    /* led init */
    led_init_func(&led_life, led_life_on, led_life_off, 10);

    /* ring buffer init */
    ring_buffer_char_init(&ring_buffer_console_rev, buffer_console_rev, BUFFER_CONSOLE_REV_SIZE);

    /* initial boot object */
	sys_boot_init();

    /******************************************************************************
    * app task initial
    *******************************************************************************/
    app_task_init();

    /******************************************************************************
    * start timer for application
    *******************************************************************************/
    app_start_timer();

    /******************************************************************************
    * run applications
    *******************************************************************************/
    return task_run();
}

/*****************************************************************************/
/* app initial functions 
 */
/*****************************************************************************/

/* send the first message to tasks */
void app_task_init() {
    task_polling_set_ability(TASK_POLLING_MQTT_ID, STK_DISABLE);
}

/* start software timer for application */
void app_start_timer() {
    timer_set(TASK_LIFE_ID, AC_LIFE_SYSTEM_CHECK, 1000, TIMER_PERIODIC);
    timer_set(TASK_FW_ID, FW_UPDATE_CURRENT_FIRMWARE_INFO, 5000, TIMER_ONE_SHOT);
    timer_set(TASK_CLOUD_ID, AC_NET_HW_INIT, 500, TIMER_ONE_SHOT);
}

/*****************************************************************************/
/* app common functions
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
*/
void ethernet_polling() {
    static uint32_t poll_counter;
    if (poll_counter == 100) {
        dns_time_handler();
        DHCP_time_handler();
        poll_counter = 0;
    }
    else {
        poll_counter++;
    }
}

void sys_irq_timer_10ms() {
    led_polling(&led_life);
    ethernet_polling();
}