/* PROVA CAN 
 * 
 * To be used with the STM8AF Discovery --> No troubles! The pin are mapped in the same way
 * 
 * This code for transmission is working; we have to realize the splitting of data into packets and send them over the line
 * 
 * Then we have to realize a sniffer that takes the packets and send them to the Serial Monitor of the PC
*/

#include "Wire.h"
#include <SPI.h>
#include <NRFLite.h>
#include <stm8s_can.h>


unsigned long ID = 0;
unsigned char tx_buffer[0x03] = {0x00, 0x00, 0x00};


void setup() {
  Serial.begin(115200);

  setup_CAN();
  
  GPIO_DeInit(GPIOC);
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  pinMode(PC3, OUTPUT);
}

void setup_CAN()
{
  CAN_DeInit();
  
  CAN_Init(CAN_MasterCtrl_NoAutoReTx,
            CAN_Mode_Normal,
            CAN_SynJumpWidth_1TimeQuantum,
            CAN_BitSeg1_8TimeQuantum,
            CAN_BitSeg2_7TimeQuantum,
            1);
            
  CAN_FilterInit(CAN_FilterNumber_0,
                  ENABLE,
                  CAN_FilterMode_IdMask/*CAN_FilterMode_IdList*/,
                  CAN_FilterScale_32Bit,
                  0x00,  
                  0x00/*0x08*/,
                  0x00/*0x24*/,
                  0x00/*0x68*/,
                  0x00/*0x00*/,
                  0x00/*0x08*/,
                  0x00/*0x24*/,
                  0x00/*0x68*/);      //Check messages from ID: 0x1234  // Check Filter Register Table
               
  CAN_ITConfig(CAN_IT_FMP,ENABLE);
}


void loop() {
  Serial.print("Hello world!\n");

  
  GPIO_WriteHigh(GPIOC, GPIO_PIN_3);

  delay(10);

  digitalWrite(PC3, LOW);

  tx_buffer[0] = 0x01;
  tx_buffer[1] = 0xFE;
  tx_buffer[2] = 0x05;
    
    send_data_to_CAN_bus();
    
    delay(5);

  delay(1);
}

void send_data_to_CAN_bus()
{
  //GPIO_WriteLow(GPIOD, GPIO_PIN_0);
  CAN_Transmit(0x000F,  CAN_Id_Extended, CAN_RTR_Data, 3, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */
  //GPIO_WriteHigh(GPIOD, GPIO_PIN_0); /* First LED blink */
  GPIO_WriteReverse(GPIOD, GPIO_PIN_0);
  delay(6);
}
