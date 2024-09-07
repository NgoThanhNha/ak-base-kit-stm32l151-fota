/**
 ******************************************************************************
 * @author: Nark
 * @date:   03/05/2024
 ******************************************************************************
**/

#include "task.h"
#include "message.h"

#include "platform.h"
#include "app_dbg.h"

static stk_pure_msg_t msg_pure_pool[STK_PURE_MSG_POOL_SIZE];
static stk_msg_t* free_list_pure_msg_pool;
static uint32_t free_list_pure_used;

static void pure_msg_init();

void msg_init() {
    pure_msg_init();
}

void pure_msg_init() {

    ENTRY_CRITICAL();

    free_list_pure_msg_pool = (stk_msg_t*)msg_pure_pool;
     
    for (uint32_t index = 0; index < STK_PURE_MSG_POOL_SIZE; index++) {
        msg_pure_pool[index].msg_header.msg_type = PURE_MSG_TYPE;
        if (index == (STK_PURE_MSG_POOL_SIZE - 1)) {
            msg_pure_pool[index].msg_header.next = STK_MSG_NULL;
        }
        else {
            msg_pure_pool[index].msg_header.next = (stk_msg_t*)&msg_pure_pool[index + 1];
        }
    }

    free_list_pure_used = 0;

    EXIT_CRITICAL();
}

stk_msg_t* get_pure_msg() {
    stk_msg_t* message_allocated;

    ENTRY_CRITICAL();

    /* assign the free_msg_list to new allocate message */
    message_allocated = free_list_pure_msg_pool;

    if (message_allocated == STK_MSG_NULL) {
        FATAL("MSG", 0x01);
    }
    else {
        /* move the free_msg_list to new position */
        free_list_pure_msg_pool = message_allocated->next;
        free_list_pure_used++;
    }

    EXIT_CRITICAL();

    return message_allocated;
}

void free_pure_msg(stk_msg_t* pure_msg_free) {
    ENTRY_CRITICAL();

    /* move the current message to front free_msg_list */
    pure_msg_free->next = free_list_pure_msg_pool;

    /* move the free_msg_list to head */
    free_list_pure_msg_pool = pure_msg_free;

    free_list_pure_used--;

    EXIT_CRITICAL();
}

uint8_t get_pure_msg_free() {
    return (STK_PURE_MSG_POOL_SIZE - free_list_pure_used);
}

void free_msg(stk_msg_t* msg_free) {
    switch (msg_free->msg_type) {
    case PURE_MSG_TYPE:
        free_pure_msg(msg_free);
        break;
    
    default:
        break;
    }
}