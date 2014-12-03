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
	uint8_t buf;
	uint8_t packet[4];
   float rssi;
	GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	printf("Thread_started. waitig for sigal\n");
   // put the device in rx mode
	CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	while(1){
		osSignalWait(RX_PKT, osWaitForever);
      // wait until all the bytes are recieved
      buf = CC2500_Strobe(CC2500_STROBE_SNOP, 0x01);
      // toggle LEDS indicating RX
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		CC2500_Read((uint8_t*)packet, CC2500_FIFO_ADDR, 0x04);
      rssi = CC2500_ComputeRssi((float)packet[2]);
      printf("Data: 0x%04x\t", *((uint16_t*)(packet)));
      printf("RSSI: %f\n", rssi);
      // put device back to rx mode
      CC2500_Strobe(CC2500_STROBE_SRX, 0x00);
	}
}

void TxPacket(void const *argument) {
   uint8_t buf[2];
   buf[0] = SMARTRF_SETTING_ADDR; // src address on the first byte
   buf[1] = 0x00; // packet sequence number
   while (1) {
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      CC2500_TxPacket((uint8_t*)buf, 0x02);
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
	wireless_init();

   Tx_thread = osThreadCreate(osThread(TxPacket), NULL);
	osKernelStart();  // start threading
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
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	// configure NVIC
	NVIC_init_st.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_init_st.NVIC_IRQChannelCmd = ENABLE;
	NVIC_init_st.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_init_st.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_Init(&NVIC_init_st);
	
	NVIC_EnableIRQ(EXTI15_10_IRQn);
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
