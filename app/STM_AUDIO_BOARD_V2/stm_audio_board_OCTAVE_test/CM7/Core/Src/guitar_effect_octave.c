/*
 * guitar_effect_octave.c
 *
 *  Created on: Jan 25, 2024
 *      Author: pappr
 */

#include "guitar_effect_octave.h"

void init_octave_effect(octave_effects_tst* self){

	// assign function pointers
	self->set_volumes			= set_volumes;
	self->calc_octave_1_up 		= algorithm_octave_1_down;
	self->calc_octave_1_down 	= algorithm_octave_1_up;
	self->callback 				= callback_octave_effect;
}

static void set_volumes(struct octave_effects_st* self){
	// reset output
	self->output_f32 = 0;

	// calculate the new output defined by the voluem params
	self->output_f32 += self->input_f32 		* self->volumes_st.clean_f32;
	self->output_f32 += self->octave_up_1_f32 	* self->volumes_st.up_1_f32;
	self->output_f32 += self->octave_down_1_f32 * self->volumes_st.sub_1_f32;
}

// Calculate the octave 1 HIGHER
static void algorithm_octave_1_up(struct octave_effects_st* self){
	// TODO
}

// Calculate the octave 1 LOWER
static void algorithm_octave_1_down(struct octave_effects_st* self){
	// TODO
}

static int32_t callback_octave_effect(struct octave_effects_st* self,int32_t input_i32){
	// 1. calculate octaves

	// 2. set volumes

	// 3. return value


	// LEGACY CODE

	// +1 octave
	subbandfilter_calculation(value_from_ADC);
	octave1up();
	// save result
	float32_t octave_1_up_f32 = octave1_up_filtered;

	// +2 octave
	subbandfilter_octave2_calculation((int32_t)(octave_1_up_f32*4));
	octave2up();
	//		 save result
	float32_t octave_2_up_f32 = octave1_up_filtered;

	// Write to DAC
	volatile static float32_t passthrough_volume = 0.3;
	volatile static float32_t octave_1_volume = 4;
	volatile static float32_t octave_2_volume = 4;
	output_test_ac=	(int32_t)octave_1_up_f32*octave_1_volume +
					(int32_t)octave_2_up_f32*octave_2_volume +
					(int32_t)((float32_t)value_from_ADC*passthrough_volume);

}
