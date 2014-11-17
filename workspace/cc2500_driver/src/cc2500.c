/**
  ******************************************************************************
  * @file    cc2500.c
  * @author  Abdul Haseeb Jehangir
  * @version V1.0.0
  * @date    15-November-2014
  * @brief   This file provides a set of functions needed to manage the CC2500
  ******************************************************************************
	**/

#include "cc2500.h"

/** @defgroup CC2500_Private_FunctionPrototypes
  * @{
  */
static uint8_t CC2500_SendByte(uint8_t byte);
static void CC2500_LowLevel_Init(void);


/**
  * @brief  Set CC2500 Initialization.
  * @param  None
  * @retval None
  */
void CC2500_Init(void) {
   
   CC2500_LowLevel_Init();
   
   // code here
   // Do config defined in project specification
   // set registers using CC2500_sendbyte
}


/**
  * @brief  Reads a block of data from the CC2500.
  * @param  pBuffer : pointer to the buffer that receives the data read from the CC2500.
  * @param  ReadAddr : CC2500's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the CC2500.
  * @retval None
  */
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) {
   // Code here
   // uses CC2500_sendbyte
}

/**
  * @brief  Writes one byte to the CC2500.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the CC2500.
  * @param  WriteAddr : CC2500's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite) {
   // Code here
   // uses CC2500_sendbyte
}

/**
  * @brief  Sends a single strobe command to CC2500
  * @param  Strobe command to send
  * @retval Status update received from CC2500
  */
uint8_t CC2500_Strobe(StrobeCommand StrobeCmd) {
   
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
static uint8_t CC2500_SendByte(uint8_t byte) {
   //Wait until the SPI transmit buffer is empty (page 704, Reference Manual)
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_RXNE) == RESET){}
		
	//Once the buffer is empty, send the byte through the SPI peripheral
	SPI_I2S_SendData(CC2500_SPI, byte); 
		
	//Wait until the SPI receive buffer has something (page 705, Reference Manual)
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE){}\
		
	//Once the buffer has something, return it 
	return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI); 
}

/**
  * @brief  Initializes the low level interface used to drive the CC2500
  * @param  None
  * @retval None
  */
static void CC2500_LowLevel_Init(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   SPI_InitTypeDef  SPI_InitStructure;
   
   /* Enable the SPI periph */
   RCC_APB1PeriphClockCmd(CC2500_SPI_CLK, ENABLE);
   
   /* Enable SCK, CS, INT, MOSI and MISO GPIO clocks */
   RCC_AHB1PeriphClockCmd(CC2500_SPI_SCK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK | CC2500_SPI_MOSI_GPIO_CLK | CC2500_SPI_CS_GPIO_CLK | CC2500_SPI_INT_GPIO_CLK, ENABLE);
   
   GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
   GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
   GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);
   
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

   /* SPI SCK pin configuration */
   GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
   GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

   /* SPI  MOSI pin configuration */
   GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
   GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

   /* SPI MISO pin configuration */
   GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
   GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
   
     /* SPI configuration -------------------------------------------------------*/
   SPI_I2S_DeInit(CC2500_SPI);
   SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
   SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
   SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
   SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
   SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
   SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
   SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
   SPI_InitStructure.SPI_CRCPolynomial = 7;
   SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
   SPI_Init(CC2500_SPI, &SPI_InitStructure);

   /* Enable SPI1  */
   SPI_Cmd(CC2500_SPI, ENABLE);
   
   /* Configure GPIO PIN for cc2500 Chip select */
   GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);
   
   /* Deselect : Chip Select high */
   GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);
   
     /* Configure GPIO PINs to detect Interrupts */
   GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_Init(CC2500_SPI_INT_GPIO_PORT, &GPIO_InitStructure);
}
