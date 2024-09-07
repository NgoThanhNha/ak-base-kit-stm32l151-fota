/**
 ***********************************************************************************************************************
 * @author: Nark
 * @date:   03/05/2024
 * STK (Simple Tasker Kernel) is a task scheduling mechanism based on the event-driven programming model includes:
    * Post signals to tasks
    * Execute tasks based on signal queue
 ***********************************************************************************************************************
**/

#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>

#include "message.h"

#define STK_VERSION                 "1.2"

#define STK_DISABLE                 (0x00)
#define STK_ENABLE                  (0x01)

typedef uint8_t task_id_t;
typedef void (*pf_task)(stk_msg_t*);
typedef void (*pf_task_polling)();

typedef struct {
	stk_msg_t* qhead;
	stk_msg_t* qtail;
} signal_queue_t;

typedef struct {
    task_id_t task_id;
    pf_task task_handler;
} task_t;

typedef struct {
	task_id_t id;
	uint8_t ability;
	pf_task_polling task_polling_handler;
} task_polling_t;

/* stk task functions */
extern void task_create(task_t* task_table_create);
extern void task_post(task_id_t task_id, stk_msg_t* msg);
extern void task_post_pure_msg(task_id_t des_task_id, uint8_t signal);

/* task polling functions */
extern void task_polling_create(task_polling_t* task_polling_table_create);
extern void task_polling_run();
extern void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability);

/* kernel start */
extern int task_run();

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */