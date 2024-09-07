/**
 ******************************************************************************
 * @author: Nark
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "timer.h"

#include "platform.h"
#include "task_list.h"

#include "app_dbg.h"

/* define message data is transfered between interrupt heart beat and timer task */
struct stk_timer_payload_irq_t {
	uint32_t counter;
	uint32_t enable_post_msg;
};

static volatile struct stk_timer_payload_irq_t stk_timer_payload_irq = {0, STK_DISABLE};

/* data to manage memory of timer message */
static stk_timer_t timer_pool[STK_TIMER_POOL_SIZE];
static stk_timer_t* free_list_timer_pool;
static uint32_t free_list_timer_used;
static stk_timer_t* timer_list_head;

/* allocate/free memory of timer message */
static void timer_pool_init();
static stk_timer_t* get_timer_msg();
static void free_timer_msg(stk_timer_t* timer_msg);

void timer_init() {
    timer_pool_init();

    ENTRY_CRITICAL();

    stk_timer_payload_irq.counter = 0;
    stk_timer_payload_irq.enable_post_msg = STK_ENABLE;

    EXIT_CRITICAL();
}

void timer_pool_init() {
    ENTRY_CRITICAL();

    free_list_timer_pool = (stk_timer_t*)timer_pool;
    timer_list_head = STK_TIMER_NULL;

    for (uint32_t index = 0; index < STK_TIMER_POOL_SIZE; index++) {
        if (index == (STK_TIMER_POOL_SIZE - 1)) {
            timer_pool[index].next = STK_TIMER_NULL;
        }
        else {
            timer_pool[index].next = (stk_timer_t*)&timer_pool[index + 1];
        }
    }

    free_list_timer_used = 0;

    EXIT_CRITICAL();
}

stk_timer_t* get_timer_msg() {
    stk_timer_t* timer_pool_allocated;

    ENTRY_CRITICAL();

    timer_pool_allocated = free_list_timer_pool;

    if (timer_pool_allocated == STK_TIMER_NULL) {
        FATAL("TIMER", 0x01);
    }
    else {
        free_list_timer_pool = timer_pool_allocated->next;
        free_list_timer_used++;
    }

    EXIT_CRITICAL();

    return timer_pool_allocated;
}

void free_timer_msg(stk_timer_t* timer_msg) {
    ENTRY_CRITICAL();

    timer_msg->next = free_list_timer_pool;
    free_list_timer_pool = timer_msg;

    free_list_timer_used--;

    EXIT_CRITICAL();
}

uint8_t get_timer_pool_free() {
    return (STK_TIMER_POOL_SIZE - free_list_timer_used);
}

uint8_t timer_set(task_id_t des_task_id, timer_sig_t sig, uint32_t duty, timer_type_t timer_type) {
    stk_timer_t* timer_msg;

    ENTRY_CRITICAL();

    /* move the timer_msg to head timer list */
    timer_msg = timer_list_head;

    /* find the previous same timer_msg in the timer list */
    while (timer_msg != STK_TIMER_NULL) {
		if ((timer_msg->des_task_id == des_task_id) && (timer_msg->sig == sig)) {

			timer_msg->counter = duty;

			EXIT_CRITICAL();

            return TIMER_OK;
		}
		else {
			timer_msg = timer_msg->next;
		}
	}

    /* create the new node timer_msg */
    timer_msg = get_timer_msg();

    timer_msg->des_task_id = des_task_id;
    timer_msg->sig = sig;
    timer_msg->counter = duty;

    if (timer_type == TIMER_PERIODIC) {
        timer_msg->period = duty;
    }
    else {
        timer_msg->period = 0;
    }

    /* put the new node timer_msg to timer list */
    if (timer_list_head == STK_TIMER_NULL) {
        timer_msg->next = STK_TIMER_NULL;
        timer_list_head = timer_msg;
    }
    else {
        timer_msg->next = timer_list_head;
        timer_list_head = timer_msg;
    }

    EXIT_CRITICAL();

    return TIMER_OK;
}

uint8_t timer_remove(task_id_t des_task_id, timer_sig_t sig) {
    stk_timer_t* current_timer_msg_ptr;
    stk_timer_t* previous_timer_msg_ptr;

    ENTRY_CRITICAL();

    /* move the current_timer_msg to head timer list */
    current_timer_msg_ptr = timer_list_head;
    previous_timer_msg_ptr = timer_list_head;

    /* find the same timer_msg in timer list */
    while (current_timer_msg_ptr != STK_TIMER_NULL) {
        if ((current_timer_msg_ptr->des_task_id == des_task_id) && (current_timer_msg_ptr->sig) == sig) {

            if (current_timer_msg_ptr == timer_list_head) {
                timer_list_head = current_timer_msg_ptr->next;
            }
            else {
                /* remove the current_timer_msg from timer list */
                previous_timer_msg_ptr->next = current_timer_msg_ptr->next;
            }

            /* return the current_timer_msg to pool timer */
            free_timer_msg(current_timer_msg_ptr);

            EXIT_CRITICAL();

            return TIMER_OK;
        }
        else {
            previous_timer_msg_ptr = current_timer_msg_ptr;
            current_timer_msg_ptr = current_timer_msg_ptr->next;
        }
    }

    EXIT_CRITICAL();

    return TIMER_ERROR;  
}

void timer_tick(uint32_t tick) {
    if (timer_list_head != STK_TIMER_NULL) {

        stk_timer_payload_irq.counter += tick;

        if (stk_timer_payload_irq.enable_post_msg == STK_ENABLE) {
            stk_timer_payload_irq.enable_post_msg = STK_DISABLE;
            task_post_pure_msg(TASK_TIMER_TICK_ID, TIMER_TICK_SIG);
        }
    }
}

void task_timer_handler(stk_msg_t* msg) {
    stk_timer_t* current_timer_msg;
    stk_timer_t* timer_msg_del = STK_TIMER_NULL; /* must-be assign STK_TIMER_NULL */

    uint32_t irq_counter;
    uint32_t get_counter;

    ENTRY_CRITICAL();

    current_timer_msg = timer_list_head;
    irq_counter = stk_timer_payload_irq.counter;

    stk_timer_payload_irq.counter = 0;
    stk_timer_payload_irq.enable_post_msg = STK_ENABLE;

    EXIT_CRITICAL();

    switch (msg->sig) {
    case TIMER_TICK_SIG:
        while (current_timer_msg != STK_TIMER_NULL) {
            ENTRY_CRITICAL();
            
            if (irq_counter < (current_timer_msg->counter)) {
                current_timer_msg->counter -= irq_counter;
            }
            else {
                current_timer_msg->counter = 0;
            }
            
            get_counter = current_timer_msg->counter;

            EXIT_CRITICAL();

            if (get_counter == 0) {
                task_post_pure_msg(current_timer_msg->des_task_id, current_timer_msg->sig); 

                ENTRY_CRITICAL();

                if (current_timer_msg->period) {
                    current_timer_msg->counter = current_timer_msg->period;
                }
                else {
                    timer_msg_del = current_timer_msg;
                }

                EXIT_CRITICAL();
            }

            current_timer_msg = current_timer_msg->next;

            if (timer_msg_del) {
                timer_remove(timer_msg_del->des_task_id, timer_msg_del->sig);
                timer_msg_del = STK_TIMER_NULL;
            }
        }
        break;

    default:
        break;
    }
}