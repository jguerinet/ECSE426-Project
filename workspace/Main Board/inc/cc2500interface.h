#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "cc2500.h"


#define RX_PKT 0x01

typedef struct packet {
	uint8_t Src_addr;
	uint8_t Seq_num;
	uint8_t Aux_rssi;
	uint8_t Rssi;
	uint8_t Crc_lqi;
} Packet;


// Function Definitions
void initializeWireless(void);