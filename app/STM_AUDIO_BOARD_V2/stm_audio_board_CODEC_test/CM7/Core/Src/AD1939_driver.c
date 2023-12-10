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

// FUNCTION: ad1939_init sets up the interface
int ad1939_init(SPI_HandleTypeDef* hspi_codec) {
    AD1939_spi = hspi_codec;

    // CLOCK settings
    if (ad1939_write_reg(AD1939_PLL_Control_0, 0x80)) return -1;
    if (ad1939_write_reg(AD1939_PLL_Control_1, 0x07)) return -1;

    // DAC settings
    if (ad1939_write_reg(AD1939_DAC_Control_0, 0x00)) return -1;
    if (ad1939_write_reg(AD1939_DAC_Control_1, 0x00)) return -1;
    if (ad1939_write_reg(AD1939_DAC_Control_2, 0x00)) return -1;

    // DAC MUTE SETTINGS
    if (ad1939_write_reg(AD1939_DAC_Mutes, 0xF3)) return -1; // enable only DAC2 L and R channels

    // DAC2 VOLUME SETTINGS (other channels are muted)
    if (ad1939_write_reg(AD1939_DAC_Vol_L2, 0x00)) return -1; // no attenuation
    if (ad1939_write_reg(AD1939_DAC_Vol_R2, 0x00)) return -1; // no attenuation

    // ADC settings
    if (ad1939_write_reg(AD1939_ADC_Control_0, 0x3A)) return -1;
    if (ad1939_write_reg(AD1939_ADC_Control_1, 0x00
    		)) return -1;
    if (ad1939_write_reg(AD1939_ADC_Control_2, 0x01)) return -1;

    return 0; // Return 0 if all writes are successful
}
