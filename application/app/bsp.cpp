/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "bsp.h"

#include "io_cfg.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

button_t button_mode;
button_t button_up;
button_t button_down;

void button_up_callback(void* _button) {

    button_t* button = (button_t*)(_button);

    switch (button->state) {
    case BUTTON_STATE_PRESSED:
        APP_PRINT("[button_callback] BUTTON_UP_PRESSED\n");
        break;

    case BUTTON_STATE_LONG_PRESSED:
        APP_PRINT("[button_callback] BUTTON_UP_LONG_PRESSED\n");
        break;

    case BUTTON_STATE_RELEASED:
        APP_PRINT("[button_callback] BUTTON_UP_RELEASED\n");
        break;

    default:
        break;
    }
}

void button_down_callback(void* _button) {

    button_t* button = (button_t*)(_button);

    switch (button->state) {
    case BUTTON_STATE_PRESSED:
        APP_PRINT("[button_callback] BUTTON_DOWN_PRESSED\n");
        break;

    case BUTTON_STATE_LONG_PRESSED:
        APP_PRINT("[button_callback] BUTTON_DOWN_LONG_PRESSED\n");
        break;

    case BUTTON_STATE_RELEASED:
        APP_PRINT("[button_callback] BUTTON_DOWN_RELEASED\n");
        break;

    default:
        break;
    }
}

void button_mode_callback(void* _button) {

    button_t* button = (button_t*)(_button);

    switch (button->state) {
    case BUTTON_STATE_PRESSED:
        APP_PRINT("[button_callback] BUTTON_MODE_PRESSED\n");
        break;

    case BUTTON_STATE_LONG_PRESSED:
        APP_PRINT("[button_callback] BUTTON_MODE_LONG_PRESSED\n");
        break;

    case BUTTON_STATE_RELEASED:
        APP_PRINT("[button_callback] BUTTON_MODE_RELEASED\n");
        break;

    default:
        break;
    }
}