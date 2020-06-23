/* This code is to be flashed on the CL1 board. 
 * Goal: make the board work as a smart wired sensor, i.e. the board polls the AIS2120SX
 * and has to send the data to on the CAN line
 */


/* Includes ----------------------------------------------*/
#include "main.h"
#include "stm8s_delay.h"
#include "stm8s_can.h"
#include "ais2120_custom.h"
/* Private defines ---------------------------------------*/
/* Private variables -------------------------------------*/
/* Private function prototypes ---------------------------*/
/* Private functions -------------------------------------*/


/* Defines inserted to see if the board is working; we assess if the board 
 * is able to correctly read the supply voltage
 */
#define VREF 3.3
#define ADC_SLOPE (VREF/1024)
#define BATT_DIV_R1 10
#define BATT_DIV_R2 2.15
#define V_BATT_GAIN ((BATT_DIV_R1+BATT_DIV_R2)/(BATT_DIV_R2))

/* Define PIN map for the useful PINs */
#define PIN_CS_ACC		GPIO_PIN_1 //PC1
//#define PIN_CS_BAR		GPIO_PIN_5 //PE5
#define PIN_MISO		GPIO_PIN_7 //PC7
#define PIN_MOSI		GPIO_PIN_6 //PC6
#define PIN_SCK			GPIO_PIN_5 //PC5



unsigned long ID = 0;
unsigned char tx_buffer[0x08] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char rx_buffer[0x08] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
volatile uint16_t ADC_Value;
float volt = 0, v_batt = 0;
int16_t X_axis_acc = 0;
int16_t Y_axis_acc = 0;


void setup(void);
void setup_clock(void);
void setup_GPIO(void);
void setup_SPI(void);
void setup_CAN(void);
void send_data_to_CAN_bus(void);
void receive_data_from_CAN_bus(void);



void main(void)
{	
	setup();
	
	ADC2_Init(	ADC2_CONVERSIONMODE_CONTINUOUS,
				ADC2_CHANNEL_0,
				ADC2_PRESSEL_FCPU_D2,
				ADC2_EXTTRIG_TIM,
				DISABLE,
				ADC2_ALIGN_RIGHT,
				ADC2_SCHMITTTRIG_CHANNEL0, 
				DISABLE );
						
	ADC2_StartConversion();

  	/* Infinite loop*/
  	while(1)
  	{
		/* Acquire data from AIS2120SX */
  		X_axis_acc = SPI_AIS_Send_Acc_X_Cmd();
  		Y_axis_acc = SPI_AIS_Send_Acc_Y_Cmd();




  		/* Prepare the CAN buffer and transmit the data */
		tx_buffer[0] = (uint8_t)(X_axis_acc >> 8);  //MSB
		tx_buffer[1] = (uint8_t)(X_axis_acc);
		tx_buffer[2] = (uint8_t)(Y_axis_acc >> 8);
		tx_buffer[3] = (uint8_t)(Y_axis_acc);
		tx_buffer[4] = 0x00;
		tx_buffer[5] = 0x00;
		tx_buffer[6] = 0x00;
		tx_buffer[7] = 0x00;
		
		send_data_to_CAN_bus();
		//receive_data_from_CAN_bus();

		
		/* See if the battery voltage is correctly read */
		ADC_Value = ADC2_GetConversionValue();
		volt = ADC_Value*ADC_SLOPE;
		v_batt = volt*V_BATT_GAIN;

		/* A small delay before starting the loop again */
		delay_ms(50);
  }
}



void setup(void)
{
	setup_clock();
	setup_GPIO();
	setup_SPI();
	setup_CAN();

	//AIS_Init(); // see page 46
	AIS_SelfTest();
	delay_ms(100);
}


void setup_clock(void)
{
	CLK_DeInit();
	
	CLK_HSECmd(ENABLE);   /* CHANGE MADE HERE --> also maybe need to disable the internal clock?? */
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);
	while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);
	
	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);
	
	/* Both the following two should work */
	//CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSE, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI, DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
	
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_CAN, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);

	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART3, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER3, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}


void setup_GPIO(void)
{
	/* We should also ADD THE BAROMETER */

	/* Initialize the PIN for the SPI Chip select for the accelerometer;
     * it has to be initialized to high --> since CS address the chip when low */
	GPIO_DeInit(GPIOC);
	GPIO_Init(GPIOC, PIN_CS_ACC, GPIO_MODE_OUT_PP_HIGH_FAST);

	/* Initialize SCK and MOSI as FAST OUT */
    GPIO_Init(GPIOC, PIN_MOSI, GPIO_MODE_OUT_PP_LOW_FAST);
    GPIO_Init(GPIOC, PIN_SCK, GPIO_MODE_OUT_PP_LOW_FAST);
    /* Initialize the MISO as INPUT, no interrupts*/
    GPIO_Init(GPIOC, PIN_MISO, GPIO_MODE_IN_FL_NO_IT);
	
	/* Initialize CAN PIN */
	GPIO_DeInit(GPIOG);
	GPIO_Init(GPIOG, (GPIO_Pin_TypeDef) (GPIO_PIN_0), GPIO_MODE_OUT_PP_HIGH_FAST);
	GPIO_Init(GPIOG, (GPIO_Pin_TypeDef) (GPIO_PIN_1), GPIO_MODE_OUT_PP_HIGH_FAST);
}


void setup_SPI(void)
{
     SPI_DeInit();
     SPI_Init(SPI_FIRSTBIT_MSB, 
              SPI_BAUDRATEPRESCALER_16, 
              SPI_MODE_MASTER, 
              SPI_CLOCKPOLARITY_HIGH, 
              SPI_CLOCKPHASE_2EDGE, 
              SPI_DATADIRECTION_2LINES_FULLDUPLEX, 
              SPI_NSS_SOFT, 
              0x07);
     SPI_Cmd(ENABLE);
}


void setup_CAN(void)
{
	CAN_DeInit();
	
	CAN_Init(CAN_MasterCtrl_NoAutoReTx,
						CAN_Mode_LoopBack, /*CAN_Mode_LoopBack,*/
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

	CAN_Transmit(0x000B,  CAN_Id_Extended, CAN_RTR_Data, 8, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */	
	delay_ms(60);
}

/* Pay attention: not clear what the GPIO_WriteReverse on PC5 does;
 * if was simply used to blink a LED on the NUCLEO, remove it since may interfere
 * with SPI SCK 
 */
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
		GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
		delay_ms(60);
	}
    
	if((IDE == CAN_Id_Extended) && (RTR == CAN_RTR_Remote))
	{
		//GPIO_WriteLow(GPIOD, GPIO_PIN_3); /*Second LED blinks if the ID and the RTR coincide (for a remote frame)*/
		//GPIO_WriteHigh(GPIOD, GPIO_PIN_3);
		GPIO_WriteReverse(GPIOC, GPIO_PIN_5);
		delay_ms(16);
	}
}



void Delay (uint16_t nCount)
{
  /* Decrement nCount value */
  for (; nCount != 0; nCount--);
}
