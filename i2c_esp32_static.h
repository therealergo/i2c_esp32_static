#ifndef _STATIC_ESP_I2C_H_
#define _STATIC_ESP_I2C_H_

#include "driver/i2c.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint32_t byte_num:    8,
                 ack_en:      1,
                 ack_exp:     1,
                 ack_val:     1,
                 op_code:     3,
                 reserved14: 17,
                 done:        1;
    };
    uint32_t val;
} i2c_hw_cmd_t;

typedef struct {
    i2c_hw_cmd_t hw_cmd;
    uint8_t *data;
    uint8_t byte_cmd;
} i2c_cmd_t;

typedef struct i2c_cmd_link {
    i2c_cmd_t cmd;
    struct i2c_cmd_link *next;
} i2c_cmd_link_t;

typedef struct {
    i2c_cmd_link_t *head;
    i2c_cmd_link_t *cur;
    i2c_cmd_link_t *free;
} i2c_cmd_desc_t;

void static_i2c_master_start(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk);
void static_i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, i2c_ack_type_t ack);
void static_i2c_master_read(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, size_t data_len, i2c_ack_type_t ack);
void static_i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t data, bool ack_en);
void static_i2c_master_write(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, size_t data_len, bool ack_en);
void static_i2c_master_stop(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk);

#ifdef __cplusplus
}
#endif

#endif