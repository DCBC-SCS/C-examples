#ifndef I2C_API_H
#define I2C_API_H

#include <stdint.h>

typedef uint8_t u8;

int open_i2c();
int read_byte(int fd, u8 dev_addr, u8 reg_addr);
int write_byte(int fd, u8 dev_addr, u8 reg_addr, u8 value);

#endif
