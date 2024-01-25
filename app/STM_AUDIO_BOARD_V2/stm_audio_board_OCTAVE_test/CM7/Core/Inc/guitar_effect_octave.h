#ifndef GUITAR_EFFECT_OCTAVE_H
#define GUITAR_EFFECT_OCTAVE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "arm_math.h"

 typedef struct{
	 float32_t sub_2_f32;
	 float32_t sub_1_f32;
	 float32_t clean_f32;
	 float32_t up_1_f32;
	 float32_t up_2_f32;
 }octave_volume_tst;

typedef struct octave_effect_tst{
	octave_volume_tst 	volume_st;
	float32_t 			input_f32;
	float32_t 			output_f32;
}octave_effect_tst;


typedef union
{
	struct{
		int16_t raw_low;
		int16_t raw_high;
	};
	int32_t value;
}io_data_converter_u;

typedef struct guitar_effect_tst{

	//Private:
	// I/O buffers for DMA
	uint16_t 			input_buffer_au16[4];
	uint16_t 			output_buffer_au16[4];

	// converts data from 16 bit to 32
	io_data_converter_u	input_data_u;
	io_data_converter_u	output_data_u;

	// AD callback sets this flag meaning hte input buffer is ready
	uint8_t 			adc_ready_flag_u8;


	// I/O converted to float32_t
	float32_t 			input_f32;
	float32_t 			output_f32;


	void (*callback) (struct guitar_effect_tst* self);

}guitar_effect_tst;

volatile guitar_effect_tst guitar_effect_st;

#endif
