#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#define ID_LED_1 19
#define ID_LED_2 20
#define ID_LED_3 21

#define ID_SW_1 2
#define ID_SW_2 3
#define ID_SW_3 4

sem_t g_SemMainWait ;
unsigned int g_SwitchCount[2] = {0,0} ;
struct timespec g_PrevTimeSpec[2] ;

int main() ;
int check_gpio() ;
void lit_bulk_led( int fLit ) ;
void sw1_brink_led() ;
void sw2_brink_led() ;
int check_chattering( struct timespec *pPrevTimeSpec ) ;
void brink_led( unsigned int LED ) ;
void quit_process() ;

int main()
{
	if( wiringPiSetupSys() == 0 )
	{
		if( check_gpio() == 0 )
		{
			lit_bulk_led( 1 ) ;
			wiringPiISR( ID_SW_1, INT_EDGE_RISING, sw1_brink_led ) ;
			wiringPiISR( ID_SW_2, INT_EDGE_RISING, sw2_brink_led ) ;
			wiringPiISR( ID_SW_3, INT_EDGE_RISING, quit_process ) ;
			if( sem_init( &g_SemMainWait, 0, 0 ) == 0 )
			{
				printf( "Waiting for semaphore\n" ) ;
				sem_wait( &g_SemMainWait ) ;
				printf( "Semaphore through\n" ) ;
				sem_destroy( &g_SemMainWait ) ;
				lit_bulk_led( 0 ) ;
				return( 0 ) ;
			}
			else
			{
				printf( "sem_init() failed \n" ) ;
			}
		}
	}
	return(1) ;
}

int check_gpio()
{
	int rc = 0 ;

	digitalWrite( ID_LED_1, 1 ) ;
	if( digitalRead( ID_LED_1 ) != 0 )
	{
		digitalWrite( ID_LED_2, 1 ) ;
		if( digitalRead( ID_LED_2 ) != 0 )
		{
			digitalWrite( ID_LED_3, 1 ) ;
			if( digitalRead( ID_LED_3 ) != 0 )
			{
				if( digitalRead( ID_SW_1 ) != 0 )
				{
					if( digitalRead( ID_SW_2 ) != 0 )
					{
						if( digitalRead( ID_SW_3 ) == 0 )
						{
							printf( "Error on reading SW_3\n" ) ;
							rc = 7 ;
						}
					}
					else
					{
						printf( "Error on reading SW_2\n" ) ;
						rc = 6 ;
					}
				}
				else
				{
					printf( "Error on reading SW_1\n" ) ;
					rc = 5 ;
				}
			}
			else
			{
				printf("Error on lit LED_3\n") ;
				rc = 4 ;
			}
		}
		else
		{
			printf("Error on lit LED_2\n") ;
			rc = 3 ;
		}
	}
	else
	{
		printf("Error on lit LED_1\n") ;
		rc = 2 ;
	}
	return( rc ) ;
}

void lit_bulk_led( int fLit )
{
    digitalWrite( ID_LED_1, fLit ) ;
    digitalWrite( ID_LED_2, fLit ) ;
    digitalWrite( ID_LED_3, fLit ) ;
}

void sw1_brink_led()
{
	if( check_chattering( &g_PrevTimeSpec[0] ) == 0 )
	{
		printf( "Rising SW1 %d\n", g_SwitchCount[0] ) ;
		g_SwitchCount[0]++ ;
		brink_led( ID_LED_1 ) ;
	}
}

void sw2_brink_led()
{
	if( check_chattering( &g_PrevTimeSpec[1] ) == 0 )
	{
		printf( "Rising SW2 %d\n", g_SwitchCount[1] ) ;
		g_SwitchCount[1]++ ;
		brink_led( ID_LED_2 ) ;
	}
}

int check_chattering( struct timespec *pPrevTimeSpec )
{
	int rc = 0 ; // 0 = Actual Interrupt / 1 = Chattering
	struct timespec prev_ts = *pPrevTimeSpec ;
	struct timespec current_ts ;
	struct timespec diff_ts ;

	if( prev_ts.tv_sec != 0 || prev_ts.tv_nsec != 0 )
	{
		clock_gettime( CLOCK_MONOTONIC_RAW, &current_ts ) ;
		if((current_ts.tv_nsec - prev_ts.tv_nsec) < 0 )
		{
			diff_ts.tv_sec = current_ts.tv_sec - prev_ts.tv_sec - 1;
			diff_ts.tv_nsec = current_ts.tv_nsec - prev_ts.tv_nsec + 1000000000;
		}
		else
		{
			diff_ts.tv_sec = current_ts.tv_sec - prev_ts.tv_sec ;
			diff_ts.tv_nsec = current_ts.tv_nsec - prev_ts.tv_nsec ;
		}
		if( diff_ts.tv_sec == 0 )
		{
			if( diff_ts.tv_nsec <= 20000000 ) // 20ms
			{
				rc = 1 ;
			}
		}
		prev_ts = current_ts ;
	}
	else
	{
		clock_gettime( CLOCK_MONOTONIC_RAW, &prev_ts ) ;
	}
	*pPrevTimeSpec = prev_ts ;
	return( rc ) ;
}
void brink_led( unsigned int LED )
{
	if( digitalRead( LED ) != 0 )
	{
		digitalWrite( LED, 0 ) ;
	}
	else
	{
		digitalWrite( LED, 1 ) ;
	}
}

void quit_process()
{
	printf( "Interrupt for quit process\n" ) ;
	sem_post( &g_SemMainWait ) ;
}
