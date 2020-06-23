/*
 * This file performs the reading of data coming from accelerometer (LIS3DH) and barometer (BMP180) 
 * and performs the sending of this data to another board, via the nRF24l01+
 * The data sent can be of "whatever" size; at the receiver side, the receiver must know what kind of
 * data he is going to receive and must handle it accordingly (e.g. perform a rearrangement of the 
 * byte received to correctly represent the data
 * 
 * Written by Group Four, 07 November 2019
 * 
 * Special thanks to "THE HOBBI(S)TS"
 */


/*
 * Accelerometer provides different Power modes by changing output bit resolution; we 
 * are just interested to HIGH RESOLUTION, but include also the other two just in case
  */
//#define LOW_POWER
//#define NORMAL_MODE
#define HIGH_RESOLUTION

/* Enable Serial debbug on Serial UART to see registers wrote */
#define LIS3DH_DEBUG Serial

/* Explain how to connect the nRF, just in case 
 *  Radio    Arduino
    CE    -> 9
    CSN   -> 10 (Hardware SPI SS)
    MOSI  -> 11 (Hardware SPI MOSI)
    MISO  -> 12 (Hardware SPI MISO)
    SCK   -> 13 (Hardware SPI SCK)
    IRQ   -> No connection
    VCC   -> No more than 3.6 volts
    GND   -> GND
 */

/* Include all the useful libraries  */
#include "lis3dh-motion-detection.h"
#include "Wire.h"
#include <SPI.h>
#include <NRFLite.h>
#include <Adafruit_BMP085.h>


/* Here we have the configuration parameters for the accelerometer  */
uint16_t sampleRate = 100;  // HZ - Samples per second - 1, 10, 25, 50, 100, 200, 400, 1600, 5000
uint8_t accelRange = 16;    // Accelerometer range = 2, 4, 8, 16g
LIS3DH myIMU(0x18);         // Default address is 0x18. 
int16_t dataHighres = 0;    // Save the raw data before conversion to float

/* Configuration parameter for the barometer */
Adafruit_BMP085 bmp;

/* Configuration parameter for the nRF24 */
const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

// Define a structure to hold the data to be sent
struct RadioPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    int32_t PressureTx;
    int32_t AccelerationTx;
    uint8_t FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;


/* Insert here all the other variables you need */
float AccAsseZ = 0;         // Measured acceleration
int32_t AccAsseZ_mg = 0;    // Acceleration in milli-g
float Temp_Meas = 0;        // Measured temperature (BMP180)
int32_t Press_Meas = 0;     // Measured pressure




/* Initialize the peripherals */
void setup() 
{
  Serial.begin(115200);
  delay(1000); //wait until serial is open...

  /* Initialization of the accelerometer */
  if( myIMU.begin(sampleRate, 1, 1, 1, accelRange) != 0 )
  {
    Serial.print("Failed to initialize IMU.\n");
  }
  else
  {
    Serial.print("IMU initialized.\n");
  }
  
  /* Detection threshold can be from 1 to 127 and depends on the Range
   * chosen above, change it and test accordingly to your application
   * Duration = timeDur x Seconds / sampleRate */
  myIMU.intConf(INT_1, DET_MOVE, 13, 2);
  myIMU.intConf(INT_2, DET_STOP, 13, 10);

  uint8_t readData = 0;

  // Confirm configuration:
  myIMU.readRegister(&readData, LIS3DH_INT1_CFG);
  myIMU.readRegister(&readData, LIS3DH_INT2_CFG);

  // Get the ID:
  myIMU.readRegister(&readData, LIS3DH_WHO_AM_I);
  Serial.print("Who am I? 0x");
  Serial.println(readData, HEX);


  /* Initialization of the barometer */
  if (!bmp.begin()) 
  {
      Serial.println("Could not find a valid BMP085 sensor, check wiring!");
      while (1);
  }


  /* Initialization of the nRF24 */
  // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
  // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
  // You can run the 'ChannelScanner' example to help select the best channel for your environment.
  // You can assign a different bitrate and channel as shown below.
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0)
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75)
  //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100) // THE DEFAULT
    
  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
  {
      Serial.println("Cannot communicate with radio");
      while (1); // Wait here forever.
  }
    
  _radioData.FromRadioId = RADIO_ID;

}


/* Start the execution of the main program */
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
  // Print the result: Temperature and Pressure reading 
  Serial.print("Temperature = ");
  Serial.print(Temp_Meas);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(Press_Meas);
  Serial.println(" Pa");
  Serial.print("\n");

  /* Send the data */
  _radioData.PressureTx = Press_Meas;
  _radioData.AccelerationTx = AccAsseZ_mg;
  Serial.print("Sending ");
  Serial.print(_radioData.PressureTx);
  Serial.print(" Pa, and ");
  Serial.print(_radioData.AccelerationTx); 
  Serial.print(" mg");

  // By default, 'send' transmits data and waits for an acknowledgement.  If no acknowledgement is received,
  // it will try again up to 16 times.  You can also perform a NO_ACK send that does not request an acknowledgement.
  // The data packet will only be transmitted a single time so there is no guarantee it will be successful.  Any random
  // electromagnetic interference can sporatically cause packets to be lost, so NO_ACK sends are only suited for certain
  // types of situations, such as streaming real-time data where performance is more important than reliability.
  //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::NO_ACK)
  //   _radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData), NRFLite::REQUIRE_ACK) // THE DEFAULT
    
  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
  {
      Serial.println("...Success");
  }
  else
  {
      Serial.println("...Failed");
      _radioData.FailedTxCount++;
  }


  Serial.print("\n");
  Serial.print("\n");
  Serial.print("\n");
  
  /* At the end, restart the cycle after a certain amount of time (ms) */
  delay(50);
}
