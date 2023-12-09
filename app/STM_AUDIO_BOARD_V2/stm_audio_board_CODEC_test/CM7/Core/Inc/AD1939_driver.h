#ifndef AD1939_DRIVER_H
#define AD1939_DRIVER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#define Global_Address_Read 0x09 // This is the chip address 4 shifted left one bit plus the R/W
bit set to read.
#define Global_Address_Write 0x08 // This is the chip address 4 shifted left one bit plus the
R/W bit set to write.
// Register Addresses:
#define PLL_Control_0 0
#define PLL_Control_1 1
#define DAC_Control_0 2
#define DAC_Control_1 3
#define DAC_Control_2 4
#define DAC_Mutes 5
#define DAC_Vol_L1 6
#define DAC_Vol_R1 7
#define DAC_Vol_L2 8
#define DAC_Vol_R2 9
#define DAC_Vol_L3 10
#define DAC_Vol_R3 11
#define DAC_Vol_L4 12
#define DAC_Vol_R4 13
#define ADC_Control_0 14
#define ADC_Control_1 15
#define ADC_Control_2 16



/*
 * TODO LIST:
 * TODO 1: config registert from SPI
 * TODO 2: setup I2S for ADC
 * TODO 3: setup I2S for DAC
 * TODO 4: create interface for in and output of data
 */


// send bytes
int ad1939_send_cmd(uint8_t reg_address, uint8_t data){

}

// FUNCTION: ad1939_init sets up the interface
int ad1939_init(SPI_HandleTypeDef* hspi_codec){

 }
