/**
 ******************************************************************************
 * @author: Nark
 * @date:   28/08/2024
 ******************************************************************************
**/

#include "task_fw.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "io_cfg.h"
#include "sys_boot.h"
#include "sys_cfg.h"

#include "led.h"
#include "utils.h"
#include "flash.h"
#include "crc32.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_cloud.h"

/* checksum atrributes */
CRC32 fw_checksum;
uint32_t fw_checksum_ret;
uint32_t reversed_checksum;
uint8_t buffer_crc[1];

void task_fw_handler(stk_msg_t* msg) {
    switch (msg->sig) {
    case FW_UPDATE_CURRENT_FIRMWARE_INFO:
        APP_PRINT("[task_fw] FW_UPDATE_CURRENT_FIRMWARE_INFO\n");
        sys_boot_t app_sys_boot;
        sys_boot_get(&app_sys_boot);

        /* set current firmware info */
        app_sys_boot.current_fw_app_header.psk = FIRMWARE_PSK;
        app_sys_boot.current_fw_app_header.bin_len = system_info.flash_used;
        app_sys_boot.fw_app_cmd.cmd = SYS_FW_CMD_NONE;
        app_sys_boot.fw_app_cmd.container = SYS_FW_CONTAINER_EXTERNAL_FLASH;
        app_sys_boot.fw_app_cmd.io_driver = SYS_FW_IO_DRIVER_SPI;
        
        /* update firmware info */
        sys_boot_set(&app_sys_boot);
        break;
    
    case FW_SHOW_CURRENT_FIRMWARE_INFO:
        sys_boot_t show_current_fw_info;
        sys_boot_get(&show_current_fw_info);
        APP_PRINT("[task_fw] FW_SHOW_CURRENT_FIRMWARE_INFO\n");
        SYS_PRINT("\n");
        SYS_PRINT("[app_cr_fw] psk: 0x%02X\n", show_current_fw_info.current_fw_app_header.psk);
        SYS_PRINT("[app_cr_fw] bin len: %d\n", show_current_fw_info.current_fw_app_header.bin_len);
        SYS_PRINT("[app_cr_fw] checksum: 0x%08X\n", show_current_fw_info.current_fw_app_header.checksum);
        SYS_PRINT("[app_cr_fw] cmd: 0x%02X\n", show_current_fw_info.fw_app_cmd.cmd);
        SYS_PRINT("[app_cr_fw] container: 0x%02X\n", show_current_fw_info.fw_app_cmd.container);
        SYS_PRINT("[app_cr_fw] io driver: 0x%02X\n", show_current_fw_info.fw_app_cmd.io_driver);
        SYS_PRINT("\n");
        SYS_PRINT("[app_update_fw] psk: 0x%02X\n", show_current_fw_info.update_fw_app_header.psk);
        SYS_PRINT("[app_update_fw] bin len: %d\n", show_current_fw_info.update_fw_app_header.bin_len);
        SYS_PRINT("[app_update_fw] checksum: 0x%08X\n", show_current_fw_info.update_fw_app_header.checksum);
        SYS_PRINT("\n");
        SYS_PRINT("[boot_cr_fw] psk: 0x%02X\n", show_current_fw_info.current_fw_boot_header.psk);
        SYS_PRINT("[boot_cr_fw] bin len: %d\n", show_current_fw_info.current_fw_boot_header.bin_len);
        SYS_PRINT("[boot_cr_fw] checksum: 0x%08X\n", show_current_fw_info.current_fw_boot_header.checksum);
        SYS_PRINT("[boot_cr_fw] cmd: 0x%02X\n", show_current_fw_info.fw_boot_cmd.cmd);
        SYS_PRINT("[boot_cr_fw] container: 0x%02X\n", show_current_fw_info.fw_boot_cmd.container);
        SYS_PRINT("[boot_cr_fw] io driver: 0x%02X\n", show_current_fw_info.fw_boot_cmd.io_driver);
        SYS_PRINT("\n");
        break;

    case FW_PREPARE_REV_NEW_FIRMWARE:
        SYS_PRINT("\n");
        APP_PRINT("[task_fw] prepare writing new firmware to external flash\n");
        if (fota_frame_rev.bin_len > BLOCK_SIZE) {
            SYS_PRINT("[flash] erase two 64k memory blocks !\n");
            flash_erase_block_64k(APP_START_ADDR_EXTERNAL_FLASH);
            flash_erase_block_64k(APP_START_ADDR_EXTERNAL_FLASH + (BLOCK_SIZE * 1));
            SYS_PRINT("\n");
            SYS_PRINT("-> Starting download firmware from cloud\n");
            SYS_PRINT("\n");
            timer_set(TASK_CLOUD_ID, AC_MQTT_FOTA_REQ_CHUNK, 500, TIMER_ONE_SHOT);
        }
        else {
            flash_erase_block_64k(APP_START_ADDR_EXTERNAL_FLASH);
            SYS_PRINT("[flash] erase only block 64k !\n");
            SYS_PRINT("\n");
            SYS_PRINT("-> Starting download firmware from cloud\n");
            SYS_PRINT("\n");
            timer_set(TASK_CLOUD_ID, AC_MQTT_FOTA_REQ_CHUNK, 500, TIMER_ONE_SHOT);
        }
        break;

#if 0
    case FW_WRITE_TO_EXTERNAL_FLASH:
        if (fota_frame_rev.fw_remain >= CHUNK_SIZE) {
            flash_write(APP_START_ADDR_EXTERNAL_FLASH + fota_frame_rev.index_write_in_flash, (uint8_t*)&fota_frame_rev.payload[0], CHUNK_SIZE);
            /* update progress */
            network_mqtt_fota_print_progress((uint8_t)(((float)(fota_frame_rev.bin_len - fota_frame_rev.fw_remain) / (float)fota_frame_rev.bin_len) * 100.0));

            /* update attr for get new packet */
            fota_frame_rev.fw_remain -= CHUNK_SIZE;
            fota_frame_rev.index_write_in_flash += CHUNK_SIZE;
            fota_frame_rev.chunk_id++;
            task_post_pure_msg(TASK_CLOUD_ID, AC_MQTT_FOTA_REQ_CHUNK);
        }
        else {
            flash_write(APP_START_ADDR_EXTERNAL_FLASH + fota_frame_rev.index_write_in_flash, (uint8_t*)&fota_frame_rev.payload[0], fota_frame_rev.fw_remain);
            fota_frame_rev.index_write_in_flash += fota_frame_rev.fw_remain;
            fota_frame_rev.fw_remain = 0;

            /* update progress */
            network_mqtt_fota_print_progress((uint8_t)(((float)(fota_frame_rev.bin_len - fota_frame_rev.fw_remain) / (float)fota_frame_rev.bin_len) * 100.0));
            SYS_PRINT("\n");
            SYS_PRINT("\n");
            APP_PRINT("[task_fw] firmware writen to external flash\n");
            APP_PRINT("[task_fw] new firmware bin len: %d\n", fota_frame_rev.index_write_in_flash);
            task_post_pure_msg(TASK_FW_ID, FW_UPDATE_CHECKSUM);
        }
        break;
#endif  

    case FW_WRITE_TO_EXTERNAL_FLASH:
        if (fota_frame_rev.fw_remain != 0) {
            if (fota_frame_rev.fw_remain >= CHUNK_SIZE) {
                fota_frame_rev.fw_packet_len = CHUNK_SIZE;
                fota_frame_rev.fw_remain -= CHUNK_SIZE;
            }
            else {
                fota_frame_rev.fw_packet_len = fota_frame_rev.fw_remain;
                fota_frame_rev.fw_remain = 0;
            }
            
            /* writing to external flash */
            flash_write(APP_START_ADDR_EXTERNAL_FLASH + fota_frame_rev.index_write_in_flash, (uint8_t*)&fota_frame_rev.payload[0], fota_frame_rev.fw_packet_len);
            network_mqtt_fota_print_progress((uint8_t)(((float)(fota_frame_rev.bin_len - fota_frame_rev.fw_remain) / (float)fota_frame_rev.bin_len) * 100.0));
            
            /* request new packet */
            if (fota_frame_rev.fw_remain != 0) {
                fota_frame_rev.chunk_id++;
                fota_frame_rev.index_write_in_flash += CHUNK_SIZE;
                task_post_pure_msg(TASK_CLOUD_ID, AC_MQTT_FOTA_REQ_CHUNK);
            }
            /* end of packet */
            else {
                SYS_PRINT("\n");
                SYS_PRINT("\n");
                APP_PRINT("[task_fw] firmware writen to external flash\n");
                APP_PRINT("[task_fw] new firmware bin len: %d\n", (fota_frame_rev.index_write_in_flash += fota_frame_rev.fw_packet_len));
                task_post_pure_msg(TASK_FW_ID, FW_UPDATE_CHECKSUM);
            }
        }
        break;

    case FW_UPDATE_CHECKSUM:
        APP_PRINT("[task_fw] FW_UPDATE_CHECKSUM\n");
        fw_checksum.reset();
        for (uint32_t i = 0; i < (fota_frame_rev.bin_len); i++) {
            flash_read(APP_START_ADDR_EXTERNAL_FLASH + i, (uint8_t *)&buffer_crc[0], 1);
            fw_checksum.update(&buffer_crc[0], 1);
        }
        fw_checksum_ret = fw_checksum.finalize();

        reversed_checksum = ((fw_checksum_ret & 0xFF000000) >> 24) | ((fw_checksum_ret & 0x00FF0000) >> 8) | ((fw_checksum_ret & 0x0000FF00) << 8) | ((fw_checksum_ret & 0x000000FF) << 24);
        APP_PRINT("fw checksum: 0x%08X\n", reversed_checksum);
        
        if (reversed_checksum == fota_frame_rev.check_sum) {
            APP_PRINT("[task_fw] firmware checksum correctly !\n");
            sys_boot_t get_fw_info;
            sys_boot_get(&get_fw_info);
            get_fw_info.update_fw_app_header.psk = FIRMWARE_PSK;
            get_fw_info.update_fw_app_header.bin_len = fota_frame_rev.bin_len;
            get_fw_info.update_fw_app_header.checksum = fota_frame_rev.check_sum;
            sys_boot_set(&get_fw_info);
        }
        else {
            APP_PRINT("[task_fw] firmware checksum incorrectly !\n");
        }
        break;

    case FW_FIRMWARE_UPDATE_REQUEST:
        APP_PRINT("[task_fw] FW_FIRMWARE_UPDATE_REQUEST\n");
        sys_boot_t app_sys_boot_get;
        sys_boot_get(&app_sys_boot_get);
        if ((app_sys_boot_get.update_fw_app_header.bin_len != 0) && (app_sys_boot_get.update_fw_app_header.checksum != 0)) {
            APP_PRINT("[task_fw] new firmware update available !\n");
            task_post_pure_msg(TASK_FW_ID, FW_JUMP_TO_BOOT);
        }
        else {
            APP_PRINT("[task_fw] no new firmware updates are available !\n");
        }
        break;

    case FW_JUMP_TO_BOOT:
        /* set state firmware update */
        sys_boot_t get_fw_info;
        sys_boot_get(&get_fw_info);
        get_fw_info.fw_app_cmd.cmd = SYS_FW_CMD_UPDATE_REQ;
        sys_boot_set(&get_fw_info);

        SYS_PRINT("\n");
        SYS_PRINT("jump to boot !\n");
        SYS_PRINT("\n");

        /* jump to boot */
        sys_ctrl_reset();
        break;

    default:
        break;
    }
}