/* 
AVR PRINTING (C) 2011 <>< Charles Lohr, modified by melektron - This file may be licensed under the MIT/x11 or New BSD Licenses 
Modification Log:
 - melektron@230924 added hardware UART support
*/

// Use tinyispterm to use this: https://github.com/cnlohr/tinyispterm

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#include "avr_print.h"


/*
 * Code Start
*/

#ifndef MUTE_PRINTF



#ifdef USE_HW_SERIAL

// hardware UART guides: 
// - http://www.rjhcoding.com/avrc-uart.php
// - https://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Der_UART

void setup_uart()
{
    // set baudrate in UBRR
    UBRR0L = UBRRL_VALUE;	// calculated by util/setbaud.h
    UBRR0H = UBRRH_VALUE;

    // enable the transmitter and receiver
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0);	// rx unused

	// format will stay default 8N1

	// wait until ready
	_delay_ms(50);
}

void sendchr(char data)
{
    // wait for transmit buffer to be empty
    while(!(UCSR0A & (1 << UDRE0)));

    // load data into transmit register
    UDR0 = data;	
}

#elif defined USE_DUMB_SERIAL

void sendchr( char c )
{
#define SCN { asm volatile( "nop" ); }
	//Weird setup for autobaud on
	DUMBSERIALPORT |= _BV(DUMBSERIALPIN);
	DUMBSERIALDDR |= _BV(DUMBSERIALPIN);
	cli();
	SCN; SCN; SCN; SCN;

	//One bit worth:
	SCN; SCN; SCN; SCN; SCN; SCN; SCN;
	DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN);
	SCN; SCN;
	if( c & 0x01 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x02 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x04 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x08 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x10 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x20 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x40 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN; }
	if( c & 0x80 )	{ SCN; DUMBSERIALPORT |= _BV(DUMBSERIALPIN); } else	{ DUMBSERIALPORT &= ~_BV(DUMBSERIALPIN); SCN; SCN;}
	SCN;  SCN;
	DUMBSERIALPORT |= _BV(DUMBSERIALPIN);
	sei();
}

#elif defined USE_HW_SPI

#if	defined (__AVR_ATtiny2313__) || defined (__AVR_ATtiny2313A__)
#define SPI_DDR_SET {DDRB&=0x1F;DDRB|=0x40;}
#define SPI_TINY
#define SPI_USI

#elif	defined (__AVR_ATtiny261__) || defined (__AVR_ATtiny261A__)
#define SPI_DDR_SET {DDRA&=0xFA;DDRA|=0x02;}
#define SPI_TINY

#elif	defined (__AVR_ATtiny44__) || defined (__AVR_ATtiny44A__) || \
	defined (__AVR_ATtiny24__) || defined (__AVR_ATtiny24A__) || \
	defined (__AVR_ATtiny84__) || defined (__AVR_ATtiny84A__)  
#define SPI_DDR_SET {DDRA&=0xAF;DDRA|=0x20;}
#define SPI_TINY
#define SPI_USI


#elif	defined (__AVR_ATmega32U2__) || defined( __AVR_ATmega8U2__ )
#define SPI_MEGA
#define SPI_DDR_SET { DDRB |= _BV(3); DDRB &= ~(_BV(0)|_BV(1)|_BV(2)); }


#elif   defined (__AVR_ATmega168__) || defined( __AVR_ATmega328__) || defined( __AVR_ATmega168PB__ )
#define SPI_DDR_SET {DDRB&=0xD3;DDRB|=0x10;} 
#define SPI_MEGA

#else
#error UNDEFINED PIN ASSIGNMENTS

#endif

//OPTIONAL!!!
//#ifdef SPI_TINY
//#define USE_SPI_INTERRUPT
//#endif


#ifdef USE_SPI_INTERRUPT

volatile unsigned char ThisCharToSend = 0;
volatile unsigned char BackChar = 0;
#ifdef SPI_MEGA
ISR( SPI_STC_vect )
{
	BackChar = SPDR;
	SPDR = ThisCharToSend;
	ThisCharToSend = 0;
}
#elif defined( SPI_TINY )
ISR( USI_OVF_vect )
{
	USIBR = ThisCharToSend;
	ThisCharToSend = 0;
	USISR |= (1<<USIOIF); 
}
#else
#error NO_SPI_VECT_DEFINED
#endif

#endif

void sendchr( char c )
{
#ifdef USE_SPI_INTERRUPT
	ThisCharToSend = c;
	while( ThisCharToSend );
#else

#ifdef SPI_USI
	while(!(USISR & (1<<USIOIF)));
	USIDR = c;
	USISR = (1<<USIOIF);
#else
	SPDR = c;
	while(!(SPSR & (1<<SPIF)));
#endif

#endif
}

static void setup_spi( void )
{
	SPI_DDR_SET;

#ifdef SPI_TINY
	USICR = (1<<USIWM0) | (1<<USICS1)
#ifdef USE_SPI_INTERRUPT
	 | (1<<USIOIE);
#else
	;
#endif

#elif defined( SPI_MEGA )

#ifdef USE_SPI_INTERRUPT
	SPCR = _BV(SPE) | _BV(SPIE);
#else
	SPCR = _BV(SPE);	
#endif

#else
#error NO_SPI_VECT_DEFINED
#endif
}

#endif	// end output selection

// functions to format some data

void sendhex1( unsigned char i )
{
	sendchr( (i<10)?(i+'0'):(i+'A'-10) );
}
void sendhex2( unsigned char i )
{
	sendhex1( i>>4 );
	sendhex1( i&0x0f );
}
void sendhex4( unsigned int i )
{
	sendhex2( i>>8 );
	sendhex2( i&0xFF);
}

// stdout stream implementation

static int stream_putc_internal(char c, FILE *stream)
{
	sendchr( c );
	return 0;
}

static FILE print_out_file = FDEV_SETUP_STREAM( stream_putc_internal, NULL, _FDEV_SETUP_WRITE );

// setup routine

void print_init()
{
	// Interface setup
#ifdef USE_HW_SERIAL
	setup_uart();
#elif USE_HW_SPI
	setup_spi();
#endif

	// configure stdout
	stdout = &print_out_file;
}

#endif	// MUTE_PRINTF
