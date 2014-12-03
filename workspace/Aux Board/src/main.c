/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "cc2500.h"

#define RX_PKT 0x01

void wireless_init(void);

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

void RxPacket(void const *argument){
	uint8_t mode_filter, transmit_mode;
   uint8_t buf, packet[SMARTRF_SETTING_PKTLEN + 2];
	
   GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
   mode_filter = 0x70;
   transmit_mode = 0x10;
	printf("Thread_started. waitig for signal\n");
   // put reciever in RX mode
	CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
   
	while(1){
		osSignalWait(RX_PKT, osWaitForever);
      //turn on LED on packet RX
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      
		CC2500_Read((uint8_t*)packet, CC2500_FIFO_ADDR, SMARTRF_SETTING_PKTLEN + 2);
      // put the measured RSSI in  byte 3 for main board
      packet[2] = packet[3];
      printf("Packet received from user beacon\n");
      printf("SRC: 0x%02x\t\t", packet[0]);
      printf("SEQ: 0x%02x\t\t", packet[1]);
      printf("RAW_RSSI: 0x%02x\t\t", packet[2]);
      
      // change the source address on the packet
      packet[0] = SMARTRF_SETTING_ADDR;
      // transmit this packet for main board
      CC2500_TxPacket((uint8_t*)packet, SMARTRF_SETTING_PKTLEN);
      
      // wait for the transmission to finish
      
      buf = CC2500_Strobe(CC2500_STROBE_SNOP, 0x01);
      while ((mode_filter & buf) != 0x20)
         buf = CC2500_Strobe(CC2500_STROBE_SNOP, 0x01);
      // turn off LED on successful Tx
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      
      // put device back to rx mode
      CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	}
}


osThreadDef(RxPacket, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
  uint8_t buf, reg;
  osKernelInitialize ();                    // initialize CMSIS-RTOS
  
  // initialize peripherals here
  Blinky_GPIO_Init();
  wireless_init();

   Rx_thread = osThreadCreate(osThread(RxPacket), NULL);
	osKernelStart();
}

void wireless_init() {
	CC2500_Init();
	
	GPIO_InitTypeDef GPIO_init_st;
	NVIC_InitTypeDef NVIC_init_st;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//Configure GPIO pin 0 in input mode
	GPIO_init_st.GPIO_Mode = GPIO_Mode_IN;
	GPIO_init_st.GPIO_Pin = CC2500_SPI_INT_PIN;
	GPIO_init_st.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_init_st.GPIO_Speed = GPIO_High_Speed;
	GPIO_init_st.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(CC2500_SPI_INT_GPIO_PORT, &GPIO_init_st);
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, CC2500_SPI_INT_EXTI_PIN_SOURCE);
	EXTI_InitStructure.EXTI_Line = CC2500_SPI_INT_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// configure NVIC
	NVIC_init_st.NVIC_IRQChannel = CC2500_SPI_INT_EXTI_IRQn;
	NVIC_init_st.NVIC_IRQChannelCmd = ENABLE;
	NVIC_init_st.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_init_st.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_Init(&NVIC_init_st);
	
	NVIC_EnableIRQ(CC2500_SPI_INT_EXTI_IRQn);
}

/**
  * @brief  Interrupt Service Routine for EXTI12_IRQHandler
  */
void EXTI15_10_IRQHandler(void) {
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
			osSignalSet(Rx_thread, RX_PKT);
		}	
	// clear EXTI inruppt pending bit.
	EXTI_ClearITPendingBit(EXTI_Line12);
}
