#include <Wire.h>

#include <SPI.h>
#include <sx1272_INSAT.h>

#define FC_PARAM        CH_868v3
#define BW_PARAM        BW_125
#define CR_PARAM        CR_5
#define SF_PARAM        SF_12
#define PW_PARAM        POW_14

#define PREAMBLE_LONG   12
#define MESSAGE_LONG    12
#define MAX_RETRIES     3

//#define EXPLICIT_HEADER //enable header explicit
#define DISABLE_CRC       //disable CRC

#define TX_ADDR         2
#define DST_ADDR        BROADCAST_ADDR //destination address

#define WAIT_TX_MAX     2000 //en ms
#define WAIT_PERIOD     1000 //en ms

char Message[MESSAGE_LONG] = "INSA";

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
  sx1272.setBW(BW_125); 

#ifdef DISABLE_CRC
  sx1272.setCRC_OFF();
#else
  sx1272.setCRC_ON();
#endif

  sx1272.setNodeAddress(TX_ADDR);
  sx1272.setPreambleLength(PREAMBLE_LONG);

  sx1272._maxRetries = MAX_RETRIES;

  Serial.println(F("#Verification of parameters:#"));

  Serial.print(F("  Node address: "));
  Serial.println(sx1272._nodeAddress, DEC);
  Serial.print(F("  Destination Address : "));

  if(0 == DST_ADDR) 
    Serial.println("Broadcast ( 0 )");
  else 
    Serial.println(DST_ADDR);

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

  float TOA = sx1272.timeOnAir();
  Serial.print("\n Payload length : ");
  sx1272.getPayloadLength();
  Serial.print(sx1272._payloadlength);
  Serial.print("\n Time On Air : ");
  Serial.print(TOA);
  Serial.print("\n Frames interval (msg) : ");
  Serial.print(WAIT_PERIOD);
  Serial.print("\n Fréquence d'émission : 1 paquet tous les ");
  int somme = WAIT_PERIOD + TOA;
  Serial.print(somme);
  Serial.print(" ms. (Tampon + ToA)");

  if(!ConfigOK) return;

  delay(1000);
  Serial.println(F("  "));
  Serial.println(F("----------------------------------------"));
  Serial.println(F("Continuous transmission in broadcast mode."));
  Serial.println(F("----------------------------------------"));
} 

int i = 0;  //number of transmitted packets

void loop() {
  if(!ConfigOK) return;

  e = sx1272.sendPacketTimeout(DST_ADDR, Message, WAIT_TX_MAX);
  if (0 == e) {
    Serial.print(F("\n Packet number "));
    Serial.print(i, DEC);
    Serial.print(F(" sent."));
    sx1272.getPayloadLength();
    Serial.print(" Payload length : ");
    Serial.print(sx1272._payloadlength);
    ++i;
  }
  else {
    Serial.print(F("\n Transmission issue !"));
    Serial.print("\n Payload length : ");
    Serial.print(sx1272._payloadlength);
  }

  delay(WAIT_PERIOD); //interval between each frame
}
