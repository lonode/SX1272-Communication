#include <Wire.h>

#include <SPI.h>
#include <sx1272_INSAT.h>

#define FC_PARAM        CH_868v3 //center freq
#define BW_PARAM        BW_125 //bandwidth
#define CR_PARAM        CR_5 //coding rate (4/5 here)
#define SF_PARAM        SF_12 //Spreading Factor
#define PW_PARAM        POW_14 //Power (from -1 to 14dBm)

#define PREAMBLE_LONG   12
#define MAX_RETRIES     3

#define EXPLICIT_HEADER     //enable header explicit
//#define DISABLE_CRC       //disable CRC

#define RX_ADDR         8

#define WAIT_RX_MAX     5000 //en ms

// status variables
int8_t e;
boolean ConfigOK = true;  //false = issue when starting the module or config issue

void setup() {
  Serial.begin(115200); // Open serial com
  Serial.println(F("SX1272 module configuration in Arduino"));
  e = sx1272.ON(); // Power ON the module
  if (0 == e)
  {
    Serial.println(F("SX1272 Module on"));
  }
  else 
  {
    Serial.println(F("Problem of activation of SX1272 Module !"));
    ConfigOK = false;
    return;
  }

  e = sx1272.setChannel(FC_PARAM); 
  Serial.println(sx1272.getChannel());
  Serial.println(sx1272._channel, HEX);
  Serial.print(F("Frequency channel "));
  Serial.print(FC_PARAM, HEX);

  if (0 == e)
  {
    Serial.println(F(" has been successfully set."));
  }
  else
  {
    Serial.println(F(" has not been set !"));
    ConfigOK = false;
  }

  e = sx1272.setPower(PW_PARAM);
  Serial.print(F("Output power "));
  Serial.print(sx1272._power, HEX);
  if (0 == e)
  {
    Serial.println(F(" has been successfully set."));
  }
  else
  {
    Serial.println(F(" has not been set !"));
    ConfigOK = false;
  }

  if(!ConfigOK) 
  {
    Serial.println(F("SX1272 initialization failed !"));
    return;
  }

#ifdef EXPLICIT_HEADER
  sx1272.setHeaderON();
#else
  sx1272.setHeaderOFF();
#endif

  sx1272.setCR(CR_PARAM);  
  sx1272.setSF(SF_PARAM); 
  sx1272.setBW(BW_PARAM); 

#ifdef DISABLE_CRC
  sx1272.setCRC_OFF();
#else
  sx1272.setCRC_ON();
#endif

  sx1272.setNodeAddress(RX_ADDR);
  sx1272.setPreambleLength(PREAMBLE_LONG);

  sx1272._maxRetries = MAX_RETRIES;

  Serial.println(F("#Verification of parameters:#"));

  Serial.print(F("  Node address: "));
  Serial.println(sx1272._nodeAddress, DEC);
  Serial.print(F("  Bandwidth: "));
  Serial.println(sx1272._bandwidth, DEC);
  Serial.print(F("  Coding rate: "));
  Serial.println(sx1272._codingRate, DEC);
  Serial.print(F("  Spreading factor: "));
  Serial.println(sx1272._spreadingFactor, DEC);
  Serial.print(F("  Header mode: "));
  Serial.println(sx1272._header, DEC);
  Serial.print(F("  CRC field: "));
  Serial.println(sx1272._CRC, DEC);
  Serial.print(F("  BW: "));
  Serial.println(sx1272._bandwidth, DEC);
  Serial.println(F("SX1272 successfully configured !"));

  if(!ConfigOK) return;

  //status (correct = 1 or bad = 0 or non received = 2) 
  Serial.println(F("\n "));
  Serial.println(F("Module ready for reception ! "));
  Serial.println(F("Packet status ; Packet number ; Received data ; RSSI packet (dBm) ; SNR ; source address ;  RSSI (dBm) "));
  Serial.println(F("\n "));
}


#define WAIT_RX_MAX     5000 //en ms
void loop()
{
  if(!ConfigOK) return;
  
  e = sx1272.receivePacketTimeout(WAIT_RX_MAX); //receive a frame 
 
  char StatusRXMessage;
  if (e == 0)   //packet received
    if (sx1272._reception == CORRECT_PACKET)
      StatusRXMessage = '1';    
    else
      StatusRXMessage = '0';
  else         //not received/lost
    StatusRXMessage = '2';  

  Serial.println();
  Serial.print(StatusRXMessage);
  Serial.print(F(" ; "));
  Serial.print(sx1272.packet_received.packnum,DEC);
  Serial.print(F(" ; "));

  for (uint8_t i =0; i < sx1272.packet_received.length; ++i)
    Serial.print(sx1272.packet_received.data[i]);  
  Serial.print(":");
  for (uint8_t i =0; i < sx1272.packet_received.length; ++i)
    Serial.write(sx1272.packet_received.data[i]);  

  Serial.print(F(" ; "));
  e = sx1272.getRSSIpacket(); //RSSI of the last packet
  Serial.print(sx1272._RSSIpacket, DEC); 
  Serial.print(F(" ; "));
  sx1272.getSNR();
  Serial.print(sx1272._SNR, DEC); 
  Serial.print(F(" ; "));
  Serial.print(sx1272.packet_received.src,DEC);

  sx1272.getRSSI(); //global RSSI
  Serial.print(F(" ; "));
  Serial.print(sx1272._RSSI,DEC);
}
