/* The code performs the reception of packets transmitted wireless using the nRF24. The packet are then to be transmitted on the CAN line (selecting an id)
 *  
 *  CAN TRANSMISSION: to be done
 */

/*
Reception of packet from the Radio

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

const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 9;
const static uint8_t PIN_RADIO_CSN = 10;

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

}

void loop()
{
    while (_radio.hasData())
    //if (_radio.hasData())
    {

      
        _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name
        
        //This line of code corrects the received data: in fact, the data is sent using a FIFO logic related to a single byte [the result is stored as follows:
        //MSByte(received data)=LSByte(transmitted data), ... , LSByte(received data)=MSByte(transmitted data)].
        //The following line thus applies a bitmask to obtain one byte of the received data; a shift to each byte in order to position it correctly inside the 32-bit word;
        //a sum to merge all the 4 repositioned Bytes together. NB: THIS WORKS ONLY FOR 32-BIT DATA, CHANGE IT FOR OTHER TYPES OF DATA.
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
    }
}
