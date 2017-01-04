#include "unistd.h"
#include "math.h"
#include "stdint.h"

// From common.c
void hal_gpio_on(void);
void hal_gpio_off(void);
void hal_gpio_init(void);
int hal_i2c_init(int devID);
void hal_i2c_deinit(void);
void hal_i2c_write(int fd, uint8_t reg, uint8_t value);
uint8_t hal_i2c_read(int fd, uint8_t reg);
void hal_i2c_BulkRead(int fd, uint8_t reg, uint8_t len, uint8_t *buf);
int32_t magnitude(int32_t x, int32_t y, int32_t z);
int32_t findArctan(double x, double y);
int32_t upness(int32_t x, int32_t y, int32_t z);
double toAngle(int x);

// From accel.c
void accel_init(void);
void accel_read(void);
int32_t accel_x(void);
int32_t accel_y(void);
int32_t accel_z(void);

// From baro3115.c
void baro_init(void);
void turnOnBaro(void);
void turnOffBaro(void);
int32_t baro_alt(void);
int32_t baro_press(void);
int32_t baro_temp(void);

// From mag3110.c
void  mag_init(void);
int32_t magnet_x();
int32_t magnet_y();
int32_t magnet_z();
int32_t mag_compass(int32_t pitch, int32_t roll);


