/* AVR PRINTING (C) 2011 <>< Charles Lohr - This file may be licensed under the MIT/x11 or New BSD Licenses */
//Use tinyispterm to use this: https://github.com/cnlohr/tinyispterm

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdio.h>

#define DUMBSERIALPIN 1
#define DUMBSERIALPORT PORTE
#define DUMBSERIALDDR  DDRE

#ifndef MUTE_PRINTF

#ifdef DUMBSERIALPIN

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

#else

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


#endif /* ending regular send DUMBSERIALPIN */

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


static int stream_putc_internal(char c, FILE *stream)
{
	sendchr( c );
	return 0;
}

static FILE mystdout = FDEV_SETUP_STREAM( stream_putc_internal, NULL, _FDEV_SETUP_WRITE );

#ifndef DUMBSERIALPIN
void setup_spi( void )
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
	stdout = &mystdout;
}
#endif

#endif
