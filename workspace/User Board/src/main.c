/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "cc2500.h"


// ID for thread
osThreadId	Rx_thread;
osThreadId  Tx_thread;
	
void Blinky_GPIO_Init(void){
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}

void TxPacket(void const *argument) {
   uint8_t buf[3];
   buf[0] = SMARTRF_SETTING_ADDR; // src address on the first byte
   buf[1] = 0x00; // packet sequence number
   buf[2] = 0x00;
   while (1) {
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      CC2500_TxPacket((uint8_t*)buf, SMARTRF_SETTING_PKTLEN);
      printf("transmitted byte: SRC: 0x%02x\t\t SEQ: 0x%02x\n", buf[0], buf[1]);
      
      // increment the sequence by 1. roll over if reached 255
      if (buf[1] == 0xFF)
         buf[1] = 0x00;
      else {
         buf[1]++;
      }
      osDelay(100);
   }
}

osThreadDef(TxPacket, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */

int main (void) {
	osKernelInitialize ();              // initialize CMSIS-RTOS
  
   // initialize peripherals here
	Blinky_GPIO_Init();
	CC2500_Init();

   Tx_thread = osThreadCreate(osThread(TxPacket), NULL);
	osKernelStart();  // start threading
}

