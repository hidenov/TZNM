#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <time.h>

#define ID_MOLE_1 19
#define ID_MOLE_2 20
#define ID_MOLE_3 21

#define ID_MOLE_BIT_1 1
#define ID_MOLE_BIT_2 2
#define ID_MOLE_BIT_3 4

#define ID_SW_1 2
#define ID_SW_2 3
#define ID_SW_3 4

#define ID_IJ_TICK 17 // Ichigo Jam tick = 17ms

int main() ;
int init_mole() ;
void lit_bulk_mole( int fLit ) ;
int blink_mole( unsigned int Moles, unsigned int MoleBit, int MoleID, int MoleCount ) ;
unsigned int check_mole_hit( unsigned int Moles, unsigned int MoleBit, int SwitchID, int MoleID, int *pScore, int *pDelayTick ) ;

int main()
{
    int rc = 1 ;
    int delay_tick = 180 ;
    int mole_count = 25 ;
    int score = 0 ;
    unsigned int moles ;
    int tick ;

    if(wiringPiSetupSys() == 0 )
    {
        if( init_mole() == 0 )
        {
            delay(3000) ;
            printf( "Ready ?\n") ;
            digitalWrite( ID_MOLE_1, 0 ) ;
            delay(1000) ;
            printf( ".\n" ) ;
            digitalWrite( ID_MOLE_2, 0 ) ;
            delay(1000) ;
            printf( ".\n" ) ;
            digitalWrite( ID_MOLE_3, 0 ) ;
            delay(1000) ;
            printf( "GO!\n\n" ) ;

            srand((unsigned)time( NULL )) ;
            while( mole_count > 0 )
            {
                lit_bulk_mole( 0 ) ;
                delay(100) ;
                lit_bulk_mole( 1 ) ;
                delay(100) ;
                lit_bulk_mole( 0 ) ;

                moles = (unsigned int)( rand() % 7 + 1 ) ;
                printf( "moles %d\n", moles ) ;
                mole_count = blink_mole( moles, ID_MOLE_BIT_1, ID_MOLE_1, mole_count ) ;
                mole_count = blink_mole( moles, ID_MOLE_BIT_2, ID_MOLE_2, mole_count ) ;
                mole_count = blink_mole( moles, ID_MOLE_BIT_3, ID_MOLE_3, mole_count ) ;

                for( tick = 0 ; tick < delay_tick; tick++ )
                {
                    moles = check_mole_hit( moles, ID_MOLE_BIT_1, ID_SW_1, ID_MOLE_1, &score, &delay_tick ) ;
                    moles = check_mole_hit( moles, ID_MOLE_BIT_2, ID_SW_2, ID_MOLE_2, &score, &delay_tick ) ;
                    moles = check_mole_hit( moles, ID_MOLE_BIT_3, ID_SW_3, ID_MOLE_3, &score, &delay_tick ) ;
                    if( moles == 0 )
                    {
                        printf("breakall! \n") ;
                        break ;
                    }
                    delay( 1 * ID_IJ_TICK ) ;
                }
                printf( "mole left %d\n", moles ) ;
                delay( 60 * ID_IJ_TICK ) ;
            }

            printf( "\n Game Over\n" ) ;
            printf( "Score : %d\n\n", score ) ;
            lit_bulk_mole( 0 ) ;
            rc = 0 ;
        }
    }
    return rc;
}

int init_mole()
{
    int rc = 0 ;

    printf( "Whack_a_mole\n" ) ;
    digitalWrite( ID_MOLE_1, 1 ) ;
    if( digitalRead(ID_MOLE_1) != 0 )
    {
        digitalWrite( ID_MOLE_2, 1 ) ;
        if( digitalRead(ID_MOLE_2) != 0 )
        {
            digitalWrite( ID_MOLE_3, 1 ) ;
            if( digitalRead(ID_MOLE_3) != 0 )
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

void lit_bulk_mole( int fLit )
{
    digitalWrite( ID_MOLE_1, fLit ) ;
    digitalWrite( ID_MOLE_2, fLit ) ;
    digitalWrite( ID_MOLE_3, fLit ) ;
}

int blink_mole( unsigned int Moles, unsigned int MoleBit, int MoleID, int MoleCount )
{
    int mole_count = MoleCount ;

    if(( Moles & MoleBit ) == 0 )
    {
        digitalWrite( MoleID, 0 ) ;
    }
    else
    {
        digitalWrite( MoleID, 1 ) ;
        mole_count-- ;
    }
    return mole_count ;
}

unsigned int check_mole_hit( unsigned int Moles, unsigned int MoleBit, int SwitchID, int MoleID, int *pScore, int *pDelayTick )
{
    int delay_tick = *pDelayTick ;
    int score = *pScore ;

    unsigned int moles = Moles ;

    if(( moles & MoleBit ) != 0 )
    {
        if( digitalRead(SwitchID) == 0 ) //Hit!
        {
            moles = moles & (~MoleBit) & 0x07 ;
            printf( "Hit! Mole : %d ( %x ) \n", MoleID, moles ) ;
            digitalWrite( MoleID, 0 ) ;
            score++ ;
            delay_tick = delay_tick - 2 ;
        }
    }
    *pDelayTick = delay_tick ;
    *pScore = score ;
    return moles ;
}
