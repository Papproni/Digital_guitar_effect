/*
 * guitar_effect_delay.h
 *
 *  Created on: Feb 1, 2024
 *      Author: pappr
 */

#ifndef INC_GUITAR_EFFECT_DELAY_H_
#define INC_GUITAR_EFFECT_DELAY_H_

#include "main.h"


// DEFINES
#define DELAY_BUFFER_LENGTH  48000//48000


typedef struct delay_parameters_st{
	float 		time_f32; 					// in ms
	uint32_t 	time_in_buffer_u32; 		// in buffer location 1 = 1/48 ms
	uint32_t 	modulation_in_buffer_u32;	// sets the modulation of the delay
	uint32_t	modulation_counter_u32;
	int32_t		modulation_amplitude_i32;
	uint32_t 	modulation_base_u32;
	float 		mix_f32;  					// delay and input
	uint8_t 	repeats_u8;					// number of repeats
	float   	feedback_gain_f32;			// how repeats weaken over time

}delay_parameters_tst;

typedef struct delay_effects_st{
	int32_t 				input_i32;
	int32_t 				output_i32;
	int32_t 				buffer_ai32[DELAY_BUFFER_LENGTH];

	// pointers
	uint32_t 				current_counter_u32;
	uint32_t 				delayed_counter_u32;

	delay_parameters_tst 	parameters_st;

	// functions
	int32_t (*callback) (struct delay_effects_st* self,int32_t input_signal_i32);


}delay_effects_tst;


//delay_effects_tst delay_effect;
#endif /* INC_GUITAR_EFFECT_DELAY_H_ */
