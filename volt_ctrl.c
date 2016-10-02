#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#define ID_DAC1_ADDR 0x60
#define ID_DAC2_ADDR 0x61
#define ID_ADC_ADDR 0x48

int main( int argc, char *argv[] ) ;
// unsigned short readADC_SingleEnded(int fd, unsigned char channel) ;

int main( int argc, char *argv[] )
{
	const int adc_config[4] = { 0xC383, 0xD383, 0xE383, 0xF383 }  ;

	int rc ;
	int h_adc ;
	int h_dac ;
	unsigned short data ;
	unsigned char channel ;
	int dac_value ;

	if( argc == 3 )
	{
		h_dac = wiringPiI2CSetup( ID_DAC1_ADDR ) ;
		if( h_dac > 0 )
		{
			dac_value = (int)strtol( argv[1], NULL, 16 ) ;
			wiringPiI2CWriteReg8( h_dac, ( dac_value >> 8 ) & 0xff, dac_value &0xff ) ;
			close( h_dac ) ;
			h_dac = wiringPiI2CSetup( ID_DAC2_ADDR ) ;
			if( h_dac > 0 )
			{
				dac_value = (int)strtol( argv[2], NULL, 16 ) ;
				wiringPiI2CWriteReg8( h_dac, ( dac_value >> 8 ) & 0xff, dac_value &0xff ) ;
				h_adc = wiringPiI2CSetup( ID_ADC_ADDR ) ;
				if( h_adc > 0 )
				{
					while( 1 )
					{
						printf( "read data = | " ) ;
						for( channel=0; channel<4; channel++ )
						{
//							data = readADC_SingleEnded( h_adc, channel ) ;
							wiringPiI2CWriteReg16( h_adc, 0x01, ntohs(adc_config[channel])) ;
							delay(1) ;
							data = wiringPiI2CReadReg16( h_adc, 0x00 ) ;
							printf( "%04x | ", ntohs( data ) >> 4 ) ;
						}
						printf( "\n" ) ;
						delay(1000) ;
					}
					close( h_adc ) ;
					rc = 0 ;
				}
				else
				{
					printf( "ADC wiringPiI2CSetup() Falied\n" ) ;
					rc = 4 ;
				}
				close( h_dac ) ;
			}
			else
			{
				printf( "DAC2 wiringPiI2CSetup() Falied\n" ) ;
				rc = 3 ;
			}
		}
		else
		{
			printf( "DAC1 wiringPiI2CSetup() Falied\n" ) ;
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

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
    #define ADS1015_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)
/*=========================================================================*/

/*=========================================================================
    CONVERSION DELAY (in mS)
    -----------------------------------------------------------------------*/
    #define ADS1015_CONVERSIONDELAY         (1)
    #define ADS1115_CONVERSIONDELAY         (8)
/*=========================================================================*/

/*=========================================================================
    POINTER REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_POINTER_MASK        (0x03)
    #define ADS1015_REG_POINTER_CONVERT     (0x00)
    #define ADS1015_REG_POINTER_CONFIG      (0x01)
    #define ADS1015_REG_POINTER_LOWTHRESH   (0x02)
    #define ADS1015_REG_POINTER_HITHRESH    (0x03)
/*=========================================================================*/

/*=========================================================================
    CONFIG REGISTER
    -----------------------------------------------------------------------*/
    #define ADS1015_REG_CONFIG_OS_MASK      (0x8000)
    #define ADS1015_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
    #define ADS1015_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
    #define ADS1015_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

    #define ADS1015_REG_CONFIG_MUX_MASK     (0x7000)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
    #define ADS1015_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
    #define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
    #define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
    #define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
    #define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

    #define ADS1015_REG_CONFIG_PGA_MASK     (0x0E00)
    #define ADS1015_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3
    #define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
    #define ADS1015_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2 (default)
    #define ADS1015_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
    #define ADS1015_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
    #define ADS1015_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

    #define ADS1015_REG_CONFIG_MODE_MASK    (0x0100)
    #define ADS1015_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
    #define ADS1015_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

    #define ADS1015_REG_CONFIG_DR_MASK      (0x00E0)
    #define ADS1015_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
    #define ADS1015_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
    #define ADS1015_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
    #define ADS1015_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
    #define ADS1015_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
    #define ADS1015_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
    #define ADS1015_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second

    #define ADS1015_REG_CONFIG_CMODE_MASK   (0x0010)
    #define ADS1015_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
    #define ADS1015_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

    #define ADS1015_REG_CONFIG_CPOL_MASK    (0x0008)
    #define ADS1015_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
    #define ADS1015_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

    #define ADS1015_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
    #define ADS1015_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
    #define ADS1015_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

    #define ADS1015_REG_CONFIG_CQUE_MASK    (0x0003)
    #define ADS1015_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
    #define ADS1015_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
    #define ADS1015_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
    #define ADS1015_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
/*
unsigned short readADC_SingleEnded(int fd, unsigned char channel)
{
    unsigned short config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                      ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                      ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                      ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                      ADS1015_REG_CONFIG_DR_1600SPS   | // 1600 samples per second (default)
                      ADS1015_REG_CONFIG_MODE_SINGLE;   // Single-shot mode (default)

    config |= ADS1015_REG_CONFIG_PGA_4_096V;

    // Set single-ended input channel
    switch (channel)
    {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;
    printf( "[%04x]", config ) ;
    wiringPiI2CWriteReg16(fd, ADS1015_REG_POINTER_CONFIG, ntohs(config));
    delay(ADS1015_CONVERSIONDELAY);
    return ntohs(wiringPiI2CReadReg16(fd, ADS1015_REG_POINTER_CONVERT) >> 4);
}
*/
