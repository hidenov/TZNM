#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#define ID_LED_1 19
#define ID_LED_2 20
#define ID_LED_3 21

int main() ;
int check_gpio() ;
// void signal_handler( int Sig, siginfo_t *Siginfo, void *pSigContext ) ;
void signal_handler( int Sig ) ;

sem_t g_SemMainWait ;
volatile sig_atomic_t g_InterruptCount ;
volatile sig_atomic_t g_fLit ;

int main()
{
    struct sigaction _sigaction ;
    struct sigevent _sigevent ;
	struct itimerspec _itimespec ;
	struct timespec start_ts, end_ts ;
	timer_t timer_id = 0 ;

	g_InterruptCount = 0 ;
	g_fLit = 0 ;
 	if( wiringPiSetupSys() == 0 )
	{
		if( check_gpio() == 0 )
		{
			if( sem_init( &g_SemMainWait, 0, 0 ) == 0 )
			{
				memset(&_sigaction, 0, sizeof(_sigaction));
				memset(&_sigevent, 0, sizeof(_sigevent));
//				_sigaction.sa_sigaction = signal_handler;
				_sigaction.sa_handler = signal_handler ;
//				_sigaction.sa_flags = SA_SIGINFO | SA_RESTART;
				_sigaction.sa_flags = SA_RESTART ;
				sigemptyset( &_sigaction.sa_mask ) ;
				if( sigaction( SIGRTMIN + 1, &_sigaction, NULL ) == 0 )
				{
					_sigevent.sigev_notify = SIGEV_SIGNAL ;
					_sigevent.sigev_signo = SIGRTMIN + 1 ;
					if( timer_create( CLOCK_MONOTONIC, &_sigevent, &timer_id) == 0 )
					{
						_itimespec.it_interval.tv_sec = 0 ;
						_itimespec.it_interval.tv_nsec = 100000 ; // 100 micro second
						_itimespec.it_value.tv_sec = 0 ;
						_itimespec.it_value.tv_nsec = 100000 ; // 100 micro second
						if( timer_settime( timer_id, 0, &_itimespec, NULL ) == 0 )
						{
							printf( "Waiting for semaphore\n" ) ;
							clock_gettime( CLOCK_MONOTONIC_RAW, &start_ts ) ;
							sem_wait( &g_SemMainWait ) ;
							clock_gettime( CLOCK_MONOTONIC_RAW, &end_ts ) ;
							printf( "Semaphore through %d %10ld.%09ld - %10ld.%09ld\n", g_InterruptCount, start_ts.tv_sec, start_ts.tv_nsec, end_ts.tv_sec, end_ts.tv_nsec ) ;
						}
						else
						{
							printf( "timer_settime() failure\n") ;
						}
						timer_delete( timer_id ) ;
					}
					else
					{
						printf( "timer_create() failure\n") ;
					}
					sigaction( SIGRTMIN + 1, NULL, NULL ) ;
				}
				else
				{
					printf( "sigaction() failure\n" ) ;
				}
				sem_destroy( &g_SemMainWait ) ;
			}
			else
			{
				printf( "sem_init() failure\n" ) ;
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
			if( digitalRead( ID_LED_3 ) == 0 )
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

// void signal_handler( int Sig, siginfo_t *Siginfo, void *pSigContext )
void signal_handler( int Sig )
{
	int f_lit = (~(int)g_fLit) & 0x01;

	digitalWrite( ID_LED_1, f_lit ) ;
	digitalWrite( ID_LED_2, f_lit ) ;
	digitalWrite( ID_LED_3, f_lit ) ;
	g_fLit = (sig_atomic_t)f_lit ;
	g_InterruptCount++ ;
	if( g_InterruptCount >= 1000 )
	{
		sem_post( &g_SemMainWait ) ;
	}
}
