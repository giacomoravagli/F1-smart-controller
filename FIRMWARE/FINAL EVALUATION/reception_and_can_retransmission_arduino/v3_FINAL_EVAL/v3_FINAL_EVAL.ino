/* The code performs the reception of packets transmitted wireless using the nRF24. The packet are then to be transmitted on the CAN line (selecting an id)
 *
 *  This version has modified PIN map to work on the STM8AF DISCOVERY 
 *  When used on the custom board, just need to eliminate the LED blinking and remap the PIN_RADIO_CE = 6
 *  
 *  X FILES - Voglio crederci
 */

/*
Reception of packet from the Radio (deprecated PIN MAP, valid on the NUCLEO)

Radio    Arduino
CE    -> 9
CSN   -> 10 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/
#include "Wire.h"
#include <SPI.h>
#include <NRFLite.h>
#include <stm8s_can.h>

/* Initialization of the parameters for CAN transmission */
unsigned long ID = 0;
unsigned char tx_buffer[0x08] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // payload = 8 bytes

/* Variables name for the Wireless transmission */
const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 2;   // PE0 in the eval board --> to be changed to 6 = PC3 in final board
const static uint8_t PIN_RADIO_CSN = 5;  // PC2

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



void setup()
{
    Serial.begin(115200);
    delay(1000);

    /* Initialize all the LEDs; will be used to encode error messages */
    pinMode(PA3, OUTPUT);
    pinMode(PD3, OUTPUT);
    pinMode(PD0, OUTPUT);
    pinMode(PE3, OUTPUT);
    pinMode(PC3, OUTPUT);
    


    // By default, 'init' configures the radio to use a 2MBPS bitrate on channel 100 (channels 0-125 are valid).
    // Both the RX and TX radios must have the same bitrate and channel to communicate with each other.
    // You can run the 'ChannelScanner' example to help select the best channel for your environment.
    // You can assign a different bitrate and channel as shown below.
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 0);
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE1MBPS, 75);
    //   _radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE2MBPS, 100); // THE DEFAULT
    
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        /* Encoding that there is a problem: 3 LEDs blink an SOS */
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);               // S
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(500);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(500);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(500);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);               // O
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);
        digitalWrite(PA3, HIGH);
        digitalWrite(PD0, HIGH);
        digitalWrite(PC3, HIGH);
        delay(100);
        digitalWrite(PA3, LOW);
        digitalWrite(PD0, LOW);
        digitalWrite(PC3, LOW);
        delay(100);               // S
        
        
        //Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }

    setup_CAN();
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


void loop()
{
    
    digitalWrite(PC3, HIGH);   // just used as a simple debug, to see if the code has arrived there
    while (_radio.hasData())
    {
        digitalWrite(PD0, HIGH);
      
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name
        
        /* This line of code corrects the received data: in fact, the data is sent using a FIFO logic related to a single byte [the result is stored as follows:
         * MSByte(received data)=LSByte(transmitted data), ... , LSByte(received data)=MSByte(transmitted data)].
         * The following line thus applies a bitmask to obtain one byte of the received data; a shift to each byte in order to position it correctly inside the 32-bit word;
         * a sum to merge all the 4 repositioned Bytes together. NB: THIS WORKS ONLY FOR 32-BIT DATA, CHANGE IT FOR OTHER TYPES OF DATA.
         * Surprisingly, no bit mask is required working with another ST Nucleo 8S 208RB
          */
        
        //_radioData.PressureTx = ((_radioData.PressureTx & 0xFF000000)>>24)+((_radioData.PressureTx & 0x00FF0000)>>8)+((_radioData.PressureTx & 0x0000FF00)<<8)+((_radioData.PressureTx & 0x000000FF)<<24);
        //_radioData.AccelerationTx = ((_radioData.AccelerationTx & 0xFF000000)>>24)+((_radioData.AccelerationTx & 0x00FF0000)>>8)+((_radioData.AccelerationTx & 0x0000FF00)<<8)+((_radioData.AccelerationTx & 0x000000FF)<<24);
        
        String msg = "Radio ";
        msg += _radioData.FromRadioId;
        msg += ", pressure:";
        msg += _radioData.PressureTx;
        msg += " Pa, acc:";
        msg += _radioData.AccelerationTx;
        msg += " mg. ";
        msg += _radioData.FailedTxCount;
        msg += " Failed TX";

        Serial.println(msg); 

        digitalWrite(PD0, LOW);


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

void send_data_to_CAN_bus()
{
  digitalWrite(PA3, HIGH);
  CAN_Transmit(0x0001,  CAN_Id_Extended, CAN_RTR_Data, 8, tx_buffer);  /* ID: 0b0000 0100 1001 0110 */
  delay(1);
  digitalWrite(PC3, LOW);
}
