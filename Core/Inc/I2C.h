/*
 * I2C.h
 *
 *  Created on: Jun 18, 2024
 *      Author: Chloe
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include <stdint.h>
#include "stm32g4xx_hal.h"

#define i2cAddress (0x38 << 1)
#define SizeData 64

extern I2C_HandleTypeDef hi2c1;

enum {
	kCommandMode = 1,
	kCommandScanSettings = 2,
	kCommandPrescaler = 3,
	kCommandNoiseThreshold = 4,
	kCommandBaselineUpdate = 6,
	kCommandeAutoScanTimer = 14,
	kCommandIdentify = 255,
	kOffsetCommand = 0,
	kOffsetData = 4,
	kTimeout=1000
};

int begin_init();
int send_data_init_I2C (int counter, ...);
int receive_data(uint8_t receiveDataBuffer[]);


#endif /* INC_I2C_H_ */
