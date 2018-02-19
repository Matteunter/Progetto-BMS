/*****************************************************

File: ADserial.cpp
Authors: F.Garbuglia, M.Unterhorst
Created: Feb 2018
Credits:

*****************************************************/

#include"ADserial.h"
#include <SPI.h>

#ifndef _SS
#define _SS 10              //set slave-select pin
#endif


#define _PDEBUG             //comment to show bits at every transaction

void setup_spi32(int slaveselect)
{
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);       //CPOL=   , CPHA=
  SPI.setBitOrder(MSBFIRST); // MSB first.
  SPI.setClockDivider(SPI_CLOCK_DIV32); // 1 Mhz Clk fo SPI. 16Mhz Div 16 = 1 Mhz.s
  pinMode(slaveselect, OUTPUT);
}


void transferspi32( uint32_t *val, int slaveselect)
{
  byte h_h; // D31-D24 received
  byte h_l; // D23-D16 received
  byte l_h; // D15 -D8 received
  byte l_l; // D07 -D0 received

#ifndef _PDEBUG
  h_h = ((*val)>>24) & 0xFF;
  h_l = ((*val)>>16) & 0xFF;
  l_h = ((*val)>>8) & 0xFF;
  l_l = (*val) & 0xFF;
  Serial.println("=================");
  Serial.println("Sending SPI32bit");
  Serial.print("HEX: ");
  Serial.print(h_h,HEX);
  Serial.print(",");
  Serial.print(h_l,HEX);
  Serial.print(",");
  Serial.print(l_h,HEX);
  Serial.print(",");
  Serial.println(l_l,HEX);
  Serial.print("BIN: ");
  Serial.print(h_h,BIN);
  Serial.print(",");
  Serial.print(h_l,BIN);
  Serial.print(",");
  Serial.print(l_h,BIN);
  Serial.print(",");
  Serial.println(l_l,BIN);
#endif

  digitalWrite(slaveselect,LOW);
  h_h = SPI.transfer( ((*val)>>24) & 0xFF );  // D31-D24
  h_l = SPI.transfer( ((*val)>>16) & 0xFF );  // D23-D16
  l_h = SPI.transfer( ((*val)>>8) & 0xFF );   // D15-D08
  l_l = SPI.transfer( (*val) & 0xFF );        // D07-D00
  delay(100);
  digitalWrite(slaveselect,HIGH);
  *val = (uint32_t) (((uint32_t) h_h <<24) | ((uint32_t) h_l <<16) | ((uint32_t) l_h << 8) | l_l);

 #ifndef _PDEBUG

  Serial.println("Receiving SPI32bit");
  Serial.print("HEX: ");
  Serial.print(h_h,HEX);
  Serial.print(",");
  Serial.print(h_l,HEX);
  Serial.print(",");
  Serial.print(l_h,HEX);
  Serial.print(",");
  Serial.println(l_l,HEX);
  Serial.print("BIN: ");
  Serial.print(h_h,BIN);
  Serial.print(",");
  Serial.print(h_l,BIN);
  Serial.print(",");
  Serial.print(l_h,BIN);
  Serial.print(",");
  Serial.println(l_l,BIN);
#endif

  return ; // Always return zero. Even if the things are not successful
}
