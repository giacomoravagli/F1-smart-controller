/* Includes ----------------------------------------------*/
#include "main.h"
/* Private defines ---------------------------------------*/
/* Private variables -------------------------------------*/
//uint8_t Key_Pressed_Number = 0x0F/*0*/;
//__IO uint8_t Key_status = 0x00;
/* Private function prototypes ---------------------------*/
/* Private functions -------------------------------------*/

/* NEW CODE */
#include "stm8s_delay.h"
//unsigned char buffer[buffer_size];
unsigned long ID = 0;
unsigned char tx_buffer[0x02] = {0x00, 0x00};
unsigned char rx_buffer[0x08] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup(void);
void setup_clock(void);
void setup_GPIO(void);
void setup_CAN(void);
void send_data_to_CAN_bus(void);
void receive_data_from_CAN_bus(void);
/* END NEW CODE */

void main(void)
{
	/* NEW CODE */
	bool toggle = FALSE;
	
	setup();
	/* END NEW CODE */
	
  //CAN_TxStatus_TypeDef status = CAN_TxStatus_Failed;
	
	//int x = 0;

  ///* Transmit Parameters */
  //CAN_Id_TypeDef Tx_IDE = CAN_Id_Standard;
  //CAN_RTR_TypeDef Tx_RTR = CAN_RTR_Data;
  //uint8_t Tx_DLC = 0;
  //uint8_t Tx_Data[8] = {0};
  //uint32_t Tx_Id = 0;
	//status = CAN_TxStatus_Ok;
  
  ///* Clock configuration ---------------------------------*/
  //CLK_Config();
  
  ///* CAN configuration -----------------------------------*/
  //CAN_Config();
  
  ///* Enable Interrupts*/
  //enableInterrupts();
	
	//status = CAN_TxStatus_Pending;


  /* Infinite loop*/
  while(1)
  {
		/* NEW CODE */
		tx_buffer[0] = 0x00;
		
		switch(toggle)
		{
				case TRUE:
					{
						tx_buffer[1] = 0xFF;
						break;
					}
				case FALSE:
					{
						tx_buffer[1] = 0x00;
						break;
					}
		}
		
		send_data_to_CAN_bus();
		//receive_data_from_CAN_bus();
		
		toggle ^= 1;
		delay_ms(50);
		
		/* END NEW CODE */
		
    //while(Key_status != Key_NoPressed)
     //{
    
     // if(Key_Pressed_Number == 0x0) 
     // {
       //   Key_Pressed_Number = 0x03;
     // }
     // else
     // {
      //    Key_Pressed_Number--;
      //}
      
      /* Sender Display*/
//      LED_Display(Key_Pressed_Number);
//      LCD_Display(Key_Pressed_Number);
      //Delay(0x7FFF);
      
      /* Transmit Parameters*/
     // Tx_Id = 0x321;
      //Tx_IDE = CAN_Id_Standard;
      //Tx_RTR = CAN_RTR_Data;
      //Tx_DLC = 1;
      //Tx_Data[0] = Key_Pressed_Number;
      
      /* Sender send Frame */
      //status = CAN_Transmit(Tx_Id,Tx_IDE,Tx_RTR,Tx_DLC,Tx_Data);
      
      /* while key is not pressed, loop*/
      //Key_status = Key_NoPressed;
			//x = x + 1;
   //}
  }
}


/* NEW CODE */
void setup(void)
{
	setup_clock();
	setup_GPIO();
	setup_CAN();
}

void setup_clock(void)
{
	CLK_DeInit();
	
	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
	
	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_CAN, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3, ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

void setup_GPIO(void)
{
	/* Probably not needed these two */
	GPIO_DeInit(GPIOD);
	GPIO_Init(GPIOD, ((GPIO_Pin_TypeDef)(GPIO_PIN_0 | GPIO_PIN_3)), GPIO_MODE_OUT_PP_LOW_FAST);
	
	//For the display
	//GPIO_DeInit(GPIOE);
	//GPIO_Init(GPIOE, GPIO_PIN_0, GPIO_MODE_OUT_PP_LOW_FAST);
}

void setup_CAN(void)
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
									0x00/*0x68*/);			//Check messages from ID: 0x1234  // Check Filter Register Table
							 
	CAN_ITConfig(CAN_IT_FMP,ENABLE);
}

void send_data_to_CAN_bus(void)
{
	//GPIO_WriteLow(GPIOD, GPIO_PIN_0);
	CAN_Transmit(0x0496,  CAN_Id_Extended, CAN_RTR_Data, 2, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */
	//GPIO_WriteHigh(GPIOD, GPIO_PIN_0); /* First LED blink */
	GPIO_WriteReverse(GPIOD, GPIO_PIN_0);
	delay_ms(60);
}

void receive_data_from_CAN_bus(void)
{
	CAN_Id_TypeDef IDE;
	CAN_RTR_TypeDef RTR;
	
	unsigned char i = 0x00;
	unsigned char DLC = 0x00;
    
	CAN_Receive();
    
	ID = CAN_GetReceivedId();
	IDE = CAN_GetReceivedIDE();
	RTR = CAN_GetReceivedRTR();
	DLC = CAN_GetReceivedDLC();
    
	for(i = 0x00; i < DLC; i++)
	{
		rx_buffer[i] = CAN_GetReceivedData(i);
	}
    
	if((IDE == CAN_Id_Extended) && (RTR == CAN_RTR_Data)) 
	{
		//GPIO_WriteLow(GPIOD, GPIO_PIN_3); /*Second LED blinks if the ID and the RTR coincide*/
		//GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
		GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
		delay_ms(600);
	}
    
	if((IDE == CAN_Id_Extended) && (RTR == CAN_RTR_Remote))
	{
		//GPIO_WriteLow(GPIOD, GPIO_PIN_3); /*Second LED blinks if the ID and the RTR coincide (for a remote frame)*/
		//GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
		GPIO_WriteReverse(GPIOD, GPIO_PIN_3);
		delay_ms(1600);
	}
}

/* END NEW CODE */


/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
//void CLK_Config(void)
//{
//  /* Initialization of the clock */
//  /* Clock divider to HSI/1 */
//  //CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
//	
//	CLK_DeInit();
 //               
//	CLK_HSECmd(DISABLE);
//	CLK_LSICmd(DISABLE);
//	CLK_HSICmd(ENABLE);
//	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
//                
//	CLK_ClockSwitchCmd(ENABLE);
//	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
//	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
 //               
//	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, 
//											DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
 //               
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_CAN, ENABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3, ENABLE);
//	
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, DISABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
//	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
//	
//}

/**
  * @brief  Configures the CAN mode and filter
  * @param  None
  * @retval None
  */
//void CAN_Config(void)
//{
//  CAN_InitStatus_TypeDef status = CAN_InitStatus_Failed;
//  
//  /* Filter Parameters */
//  CAN_FilterNumber_TypeDef CAN_FilterNumber;
//  FunctionalState CAN_FilterActivation;
//  CAN_FilterMode_TypeDef CAN_FilterMode;
//  CAN_FilterScale_TypeDef CAN_FilterScale;
//  uint8_t CAN_FilterID1;
//  uint8_t CAN_FilterID2;
//  uint8_t CAN_FilterID3;
//  uint8_t CAN_FilterID4;
//  uint8_t CAN_FilterIDMask1;
//  uint8_t CAN_FilterIDMask2;
//  uint8_t CAN_FilterIDMask3;
//  uint8_t CAN_FilterIDMask4; 
  
//  /* Init Parameters*/
//  CAN_MasterCtrl_TypeDef CAN_MasterCtrl;
//  CAN_Mode_TypeDef CAN_Mode;
//  CAN_SynJumpWidth_TypeDef CAN_SynJumpWidth;
//  CAN_BitSeg1_TypeDef CAN_BitSeg1;
//  CAN_BitSeg2_TypeDef CAN_BitSeg2;
//  uint8_t CAN_Prescaler;  
  
//  /* CAN register init */
//  CAN_DeInit();
    
//  /* CAN  init */
//  CAN_MasterCtrl=CAN_MasterCtrl_AllDisabled;
//  CAN_Mode = CAN_Mode_Normal;
//  CAN_SynJumpWidth = CAN_SynJumpWidth_1TimeQuantum;
//  CAN_BitSeg1 = CAN_BitSeg1_8TimeQuantum;
//  CAN_BitSeg2 = CAN_BitSeg2_7TimeQuantum;
 // CAN_Prescaler = 1;
 // status = CAN_Init(CAN_MasterCtrl, CAN_Mode, CAN_SynJumpWidth, CAN_BitSeg1, \
                    CAN_BitSeg2, CAN_Prescaler);

  /* CAN filter init */
//  CAN_FilterNumber = CAN_FilterNumber_0;
//  CAN_FilterActivation = ENABLE;
//  CAN_FilterMode = CAN_FilterMode_IdMask;
//  CAN_FilterScale = CAN_FilterScale_32Bit;
//  CAN_FilterID1=0;  
//  CAN_FilterID2=0;
//  CAN_FilterID3=0;
//  CAN_FilterID4=0;
//  CAN_FilterIDMask1=0;
 // CAN_FilterIDMask2=0;
//  CAN_FilterIDMask3=0;
//  CAN_FilterIDMask4=0;  
//  CAN_FilterInit(CAN_FilterNumber, CAN_FilterActivation, CAN_FilterMode, 
//                 CAN_FilterScale,CAN_FilterID1, CAN_FilterID2, CAN_FilterID3,
//                 CAN_FilterID4,CAN_FilterIDMask1, CAN_FilterIDMask2, 
//                 CAN_FilterIDMask3, CAN_FilterIDMask4);
        
 // /* Enable Fifo message pending interrupt*/
 // /* Message reception is done by CAN_RX ISR*/
 // CAN_ITConfig(CAN_IT_FMP, ENABLE);
//}

/**
  * @brief Generates a delay time.
  * @param nCount the value for looping.
  * @retval None
  */
void Delay (uint16_t nCount)
{
  /* Decrement nCount value */
  for (; nCount != 0; nCount--);
}
