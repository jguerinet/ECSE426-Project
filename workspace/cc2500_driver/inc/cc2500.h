/**
  ******************************************************************************
  * @file    cc2500.h
  * @author  Abdul Haseeb Jehangir
  * @version V1.0.0
  * @date    15-November-2014
  * @brief   This file contains all the functions prototypes for the cc2500.c
  *          firmware driver.
  ******************************************************************************
	**/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_spi.h"


/**
  * @brief  CC2500 SPI Interface pins
  */

#define CC2500_SPI                        SPI2
#define CC2500_SPI_CLK                    RCC_APB1Periph_SPI2

#define CC2500_SPI_SCK_PIN                GPIO_Pin_13                  /* PB.13 */
#define CC2500_SPI_SCK_GPIO_PORT          GPIOB                        /* GPIOB */
#define CC2500_SPI_SCK_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define CC2500_SPI_SCK_SOURCE             GPIO_PinSource10
#define CC2500_SPI_SCK_AF                 GPIO_AF_SPI2

#define CC2500_SPI_MISO_PIN               GPIO_Pin_14                  /* PB.14 */
#define CC2500_SPI_MISO_GPIO_PORT         GPIOB                        /* GPIOB */
#define CC2500_SPI_MISO_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MISO_SOURCE            GPIO_PinSource14
#define CC2500_SPI_MISO_AF                GPIO_AF_SPI1

#define CC2500_SPI_MOSI_PIN               GPIO_Pin_15                  /* PB.15 */
#define CC2500_SPI_MOSI_GPIO_PORT         GPIOB                        /* GPIOB */
#define CC2500_SPI_MOSI_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MOSI_SOURCE            GPIO_PinSource15
#define CC2500_SPI_MOSI_AF                GPIO_AF_SPI2

#define CC2500_SPI_CS_PIN                 GPIO_Pin_11                  /* PB.11 */
#define CC2500_SPI_CS_GPIO_PORT           GPIOB                        /* GPIOB */
#define CC2500_SPI_CS_GPIO_CLK            RCC_AHB1Periph_GPIOB

#define CC2500_SPI_INT_PIN                GPIO_Pin_12                  /* PB.12 */
#define CC2500_SPI_INT_GPIO_PORT          GPIOB                        /* GPIOB */
#define CC2500_SPI_INT_GPIO_CLK           RCC_AHB1Periph_GPIOB
#define CC2500_SPI_INT_EXTI_LINE          EXTI_Line1
#define CC2500_SPI_INT_EXTI_PORT_SOURCE   EXTI_PortSourceGPIOB
#define CC2500_SPI_INT_EXTI_PIN_SOURCE    EXTI_PinSource1
#define CC2500_SPI_INT_EXTI_IRQn          EXTI1_IRQn


/******************************************************************************/
/*************************** STROBE COMMAND MAPPING  **************************/
/******************************************************************************/
typedef enum {
CC2500_STROBE_SRES         =      ((uint8_t)0x30),
CC2500_STROBE_SFSTXON      =      ((uint8_t)0x31),
CC2500_STROBE_SXOFF        =      ((uint8_t)0x32),
CC2500_STROBE_SCAL         =      ((uint8_t)0x33),
CC2500_STROBE_SRX          =      ((uint8_t)0x34),
CC2500_STROBE_STX          =      ((uint8_t)0x35),
CC2500_STROBE_SIDLE        =      ((uint8_t)0x36),
CC2500_STROBE_SWOR         =      ((uint8_t)0x38),
CC2500_STROBE_SPWD         =      ((uint8_t)0x39),
CC2500_STROBE_SFRX         =      ((uint8_t)0x3A),
CC2500_STROBE_SFTX         =      ((uint8_t)0x3B),
CC2500_STROBE_SWORRST      =      ((uint8_t)0x3C),
CC2500_STROBE_SNOP         =      ((uint8_t)0x3D)
} StrobeCommand;

/******************************************************************************/
/*************************** STATUS REGISTER MAPPING  *************************/
/******************************************************************************/
#define CC2500_STATUS_REG_PARTNUM         ((uint8_t)0x30)
#define CC2500_STATUS_REG_VERSION         ((uint8_t)0x31)
#define CC2500_STATUS_REG_FREQEST         ((uint8_t)0x32)
#define CC2500_STATUS_REG_LQI             ((uint8_t)0x33)
#define CC2500_STATUS_REG_RSSI            ((uint8_t)0x34)
#define CC2500_STATUS_REG_MARCSTATE       ((uint8_t)0x35)
#define CC2500_STATUS_REG_WORTIME1        ((uint8_t)0x36)
#define CC2500_STATUS_REG_WORTIME0        ((uint8_t)0x37)
#define CC2500_STATUS_REG_PKTSTATUS       ((uint8_t)0x38)
#define CC2500_STATUS_REG_VCOVCDAC        ((uint8_t)0x39)
#define CC2500_STATUS_REG_TXBYTES         ((uint8_t)0x3A)
#define CC2500_STATUS_REG_RXBYTES         ((uint8_t)0x3B)
#define CC2500_STATUS_REG_RCCTRL1         ((uint8_t)0x3C)
#define CC2500_STATUS_REG_RCCTRL0         ((uint8_t)0x3D)

/******************************************************************************/
/*************************** CONFIG REGISTER MAPPING  *************************/
/******************************************************************************/
#define CC2500_CFG_REG_IOCFG2             ((uint8_t)0x00)
#define CC2500_CFG_REG_IOCFG1             ((uint8_t)0x01)
#define CC2500_CFG_REG_IOCFG0             ((uint8_t)0x02)
#define CC2500_CFG_REG_FIFOTHR            ((uint8_t)0x03)
#define CC2500_CFG_REG_SYNC1              ((uint8_t)0x04)
#define CC2500_CFG_REG_SYNC0              ((uint8_t)0x05)
#define CC2500_CFG_REG_PKTLEN             ((uint8_t)0x06)
#define CC2500_CFG_REG_PKTCTRL1           ((uint8_t)0x07)
#define CC2500_CFG_REG_PKTCTRL0           ((uint8_t)0x08)
#define CC2500_CFG_REG_ADDR               ((uint8_t)0x09)
#define CC2500_CFG_REG_CHANNR             ((uint8_t)0x0A)
#define CC2500_CFG_REG_FSCTRL1            ((uint8_t)0x0B)
#define CC2500_CFG_REG_FSCTRL0            ((uint8_t)0x0C)
#define CC2500_CFG_REG_FREQ2              ((uint8_t)0x0D)
#define CC2500_CFG_REG_FREQ1              ((uint8_t)0x0E)
#define CC2500_CFG_REG_FREQ0              ((uint8_t)0x0F)

#define CC2500_CFG_REG_MDMCFG4            ((uint8_t)0x10)
#define CC2500_CFG_REG_MDMCFG3            ((uint8_t)0x11)
#define CC2500_CFG_REG_MDMCFG2            ((uint8_t)0x12)
#define CC2500_CFG_REG_MDMCFG1            ((uint8_t)0x13)
#define CC2500_CFG_REG_MDMCFG0            ((uint8_t)0x14)
#define CC2500_CFG_REG_DEVIATN            ((uint8_t)0x15)
#define CC2500_CFG_REG_MCSM2              ((uint8_t)0x16)
#define CC2500_CFG_REG_MCSM1              ((uint8_t)0x17)
#define CC2500_CFG_REG_MCSM0              ((uint8_t)0x18)
#define CC2500_CFG_REG_FOCCFG             ((uint8_t)0x19)
#define CC2500_CFG_REG_BSCFG              ((uint8_t)0x1A)
#define CC2500_CFG_REG_AGCCTRL2           ((uint8_t)0x1B)
#define CC2500_CFG_REG_AGCCTRL1           ((uint8_t)0x1C)
#define CC2500_CFG_REG_AGCCTRL0           ((uint8_t)0x1D)
#define CC2500_CFG_REG_WOREVT1            ((uint8_t)0x1E)
#define CC2500_CFG_REG_WOREVT0            ((uint8_t)0x1F)

#define CC2500_CFG_REG_FRIEND1            ((uint8_t)0x20)
#define CC2500_CFG_REG_FRIEND0            ((uint8_t)0x21)
#define CC2500_CFG_REG_WORCTRL            ((uint8_t)0x22)
#define CC2500_CFG_REG_FSCAL3             ((uint8_t)0x23)
#define CC2500_CFG_REG_FSCAL2             ((uint8_t)0x24)
#define CC2500_CFG_REG_FSCAL1             ((uint8_t)0x25)
#define CC2500_CFG_REG_FSCAL0             ((uint8_t)0x26)
#define CC2500_CFG_REG_RCCTRL1            ((uint8_t)0x27)
#define CC2500_CFG_REG_RCCTRL0            ((uint8_t)0x28)
#define CC2500_CFG_REG_FSTEST             ((uint8_t)0x29)
#define CC2500_CFG_REG_PTEST              ((uint8_t)0x2A)
#define CC2500_CFG_REG_AGCTEST            ((uint8_t)0x2B)
#define CC2500_CFG_REG_TEST2              ((uint8_t)0x2C)
#define CC2500_CFG_REG_TEST1              ((uint8_t)0x2D)
#define CC2500_CFG_REG_TEST0              ((uint8_t)0x2E)


  /** @defgroup CC2500_Exported_Macros
  * @{
  */
#define CC2500_CS_LOW()       GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define CC2500_CS_HIGH()      GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)


/** @defgroup CC2500_Exported_Functions
  * @{
  */ 
void CC2500_Init(void);
void CC2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void CC2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t CC2500_Strobe(StrobeCommand StrobeCmd);