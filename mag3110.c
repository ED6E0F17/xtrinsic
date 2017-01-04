
//  mag3110.c -- Magnetometer support
//
//  Copyright (c) 2013/2017 John Greb <github.com/ed6e0f17>

// Based on example code from Freescale, released by Element 14.

#include "xtrinsic.h"

/* Magnetic calibration needs to be done after board assembly
  Offsets can be programmed into chip, or added in code.
  Median X: 452
  Median Y: 577
  Median Z: -123
*/
#define MAG_CAL_X ( -450 )
#define MAG_CAL_Y ( -580 )
#define MAG_CAL_Z ( +120 )

#define MAG_ADD     ( 0x0E )
#define MAG_ID      ( 0xC4 )

#define MAG_READY_REG   ( 0x00 )
#define MAG_X_OUT   ( 0x01 )
#define MAG_Y_OUT   ( 0x03 )
#define MAG_Z_OUT   ( 0x05 )
#define MAG_WHOAMI      ( 0x07 )
#define MAG_TEMP    ( 0x00 )
#define MAG_CTRL1   ( 0x10 )
#define MAG_CTRL2   ( 0x11 )

#define MAG_XYZ_READY   ( 1 << 3 )

int mfd = -1;
uint8_t magReady;
void mag_init( void ) {
	mfd = hal_i2c_init( MAG_ADD );
	hal_i2c_write( mfd, MAG_CTRL1, 0 );
	magReady = hal_i2c_read( mfd, MAG_WHOAMI );
	if ( MAG_ID != magReady ) {
		return;
	}

	// Setup autoresets and one-shot trigger in raw mode
	hal_i2c_write( mfd, MAG_CTRL2, 0x90 );
	hal_i2c_write( mfd, MAG_CTRL1, 0x1A );
}

int magX, magY, magZ;

int32_t magnet_x() { return magX; }
int32_t magnet_y() { return magY; }
int32_t magnet_z() { return magZ; }

// Forward is ?
// Pitch is up at front
// Roll is righthand up

// Measure Mag Field, transform by attitude and calculate North
int mag_compass( int pitch, int roll ) {
	uint8_t dataready;
	int16_t magnet;
	double magXd, magYd, magZd;
	double anglex, angley, result;
	double sin_pitch, sin_roll, cos_pitch, cos_roll;

	if ( MAG_ID != magReady ) {
		return 0;
	}
	dataready = MAG_XYZ_READY & hal_i2c_read( mfd, MAG_READY_REG );
	if ( !dataready ) {
		return 0;
	}

	magnet = hal_i2c_read( mfd, MAG_X_OUT ) << 8;
	magX = (int32_t)magnet + hal_i2c_read( mfd, MAG_X_OUT + 1 );
	magXd = (double)( magX - MAG_CAL_X );

	magnet = hal_i2c_read( mfd, MAG_Y_OUT ) << 8;
	magY = (int32_t)magnet + hal_i2c_read( mfd, MAG_Y_OUT + 1 );
	magYd = (double)( magY - MAG_CAL_Y );

	magnet = hal_i2c_read( mfd, MAG_Z_OUT ) << 8;
	magZ = (int32_t)magnet + hal_i2c_read( mfd, MAG_Z_OUT + 1 );
	magZd = (double)( magZ - MAG_CAL_Z );

	// using one-shot mode for minimum power, 128 oversample
	hal_i2c_write( mfd, MAG_CTRL1, 0x1A );

	sin_pitch = sin( toAngle( -pitch ) );
	cos_pitch = cos( toAngle( -pitch ) );
	sin_roll = sin( toAngle( -roll ) );
	cos_roll = cos( toAngle( -roll ) );

	// TODO - Use real formula, not this approximation
	angley = magZd * sin_roll - magYd * cos_roll;
	anglex = magZd * cos_roll * sin_pitch + magXd * cos_pitch;

	result = 180 - findArctan( angley, anglex );
	return result;
}
