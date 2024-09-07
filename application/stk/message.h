/**
 ******************************************************************************
 * @author: Nark
 * @date:   03/05/2024
 ******************************************************************************
**/

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdio.h>
#include <stdint.h>

#define STK_PURE_MSG_POOL_SIZE          (16)
#define STK_MSG_NULL 					((stk_msg_t*)0)

typedef struct stk_msg_t {
    /* message management - private for kernel */
    struct stk_msg_t* next;
	uint8_t msg_type;

    /* task header - public for user application*/
    uint8_t des_task_id;
    uint8_t sig;
} stk_msg_t;

/*****************************************************************************
 * DEFINITION: message
 *****************************************************************************/
#define PURE_MSG_TYPE 					(0x01)

extern void msg_init();
extern void free_msg(stk_msg_t* msg_free);

/*****************************************************************************
 * DEFINITION: pure message
 *****************************************************************************/
typedef struct {
	stk_msg_t msg_header;
} stk_pure_msg_t;

extern stk_msg_t* get_pure_msg();
extern uint8_t get_pure_msg_free();

#ifdef __cplusplus
}
#endif

#endif /* __MESSAGE_H__ */