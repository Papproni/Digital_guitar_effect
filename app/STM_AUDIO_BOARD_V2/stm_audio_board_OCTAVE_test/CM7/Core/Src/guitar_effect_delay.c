/*
 * guitar_effect_delay.c
 *
 *  Created on: Feb 1, 2024
 *      Author: pappr
 */

#include "guitar_effect_delay.h"




/*
 * FUNC:
 * 		calculate_buffer_location()
 * DESC:
 * 		calculates the position in the buffer where the delayed signal should be read from
 * 		For now this will only use the time_buffer_u32
 */
static void calculate_buffer_location(struct delay_effects_st* self){
	// current_pointer + buffer[time]
	self->delayed_counter_u32 = self->current_counter_u32 + self->parameters_st.time_in_buffer_u32;

	// if delay sample is bigger than the buffer go back
	if(self->delayed_counter_u32 > (DELAY_BUFFER_LENGTH-1) ){
		self->delayed_counter_u32 = self->delayed_counter_u32- (DELAY_BUFFER_LENGTH-1);
	}
}

/*
 * FUNC:
 * 		increment_current_sample_counter()
 * DESC:
 * 		Increments the current sample counter
 */
static void increment_current_sample_counter(struct delay_effects_st* self){
	self->current_counter_u32++;
	if(self->current_counter_u32 > (DELAY_BUFFER_LENGTH-1) ){
		self->current_counter_u32 = 0;
	}
}

/*
 * FUNC:
 * 		get_delayed_signal()
 * DESC:
 * 		Returns the delayed signal value stored in buffer
 */
static int32_t get_delayed_signal(struct delay_effects_st* self){
	return self->buffer_ai32[self->delayed_counter_u32];
}

/*
 * FUNC:
 * 		callback()
 * DESC:
 * 		Delay algorithm
 */
static int32_t callback(struct delay_effects_st* self,int32_t input_signal_i32){

	self->input_i32 = input_signal_i32;
	calculate_buffer_location(self);

	// delay effect
	int32_t delayed_sample_i32 = get_delayed_signal(self);

	// feedback calculation input,delayed,feedback
	self->buffer_ai32[self->current_counter_u32] = 	(int32_t)( (float)self->buffer_ai32[self->delayed_counter_u32] * self->parameters_st.feedback_gain_f32) +
													(int32_t)( (float)self->input_i32 * (1.0 - self->parameters_st.feedback_gain_f32));

	self->output_i32 = (float)delayed_sample_i32*self->parameters_st.mix_f32 +
						(float)self->input_i32*(1.0 - self->parameters_st.mix_f32);

	increment_current_sample_counter(self);

	// modulation
	if( self->modulation_on_u8 ){
		self->parameters_st.time_in_buffer_u32 = DELAY_BUFFER_LENGTH - self->parameters_st.modulation_base_u32 + self->parameters_st.modulation_amplitude_i32*sin((float)self->parameters_st.modulation_counter_u32/(float)self->parameters_st.modulation_in_buffer_u32*6.28);
		self->parameters_st.modulation_counter_u32++;
	}

	return self->output_i32;
}

/*
 * FUNC:
 * 		init_guitar_effect_delay()
 * DESC:
 * 		Sets basic paramters and function pointers to the struct
 */
void init_guitar_effect_delay(struct delay_effects_st* self){

	// set basic parameters
	self->parameters_st.time_in_buffer_u32 			= 80000;	// 12ms delay
	self->modulation_on_u8 							= 0;
	self->parameters_st.modulation_counter_u32 		= 0;
	self->parameters_st.modulation_in_buffer_u32 	= 25000;						// 1ms
	self->parameters_st.modulation_amplitude_i32	= 20;
	self->parameters_st.modulation_base_u32			= 50;
	self->parameters_st.mix_f32						= 0.3; 		// 50%
	self->parameters_st.feedback_gain_f32			= 0.5; 		// 50%


	self->current_counter_u32 = 0;

	for(int i = 0; i<DELAY_BUFFER_LENGTH;i++){
		self->buffer_ai32[i] = 0;
	}
	// add function pointers
	self->callback = callback;
}


