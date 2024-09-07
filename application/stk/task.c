/**
 ******************************************************************************
 * @author: Nark
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "task.h"
#include "message.h"

#include "sys_cfg.h"
#include "platform.h"

#include "app_dbg.h"
#include "task_list.h"

/* stk tasks */
static task_t* task_table = (task_t*)0;
static uint8_t task_table_size = 0;

/* polling task */
static task_polling_t* task_polling_table = (task_polling_t*)0;
static uint8_t task_polling_table_size = 0;

/* event mail box */
static signal_queue_t event_signal;

void task_create(task_t* task_table_create) {

    task_table = task_table_create;
    task_table_size = 0;

    while (task_table[task_table_size].task_id != STK_TASK_EOT_ID) {
        task_table_size++;
    }
    
    event_signal.qhead = STK_MSG_NULL;
    event_signal.qtail = STK_MSG_NULL;

    SYS_PRINT("[task run] stk tasks: %d\n", task_table_size);
}

void task_post(task_id_t task_id, stk_msg_t* msg) {
    if (task_id >= task_table_size) {
        FATAL("TASK", 0x01);
    }
    else {
        ENTRY_CRITICAL();

        /* assign the task_id to message */
        msg->next = STK_MSG_NULL;
        msg->des_task_id = task_id;

        /* put message to queue */
        if (event_signal.qtail == STK_MSG_NULL) {
            event_signal.qhead = msg;
            event_signal.qtail = msg;
        }
        else {
            event_signal.qtail->next = msg;
            event_signal.qtail = msg;
        }

        EXIT_CRITICAL();
    }
}

void task_post_pure_msg(task_id_t des_task_id, uint8_t signal) {
    stk_msg_t* get_pure_msg_from_pool = get_pure_msg();
    get_pure_msg_from_pool->sig = signal;
    task_post(des_task_id, get_pure_msg_from_pool);
}

void task_scheduler() {
    while (event_signal.qhead != STK_MSG_NULL) {

        ENTRY_CRITICAL();

        /* get msg from queue */
        stk_msg_t* get_msg = event_signal.qhead;

        /* move head queue to new position */
        event_signal.qhead = get_msg->next;

        if (get_msg->next == STK_MSG_NULL) {
            event_signal.qtail = STK_MSG_NULL;
        }
        
        EXIT_CRITICAL();

        /* execute task */
        task_table[get_msg->des_task_id].task_handler(get_msg);

        /* free message */
        free_msg(get_msg);
    }
}

void task_polling_create(task_polling_t* task_polling_table_create) {
	uint8_t index = 0;
	if (task_polling_table_create) {
		task_polling_table = task_polling_table_create;
		while (task_polling_table_create[index].id != STK_TASK_POLLING_EOT_ID) {
			index++;
		}
		task_polling_table_size = index;
        SYS_PRINT("[task run] polling tasks: %d\n", task_polling_table_size);
        SYS_PRINT("\n");
        SYS_PRINT("-> Active Objects ready !\n");
        SYS_PRINT("\n");
	}
	else {
		FATAL("TASK", 0x02);
	}
}

void task_polling_run() {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < STK_TASK_POLLING_EOT_ID) {

		ENTRY_CRITICAL();
		if (__task_polling_table->ability == STK_ENABLE) {

			EXIT_CRITICAL();
			__task_polling_table->task_polling_handler();
		}
		else {
			EXIT_CRITICAL();
		}
		__task_polling_table++;
	}
}

void task_polling_set_ability(task_id_t task_polling_id, uint8_t ability) {
	task_polling_t* __task_polling_table = task_polling_table;

	while (__task_polling_table->id < STK_TASK_POLLING_EOT_ID) {

		if (__task_polling_table->id == task_polling_id) {

			ENTRY_CRITICAL();

			__task_polling_table->ability = ability;

			EXIT_CRITICAL();

			break;
		}

		__task_polling_table++;
	}

	if (__task_polling_table->id == STK_TASK_POLLING_EOT_ID) {
		FATAL("TASK", 0x03);
	}
}

int task_run() {
    for (;;) {
        task_scheduler();
        task_polling_run();
    }
}