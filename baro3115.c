
//  baro3115.c -- Barometer support
//
//  Copyright (c) 2013/2017 John Greb <github.com/ed6e0f17>

// Based on example code from Freescale, released by Element 14.

#include "xtrinsic.h"

#define BARO_ADD    ( 0x60 )
#define BARO_ID         ( 0xc4 )

#define BARO_A_OUT  ( 1 )
#define BARO_T_OUT  ( 4 )

#define BARO_CTRL1  ( 0x26 )
#define BARO_STANDBY    ( 0xFE )
#define BARO_ACTIVE ( 0x01 )
#define PT_DATA_CFG_REG ( 0x13 )
#define PP_OD12_MASK    ( 0x11 )

// For altitude mode, with 32x oversample
#define ALT_MASK    ( 1 << 7 )
#define OVERSAMP_MASK   ( 5 << 3 )

int bfd = -1;

// Wake Barometer from Standby
void turnOnBaro( void ) {
	uint8_t reg;
	reg = hal_i2c_read( bfd, BARO_CTRL1 );
	hal_i2c_write( bfd, BARO_CTRL1, reg | BARO_ACTIVE );
}

// Put Barometer into standby
void turnOffBaro( void ) {
	uint8_t reg;
	reg = hal_i2c_read( bfd, BARO_CTRL1 );
	hal_i2c_write( bfd, BARO_CTRL1, reg & BARO_STANDBY );
}

// 12 bits == 4096m ??
int32_t baro_alt() {
	int32_t alt;
	uint8_t reg = BARO_A_OUT;

	alt = hal_i2c_read( bfd, reg++ );
	alt <<= 8;
	alt += hal_i2c_read( bfd, reg++ );
	alt >>= 4;

	return alt;
}

// pressure at ground is 101k (17 bits)
int32_t baro_press() {
	int32_t bar = 0;
	uint8_t reg = BARO_A_OUT;
	bar |= hal_i2c_read( bfd, reg++ );
	bar <<= 8;
	bar |= hal_i2c_read( bfd, reg++ );
	bar <<= 8;
	bar |= hal_i2c_read( bfd, reg++ );
	bar >>= 6; // 18 significant bits of data

	return bar;
}

int32_t baro_temp() {
	int32_t temp;
	uint8_t reg = BARO_T_OUT;

	temp = hal_i2c_read( bfd, reg++ );
	// temp <<=8;
	// temp += hal_i2c_read(bfd, reg++);
	// temp >> 4;
	return temp;
}

void baro_init( void ) {
	uint8_t reg = BARO_CTRL1;
	bfd = hal_i2c_init( BARO_ADD );

	// Set barometer mode, for altitude use (ALT_MASK | OVERSAMP_MASK ));
	hal_i2c_write( bfd, reg++, ( OVERSAMP_MASK ) );
	hal_i2c_write( bfd, reg++, 0 );
	hal_i2c_write( bfd, reg++, PP_OD12_MASK );
	hal_i2c_write( bfd, reg++, 0 );
	hal_i2c_write( bfd, reg++, 0 );
	hal_i2c_write( bfd, PT_DATA_CFG_REG, 0x07 );

	turnOnBaro();
}
