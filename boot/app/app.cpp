/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#include "app.h"

/* peripheral includes */
#include "io_cfg.h"

/* system includes */
#include "sys_cfg.h"
#include "sys_boot.h"
#include "flash.h"

/* driver includes */
#include "led.h"
#include "app_dbg.h"

/* system common functions */
static void boot_jump_to_app_request();
static void boot_update_fw_info();

int boot_main() {
    SYS_PRINT("\n\n");
    SYS_PRINT("[boot] start boot\n");
    SYS_PRINT("[boot] version: %s\n", BOOT_VER);

    /******************************************************************************
    * hardware configure
    *******************************************************************************/
    io_init();
    led_init_func(&led_life, led_life_on, led_life_off, 10);
    led_blink_set(&led_life, 250);
    
    /******************************************************************************
    * boot share flash partition
    *******************************************************************************/
    sys_boot_t app_sys_boot;
    sys_boot_init();
    sys_boot_get(&app_sys_boot);

    /******************************************************************************
    * boot
    *******************************************************************************/
    /* normal mode */
    if ((app_sys_boot.fw_app_cmd.cmd == SYS_FW_CMD_NONE) && (app_sys_boot.current_fw_app_header.psk) == FIRMWARE_PSK) {
        SYS_PRINT("[boot] normal status\n");
        SYS_PRINT("[boot] start application !\n");
        boot_jump_to_app_request();
    }
    /* firmware update request */
    else if ((app_sys_boot.fw_app_cmd.cmd == SYS_FW_CMD_UPDATE_REQ) && \
             (app_sys_boot.fw_app_cmd.container == SYS_FW_CONTAINER_EXTERNAL_FLASH) && \
             (app_sys_boot.update_fw_app_header.psk == FIRMWARE_PSK) && \
             (app_sys_boot.update_fw_app_header.bin_len != 0) && \
             (app_sys_boot.update_fw_app_header.checksum != 0)) {

        /* read firmware info from share boot */
        SYS_PRINT("[boot] application firmware update request\n");
        SYS_PRINT("\n");
        SYS_PRINT("firmware bin length: %d\n", app_sys_boot.update_fw_app_header.bin_len);
        SYS_PRINT("firmware checksum: 0x%08X\n", app_sys_boot.update_fw_app_header.checksum);

        /* unclock internal flash */
        FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
						FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);

        /* erase application internal flash, prepare for new firmware */
        SYS_PRINT("[boot] erase application internal flash \n");
        internal_flash_erase_pages(APP_START_ADDR, app_sys_boot.update_fw_app_header.bin_len);

        /* copy data from external flash to internal flash */
        SYS_PRINT("[boot] copy firmware from external flash to internal flash\n");
        led_blink_set(&led_life, 100);

        uint8_t halfpage_buffer[128]; /* halfpage external flash buffer */
		uint32_t external_fw_remain = app_sys_boot.update_fw_app_header.bin_len; /* remain firmware */
        uint32_t external_fw_packet_len = 0; /* size of each packet reading from external flash */
		uint32_t external_fw_read_index = 0; /* index for read fw from external flash */
        uint32_t external_flash_status = 0;

        while (external_fw_remain != 0) {
            if (external_fw_remain >= PACKET_SIZE_COPY) {
                external_fw_packet_len = PACKET_SIZE_COPY;
                external_fw_remain -= PACKET_SIZE_COPY;
            }
            else {
                external_fw_packet_len = external_fw_remain;
                external_fw_remain = 0;
            }

            /* read fw packet from external flash */
			memset(halfpage_buffer,0 , 128);
			flash_read(APP_START_ADDR_EXTERNAL_FLASH + external_fw_read_index, halfpage_buffer, external_fw_packet_len);
            external_flash_status = FLASH_BUSY;

            /* write to internal flash */
            __disable_irq();
            external_flash_status = FLASH_ProgramHalfPage(APP_START_ADDR + external_fw_read_index, (uint32_t*)halfpage_buffer);
            __enable_irq();

			if (external_flash_status == FLASH_COMPLETE) {
                external_fw_read_index += PACKET_SIZE_COPY;
			}
			else {
				FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
								FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
			}
        }

        /* reset status to normal mode */
        app_sys_boot.fw_app_cmd.cmd = SYS_FW_CMD_NONE;

        /* reset status firmware update info */
        app_sys_boot.update_fw_app_header.bin_len = 0;
        app_sys_boot.current_fw_app_header.checksum = app_sys_boot.update_fw_app_header.checksum;
        app_sys_boot.update_fw_app_header.checksum = 0;
        app_sys_boot.update_fw_app_header.psk = 0x1A2B3C4D; /* magic number */
        sys_boot_set(&app_sys_boot);

        SYS_PRINT("[boot] writen to internal flash\n");
        SYS_PRINT("\n");
        SYS_PRINT("\n");
        boot_jump_to_app_request();
    }
    /* exception status */
    else {
        SYS_PRINT("[boot] exception status\n");
		SYS_PRINT("[boot] start application !\n");
        boot_jump_to_app_request();
    }
    
    return 0;
}

/*****************************************************************************/
/* system common function
 */
/*****************************************************************************/
void boot_jump_to_app_request() {
    boot_update_fw_info();
	sys_ctrl_jump_to_app_req = SYS_CTRL_JUMP_TO_APP_REQ;
	sys_ctrl_reset();
}

void boot_update_fw_info() {
    sys_boot_t app_sys_boot_get;
    sys_boot_get(&app_sys_boot_get);
    app_sys_boot_get.fw_boot_cmd.cmd = SYS_FW_CMD_NONE;
    app_sys_boot_get.fw_boot_cmd.container = SYS_FW_CONTAINER_INTERNAL_FLASH;
    app_sys_boot_get.fw_boot_cmd.io_driver = SYS_FW_IO_DRIVER_NONE;
    
    /* boot fw checksum */
    app_sys_boot_get.current_fw_boot_header.psk = FIRMWARE_PSK;
    uint32_t boot_bin_len = system_info.flash_used;
    if (app_sys_boot_get.current_fw_boot_header.bin_len != boot_bin_len) {
        app_sys_boot_get.current_fw_boot_header.bin_len = boot_bin_len;
    }
    /* TODO: boot fw checksum */
    sys_boot_set(&app_sys_boot_get);
}

/*****************************************************************************/
/* app common function
 */
/*****************************************************************************/

/* hardware timer interrupt 10ms
 * used for led, button polling
*/
void sys_irq_timer_10ms() {
    led_polling(&led_life);
}