
#include "I2C.h"
#include "main.h"
#include <stdarg.h> //librairie qui permet de coder des fonctions à nombre variable de paramètres
#include <stdint.h>


int begin_init() {

	//prepareForDataRead()
	send_data_init_I2C(1, kOffsetData);

	//setMode
    send_data_init_I2C(3, kOffsetCommand, kCommandMode, 3, 8, 0);//mode diff : 3, mode raw : 1, mode centroid : 0

     //setScanSettings()
	send_data_init_I2C(4, kOffsetCommand, kCommandScanSettings, 0, 11); // speed and resolution

	//updateBaseline()
    //send_data_init_I2C(2, kOffsetCommand, kCommandBaselineUpdate);
    //CSD_InitializeBaselines();

    send_data_init_I2C(4, kOffsetCommand, kCommandeAutoScanTimer, 16, 10); // 16*10 = counter of 160
    // 32 000 / 160 = 200Hz so 5ms for Scan timing period

	//setPrescaler()
    send_data_init_I2C(3 , kOffsetCommand, kCommandPrescaler, 0x01); // prescaler at 0x01 for the value 2

    return 0;
}

int send_data_init_I2C (int counter, ...) { // prend un nombre variable de paramètre
	va_list args;
	va_start(args, counter); // initialise l'allocation mémoire pour la macro va_list


	//Filling of the transmit buffer
	uint8_t sendBuf[counter];
	for (int i = 0; i<counter; i++) {
		sendBuf[i] = va_arg(args, int);
	}
	va_end(args);

	uint32_t startTime = HAL_GetTick();
    int ret = HAL_I2C_Master_Transmit(&hi2c1, i2cAddress, sendBuf, sizeof(sendBuf), kTimeout);
    while ((HAL_GetTick()-startTime)<25) {
    	//on attend sans utiliser de delai
    }
    if(HAL_OK != ret) {

	  return 1;
    }
    return 0;
}

int receive_data(uint8_t receiveDataBuffer[]) {

  uint32_t startTime = HAL_GetTick();
  int ret = HAL_I2C_Master_Receive(&hi2c1, i2cAddress, receiveDataBuffer, SizeData, kTimeout);
  while(ret != HAL_OK);
  while ((HAL_GetTick() - startTime)<25) {
    	//on attend sans utiliser de delai
    }
  if(ret != HAL_OK) {
	Error_Handler();
    return 1;
  }
  return 0;
}

/*void formatingData(uint8_t nonFormatedData[], uint16_t formatedData[]) {
	for (int pad = 0; pad < 30; pad++) {
		formatedData[pad] = (nonFormatedData[2*pad]<<8) | nonFormatedData[2*pad+1];
	}
}//*/
