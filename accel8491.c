
// accel8491.c -- Accelerometer support
//
//  Copyright (c) 2013/2017 John Greb <github.com/ed6e0f17>

// Based on example code from Freescale, released by Element 14.

#include "xtrinsic.h"

#define MMA8491_ADD          ( 0x55 )
#define MMA8491_STATUS       ( 0 )
#define MMA8491_3AXIS        ( 1 << 3 )

int afd = -1;
int16_t acc8491data[3] = {4096,0,0};

void accel_read() {
	uint8_t status;
	uint8_t rawdata[8];
	int32_t loops = 5;

	// 8491 needs 0.9ms to power on for every read.
	hal_gpio_on();

	do {
		usleep( 5000 ); // 5 ms
		status = hal_i2c_read( afd, MMA8491_STATUS );
	} while ( --loops && !status );

	if ( ( status & MMA8491_3AXIS ) && ( status <= 0x0f ) ) {
		hal_i2c_BulkRead( afd, 1, 6, rawdata );
		acc8491data[0] = ( ( (int16_t)rawdata[0] << 8 ) + rawdata[1] ) & ~3;
		acc8491data[1] = ( ( (int16_t)rawdata[2] << 8 ) + rawdata[3] ) & ~3;
		acc8491data[2] = ( ( (int16_t)rawdata[4] << 8 ) + rawdata[5] ) & ~3;
	}

	// Power off after every read
	hal_gpio_off();
}

void accel_init( void ) {
	hal_gpio_init();
	afd = hal_i2c_init( MMA8491_ADD );
}

// Read acceleration values for all axis in bulk when asked for X
// 8491 only supports 8g mode, so return 16 bit values to match 8451

int32_t accel_x( void ) { return acc8491data[0]; }
int32_t accel_y( void ) { return acc8491data[1]; }
int32_t accel_z( void ) { return acc8491data[2]; }
