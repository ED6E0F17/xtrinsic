// Copyright John Greb 2017, MIT License
#include "xtrinsic.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"

#define MMA_GPIO 6
void hal_gpio_on() {
	digitalWrite( MMA_GPIO, 1 );
}

void hal_gpio_off() {
	digitalWrite( MMA_GPIO, 0 );
}

void hal_gpio_init() {
	if ( wiringPiSetup() < 0 ) {
		return;
	}
	pinMode( MMA_GPIO, OUTPUT );
	digitalWrite( MMA_GPIO, 0 );
}

int hal_i2c_init( int devID ) {
	if ( wiringPiSetup() < 0 ) {
		return -1;
	}
	pinMode( MMA_GPIO, OUTPUT );
	hal_gpio_off();
	return wiringPiI2CSetup( devID );
}

void hal_i2c_deinit() {
}

void hal_i2c_write( int fd, uint8_t reg, uint8_t value ) {
	if ( fd < 0 ) {
		return;
	}
	wiringPiI2CWriteReg8( fd, reg, value );
}

uint8_t hal_i2c_read( int fd, uint8_t reg ) {
	if ( fd < 0 ) {
		return 0;
	}
	return (uint8_t)wiringPiI2CReadReg8( fd, reg );
}

void hal_i2c_BulkRead( int fd, uint8_t reg, uint8_t len, uint8_t *buf ) {
	int i;
	if ( fd < 0 ) {
		return;
	}
	for ( i = 0; i < len; i++ )
		*buf++ = (uint8_t)wiringPiI2CReadReg8( fd, reg++ );
}

// returns hypotenuse of 3D vector
int32_t  magnitude( int32_t x, int32_t y, int32_t z ) {
	int64_t square, result;
	int32_t bit = 1 << 15;

	result = 0;
	square = (int64_t)x * x + (int64_t)y * y + (int64_t)z * z;

	do {
		result |= bit;
		if ( result * result > square ) {
			result ^= bit;
		}
		bit >>= 1;
	} while ( bit );

	return result;
}

// returns upper-most face of case
int32_t upness( int32_t x, int32_t y, int32_t z ) {
	int32_t result = 1;
	int32_t mag = x;

	if ( x < 0 ) {
		result = 6;
		mag = -x;
	}

	if ( y > mag ) {
		result = 2;
		mag = y;
	} else if ( y < -mag ) {
		result = 5;
		mag = -y;
	}

	if ( z > mag ) {
		result = 3;
	} else if ( z < -mag ) {
		result = 4;
	}

	return result;
}

double toAngle( int angle ) {
	return angle * M_PI / 180.0;
}

int32_t findArctan( double y, double x ) {
	if ( ( 0.0 == fabs( y ) ) && ( 0.0 == fabs( x ) ) ) {
		return 0;
	}
	return (int32_t)round( atan2( y, x ) * 180.0 / M_PI );
}
