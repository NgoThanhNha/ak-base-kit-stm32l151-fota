/**
 ******************************************************************************
 * @author: Nark
 * @date:   02/04/2024
 ******************************************************************************
**/

#include "screen_manager.h"

#include "stk.h"
#include "message.h"
#include "platform.h"

#include "console.h"

static stk_msg_t screen_msg_entry;
static scr_mng_t* screen_manager = SCREEN_MANAGER_NULL;

void scr_mng_init(scr_mng_t* screen, pf_screen_handler init_screen) {
    screen_msg_entry.sig = SCREEN_ENTRY;
    screen_manager = screen;
    screen_manager->current_screen_display = init_screen;
    screen_manager->current_screen_display(&screen_msg_entry);
}

void screen_mng_dispatch(stk_msg_t* msg) {
    if (screen_manager == SCREEN_MANAGER_NULL) {
        FATAL("SCREEN_MANAGER", 0x01);
    }
    else {
        screen_manager->current_screen_display(msg);
    }
}

void scr_mng_trans(pf_screen_handler target_screen) {
    if (screen_manager == SCREEN_MANAGER_NULL) {
        FATAL("SCREEN_MANAGER", 0x02);
    }
    else {
        screen_manager->current_screen_display = target_screen;
        screen_manager->current_screen_display(&screen_msg_entry);
    }
}