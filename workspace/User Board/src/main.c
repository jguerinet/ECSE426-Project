/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "cc2500.h"

#define Transmit_Interval  100

// ID for thread
osThreadId	Rx_thread;
osThreadId  Tx_thread;

typedef struct packet {
   uint8_t Src_addr;
   uint8_t Seq_num;
   uint8_t Aux_rssi;
   uint8_t Rssi;
   uint8_t Crc_id;
} Packet;


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
   Packet pkt;
   pkt.Src_addr = SMARTRF_SETTING_ADDR; // src address on the first byte
   pkt.Seq_num = 0x00;  // packet sequence number
   pkt.Aux_rssi = 0x00; // Aux sequence N/A
   while (1) {
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      CC2500_TxPacket((uint8_t*)&pkt, SMARTRF_SETTING_PKTLEN);
      printf("Transmitted Data: SRC: 0x%02x\t\t SEQ: 0x%02x\n", pkt.Src_addr, pkt.Seq_num);
      
      // increment the sequence by 1. roll over if reached 255
      if (pkt.Seq_num == 0xFF)
         pkt.Seq_num = 0x00;
      else {
         pkt.Seq_num++;
      }
      osDelay(Transmit_Interval);
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

