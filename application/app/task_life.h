/**
 ******************************************************************************
 * @author: Nark
 * @date:   06/09/2024
 ******************************************************************************
**/

#ifndef __TASK_LIFE_H__
#define __TASK_LIFE_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "message.h"
#include "led.h"

extern led_t led_life;
extern void task_life_handler(stk_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_LIFE_H__ */