/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "task_dbg.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "io_cfg.h"
#include "led.h"
#include "utils.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

void task_dbg_handler(stk_msg_t* msg) {
    switch (msg->sig) {
    case AC_DBG_A:
        APP_PRINT("[task_dbg] AC_DBG_A\n");
        break;
    
    case AC_DBG_B:
        APP_PRINT("[task_dbg] AC_DBG_B\n");
        break;

    case AC_DBG_C:
        APP_PRINT("[task_dbg] AC_DBG_C\n");
        break;

    default:
        break;
    }
}