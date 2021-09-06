#include "i2c_esp32_static.h"

static void static_i2c_cmd_link_append(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk) {
    i2c_cmd_desc_t* cmd_desc = (i2c_cmd_desc_t*) cmd_handle;
    if (cmd_desc->head == NULL) {
        cmd_desc->head = lnk;
        cmd_desc->cur = lnk;
    } else {
        cmd_desc->cur->next = lnk;
        cmd_desc->cur = cmd_desc->cur->next;
    }
    cmd_desc->cur->next = NULL;
}

void static_i2c_master_start(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk) {
    lnk->cmd.hw_cmd.ack_en = 0;
    lnk->cmd.hw_cmd.ack_exp = 0;
    lnk->cmd.hw_cmd.ack_val = 0;
    lnk->cmd.hw_cmd.op_code = I2C_CMD_RESTART;
    lnk->cmd.hw_cmd.byte_num = 0;
    lnk->cmd.data = NULL;
    static_i2c_cmd_link_append(cmd_handle, lnk);
}

void static_i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, i2c_ack_type_t ack) {
    lnk->cmd.hw_cmd.ack_en = 0;
    lnk->cmd.hw_cmd.ack_exp = 0;
    lnk->cmd.hw_cmd.ack_val = ((ack == I2C_MASTER_LAST_NACK) ? I2C_MASTER_NACK : (ack & 0x1));
    lnk->cmd.hw_cmd.byte_num = 1;
    lnk->cmd.hw_cmd.op_code = I2C_CMD_READ;
    lnk->cmd.data = data;
    static_i2c_cmd_link_append(cmd_handle, lnk);
}

void static_i2c_master_read(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, size_t data_len, i2c_ack_type_t ack) {
    for (size_t i = 0; i < data_len-1; i++) {
        static_i2c_master_read_byte(cmd_handle, lnk, &data[i], ack == I2C_MASTER_LAST_NACK ? I2C_MASTER_ACK : ack);
        lnk++;
    }
    static_i2c_master_read_byte(cmd_handle, lnk, &data[data_len-1], ack == I2C_MASTER_LAST_NACK ? I2C_MASTER_NACK : ack);
}

void static_i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t data, bool ack_en) {
    lnk->cmd.hw_cmd.ack_en = ack_en;
    lnk->cmd.hw_cmd.ack_exp = 0;
    lnk->cmd.hw_cmd.ack_val = 0;
    lnk->cmd.hw_cmd.op_code = I2C_CMD_WRITE;
    lnk->cmd.hw_cmd.byte_num = 1;
    lnk->cmd.data = NULL;
    lnk->cmd.byte_cmd = data;
    static_i2c_cmd_link_append(cmd_handle, lnk);
}

void static_i2c_master_write(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk, uint8_t* data, size_t data_len, bool ack_en) {
    for (size_t i = 0; i < data_len; i++) {
        static_i2c_master_write_byte(cmd_handle, lnk, data[i], ack_en);
        lnk++;
    }
}

void static_i2c_master_stop(i2c_cmd_handle_t cmd_handle, i2c_cmd_link_t* lnk) {
    lnk->cmd.hw_cmd.ack_en = 0;
    lnk->cmd.hw_cmd.ack_exp = 0;
    lnk->cmd.hw_cmd.ack_val = 0;
    lnk->cmd.hw_cmd.op_code = I2C_CMD_STOP;
    lnk->cmd.hw_cmd.byte_num = 0;
    lnk->cmd.data = NULL;
    static_i2c_cmd_link_append(cmd_handle, lnk);
}