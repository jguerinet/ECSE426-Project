/**
  ******************************************************************************
  * @file    cc2500.c
  * @author  Abdul Haseeb Jehangir, Julien Guerinet
  * @version V1.0.0
  * @date    15-November-2014
  * @brief   This file provides a set of functions needed to manage the CC2500
  ******************************************************************************
	**/

#include "cc2500.h"
#include <stdio.h>
/* Set the Read/Write bit to 1 if we are reading, 0 if we are writing */
#define READWRITE_BIT 			((uint8_t)0x80)
/* Set the burst bit to 1 if we are reading/writing multiple bytes
7th bit of the header byte (page 21, CC2500) */ 
#define BURST_BIT           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE          ((uint8_t)0x00)
#define RSSI_OFFSET           72
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
   
	 // Wireless configuration as defined on project specification page 9
   uint8_t reg;
   
	 // reset the device first
	 CC2500_Strobe(CC2500_STROBE_SRES, DUMMY_BYTE);
	 osDelay(50);
	
	 // put the device in idle mode
	 CC2500_Strobe(CC2500_STROBE_SIDLE, DUMMY_BYTE);
	
	 osDelay(50);
	
   reg = (uint8_t)SMARTRF_SETTING_FSCTRL1;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCTRL1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSCTRL0;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCTRL0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FREQ2;
   CC2500_Write(&reg, CC2500_CFG_REG_FREQ2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FREQ1;
   CC2500_Write(&reg, CC2500_CFG_REG_FREQ1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FREQ0;
   CC2500_Write(&reg, CC2500_CFG_REG_FREQ0, 1);   
   
   reg = (uint8_t)SMARTRF_SETTING_MDMCFG4;
   CC2500_Write(&reg, CC2500_CFG_REG_MDMCFG4, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_MDMCFG3;
   CC2500_Write(&reg, CC2500_CFG_REG_MDMCFG3, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_MDMCFG2;
   CC2500_Write(&reg, CC2500_CFG_REG_MDMCFG2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_MDMCFG1;
   CC2500_Write(&reg, CC2500_CFG_REG_MDMCFG1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_MDMCFG0;
   CC2500_Write(&reg, CC2500_CFG_REG_MDMCFG0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_CHANNR;
   CC2500_Write(&reg, CC2500_CFG_REG_CHANNR, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_DEVIATN;
   CC2500_Write(&reg, CC2500_CFG_REG_DEVIATN, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FREND1;
   CC2500_Write(&reg, CC2500_CFG_REG_FRIEND1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FREND0;
   CC2500_Write(&reg, CC2500_CFG_REG_FRIEND0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_MCSM0;
   CC2500_Write(&reg, CC2500_CFG_REG_MCSM0, 1);
   
	 reg = (uint8_t)SMARTRF_SETTING_MCSM1;
   CC2500_Write(&reg, CC2500_CFG_REG_MCSM1, 1);
	 
   reg = (uint8_t)SMARTRF_SETTING_FOCCFG;
   CC2500_Write(&reg, CC2500_CFG_REG_FOCCFG, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_BSCFG;
   CC2500_Write(&reg, CC2500_CFG_REG_BSCFG, 1);
	 
   reg = (uint8_t)SMARTRF_SETTING_AGCCTRL2;
   CC2500_Write(&reg, CC2500_CFG_REG_AGCCTRL2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_AGCCTRL1;
   CC2500_Write(&reg, CC2500_CFG_REG_AGCCTRL1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_AGCCTRL0;
   CC2500_Write(&reg, CC2500_CFG_REG_AGCCTRL0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSCAL3;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCAL3, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSCAL2;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCAL2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSCAL1;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCAL1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSCAL0;
   CC2500_Write(&reg, CC2500_CFG_REG_FSCAL0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FSTEST;
   CC2500_Write(&reg, CC2500_CFG_REG_FSTEST, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_TEST2;
   CC2500_Write(&reg, CC2500_CFG_REG_TEST2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_TEST1;
   CC2500_Write(&reg, CC2500_CFG_REG_TEST1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_TEST0;
   CC2500_Write(&reg, CC2500_CFG_REG_TEST0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_FIFOTHR;
   CC2500_Write(&reg, CC2500_CFG_REG_FIFOTHR, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_IOCFG2;
   CC2500_Write(&reg, CC2500_CFG_REG_IOCFG2, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_IOCFG0;
   CC2500_Write(&reg, CC2500_CFG_REG_IOCFG0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_PKTCTRL1;
   CC2500_Write(&reg, CC2500_CFG_REG_PKTCTRL1, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_PKTCTRL0;
   CC2500_Write(&reg, CC2500_CFG_REG_PKTCTRL0, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_ADDR;
   CC2500_Write(&reg, CC2500_CFG_REG_ADDR, 1);
   
   reg = (uint8_t)SMARTRF_SETTING_PKTLEN;
   CC2500_Write(&reg, CC2500_CFG_REG_PKTLEN, 1);
   
	 // flush the rx and tx buffer
	 CC2500_Strobe(CC2500_STROBE_SFTX, DUMMY_BYTE);
	 CC2500_Strobe(CC2500_STROBE_SFRX, DUMMY_BYTE);
}

/**
  * @brief  Reads a block of data from the CC2500.
  * @param  pBuffer : pointer to the buffer that receives the data read from the CC2500.
  * @param  ReadAddr : CC2500's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the CC2500.
  * @retval None
  */
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead) {
  //Configure the header byte: burst access bit (B) and a 6-bit address (page 21, CC2500)
	//If more than one register needs to be written to, set B to 1 (page 23, CC2500)
	//Regardless, set the read/write bit 
	if(NumByteToRead > 0x01){
		ReadAddr |= BURST_BIT | READWRITE_BIT;
	}	
	else{
		ReadAddr |= READWRITE_BIT;
	}
	
	//Set the Chip Select to low at the beginning of the transmission (page 21, CC2500)
	CC2500_CS_LOW(); 
	
	//Send the address of the register
   CC2500_SendByte(ReadAddr);
	//Then loop through the number of bytes that need to be read (MSB first)
	while(NumByteToRead > 0x00){
		//Send a dummy byte, store the response in the buffer
		*pBuffer = CC2500_SendByte(DUMMY_BYTE);
		
		//Decrement the number of bytes to read
		NumByteToRead--; 
		
		//Increment the pBuffer pointer position
		pBuffer++; 
	}
	
	//Set the Chip Select to high at the end of the transmission (page 23, CC2500)
	CC2500_CS_HIGH();
}

/**
  * @brief  Writes bytes to the CC2500.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the CC2500.
  * @param  WriteAddr : CC2500's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite) {//Configure the header byte: burst access bit (B) and a 6-bit address (page 21, CC2500)
	//If more than one register needs to be written to, set B to 1 (page 23, CC2500)
	if(NumByteToWrite > 0x01){
		WriteAddr |= BURST_BIT;
	}
	
	//Set the Chip Select to low at the beginning of the transmission (page 21, CC2500)
	CC2500_CS_LOW(); 
	
	//Send the address of the register
	CC2500_SendByte(WriteAddr); 
	//Then loop through all of the bytes that need ot be send (MSB first)
	while(NumByteToWrite > 0x00){
		//Send the current byte at the pBuffer address
		CC2500_SendByte(*pBuffer);

		//Decrement the number of bytes to write
		NumByteToWrite--; 
		
		//Increment the pBuffer pointer position
		pBuffer++; 
	}
	
	//Set the Chip Select to high at the end of the transmission (page 23, CC2500)
	CC2500_CS_HIGH(); 
}


/**
  * @brief  Transmits one packet
  * @param  Memory pointer to packet for Tx
  * @param 	Number of bytes to Tx (length of packet)
  * @note   Flushes FIFO before placing packet in buffer.
  */
void CC2500_TxPacket(uint8_t* pBuffer, uint16_t NumByteToWrite) {
   // Put the device to idle mode to flush the TX buffer
   CC2500_Strobe(CC2500_STROBE_SIDLE, 0x00);
   // flust the tx fifo
   CC2500_Strobe(CC2500_STROBE_SFTX, 0x00); 
   //write bytes to tx fifo
   CC2500_Write(pBuffer, CC2500_FIFO_ADDR, NumByteToWrite);
   //enable transmission   
   CC2500_Strobe(CC2500_STROBE_STX, 0x00);
   // status of device after TX will vary depending on MCSM1.TXOFF_MODE[1:0]
}

/**
  * @brief  Reads one packet from 
  * @param  Memory pointer to store recieved packets
  * @param 	Number of bytes to read (packet length + <optional 
  * @note   Flushes the FIFO after retrieving and puts CC2500 in IDLE mode.
            Assumes RX packets are already in RX FIFO
  */
void CC2500_RxPackets(uint8_t* pBuffer, uint16_t NumByteToRead) {
   // Read the packet(s)
   CC2500_Read(pBuffer, CC2500_FIFO_ADDR, NumByteToRead);
   // Go to idle and flush rx buffer.
   CC2500_Strobe(CC2500_STROBE_SIDLE, DUMMY_BYTE);
   CC2500_Strobe(CC2500_STROBE_SFRX, DUMMY_BYTE);
}

/**
  * @brief  Sends a single strobe command to CC2500
  * @param  Strobe command to send
  * @param 	1 for the FIFO_BYTES_AVAILABLE field in status byte should be for 
			the RX FIFO, 0 if it should be for the TX FIFO
  * @retval Status update received from CC2500
  */
uint8_t CC2500_Strobe(StrobeCommand StrobeCmd, uint8_t RX_FIFO) {
	//Set the Chip Select to low at the beginning of the transmission (page 21, CC2500)
	CC2500_CS_LOW(); 
	
	//The address we'll be writing to 
	uint8_t StrobeAddr; 
	
	//Check which FIFO status should be sent back with the status byte (page 24, CC2500)
	if(RX_FIFO){
		//Set the read/write bit if we want the RX FIFO
		StrobeAddr = ((uint8_t)StrobeCmd) | READWRITE_BIT; 
	}
	else{
		//If not, just use the StrobeCmd address
		StrobeAddr = (uint8_t)StrobeCmd; 
	}
	
	//Send the address of the register, get the response
	uint8_t statusByte = CC2500_SendByte(StrobeAddr); 
	
	//Set the Chip Select to high at the end of the transmission 
	//only for the SPWD and SXOFF strobes (page 24, CC2500)
	if(StrobeCmd == (CC2500_STROBE_SPWD || CC2500_STROBE_SXOFF)){
		CC2500_CS_HIGH(); 
	}
	
	//Return the response
	return statusByte;
}

/**
  * @brief  Read status registor on he device
  * @param  Address of the status register to read
  * @retval Status register bits received from CC2500
  */
uint8_t CC2500_StatusReg(uint8_t StatusRegAddr) {
	uint8_t reg;
	//Set the Chip Select to low at the beginning of the transmission (page 21, CC2500)
	CC2500_CS_LOW();
	
	// set burst bit to high to distinct from strobe command (page 5, cc2500 design note)
	StatusRegAddr |= BURST_BIT;
	CC2500_Read(&reg, StatusRegAddr, 1);
 	
	// set the chip select to high marking end of transmission	
	CC2500_CS_HIGH(); 
	return reg;
}

/**
  * @brief  Computes the RSSI value
  * @param  2's complement value on rx packet
  * @retval rssi value
  * @note   LIMITATION: assumes baud rate of 256k or 512k
  */
float CC2500_ComputeRssi(float rssi_dec) {
   // check page 35 of cc2500.pdf
   // to remove limitation of baudrate, write algorith to determine RSSI OFFSET
   float rssi;
   if (rssi_dec >= 128)
      rssi = (rssi_dec - 256) / 2 - RSSI_OFFSET;
   else
      rssi = rssi_dec / 2 - RSSI_OFFSET;
   return rssi;
}

/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t CC2500_SendByte(uint8_t byte) {
   //Wait until the SPI transmit buffer is empty (page 704, Reference Manual)
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET);
		
	//Once the buffer is empty, send the byte through the SPI peripheral
	SPI_I2S_SendData(CC2500_SPI, byte); 
		
	//Wait until the SPI receive buffer has something (page 705, Reference Manual)
	while(SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET);
		
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
   CC2500_CS_HIGH();
   
     /* Configure GPIO PINs to detect Interrupts */
   GPIO_InitStructure.GPIO_Pin = CC2500_SPI_INT_PIN;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
   GPIO_Init(CC2500_SPI_INT_GPIO_PORT, &GPIO_InitStructure);
}
