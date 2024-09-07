/**
 ******************************************************************************
 * @author: Nark
 * @date:   06/09/2024
 ******************************************************************************
**/

#include "task_life.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "io_cfg.h"
#include "sys_cfg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

led_t led_life;

void task_life_handler(stk_msg_t* msg) {
    switch (msg->sig) {
    case AC_LIFE_SYSTEM_CHECK:
        led_toggle(&led_life);
        sys_ctrl_independent_watchdog_reset();
        break;

    default:
        break;
    }
}