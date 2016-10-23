#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#define ID_OLED_ADDR 0x3C
#define ID_DATA_REG 0x40
#define ID_COMMAND_REG 0x00

typedef enum
{
	CLEAR_DISPLAY = 0x01,
	RETURN_HOME = 0x02,
	DISPLAY_ON = 0x0F,
	RETURN_1ST_HOME = 0x80,
	RETURN_2ND_HOME = 0xA0,
} M_OLED_COMMAND ;

int main( int argc, char *argv[] ) ;
void init_oled( int hOled ) ;

int main( int argc, char *argv[] )
{
	int rc = 0 ;
	int h_oled ;
	int i ;
	char x, y ;

	if( argc == 2 )
	{
		h_oled = wiringPiI2CSetup( ID_OLED_ADDR ) ;
		if( h_oled > 0 )
		{
			init_oled( h_oled ) ;
			for( i=0, x=0, y=0; i<strlen(argv[1]); i++ )
//			for( i=0, x=0, y=0; i<=0xff; i++ )
			{
				if( y == 0 )
				{
					wiringPiI2CWriteReg8( h_oled, ID_COMMAND_REG, RETURN_1ST_HOME+x ) ;

				}
				else
				{
					wiringPiI2CWriteReg8( h_oled, ID_COMMAND_REG, RETURN_2ND_HOME+x ) ;

				}
				wiringPiI2CWriteReg8( h_oled, ID_DATA_REG, argv[1][i] ) ;
//				wiringPiI2CWriteReg8( h_oled, ID_DATA_REG, (char)( i & 0xff )) ;
				x == 19 ? y++, x=0 : x++ ;
				if( y == 2 ) { y = 0 ; }
				delay(100) ;

			}
			close( h_oled ) ;
		}
		else
		{
			printf( "wiringPiI2CSetup() Falied\n" ) ;
			rc = 2 ;
		}
	}
	else
	{
		printf( "Parameter Error\n" ) ;
		rc = 1 ;
	}
	return( rc ) ;
}

void init_oled( int hOled )
{
	wiringPiI2CWriteReg8( hOled, ID_COMMAND_REG, CLEAR_DISPLAY ) ;
	delay(20) ;
	wiringPiI2CWriteReg8( hOled, ID_COMMAND_REG, RETURN_HOME ) ;
	delay(2) ;
	wiringPiI2CWriteReg8( hOled, ID_COMMAND_REG, DISPLAY_ON ) ;
	delay(2) ;
	wiringPiI2CWriteReg8( hOled, ID_COMMAND_REG, CLEAR_DISPLAY ) ;
	delay(2) ;
}

