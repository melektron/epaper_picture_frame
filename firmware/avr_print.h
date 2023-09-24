/* 
AVR PRINTING (C) 2011 <>< Charles Lohr, modified by melektron - This file may be licensed under the MIT/x11 or New BSD Licenses 
Modification Log:
 - melektron@230924 added hardware UART support
*/

#ifndef _AVR_PRINT_H


/**
 * Interface Config
 */

// if using hardware serial (only tested on 328p)
#define USE_HW_SERIAL
#define BAUD 9600
#include <util/setbaud.h>

// if using dumb bit-banged serial (only works with 4800 baud and 8MHz F_CPU)
//#define USE_DUMB_SERIAL
//#define DUMBSERIALPIN 1
//#define DUMBSERIALPORT PORTD
//#define DUMBSERIALDDR  DDRD


// if using SPI output
//#define USE_HW_SPI

/*
 * Definitions
*/

/**
 * @brief initializes everything needed for avr_print to work 
 * (such as hardware UART if configured).
 */
void print_init();

#ifdef MUTE_PRINTF

#define sendchr( x )
#define sendstr( x )
#define sendhex1( x )
#define sendhex2( x )
#define sendhex4( x )
#define setup_spi( x ) 

#else

void sendchr( char c );

#define sendstr( s ) { unsigned char rcnt; \
	for( rcnt = 0; s[rcnt] != 0; rcnt++ ) \
		sendchr( s[rcnt] ); }

void sendhex1( unsigned char i );
void sendhex2( unsigned char i );
void sendhex4( unsigned int i );
void setup_spi( void );

#endif


#endif	// _AVR_PRINT_H

