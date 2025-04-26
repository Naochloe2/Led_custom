/*
 * NeoPixel.c
 *
 *  Created on: Jun 24, 2024
 *      Author: Chloe
 */

#include "NeoPixel.h"
#include "main.h"
#include <stdint.h>

// tableau contenant les bytes des 4 couleurs pour chaques LEDs
uint8_t rgb_array[NUM_BYTES];

// Buffer d'ecriture
uint8_t wr_buffer[Size_buffer];
uint_fast8_t wr_buffer_p = 0; //pointeur sur le buffer d'ecriture

static inline uint8_t scale8(uint8_t x, uint8_t scale) {
  return ((uint16_t)x * scale) >> 8;
}

void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
	rgb_array[4 * index    ] = g;
	rgb_array[4 * index + 1] = r;
	rgb_array[4 * index + 2] = b;
	rgb_array[4 * index + 3] = w;
}

void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for(uint_fast8_t i = 0; i < NUM_LED; ++i) led_set_RGBW(i, r, g, b, w);
}

void multicolor(uint8_t intensity, uint8_t r_buffer[23], uint8_t g_buffer[23], uint8_t b_buffer[23], uint8_t w_buffer[23])
{
	uint8_t color = intensity;

	r_buffer[0] = intensity;
	g_buffer[0] = 0;
	b_buffer[0] = 0;
	w_buffer[0] = 0;
	//led_set_RGBW(0, r_buffer[0],g_buffer[0], b_buffer[0], w_buffer[0]);
	for (uint_fast8_t i = 1; i < NUM_LED; ++i){

		int section = (NUM_LED/3);
		if (i < (section+1)){ //9
			r_buffer[i] = color;
			g_buffer[i] = (intensity-color);
			b_buffer[i] = 0;
			w_buffer[i] = 0;
			color = color - (intensity/section); //8
		}
		else if (i <(section*2+1)){ //17
			r_buffer[i] = 0;
			g_buffer[i] = (intensity-color);
			b_buffer[i] = color;
			w_buffer[i] = 0;
			color = color + (intensity/section);
		}
		else {
			r_buffer[i] = (intensity-color);
			g_buffer[i] = 0;
			b_buffer[i] = color;
			w_buffer[i] = 0;
			color = color - (intensity/section);
		}
		//led_set_RGBW(i, r_buffer[i],g_buffer[i], b_buffer[i], w_buffer[i]);
	}
}

void led_render() {
  if(wr_buffer_p != 0 || hdma_tim2_ch1.State != HAL_DMA_STATE_READY) {
    // Ongoing transfer, cancel!
    for(uint8_t i = 0; i < Size_buffer; ++i) wr_buffer[i] = 0;
    wr_buffer_p = 0;
    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
    return;
  }
  // Ooh boi the first data buffer half (and the second!)
	for(uint_fast8_t i = 0; i < 8; ++i) {
	  wr_buffer[i     ] = PWM_LO << (((rgb_array[0] << i) & 0x80) > 0);
	  wr_buffer[i +  8] = PWM_LO << (((rgb_array[1] << i) & 0x80) > 0);
	  wr_buffer[i + 16] = PWM_LO << (((rgb_array[2] << i) & 0x80) > 0);
	  wr_buffer[i + 24] = PWM_LO << (((rgb_array[3] << i) & 0x80) > 0);
	  wr_buffer[i + 32] = PWM_LO << (((rgb_array[4] << i) & 0x80) > 0);
	  wr_buffer[i + 40] = PWM_LO << (((rgb_array[5] << i) & 0x80) > 0);
	  wr_buffer[i + 48] = PWM_LO << (((rgb_array[6] << i) & 0x80) > 0);
	  wr_buffer[i + 56] = PWM_LO << (((rgb_array[7] << i) & 0x80) > 0);
  }

  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)wr_buffer, 64);
  wr_buffer_p = 2; // Since we're ready for the next buffer
}

void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim) {
  // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
  if(wr_buffer_p < NUM_LED) {
    // We're in. Fill the even buffer
    for(uint_fast8_t i = 0; i < 8; ++i) {
    	wr_buffer[i     ] = PWM_LO << (((rgb_array[4 * wr_buffer_p    ] << i) & 0x80) > 0);
    	wr_buffer[i +  8] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 1] << i) & 0x80) > 0);
    	wr_buffer[i + 16] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 2] << i) & 0x80) > 0);
    	wr_buffer[i + 24] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 3] << i) & 0x80) > 0);
    }
    wr_buffer_p++;
  } else if (wr_buffer_p < NUM_LED + 2) {
    // Last two transfers are resets. SK6812: 64 * 1.25 us = 80 us == good enough reset
    // First half reset zero fill
	  for(uint8_t i = 0; i < (Size_buffer/2); ++i) wr_buffer[i] = 0;
	  wr_buffer_p++;
  }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
  // DMA buffer set from LED(wr_buf_p) to LED(wr_buf_p + 1)
  if(wr_buffer_p < NUM_LED) {
    // We're in. Fill the odd buffer
    for(uint_fast8_t i = 0; i < 8; ++i) {
    	wr_buffer[i + 32] = PWM_LO << (((rgb_array[4 * wr_buffer_p    ] << i) & 0x80) > 0);
    	wr_buffer[i + 40] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 1] << i) & 0x80) > 0);
    	wr_buffer[i + 48] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 2] << i) & 0x80) > 0);
    	wr_buffer[i + 56] = PWM_LO << (((rgb_array[4 * wr_buffer_p + 3] << i) & 0x80) > 0);
    }
    wr_buffer_p++;
  }
  else if (wr_buffer_p < NUM_LED + 2) {
    // Second half reset zero fill
    for(uint8_t i = (Size_buffer/2); i < Size_buffer; ++i) wr_buffer[i] = 0;
    ++wr_buffer_p;
  }
  else {
    // We're done. Lean back and until next time!
	  wr_buffer_p = 0;
    HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
  }
}//*/
