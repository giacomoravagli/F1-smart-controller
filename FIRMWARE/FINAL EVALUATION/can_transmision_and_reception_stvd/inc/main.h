
/* Define to prevent recursive inclusion------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ----------------------------------------------*/
#include "stm8s.h"

/* Private define ----------------------------------------*/
#define Key_Pressed 0x01
#define Key_NoPressed 0x00

/* Public function prototypes ----------------------------*/
void CLK_Config(void);
void CAN_Config(void);
void Delay (uint16_t nCount);


#endif /* __MAIN_H */
