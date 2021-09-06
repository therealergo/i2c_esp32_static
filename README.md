# I2C-ESP32-Static
The ESP32's I2C master driver only supports dynamic allocation. This can be a serious problem for systems that need high throughput, reliability, or tight timing. While Espressif is working on a static-allocation-capable version of their I2C driver, this component allows for that functionality right now, even in older versions of ESP-IDF. We've tested everything with `release/v4.3`.

# Installation
Include `i2c_esp32_static` as a component in your ESP32 system.

# Usage
You need to perform the following three steps:
1. Statically allocate an array of `i2c_cmd_link` big enough to hold each of your I2C transactions.
2. Fill each of those command links with an I2C transaction. For each `i2c_master_**` function in the driver, there is a `static_i2c_master_**` equivalent. You will need one `i2c_cmd_link` for each `static_i2c_master_**` call.
3. Run the transaction (using the regular `i2c_master_cmd_begin` function).

# Example
Let's say that you had the following I2C transaction, for a simple single-byte repeated-start read:
```
i2c_cmd_handle_t cmd = i2c_cmd_link_create();
err = i2c_master_start(cmd);
ESP_ERROR_CHECK(err);
err = i2c_master_write_byte(cmd, (i2cAddr << 1) | I2C_MASTER_WRITE, true);
ESP_ERROR_CHECK(err);
err = i2c_master_write_byte(cmd, reg | 0x80, true);
ESP_ERROR_CHECK(err);
err = i2c_master_start(cmd);
ESP_ERROR_CHECK(err);
err = i2c_master_write_byte(cmd, (i2cAddr << 1) | I2C_MASTER_READ, true);
ESP_ERROR_CHECK(err);
err = i2c_master_read_byte(cmd, &val, I2C_MASTER_NACK);
ESP_ERROR_CHECK(err);
err = i2c_master_stop(cmd);
ESP_ERROR_CHECK(err);
err = i2c_master_cmd_begin(I2C_PORT, cmd, 250);
ESP_ERROR_CHECK(err);
i2c_cmd_link_delete(cmd);
```
When converted to use I2C-ESP32-Static, we get:
```
i2c_cmd_desc_t cmd = {};
i2c_cmd_handle_t h_cmd = &cmd;
i2c_cmd_link links[7];
int i_len = 0;

static_i2c_master_start     (h_cmd, &links[i_len++]);
static_i2c_master_write_byte(h_cmd, &links[i_len++], (i2cAddr << 1) | I2C_MASTER_WRITE, true);
static_i2c_master_write_byte(h_cmd, &links[i_len++], reg | 0x80, true);
static_i2c_master_start     (h_cmd, &links[i_len++]);
static_i2c_master_write_byte(h_cmd, &links[i_len++], (i2cAddr << 1) | I2C_MASTER_READ, true);
static_i2c_master_read_byte (h_cmd, &links[i_len++], &val, I2C_MASTER_NACK);
static_i2c_master_stop      (h_cmd, &links[i_len++]);
err = i2c_master_cmd_begin(I2C_PORT, h_cmd, 250);
ESP_ERROR_CHECK(err);
```
We've used `i_len++` to assign each I2C transaction an element in the `links` array, but notice how there are `7` elements in the array and `7` corresponding calls to `static_i2c_master_**`.

# But what about i2c_master_read/i2c_master_write?
In the standard ESP32 I2C driver, `i2c_master_read` and `i2c_master_write` are simply wrappers that call `i2c_master_read_byte` and `i2c_master_write_byte` multiple times. Take care to assign links into the array appropriately when using their static equivalents. For example, if `data_len==4` is passed into `static_i2c_master_read` it will consume the proceeding `4` indices in the supplied link array.
