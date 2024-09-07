/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "task_list.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "task_life.h"
#include "task_dbg.h"
#include "task_shell.h"
#include "task_cloud.h"
#include "task_fw.h"

task_t app_task_table[] = {
    /*************************************************************************/
    /* SYSTEM TASKS */
    /*************************************************************************/
    {TASK_TIMER_TICK_ID,            task_timer_handler},

    /*************************************************************************/
    /* APP TASKS */
    /*************************************************************************/
    {TASK_LIFE_ID,                  task_life_handler},
    {TASK_DBG_ID,                   task_dbg_handler},
    {TASK_SHELL_ID,                 task_shell_handler},
    {TASK_CLOUD_ID,                 task_cloud_handler},
    {TASK_FW_ID,                    task_fw_handler},

    /*************************************************************************/
    /* END OF TABLE */
    /*************************************************************************/
    {STK_TASK_EOT_ID,               (pf_task)0},
};

task_polling_t app_task_polling_table[] = {
    {TASK_POLLING_CONSOLE_ID,       STK_ENABLE,         task_polling_console},
    {TASK_POLLING_MQTT_ID,          STK_ENABLE,         task_polling_mqtt},
    {STK_TASK_POLLING_EOT_ID,       STK_DISABLE,        (pf_task_polling)0},
};