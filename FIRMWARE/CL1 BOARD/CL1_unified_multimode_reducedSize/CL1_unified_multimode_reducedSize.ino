/**
  ******************************************************************************
  * @file    CL1_unified_multimode_reducedSize.c
  * @author  CL1 Team
  * @version V3.0
  * @date    09-December-2019
  * @brief   This file is aimed at providing an handy tool to the user of the CL1 board. 
  *          It allows an easy reconfiguration of the functionality of the board according to the use case,
  *          without wasting resources: only the relevant portion of code will be compiled, exploiting
  *          preprocessor's directives.
  * 
  * 
  *          Three OPERATING MODES are allowed:
  *          1) Slave mode (Smart Sensor): the CL1 board acquires the pressure and the acceleration, and send
  *          it to the master via radio packet
  *          2) Master mode: the CL1 board has no sensor; acquires radio packet sent by a slave node, and 
  *          transmit the packet on the CAN bus
  *          3) Wired Sensor: the CL1 board acquires pressure and acceleration from sensor installed on it, and 
  *          transmit the data directly on the CAN bus (no wireless communication in this case)
  * 
  *          Pay attention: it is also needed to reconfigure the CE and CSN pin for the nRF24 module, according 
  *          to the board where it is used --> change if used on the CL1 board
  *******************************************************************************
  */


/** Uncomment according to what is the functionality required
  * ATTENTION: JUST UNCOMMENT ONE AT A TIME!
  */
/* #define SLAVE_MODE */
#define MASTER_MODE
/* #define WIRED_SENSOR */


#define HIGH_RESOLUTION
#define LIS3DH_DEBUG Serial  /* Enable Serial debbug on Serial UART to see registers wrote */


/* Includes ------------------------------------------------------------------*/
/* Include the libraries, according to what is the functionality required */
#include "Wire.h"
#include <SPI.h>
#if defined(MASTER_MODE) || defined(WIRED_SENSOR)
#include <stm8s_can.h>
#endif
#if defined(SLAVE_MODE) || defined(MASTER_MODE)
#include <NRFLite.h>
#endif
#if defined(SLAVE_MODE) || defined(WIRED_SENSOR)
#include <Adafruit_BMP085.h>
#include "lis3dh-motion-detection.h"
#endif


/* Private variables ---------------------------------------------------------*/
#if defined(MASTER_MODE) || defined(WIRED_SENSOR)
/* Initialization of the parameters for CAN transmission */
unsigned long ID = 0;
unsigned char tx_buffer[0x08] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // payload = 8 bytes
#endif

#if defined(MASTER_MODE)
/* Variables name for the Wireless transmission */
const static uint8_t RADIO_ID = 0;             // Receiver radio's id.  The transmitter will send to this id.
#endif
#if defined(SLAVE_MODE)
/* Variables name for the Wireless transmission */
const static uint8_t RADIO_ID = 1;             // Transmitter radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
#endif
#if defined(SLAVE_MODE) || defined(MASTER_MODE)
const static uint8_t PIN_RADIO_CE = 6;         // 6 = PC3 in final board
const static uint8_t PIN_RADIO_CSN = 5;        // 5 = PC2

/* Definition of the radio packet */
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    int32_t PressureTx;
    int32_t AccelerationTx;
    uint8_t FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;
#endif

#if defined(SLAVE_MODE) || defined(WIRED_SENSOR)
/* Here we have the configuration parameters for the accelerometer  */
uint16_t sampleRate = 100;  // HZ - Samples per second - 1, 10, 25, 50, 100, 200, 400, 1600, 5000
uint8_t accelRange = 16;    // Accelerometer range = 2, 4, 8, 16g
LIS3DH myIMU(0x18);         // Default address is 0x18. 
int16_t dataHighres = 0;    // Save the raw data before conversion to float

/* Configuration parameter for the barometer */
Adafruit_BMP085 bmp;

float AccAsseZ = 0;         // Measured acceleration
int32_t AccAsseZ_mg = 0;    // Acceleration in milli-g
float Temp_Meas = 0;        // Measured temperature (BMP180)
int32_t Press_Meas = 0;     // Measured pressure
#endif



#if defined(SLAVE_MODE)
/**
  * @brief  SLAVE MODE initialization function
  * @param  None
  * @retval None
  * @note   Initialization of the SLAVE node.
  */
void setup() 
{
  delay(1000); //wait until serial is open...

  /* Initialization of the accelerometer */
  if( myIMU.begin(sampleRate, 1, 1, 1, accelRange) != 0 )
  {
  }
  else
  {
  }
  
  /* Detection threshold can be from 1 to 127 and depends on the Range
   * chosen above, change it and test accordingly to your application
   * Duration = timeDur x Seconds / sampleRate */
  myIMU.intConf(INT_1, DET_MOVE, 13, 2);
  myIMU.intConf(INT_2, DET_STOP, 13, 10);

  uint8_t readData = 0;

  /* Confirm configuration: */
  myIMU.readRegister(&readData, LIS3DH_INT1_CFG);
  myIMU.readRegister(&readData, LIS3DH_INT2_CFG);

  /* Get the ID: */
  myIMU.readRegister(&readData, LIS3DH_WHO_AM_I);


  /* Initialization of the barometer */
  if (!bmp.begin()) 
  {
      while (1);
  }


  /* Initialization of the nRF24 */
  /* By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
   * Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
   * You can run the 'ChannelScanner' example to help select the best channel for your environment.
   * You can assign a different bitrate and channel as shown below.
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
   */
    
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
      while (1); // Wait here forever.
  }
    
  _radioData.FromRadioId = RADIO_ID;
}
#endif


#if defined(MASTER_MODE)
/**
  * @brief  MASTER MODE initialization function
  * @param  None
  * @retval None
  * @note   Initialization of the MASTER node.
  */
void setup()
{
    delay(1000);

    setup_CAN();
    
  /* By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
   * Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
   * You can run the 'ChannelScanner' example to help select the best channel for your environment.
   * You can assign a different bitrate and channel as shown below.
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
   *   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
   */
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        ErrorSignal_nRF_CAN();
        while (1); // Wait here forever.
    }
}
#endif


#if defined(WIRED_SENSOR)
/**
  * @brief  WIRED SENSOR initialization function
  * @param  None
  * @retval None
  * @note   Initialization of the WIRED SENSOR.
  */
void setup()
{
  delay(1000);

  setup_CAN();

  /* Initialization of the accelerometer */
  if( myIMU.begin(sampleRate, 1, 1, 1, accelRange) != 0 )
  {
    ErrorSignal_LIS_CAN();
  }
  else
  {
  }
  
  /* Detection threshold can be from 1 to 127 and depends on the Range
   * chosen above, change it and test accordingly to your application
   * Duration = timeDur x Seconds / sampleRate */
  myIMU.intConf(INT_1, DET_MOVE, 13, 2);
  myIMU.intConf(INT_2, DET_STOP, 13, 10);

  uint8_t readData = 0;

  /* Confirm configuration: */
  myIMU.readRegister(&readData, LIS3DH_INT1_CFG);
  myIMU.readRegister(&readData, LIS3DH_INT2_CFG);

  /* Get the ID: */
  myIMU.readRegister(&readData, LIS3DH_WHO_AM_I);


  /* Initialization of the barometer */
  if (!bmp.begin()) 
  {
      ErrorSignal_BMP_CAN();
      while (1);
  }
}
#endif


#if defined(MASTER_MODE) || defined(WIRED_SENSOR)
/**
  * @brief  CAN peripheral initialization function
  * @param  None
  * @retval None
  */
void setup_CAN()
{
  CAN_DeInit();
  
  CAN_Init(CAN_MasterCtrl_NoAutoReTx,
            CAN_Mode_Normal,
            CAN_SynJumpWidth_1TimeQuantum,
            CAN_BitSeg1_8TimeQuantum,
            CAN_BitSeg2_7TimeQuantum,
            4);
            
  CAN_FilterInit(CAN_FilterNumber_0,
                  ENABLE,
                  CAN_FilterMode_IdMask,
                  CAN_FilterScale_32Bit,
                  0x00,  
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00,
                  0x00);
               
  CAN_ITConfig(CAN_IT_FMP,ENABLE);
}
#endif


#if defined(MASTER_MODE)
/**
  * @brief  NRF24L01 initialization error function 
  * @param  None
  * @retval None
  */
void ErrorSignal_nRF_CAN()
{
  /* Send an error signal on the CAN bus if radio is not correctly initialized */
  tx_buffer[0] = 0x00;
  tx_buffer[1] = 0x00;
  tx_buffer[2] = 0x00;
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;
  tx_buffer[5] = 0x00;
  tx_buffer[6] = 0x00;
  tx_buffer[7] = 0x01;

  send_data_to_CAN_bus();
}
#endif


#if defined(WIRED_SENSOR)
/**
  * @brief  BMP initialization error function 
  * @param  None
  * @retval None
  */
void ErrorSignal_BMP_CAN()
{
  /* Send an error signal on the CAN bus if BMP is not correctly initialized */
  tx_buffer[0] = 0x00;
  tx_buffer[1] = 0x00;
  tx_buffer[2] = 0x00;
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;
  tx_buffer[5] = 0x00;
  tx_buffer[6] = 0x00;
  tx_buffer[7] = 0x02;

  send_data_to_CAN_bus();
}


/**
  * @brief  LIS3DH initialization error function 
  * @param  None
  * @retval None
  */
void ErrorSignal_LIS_CAN()
{
  /* Send an error signal on the CAN bus if LIS (accelerometer) is not correctly initialized */
  tx_buffer[0] = 0x00;
  tx_buffer[1] = 0x00;
  tx_buffer[2] = 0x00;
  tx_buffer[3] = 0x00;
  tx_buffer[4] = 0x00;
  tx_buffer[5] = 0x00;
  tx_buffer[6] = 0x00;
  tx_buffer[7] = 0x03;

  send_data_to_CAN_bus();
}
#endif



#if defined(SLAVE_MODE)
/**
  * @brief  SLAVE MODE application entry point.
  * @retval None
  * @note   Execution of the main program
  */
void loop()
{
  /* Here perform the acquisition from the accelerometer */
  dataHighres = 0;
  myIMU.readRegisterInt16( &dataHighres, LIS3DH_OUT_X_L );
  myIMU.readRegisterInt16( &dataHighres, LIS3DH_OUT_Z_L );
  AccAsseZ = myIMU.axisAccel( Z );     // store the data in floating point
  AccAsseZ_mg = (int32_t)(AccAsseZ * 1000);

  /* Perform the acquisition from the barometer */
  Temp_Meas = bmp.readTemperature();
  Press_Meas = bmp.readPressure();

  /* Send the data */
  _radioData.PressureTx = Press_Meas;
  _radioData.AccelerationTx = AccAsseZ_mg;

  /* By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
   * it will try again up to 16 times.  You can also perform a NO_ACK send that does not request an acknowledgement.
   * The data packet will only be transmitted a single time so there is no guarantee it will be successful.  Any random
   * electromagnetic interference can sporatically cause packets to be lost, so NO_ACK sends are only suited for certain
   * types of situations, such as streaming real-time data where performance is more important than reliability.
   *   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
   *   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
   */
    
  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
  {
  }
  else
  {
      _radioData.FailedTxCount++;
  }
  
  /* At the end, restart the cycle after a certain amount of time (ms) */
  delay(10);
}
#endif


#if defined(MASTER_MODE)
/**
  * @brief  MASTER MODE application entry point.
  * @retval None
  * @note   Execution of the main program
  */
void loop()
{
    while (_radio.hasData())
    {
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name
        
        /* This line of code corrects the received data: in fact, the data is sent using a FIFO logic related to a single byte [the result is stored as follows:
         * MSByte(received data)=LSByte(transmitted data), ... , LSByte(received data)=MSByte(transmitted data)].
         * The following line thus applies a bitmask to obtain one byte of the received data; a shift to each byte in order to position it correctly inside the 32-bit word;
         * a sum to merge all the 4 repositioned Bytes together. NB: THIS WORKS ONLY FOR 32-BIT DATA, CHANGE IT FOR OTHER TYPES OF DATA.
         * Surprisingly, no bit mask is required working with another ST Nucleo 8S 208RB
          */
        
        //_radioData.PressureTx = ((_radioData.PressureTx & 0xFF000000)>>24)+((_radioData.PressureTx & 0x00FF0000)>>8)+((_radioData.PressureTx & 0x0000FF00)<<8)+((_radioData.PressureTx & 0x000000FF)<<24);
        //_radioData.AccelerationTx = ((_radioData.AccelerationTx & 0xFF000000)>>24)+((_radioData.AccelerationTx & 0x00FF0000)>>8)+((_radioData.AccelerationTx & 0x0000FF00)<<8)+((_radioData.AccelerationTx & 0x000000FF)<<24);
        

        /* Create the bit mask for the PRESSURE data to be transmitted on the CAN bus
         * int32_t --> 4 x uint8_t  (then, at the receiver size we need to do the same
         */
        tx_buffer[0] = (uint8_t)(_radioData.PressureTx);
        tx_buffer[1] = (uint8_t)(_radioData.PressureTx>>8);
        tx_buffer[2] = (uint8_t)(_radioData.PressureTx>>16);
        tx_buffer[3] = (uint8_t)(_radioData.PressureTx>>24);
        /* Create the bit mask for the ACCELEROMETER data to be transmitted on the CAN bus
         * int32_t --> 4 x uint8_t  (then, at the receiver size we need to do the same
         */
        tx_buffer[4] = (uint8_t)(_radioData.AccelerationTx);
        tx_buffer[5] = (uint8_t)(_radioData.AccelerationTx>>8);
        tx_buffer[6] = (uint8_t)(_radioData.AccelerationTx>>16);
        tx_buffer[7] = (uint8_t)(_radioData.AccelerationTx>>24);


        send_data_to_CAN_bus();
    }
}
#endif


#if defined(WIRED_SENSOR)
/**
  * @brief  WIRED SENSOR application entry point.
  * @retval None
  * @note   Execution of the main program
  */
void loop()
{
  /* Here perform the acquisition from the accelerometer */
  dataHighres = 0;
  myIMU.readRegisterInt16( &dataHighres, LIS3DH_OUT_X_L );
  myIMU.readRegisterInt16( &dataHighres, LIS3DH_OUT_Z_L );
  AccAsseZ = myIMU.axisAccel( Z );     // store the data in floating point
  AccAsseZ_mg = (int32_t)(AccAsseZ * 1000);
  // Print the result, i.e. acceleration in mg
  Serial.print("Acc. axis Z in mg = ");
  Serial.println(AccAsseZ_mg); // print with 4 digit after comma

  /* Perform the acquisition from the barometer */
  Temp_Meas = bmp.readTemperature();
  Press_Meas = bmp.readPressure();

  /* Create the bit mask for the PRESSURE data to be transmitted on the CAN bus
   * int32_t --> 4 x uint8_t  (then, at the receiver size we need to do the same
   */
  tx_buffer[0] = (uint8_t)(Press_Meas);
  tx_buffer[1] = (uint8_t)(Press_Meas>>8);
  tx_buffer[2] = (uint8_t)(Press_Meas>>16);
  tx_buffer[3] = (uint8_t)(Press_Meas>>24);
  /* Create the bit mask for the ACCELEROMETER data to be transmitted on the CAN bus
   * int32_t --> 4 x uint8_t  (then, at the receiver size we need to do the same
   */
  tx_buffer[4] = (uint8_t)(AccAsseZ_mg);
  tx_buffer[5] = (uint8_t)(AccAsseZ_mg>>8);
  tx_buffer[6] = (uint8_t)(AccAsseZ_mg>>16);
  tx_buffer[7] = (uint8_t)(AccAsseZ_mg>>24);

  send_data_to_CAN_bus();  
}
#endif



#if defined(MASTER_MODE) || defined(WIRED_SENSOR)
/**
  * @brief  CAN sending data function
  * @param  None
  * @retval None
  * @note   This function sends the specified payload on the CAN bus.
  */
void send_data_to_CAN_bus()
{
  CAN_Transmit(0x0001,  CAN_Id_Extended, CAN_RTR_Data, 8, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */
  delay(1);
}
#endif