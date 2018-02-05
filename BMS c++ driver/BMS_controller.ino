#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>
#include "AD7280.h"
#include "thermistor.h"




#ifndef _SS
#define _SS 10
#endif

#define _EN_PIN	5


long int startup_time;
AD7280 myAD;                  //1 AD class allocation
THERMISTOR res1;              // 1 onboard resistor class allocation
THERMISTOR ntc[6];            // 6 cell resistor class allocation


byte a;





void setup() {
  digitalWrite(_EN_PIN, HIGH);
  Serial.begin(9600);
  
  myAD.init(_SS);        //initialize slave AD on pin 10
  
  res1.init(ONBOARD_NTC);     //initialize resistor as onboard ntc


  ntc[0].init(CELL_NTC);      //initialize resistors ad cell ntc
  ntc[1].init(CELL_NTC);
  ntc[2].init(CELL_NTC);
  ntc[3].init(CELL_NTC);
  ntc[4].init(CELL_NTC);
  ntc[5].init(CELL_NTC);


 
  
  
  
  //uint16_t chann[12];
           




  
  
}

void loop() {
  // put your main code here, to run repeatedly:

   uint16_t cell_voltage[myAD.ADinst.scan_cnt];  //initialize 1 voltage for each cell
   
    a= Serial.read();
  if (a == 'a') {
    uint32_t provascrittura;
    myAD.writereg(AD7280A_DEVADDR_MASTER, AD7280A_CB1_TIMER, 1, 0x11);
    myAD.read32(&provascrittura);
    Serial.println(provascrittura, HEX);
    
    Serial.println("prova1");
    
    
  
    myAD.read_all( myAD.ADinst.scan_cnt, &cell_voltage[0]);

    Serial.println("prova2");

    int i;
    for(i =0 ; i < 12 ; i++ )
    {
        if( ((i/6)%2) == 0 )
        //if(1)
        {
        Serial.print("VIN_");
        Serial.print( (1+i),DEC );
        Serial.print(": ");
        Serial.println(cell_voltage[i],DEC);
        }
        else
        {
        Serial.print("AUX_");
        Serial.print( (1+i),DEC );
        Serial.print(": ");
        Serial.println(cell_voltage[i],DEC);
        }
     }
   i=0;

   
   uint16_t temp1 = res1.getTemperature(analogRead(A4));   //pin onboard ntc
   Serial.print("temperatura on-board NTC:");
   Serial.println(temp1, DEC);

   

   myAD.cell_balance_enable(1, 120);
   Serial.println("test balancing, cell_bal_register:");
   int trybal = myAD.readreg(AD7280A_DEVADDR_MASTER, 0x17);
   Serial.println(trybal, BIN);

    
  }

}


