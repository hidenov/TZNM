#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>

int main() ;
int check_gpio();
void lit_led( int Index ) ;

const int g_GPIOPort[6] = {16, 20, 21, 13, 19, 26} ;

int main()
{
    int rc = 1 ;
	int i ;

    if( wiringPiSetupSys() == 0 )
    {
        if( check_gpio() == 0 )
        {
        	while(1)
			{
				for( i=0; i<6; i++ )
				{
					lit_led( i ) ;
				}
			}
        }
    }
    return rc;
}

int check_gpio()
{
	int rc = 0 ;
	int i ;

	for( i=0; i<6; i++ )
	{
		digitalWrite( g_GPIOPort[i], 1 ) ;
		if( digitalRead( g_GPIOPort[i] ) == 0 )
		{
			printf( "Error on lit GPIO =%d\n", g_GPIOPort[i] ) ;
			rc = i;
			break ;
		}
    }
    return( rc ) ;
}

void lit_led( int Index )
{
	const int led_line[6][5] =
	{
// #1 LED Y1[GPIO_4=0 / GPIO_3=1] - Y6[GPIO_7=1 / GPIO_6=1 / GPIO_5=0]
		{ 1, 0, 0, 1, 1 },
// #2 LED Y2[GPIO_4=1 / GPIO_3=0] - Y7[GPIO_7=1 / GPIO_6=1 / GPIO_5=1]
		{ 0, 1, 1, 1, 1 },
// #3 LED Y1[GPIO_4=0 / GPIO_3=1] - Y7[GPIO_7=1 / GPIO_6=1 / GPIO_5=1]
		{ 1, 0, 1, 1, 1 },
// #4 LED Y3[GPIO_4=1 / GPIO_3=1] - Y7[GPIO_7=1 / GPIO_6=1 / GPIO_5=1]
		{ 1, 1, 1, 1, 1 },
// #5 LED Y3[GPIO_4=1 / GPIO_3=1] - Y0[GPIO_7=0 / GPIO_6=0 / GPIO_5=0]
		{ 1, 1, 0, 0, 0 },
// #6 LED Y0[GPIO_4=0 / GPIO_3=0] - Y0[GPIO_7=0 / GPIO_6=0 / GPIO_5=0]
		{ 0, 0, 0, 0, 0 }
	} ;

	digitalWrite( g_GPIOPort[0], 1 ) ;
	digitalWrite( g_GPIOPort[1], led_line[Index][0] ) ;
	digitalWrite( g_GPIOPort[2], led_line[Index][1] ) ;
	digitalWrite( g_GPIOPort[3], led_line[Index][2] ) ;
	digitalWrite( g_GPIOPort[4], led_line[Index][3] ) ;
	digitalWrite( g_GPIOPort[5], led_line[Index][4] ) ;
	digitalWrite( g_GPIOPort[0], 0 ) ;
	delay(40) ;
}

