#ifndef GUITAR_EFFECT_OCTAVE_H
#define GUITAR_EFFECT_OCTAVE_H


/* //////////////////////////////////////////////////////////////
 * ****************  OCTAVE EFFECT CALCULATOR ******************
 * --------------------------------------------------------------
 * This library is created to calculate the octave ffect of a guitar signal.
 * The algorythm uses ERB-PS2 algorythm for OCTAVE UP
 * and uses the classic Resample algorythm for the OCTAVE DOWN
 *  -------------------------------------------------------------
 */

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "arm_math.h"

#define numberofsubbands 55
// FILTER DEFINITIONS START





// FILTER DEFINITIONS END

typedef struct{
	float32_t sub_2_f32;
	float32_t sub_1_f32;
	float32_t clean_f32;
	float32_t up_1_f32;
	float32_t up_2_f32;
}octave_volume_tst;

typedef struct octave_effects_st{
	// Volumes
	octave_volume_tst 	volumes_st;

	// I/O
	float32_t 			input_f32;
	float32_t 			output_f32;

	// output of dsp functions
	float32_t 			octave_up_1_f32;
	float32_t 			octave_up_2_f32;
	float32_t 			octave_down_1_f32;
	float32_t 			octave_down_2_f32;

	// create a filter to let only the AC signals to pass to the output
	arm_biquad_cascade_df2T_instance_f32 highpass_iir_50hz_octave_1_up_st;
	arm_biquad_cascade_df2T_instance_f32 highpass_iir_50hz_octave_1_down_st;

	// SUBBAND FILTER VECTORS
	float32_t subbandfilter_input[numberofsubbands];
	float32_t subbandfilter_output[numberofsubbands];

	// saved states
	// x[n] , X[n-1], X[n-2]
	float32_t subbandfilter_dn[numberofsubbands];
	float32_t subbandfilter_dn1[numberofsubbands];
	float32_t subbandfilter_dn2[numberofsubbands];
	// Y[n-1] , Y[n-2]
	float32_t subbandfilter_yn1[numberofsubbands];
	float32_t subbandfilter_yn2[numberofsubbands];

	// x[n] , X[n-1], X[n-2]
	float32_t subbandfilter_octave2_dn[numberofsubbands];
	float32_t subbandfilter_octave2_dn1[numberofsubbands];
	float32_t subbandfilter_octave2_dn2[numberofsubbands];
	// Y[n-1] , Y[n-2]
	float32_t subbandfilter_octave2_yn1[numberofsubbands];
	float32_t subbandfilter_octave2_yn2[numberofsubbands];

	 float32_t subbandfilter_A1[numberofsubbands];
	 float32_t subbandfilter_A2[numberofsubbands];
	 float32_t subbandfilter_A[numberofsubbands];

	 float32_t subbandfilter_B0[numberofsubbands];
	 float32_t subbandfilter_B1[numberofsubbands];
	 float32_t subbandfilter_B2[numberofsubbands];
	 float32_t subbandfilter_B[numberofsubbands];

	// DSP functions
	 void (*subbandfilter_calculation) (struct octave_effects_st* self);
	// Calculate the octave 1 HIGHER
	void (*calc_octave_1_up) (struct octave_effects_st* self);
	// Calculate the octave 1 LOWER
	void (*calc_octave_1_down) (struct octave_effects_st* self);
	// Set volumes
	void (*set_volumes)(struct octave_effects_st* self);

	// Calculates the dsp functions on the input, returns the output
	int32_t (*callback) (struct octave_effects_st* self,int32_t input_i32);
}octave_effects_tst;

// creates the initialisation for the effect
void init_guitar_effect_octave(octave_effects_tst* self);
//volatile octave_effects_tst octave_effect_st;

#endif
