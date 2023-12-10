#ifndef AD1939_DRIVER_H
#define AD1939_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "main.h"


 /*
  * TODO LIST:
  * TODO 1: config registert from SPI
  * TODO 2: setup I2S for ADC
  * TODO 3: setup I2S for DAC
  * TODO 4: create interface for in and output of data
  */

#define AD1939_Global_Address_Read 0x09 // This is the chip address 4 shifted left one bit plus the R/Wbit set to read.
#define AD1939_Global_Address_Write 0x08 // This is the chip address 4 shifted left one bit plus theR/W bit set to write.
// Register Addresses:

 // CLOCK settings
#define AD1939_PLL_Control_0 0
#define AD1939_PLL_Control_1 1

#define AD1939_DAC_Control_0 2
#define AD1939_DAC_Control_1 3
#define AD1939_DAC_Control_2 4
 // DAC MUTE SETTINGS
#define AD1939_DAC_Mutes 5
 // DAC1
#define AD1939_DAC_Vol_L1 6
#define AD1939_DAC_Vol_R1 7
 // DAC2
#define AD1939_DAC_Vol_L2 8
#define AD1939_DAC_Vol_R2 9
 // DAC3
#define AD1939_DAC_Vol_L3 10
#define AD1939_DAC_Vol_R3 11
 // DAC4
#define AD1939_DAC_Vol_L4 12
#define AD1939_DAC_Vol_R4 13
 // ADC
#define AD1939_ADC_Control_0 14
#define AD1939_ADC_Control_1 15
#define AD1939_ADC_Control_2 16

/////////////////////////////////////////////////////



static SPI_HandleTypeDef* AD1939_spi;

static uint8_t TXdata[3];
static uint8_t RXdata[3];

// send bytes
int ad1939_write_reg(uint8_t reg_address, uint8_t data);

int ad1939_read_reg(uint8_t reg_address, uint8_t data);

// FUNCTION: ad1939_init sets up the interface
int ad1939_init(SPI_HandleTypeDef* hspi_codec);

#endif
