#include "xtrinsic.h"
#include "stdio.h"
#include "unistd.h"
#include "curses.h"
#include "signal.h"

int running = 1;
void sigint_handler( int unused ) {
	unused += running;
	running = 0;
	return;
}

static WINDOW *logwindow = NULL;
void logprintf( const char *format, ... ) {
	char Buffer[200];

	if ( logwindow == NULL ) {
		logwindow = newwin( 12, 80, 2, 2  );
		scrollok( logwindow, TRUE );
	}
	va_list args;
	va_start( args, format );
	vsnprintf( Buffer, 159, format, args );
	va_end( args );
	waddstr( logwindow, Buffer );
}

int main() {
	int i;
	int magx, magy, magz;
	int maxx, maxy, maxz;
	int minx, miny, minz;
	double x, y, z, force;
	int pitch, roll, heading;
	WINDOW * mainwin;

	/*  Initialize ncurses  */
	mainwin = initscr();
	if ( mainwin == NULL ) {
		fprintf( stderr, "Error initialising ncurses.\n" );
		return 0;
	}

	accel_init();
	baro_init();
	mag_init();

	heading = mag_compass( 0.0, 0.0 );
	accel_read();
	maxx = minx = magnet_x();
	maxy = miny = magnet_y();
	maxz = minz = magnet_z();

	signal( SIGINT, sigint_handler );
	while ( running ) {
		for ( i = 0; i < 5; i++ ) {
			x = (double)accel_x() / 4096.0;
			y = (double)accel_y() / 4096.0;
			z = (double)accel_z() / 4096.0;
			force = 9.81 * sqrt( x * x + y * y + z * z );

			pitch = findArctan( y, z );
			roll  = findArctan( x, sqrt( y * y + z * z ) );
			heading = mag_compass( pitch, roll );
			accel_read(); // timed to match one-shot magnetometer
			magx = magnet_x();
			magy = magnet_y();
			magz = magnet_z();

			if ( magx > maxx ) {
				maxx = magx;
			}
			if ( magy > maxy ) {
				maxy = magy;
			}
			if ( magz > maxz ) {
				maxz = magz;
			}
			if ( minx > magx ) {
				minx = magx;
			}
			if ( miny > magy ) {
				miny = magy;
			}
			if ( minz > magz ) {
				minz = magz;
			}

			usleep( 200 * 1000 );
		}
		clear();
		logprintf( "Median X: %d\n", ( maxx + minx ) >> 1 );
		logprintf( "Median Y: %d\n", ( maxy + miny ) >> 1 );
		logprintf( "Median Z: %d\n", ( maxz + minz ) >> 1 );
		logprintf( "Gravity: %2.4fm/s\n", force );

		logprintf( "Pitch  : %3d\n", pitch );
		logprintf( "Roll   : %3d\n", roll );
		logprintf( "Heading: %3d\n", heading );

		logprintf( "Mag X: %d\n", magx );
		logprintf( "Mag Y: %d\n", magy );
		logprintf( "Mag Z: %d\n", magz );
		logprintf( "\n" );

		wrefresh( logwindow );
	}

	turnOffBaro();

	delwin( mainwin );
	endwin();
	refresh();

	return 0;
}
