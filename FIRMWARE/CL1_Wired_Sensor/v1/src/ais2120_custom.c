#include "ais2120_custom.h"

/* PAY ATTENTION: in this application we have to send 32 bit, then we will
 * receive the answer only after the 32 bit have been sent
 */



uint8_t MEMS_SendByte(uint8_t byte)
{
 /* Loop while DR register in not emplty */
 while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
 /* Send byte through the SPI2 peripheral */
 SPI_SendData(byte);
 /* Wait to receive a byte */
 while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
 /* Return the byte read from the SPI bus */
 return SPI_ReceiveData();
}

/* IF THE FORMER NOT WORK, TRY THIS */
/*
uint8_t MEMS_SendByte(uint8_t byte)
{
 while(SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
 SPI_SendData(byte);
 return 1;
}
*/



void AIS_Init(void)
{
  /* Initialize variables for SPI send */
  uint8_t cmd_byte_1 = 0x00; // MSB
  uint8_t cmd_byte_2 = 0x00;
  uint8_t cmd_byte_3 = 0x00;
  uint8_t cmd_byte_4 = 0x00; // LSB, contains CRC calculated offline
  uint8_t dummy_b = 0;

  /* Set the filter bandwidth for both axis */
  /* If we do nothing, it keep the default 400 Hz
   * maybe is good, try before complicating your life */
  delay_ms(1000);

  /* Write 1 in EOI bit of REG_CTRL_0 (address: 0x00) */
  cmd_byte_1 = 0x40; 
  cmd_byte_2 = 0x00;
  cmd_byte_3 = 0x20;
  cmd_byte_4 = 0xD2; //CRC

  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  dummy_b = MEMS_SendByte(cmd_byte_1);
  dummy_b = MEMS_SendByte(cmd_byte_2);
  dummy_b = MEMS_SendByte(cmd_byte_3);
  dummy_b = MEMS_SendByte(cmd_byte_4);
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);
}


status_t AIS_Get_Err_Status(void)
{
  // TBD if everything else works
  return MEMS_SUCCESS;
}


/* Send an acceleration command (axis = X), receive back the data from AIS */  
int16_t SPI_AIS_Send_Acc_X_Cmd(void)
{
  int16_t result = 0;
  int16_t temp_raw = 0;
  uint8_t temp1 = 0;
  uint8_t temp2 = 0;
  uint8_t temp3 = 0;

  /* Send a 32-bit command through the SPI, MSB first */
  uint8_t cmd_byte_1 = 0x20; // MSB
  uint8_t cmd_byte_2 = 0x00;
  uint8_t cmd_byte_3 = 0x00;
  uint8_t cmd_byte_4 = 0x3D; // LSB, contains CRC calculated offline
  // Check the CRC since not sure if correct

  uint8_t rd_byte_1 = 0x00; 
  uint8_t rd_byte_2 = 0x00;
  uint8_t rd_byte_3 = 0x00;
  uint8_t rd_byte_4 = 0x00;

  uint8_t dummy_b = 0x00; // to clear the SDO (=MISO) buffer

  /* Start the transmission = frame 1 
   * Please refer to page 43 of the datasheet 
   */
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  dummy_b = MEMS_SendByte(cmd_byte_1);
  dummy_b = MEMS_SendByte(cmd_byte_2);
  dummy_b = MEMS_SendByte(cmd_byte_3);
  dummy_b = MEMS_SendByte(cmd_byte_4);
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);


  /* Start the reception = frame 2 */
  /* ALTERNATIVE 1 */
  /*
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  rd_byte_1 = MEMS_SendByte(cmd_byte_1);
  // We send another dataX request, just to avoid doing something strange
  // The request will be unused, there is a bit of loss of performance
  // Just to see if work
  rd_byte_2 = MEMS_SendByte(cmd_byte_2);
  rd_byte_3 = MEMS_SendByte(cmd_byte_3);
  rd_byte_4 = MEMS_SendByte(cmd_byte_4);
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);
  */

  /* ALTERNATIVE 2 */
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  /* Wait to receive a byte */
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  /* Return the byte read from the SPI bus */
  rd_byte_1 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_2 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_3 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_4 = SPI_ReceiveData();
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);



  /* HERE IT IS POSSIBLE TO PERFORM SOME TEST, EG SEE IF THE MSBs OF
   * rd_byte_1 are 1000 */


  /* Useful data extraction */
  temp3 = (uint8_t)(rd_byte_3 >> 4);   // right alignment
  temp2 = (uint8_t)(rd_byte_2);
  temp1 = (uint8_t)(rd_byte_1 & 0x03); // preserve only the 2 LSB
  temp_raw = (int16_t)(temp3 + (temp2 << 4) + (temp1 << 12));
  /* At that point we have a raw data, we have to convert it --> positive or negative? */
  /* Correction of the 14-bit raw output data */
  if((temp_raw & 0x2000) == 0x2000)
  {
    /* Means that the result is negative, so we need to invert */
    result = ~temp_raw;              // invert bits
    result &= ( 0xFFFF >> (16-14) ); //but keep just the 14 bits
    result = ~result;
  }  
  else result = temp_raw;
}


/* Send an acceleration command (axis = Y), receive back the data from AIS */  
int16_t SPI_AIS_Send_Acc_Y_Cmd(void)
{
  int16_t result = 0;
  int16_t temp_raw = 0;
  uint8_t temp1 = 0;
  uint8_t temp2 = 0;
  uint8_t temp3 = 0;

  /* Send a 32-bit command through the SPI, MSB first */
  uint8_t cmd_byte_1 = 0x70; // MSB
  uint8_t cmd_byte_2 = 0x00;
  uint8_t cmd_byte_3 = 0x00;
  uint8_t cmd_byte_4 = 0x45; // LSB, contains CRC calculated offline
  // Check the CRC since not sure if correct

  uint8_t rd_byte_1 = 0x00; 
  uint8_t rd_byte_2 = 0x00;
  uint8_t rd_byte_3 = 0x00;
  uint8_t rd_byte_4 = 0x00;

  uint8_t dummy_b = 0x00; // to clear the SDO (=MISO) buffer

  /* Start the transmission = frame 1 
   * Please refer to page 43 of the datasheet 
   */
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  dummy_b = MEMS_SendByte(cmd_byte_1);
  dummy_b = MEMS_SendByte(cmd_byte_2);
  dummy_b = MEMS_SendByte(cmd_byte_3);
  dummy_b = MEMS_SendByte(cmd_byte_4);
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);


  /* Start the reception = frame 2 */
  /* ALTERNATIVE 1 */
  /*
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  rd_byte_1 = MEMS_SendByte(cmd_byte_1);
  // We send another dataX request, just to avoid doing something strange
  // The request will be unused, there is a bit of loss of performance
  // Just to see if work
  rd_byte_2 = MEMS_SendByte(cmd_byte_2);
  rd_byte_3 = MEMS_SendByte(cmd_byte_3);
  rd_byte_4 = MEMS_SendByte(cmd_byte_4);
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);
  */

  /* ALTERNATIVE 2 */
  GPIO_WriteLow(GPIOC, PIN_CS_ACC);
  /* Wait to receive a byte */
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  /* Return the byte read from the SPI bus */
  rd_byte_1 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_2 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_3 = SPI_ReceiveData();
  while(SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
  rd_byte_4 = SPI_ReceiveData();
  GPIO_WriteHigh(GPIOC, PIN_CS_ACC);



  /* HERE IT IS POSSIBLE TO PERFORM SOME TEST, EG SEE IF THE MSBs OF
   * rd_byte_1 are 1000 */


  /* Useful data extraction */
  temp3 = (uint8_t)(rd_byte_3 >> 4);   // right alignment
  temp2 = (uint8_t)(rd_byte_2);
  temp1 = (uint8_t)(rd_byte_1 & 0x03); // preserve only the 2 LSB
  temp_raw = (int16_t)(temp3 + (temp2 << 4) + (temp1 << 12));
  /* At that point we have a raw data, we have to convert it --> positive or negative? */
  /* Correction of the 14-bit raw output data */
  if((temp_raw & 0x2000) == 0x2000)
  {
    /* Means that the result is negative, so we need to invert */
    result = ~temp_raw;              // invert bits
    result &= ( 0xFFFF >> (16-14) ); //but keep just the 14 bits
    result = ~result;
  }  
  else result = temp_raw;
}

