/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#ifndef __TASK_DBG_H__
#define __TASK_DBG_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "message.h"

extern void task_dbg_handler(stk_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_DBG_H__ */