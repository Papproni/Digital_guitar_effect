#include "AD1939_driver.h"

// send bytes
int ad1939_write_reg(uint8_t reg_address, uint8_t data){
	int8_t retval = 0;
	TXdata[0] = AD1939_Global_Address_Write;
	TXdata[1] = reg_address;
	TXdata[2] = data;

	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 0);
	 // SPI SEND CMD
	 retval = HAL_SPI_TransmitReceive(AD1939_spi, TXdata, RXdata, 3, 1000);
	 // CS HIGH
	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 1);

	 //HAL_Delay(10);
	return retval;
}

int ad1939_read_reg(uint8_t reg_address, uint8_t data){
	int8_t retval = 0;
	TXdata[0] = AD1939_Global_Address_Read;
	TXdata[1] = reg_address;
	TXdata[2] = data;

	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 0);
	 // SPI SEND CMD
	 retval = HAL_SPI_TransmitReceive(AD1939_spi, TXdata, RXdata, 3, 1000);
	 // CS HIGH
	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 1);

	return retval;
}

int ad1939_pll_locked()
{
	TXdata[0] = AD1939_Global_Address_Read;
	TXdata[1] = AD1939_PLL_Control_1;
	TXdata[2] = 0b00000100;
	HAL_Delay(10);
	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 0);
	 // SPI SEND CMD
	 HAL_SPI_TransmitReceive(AD1939_spi, TXdata, RXdata, 3, 1000);
	 // CS HIGH
	 HAL_GPIO_WritePin(CODEC_CS_GPIO_Port, CODEC_CS_Pin, 1);

	 return 0;
}

// FUNCTION: ad1939_init sets up the interface
int ad1939_init(SPI_HandleTypeDef* hspi_codec) {
    AD1939_spi = hspi_codec;

    // TODO: TURN OFF PLL and DISABLE ADC, DAC


    // CLOCK settings

    if (ad1939_write_reg(AD1939_PLL_Control_0, 0b10011001)) return -1;
// PLL LOCKS for some reason!! not output still
    if (ad1939_write_reg(AD1939_PLL_Control_0, 0b10111110)) return -1;

    if (ad1939_write_reg(AD1939_PLL_Control_1, 0b00000100)) return -1;


//    if (ad1939_write_reg(AD1939_PLL_Control_0, 0b10011000)) return -1;
//
//    if (ad1939_write_reg(AD1939_PLL_Control_1, 0b00000100)) return -1;





    HAL_Delay(100);

    if(ad1939_pll_locked()){
    	// wait till pll is locked takes about 10 ms acc to datasheet
    }
    // DAC settings
    // 192 khz
    //    if (ad1939_write_reg(AD1939_DAC_Control_0, 0b00000100)) return -1;
    // 96 khz
//            if (ad1939_write_reg(AD1939_DAC_Control_0, 0b00000010)) return -1;
    // 48 Khz
    if (ad1939_write_reg(AD1939_DAC_Control_0, 0b00000000)) return -1;
    if (ad1939_write_reg(AD1939_DAC_Control_1, 0b00000000)) return -1; // Slave setup
    //if (ad1939_write_reg(AD1939_DAC_Control_1, 0b01110000)) return -1; // MASTER setup
    if (ad1939_write_reg(AD1939_DAC_Control_2, 0b00000000)) return -1;

    // DAC MUTE SETTINGS
    // OL2N and OL2P 0011 1011
    if (ad1939_write_reg(AD1939_DAC_Mutes, 0x00)) return -1; // enable only DAC2 L and R channels

    // DAC2 VOLUME SETTINGS (other channels are muted)
    if (ad1939_write_reg(AD1939_DAC_Vol_L2, 0x00)) return -1; // no attenuation
    if (ad1939_write_reg(AD1939_DAC_Vol_R2, 0x00)) return -1; // no attenuation

    // ADC settings
    // 192 khz
	//    if (ad1939_write_reg(AD1939_ADC_Control_0, 0b10000000)) return -1;
    // 96khz
//        if (ad1939_write_reg(AD1939_ADC_Control_0, 0b01000000)) return -1;
        // 48 Khz
    if (ad1939_write_reg(AD1939_ADC_Control_0, 0b00000000)) return -1;
    if (ad1939_write_reg(AD1939_ADC_Control_1, 0x00 )) return -1;
    if (ad1939_write_reg(AD1939_ADC_Control_2, 0x00 )) return -1;



    return 0; // Return 0 if all writes are successful
}
