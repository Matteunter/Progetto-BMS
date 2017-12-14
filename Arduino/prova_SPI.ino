
#include <SPI.h>


const int currMISOPin = 12;
const int currMOSIPin = 11;

const int currSlavePin = 10;
const int CNVSTnot = 17;
const int DCDC_EN= 7;
int count=0;

  long int datatosend =0;
  byte datatosendbyte1=0;
  byte datatosendbyte2=0;
  byte datatosendbyte3=0;
  byte datatosendbyte4=0;
  long int result=0;
 int a;
void setup(){
 
    // put your setup code here, to run once:
  Serial.begin(9600);

  // start the SPI library:
  SPI.begin();
  SPI.setDataMode(SPI_MODE1);
  SPI.setBitOrder(MSBFIRST); // MSB first.
  SPI.setClockDivider(SPI_CLOCK_DIV16); // 1 Mhz Clk fo SPI. 16Mhz Div 16 = 1 Mhz.s
  

  // initalize the  data ready and chip select pins:


  pinMode(currSlavePin, OUTPUT);
  
  digitalWrite(5, HIGH);
 




  
}
void loop() {

  digitalWrite(5, HIGH);
  a = Serial.read(); 
  //Serial.println(a,DEC);
  if( a == 'a') // Datasheet Initialise
  {
  Serial.print("start\n");

  
  datatosend= 0x01C2B6E2;
  datatosendbyte1= ((datatosend)>>24) & 0xFF;
  datatosendbyte2= ((datatosend)>>16) & 0xFF;
  datatosendbyte3= ((datatosend)>>8) & 0xFF;
  datatosendbyte4= (datatosend) & 0xFF;
  
  digitalWrite(currSlavePin, LOW);
  SPI.transfer(datatosendbyte1);
  SPI.transfer(datatosendbyte2);
  SPI.transfer(datatosendbyte3);
  SPI.transfer(datatosendbyte4);
  //result=SPI.transfer(0x00);
  
  
  digitalWrite(currSlavePin, HIGH);

  

  result= (uint32_t) (((uint32_t) datatosendbyte1 <<24) | ((uint32_t) datatosendbyte2 <<16) | ((uint32_t) datatosendbyte3 << 8) | datatosendbyte4);
//  Serial.print(result, BIN);
//  Serial.print("\n");
  
//  digitalWrite(currSlavePin, LOW);
//  datatosend= 0x038716CA;
//  SPI.transfer(datatosend);
//  digitalWrite(currSlavePin, HIGH);
//
//  digitalWrite(currSlavePin, LOW);
//  datatosend= 0xF800030A;
//  SPI.transfer(datatosend);
//  result=SPI.transfer(0x00);
//  digitalWrite(currSlavePin, HIGH);
  

  datatosend= 0x038716CA;
  datatosendbyte1= ((datatosend)>>24) & 0xFF;
  datatosendbyte2= ((datatosend)>>16) & 0xFF;
  datatosendbyte3= ((datatosend)>>8) & 0xFF;
  datatosendbyte4= (datatosend) & 0xFF;
  
  digitalWrite(currSlavePin, LOW);
  datatosendbyte1=SPI.transfer(datatosendbyte1);
  datatosendbyte2=SPI.transfer(datatosendbyte2);
  datatosendbyte3=SPI.transfer(datatosendbyte3);
  datatosendbyte4=SPI.transfer(datatosendbyte4);

  digitalWrite(currSlavePin, HIGH);

  result= (uint32_t) (((uint32_t) datatosendbyte1 <<24) | ((uint32_t) datatosendbyte2 <<16) | ((uint32_t) datatosendbyte3 << 8) | datatosendbyte4);
//  Serial.print(result, BIN);
//  Serial.print("\n");


  datatosend= 0xF800030A;
  datatosendbyte1= ((datatosend)>>24) & 0xFF;
  datatosendbyte2= ((datatosend)>>16) & 0xFF;
  datatosendbyte3= ((datatosend)>>8) & 0xFF;
  datatosendbyte4= (datatosend) & 0xFF;

  digitalWrite(currSlavePin, LOW);
  datatosendbyte1=SPI.transfer(datatosendbyte1);
  datatosendbyte2=SPI.transfer(datatosendbyte2);
  datatosendbyte3=SPI.transfer(datatosendbyte3);
  datatosendbyte4=SPI.transfer(datatosendbyte4);

  digitalWrite(currSlavePin, HIGH);

  result= (uint32_t) (((uint32_t) datatosendbyte1 <<24) | ((uint32_t) datatosendbyte2 <<16) | ((uint32_t) datatosendbyte3 << 8) | datatosendbyte4);
//  Serial.print(result, BIN);
//  Serial.print("\n");

 
  }

//test

  //read commands

 
}



