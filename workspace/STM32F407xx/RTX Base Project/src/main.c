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
	while(1){
		osSignalWait(RX_PKT, osWaitForever);
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
   uint8_t buf[2], reg;
   buf[0] = 0x2B;
   buf[1] = 0x00;
   while (1) {
      GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
      CC2500_TxPacket((uint8_t*)buf, 0x02);
      printf("transmitted byte: 0x%02x\n", buf[1]);
      osDelay(1000);
      if (buf[1] == 0xFF)
         buf[1] = 0x00;
      else {
         buf[1]++;
      }
   }
}

osThreadDef(RxPacket, osPriorityNormal, 1, 0);
osThreadDef(TxPacket, osPriorityNormal, 1, 0);

/*
 * main: initialize and start the system
 */
int main (void) {
  uint8_t buf;
	osKernelInitialize ();                    // initialize CMSIS-RTOS
  
  // initialize peripherals here
	Blinky_GPIO_Init();
	wireless_init();
	
  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	//Rx_thread = osThreadCreate(osThread(RxPacket), NULL);
    
	uint8_t reg;

	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCTRL1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCTRL0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FREQ2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FREQ1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FREQ0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MDMCFG4, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MDMCFG3, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MDMCFG2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MDMCFG1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MDMCFG0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_CHANNR, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_DEVIATN, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FRIEND1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FRIEND0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_MCSM0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FOCCFG, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_BSCFG, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_AGCCTRL2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_AGCCTRL1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_AGCCTRL0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCAL3, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCAL2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCAL1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FSCAL0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,SMARTRF_SETTING_FSTEST, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_TEST2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_TEST1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_TEST0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_FIFOTHR, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_IOCFG2, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_IOCFG0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_PKTCTRL1, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_PKTCTRL0, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_ADDR, 1);
	printf("RSSI: 0x%02x\n", buf);
	
	CC2500_Read(&buf,CC2500_CFG_REG_PKTLEN, 1);
	printf("RSSI: 0x%02x\n", buf);
	
   Tx_thread = osThreadCreate(osThread(TxPacket), NULL);
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
