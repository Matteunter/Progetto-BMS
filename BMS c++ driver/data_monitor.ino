#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>
#include "AD7280.h"
#include "thermistor.h"




#ifndef _SS
#define _SS 10
#endif

#define _EN_PIN	5
#define _BOARD_TEMP_PIN A4
#define _CURRENT_OUT_PIN A0
#define _CURRENT_REF_PIN A1


long int startup_time;
AD7280 myAD;                  //1 AD class allocation
THERMISTOR res1;              // 1 onboard resistor class allocation
THERMISTOR ntc1;            // 6 cell resistor class allocation

int CELL_NUMBER = 6;
byte a;

int cell_current;
long int curr_time;
int16_t board_temp;



void setup() {
  digitalWrite(_EN_PIN, HIGH);
 
  
  myAD.init(_SS);        //initialize slave AD on pin 10
  
  res1.init(ONBOARD_NTC);     //initialize resistor as onboard ntc


  ntc1.init(CELL_NTC);      //initialize resistor as cell ntc
  

  

           
      
  Serial.print("Time\t");
  int i;
  for(i =0 ; i < myAD.ADinst.scan_cnt ; i++ )
    {
      if ((i/(CELL_NUMBER) %2) == 0 )
      {
      Serial.print("V");
      Serial.print( (1+i),DEC );
      Serial.print("\t");
      }
      else
      {
      Serial.print("T");
      Serial.print( (1+i-6),DEC );
      Serial.print("\t");
      }
    }
  i=0;

  Serial.print("BOARD_TEMP\t");
  Serial.print("CURRENT\t");
  Serial.print("\n");	
  startup_time = millis();



  
  return;
}

void loop() {

   
   
 

   a= Serial.read();
   uint16_t adc_channel[myAD.ADinst.scan_cnt];


   myAD.read_all( myAD.ADinst.scan_cnt, &adc_channel[0]);
   //calcolo temperatura degli ntc e rimetto dentro adc-channel
   

   board_temp= res1.getTemperature(analogRead(A4));   //pin onboard ntc
   
   curr_time = millis()- startup_time;

   Serial.print(curr_time, DEC);
   Serial.print("\t");
   int i;

   for(i =0 ; i < myAD.ADinst.scan_cnt ; i++ )		// print voltages and temperature
    {
      Serial.print(adc_channel[i], DEC);
      Serial.print("\t");
    }

   Serial.print(board_temp, DEC);   		//print board temperature
   Serial.print("\t");


   Serial.print("CURRENT");
   Serial.print("\n");

   

  return;

  }




