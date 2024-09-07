/**
 ******************************************************************************
 * @author: Nark
 * @date:   28/08/2024
 ******************************************************************************
**/

#ifndef __TASK_FW_H__
#define __TASK_FW_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "message.h"

#define APP_START_ADDR_EXTERNAL_FLASH           (0x00000000)
#define BLOCK_SIZE                              (65536) /* bytes */

extern void task_fw_handler(stk_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_FW_H__ */