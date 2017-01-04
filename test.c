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
	int press, temp;
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

	signal( SIGINT, sigint_handler );
	while ( running ) {
		x = (double)accel_x() / 4096.0;
		y = (double)accel_y() / 4096.0;
		z = (double)accel_z() / 4096.0;
		force = 9.81 * sqrt( x * x + y * y + z * z );

		logprintf( "Force X: %2.4fG\n", x );
		logprintf( "Force Y: %2.4fG\n", y );
		logprintf( "Force Z: %2.4fG\n", z );
		logprintf( "Gravity: %2.4fm/s\n", force );

		pitch = findArctan( y, z );
		roll  = findArctan( x, sqrt( y * y + z * z ) );
		heading = mag_compass( pitch, roll );
		accel_read(); // timed to match one-shot magnetometer

		logprintf( "Pitch  : %3d\n", pitch );
		logprintf( "Roll   : %3d\n", roll );
		logprintf( "Heading: %3d\n", heading );

		press = baro_press();
		temp = baro_temp();

		logprintf( "Temperature: %d\n", temp );
		logprintf( "Pressure: %d\n \n", press );
		wrefresh( logwindow );
		usleep( 1000 * 1000 );
		clear();
	}

	turnOffBaro();

	delwin( mainwin );
	endwin();
	refresh();

	return 0;
}
