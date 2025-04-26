/*
 * NeoPixel.h
 *
 *  Created on: Jun 24, 2024
 *      Author: Chloe
 */

#ifndef INC_NEOPIXEL_H_
#define INC_NEOPIXEL_H_


#include <stdint.h>
#include "stm32g4xx_hal.h"

extern TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_tim2_ch1;

#define PWM_LO (59 * 32 / 100)
#define PWM_HI (59 * 64 / 100)

#define NUM_LED 24
#define NUM_COLOR 4
#define NUM_BYTES (NUM_LED * NUM_COLOR)
#define Size_buffer 64

void led_set_RGBW(uint8_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_set_all_RGBW(uint8_t r, uint8_t g, uint8_t b, uint8_t w);
void led_render();
void multicolor(uint8_t intensity, uint8_t r_buffer[23], uint8_t g_buffer[23], uint8_t b_buffer[23], uint8_t w_buffer[23]);

#endif /* INC_NEOPIXEL_H_ */
