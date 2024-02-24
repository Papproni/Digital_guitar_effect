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


// SUBBAND FILTER FUNCTION - DIRECT FORM 2 - normalfunction exectime: ~6us
static void subbandfilter_calculation(int32_t input){
  float32_t input_f32=(float32_t)input;
  // set d[n], d[n-1], d[n-2]
  for(int i=0;i<numberofsubbands;i++){
//			  subbandfilter_input[i]	= input_f32;
			  subbandfilter_dn2[i]		= subbandfilter_dn1[i];
			  subbandfilter_dn1[i]		= subbandfilter_dn[i];
			  subbandfilter_dn[i]		= input_f32;

  }
  // A1 = a1*y[n-1]
  arm_mult_f32(subbandfilter_a1, subbandfilter_yn1, subbandfilter_A1, numberofsubbands);
  // A2 = a2*y[n-2]
  arm_mult_f32(subbandfilter_a2, subbandfilter_yn2, subbandfilter_A2, numberofsubbands);

  // A = A1+A2
  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_A, numberofsubbands);

  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

  // B1 = b1*x[n-1]
  arm_mult_f32(subbandfilter_b1, subbandfilter_dn1, subbandfilter_B1, numberofsubbands);
  // B2 = b2*x[n-2]
  arm_mult_f32(subbandfilter_b2, subbandfilter_dn2, subbandfilter_B2, numberofsubbands);
  // B1+B2
  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_B, numberofsubbands);

  // B0 = b0*x[n]
  arm_mult_f32(subbandfilter_b0, subbandfilter_dn, subbandfilter_B0, numberofsubbands);

  // y=B0+B1+B2
  arm_add_f32(subbandfilter_B, subbandfilter_B0, subbandfilter_B, numberofsubbands);

  // y[n]= B - A
  arm_sub_f32(subbandfilter_B,subbandfilter_A, subbandfilter_output, numberofsubbands);
  for(int i=0;i<numberofsubbands;i++){
	  subbandfilter_yn2[i]		= subbandfilter_yn1[i];
	  subbandfilter_yn1[i]		= subbandfilter_output[i];
   }
}


void subbandfilter_octave2_calculation(int32_t input){
  float32_t input_f32=(float32_t)input;
  // set d[n], d[n-1], d[n-2]
  for(int i=0;i<numberofsubbands;i++){
			  subbandfilter_octave2_dn2[i]=subbandfilter_octave2_dn1[i];
			  subbandfilter_octave2_dn1[i]=subbandfilter_octave2_dn[i];
			  subbandfilter_octave2_dn[i] = input_f32;
  }
  // A1 = a1*y[n-1]
  arm_mult_f32(subbandfilter_a1, subbandfilter_octave2_yn1, subbandfilter_A1, numberofsubbands);
  // A2 = a2*y[n-2]
  arm_mult_f32(subbandfilter_a2, subbandfilter_octave2_yn2, subbandfilter_A2, numberofsubbands);

  // A = A1+A2
  arm_add_f32(subbandfilter_A1, subbandfilter_A2, subbandfilter_A, numberofsubbands);

  // y_n=b0*d[n]+b1*d[n-1]+b2*d[n-2]

  // B1 = b1*x[n-1]
  arm_mult_f32(subbandfilter_b1, subbandfilter_octave2_dn1, subbandfilter_B1, numberofsubbands);
  // B2 = b2*x[n-2]
  arm_mult_f32(subbandfilter_b2, subbandfilter_octave2_dn2, subbandfilter_B2, numberofsubbands);
  // B1+B2
  arm_add_f32(subbandfilter_B1, subbandfilter_B2, subbandfilter_B, numberofsubbands);

  // B0 = b0*x[n]
  arm_mult_f32(subbandfilter_b0, subbandfilter_octave2_dn, subbandfilter_B0, numberofsubbands);

  // y=B0+B1+B2
  arm_add_f32(subbandfilter_B, subbandfilter_B0, subbandfilter_B, numberofsubbands);

  // y[n]= B - A
  arm_sub_f32(subbandfilter_B,subbandfilter_A, subbandfilter_output, numberofsubbands);
  for(int i=0;i<numberofsubbands;i++){
 			  subbandfilter_octave2_yn2[i] 	= subbandfilter_octave2_yn1[i];
 			  subbandfilter_octave2_yn1[i]  = subbandfilter_output[i];
   }
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
