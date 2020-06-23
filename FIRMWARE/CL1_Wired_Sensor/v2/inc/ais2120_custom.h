#include "stm8s_spi.h"
#include "stm8s_delay.h"

#define DUMMY_BYTE 					0xA5


/* Maybe could be useful to assess if reading okay or not
 * see LIS3DH_GetAccAxesRaw 
 * Insert it if everything else works */
typedef enum {
  MEMS_SUCCESS				=		0x01,
  MEMS_ERROR				=		0x00	
} status_t;


/* Functions prototypes */
uint8_t MEMS_SendByte(uint8_t byte);
void AIS_Init(void);
status_t AIS_Get_Err_Status(void);
int16_t SPI_AIS_Send_Acc_X_Cmd(void);
int16_t SPI_AIS_Send_Acc_Y_Cmd(void);