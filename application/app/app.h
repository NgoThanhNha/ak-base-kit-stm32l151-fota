/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

#define APP_VER         "1.0"

/*************************************************************************/
/* APP DEFINE SIGNAL 
**************************************************************************/
enum app_signals {
    /* TASK LIFE */
    AC_LIFE_SYSTEM_CHECK,

    /* TASK SHELL */
    AC_SHELL_CMD_PARSER,

    /* TASK DBG */
    AC_DBG_A,
    AC_DBG_B,
    AC_DBG_C,

    /* TASK CLOUD */
    AC_NET_HW_INIT,
    AC_NET_DHCP_INIT,
    AC_NET_GET_INFO,
    AC_MQTT_INIT,
    AC_MQTT_PUB,
    AC_MQTT_FOTA_REQ_ATTR,
    AC_MQTT_FOTA_REQ_CHUNK,
    AC_MQTT_FOTA_REV_PAYLOAD,

    /* FIRMWARE */
    FW_UPDATE_CURRENT_FIRMWARE_INFO,
    FW_SHOW_CURRENT_FIRMWARE_INFO,
    FW_FIRMWARE_UPDATE_REQUEST,
    FW_PREPARE_REV_NEW_FIRMWARE,
    FW_WRITE_TO_EXTERNAL_FLASH,
    FW_UPDATE_CHECKSUM,
    FW_JUMP_TO_BOOT,
};

/* main application */
extern int main_app();

/* polling with system tick */
extern void sys_irq_timer_10ms();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */