/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/07/2024
 ******************************************************************************
**/

#include "shell.h"

#include "platform.h"

#include "app.h"
#include "task_list.h"

#include "sys_irq.h"
#include "sys_cfg.h"
#include "flash.h"

#include "task_list.h"
#include "task_cloud.h"
#include "task_fw.h"

/* command shell functions */
int32_t shell_help(uint8_t* argv);
int32_t shell_dbg(uint8_t* argv);
int32_t shell_boot(uint8_t* argv);

cmd_line_t shell_table[] {
    /****************************************************************************************/
    /* DEBUG COMMAND */
    /****************************************************************************************/
    {(const int8_t*)"help",         shell_help,         (const uint8_t*)"help info"},
    {(const int8_t*)"dbg",          shell_dbg,          (const uint8_t*)"shell debug"},
    {(const int8_t*)"boot",         shell_boot,         (const uint8_t*)"shell boot"},

    /****************************************************************************************/
    /* END OF TABLE */
    /****************************************************************************************/
    {(const int8_t*)0,              (pf_cmd_func)0,     (const uint8_t*)0}     
};

int32_t shell_help(uint8_t* argv) {
	uint32_t index = 0;
	switch (*(argv + 4)) {
	default:
		SHELL_LOG("\nCOMMANDS INFORMATION:\n\n");
		while(shell_table[index].cmd != (const int8_t*)0) {
			SHELL_LOG("%s\t-> %s\n\n", shell_table[index].cmd, shell_table[index].info);
			index++;
		}
		break;
	}
	return 0;
}

int32_t shell_dbg(uint8_t* argv) {
    switch (*(argv + 4)) {
    case 'v':
        SHELL_LOG("Kernel version: %s\n", STK_VERSION);
        break;
    
    case 'l':
        SHELL_LOG("Current millis: %d\n", sys_ctrl_millis());
        break;

    case 'r':
        SHELL_LOG("App reboot !\n");
        sys_ctrl_reset();
        break;

    case 't':
        SHELL_LOG("Stack used: %d\n", sys_stack_used());
        break;

    case 'f':
        FATAL("TEST", 0x00);
        break;

    default:
        SHELL_LOG("[shell dbg] unknown option\n");
        break;
    }
    return 0;
}

int32_t shell_boot(uint8_t* argv) {
    switch (*(argv + 5)) {
    case 's':
        task_post_pure_msg(TASK_FW_ID, FW_SHOW_CURRENT_FIRMWARE_INFO);
        break;

    case 'v':
        SHELL_LOG("App ver: %s\n", APP_VER);
        break;

    case 'o':
        task_post_pure_msg(TASK_CLOUD_ID, AC_MQTT_FOTA_REQ_ATTR);
        break;

    case 'u':
        task_post_pure_msg(TASK_FW_ID, FW_FIRMWARE_UPDATE_REQUEST);
        break;
    
    case 'c':
        task_post_pure_msg(TASK_FW_ID, FW_UPDATE_CHECKSUM);
        break;

    default:
        SHELL_LOG("[shell boot] unknown option\n");
        break;
    }
    return 0;
}