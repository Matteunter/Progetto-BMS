/*****************************************************

File: BMS_controller.ino
Authors: F.Garbuglia, M.Unterhorst
Created: Feb 2018

Description:
Brief algorithm for managing battery charging
using BMSino and Arduino

*****************************************************/

#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>
#include "AD7280.h"
#include "ADserial.h"
#include "thermistor.h"
#include "current_sensor.h"
#include "psu.h"



#ifndef PINS
#define _SS 10
#define _EN_PIN	5
#define _ONBOARD_NTC_PIN A4
#define _CURRENT_OUT_PIN A0
#define _CURRENT_REF_PIN A1
#define CELLS 6
#define CHANNELS 12
#define SEPARATOR ' '
#endif


#ifndef LIMITS
#define C_LIM  0.5
#define V_LIM  13
#define V_TH  4
#define T_TH  50
#define BT_TH 60
#endif


///////////////////////////////////////////////////////////////////////////////
//GLOBAL VARIABLES
///////////////////////////////////////////////////////////////////////////////

AD7280 myAD;                  //1 AD class allocation
PSU    myPSU;
THERMISTOR res;              // 1 onboard resistor class allocation
THERMISTOR ntc;            // 6 cell resistor class allocation
//CURRENT_SENSOR cell_curr;

int16_t board_temp;     //temperatura della board


byte a;
byte balance_reg;
byte full_balance_reg;
byte prev_balance_reg;
int sum;
int k=0;
int i=0;
uint16_t adc_channel[CHANNELS];
uint16_t cell_temperatures[CELLS];
uint16_t cell_voltages[CELLS];

long int startup_time;
long int curr_time;



///////////////////////////////////////////////////////////////////////////////
//SETUP
///////////////////////////////////////////////////////////////////////////////
void setup() {
  digitalWrite(_EN_PIN, HIGH);
  Serial.begin(38400);
  //Serial2.begin(9600);  for pc communication
  delay(1000);  //wait to set coomunication
  myAD.init(_SS);        //initialize slave AD on pin 10
  res.init(ONBOARD_NTC);     //initialize resistor as onboard ntc
  ntc.init(CELL_NTC);      //initialize resistors ad cell ntc
  //cell_curr.init(ACS712);   //setup Hall Effect Sensor

  //print_table();
}




///////////////////////////////////////////////////////////////////////////////
//LOOP
///////////////////////////////////////////////////////////////////////////////
void loop() {
  balance_reg = 0b00000000;
  myAD.balance_all(balance_reg, 0);
  prev_balance_reg;
  full_balance_reg = 0b00000000;


  for (i = 0; i<CHANNELS; i++ ){
    full_balance_reg = full_balance_reg | (1<<i);
  }


  do {
  //reading cell Values

  sum = myAD.read_all( CHANNELS, &adc_channel[0]);

  //separate channels between voltages and temperatures


  for (int i=0;i<CELLS; i++){
    cell_voltages[i]=(adc_channel[i]*0.976)+1000;   //
  }

  for (int i=CHANNELS/2; i<CHANNELS/2+CELLS; i++){
    cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);
  }

  //getting board temperature from analog pin

  board_temp= res.getTemperature(analogRead(_ONBOARD_NTC_PIN));
  curr_time = millis() - startup_time;

  //print_all(cell_voltages, cell_temperatures, curr_time, 0, board_temp);

  k++;    //
  if (k == CELLS) k = 0;
  //wait the end of discharge or overheating

} while ((sum > (CELLS * V_TH))||(cell_temperatures[k]> T_TH));



    // qui possiamo inserire un print di tutto via seriale per pc

  //start Charging
  myPSU.setCurrent(C_LIM);
  delay(1000);
  myPSU.setVoltage(V_LIM);
  delay(1000);
  myPSU.startCharging();
  delay(1000);


  //CYCLE DURING CHARGING



  while (myPSU.isCharging() == 1 ){
    //misuro tensione e temperature

    // se tensioni troppo alte:
    // genero stringa con 1 per le celle sovraccariche
    //attivo bilanciamento con la stringa

    //ciclo per controllore che tutte le celle siano bilanciate

    myAD.read_all(CHANNELS, &adc_channel[0]);

    //separate channels between voltages and temperatures

    for (int i=0;i<CELLS; i++){
      cell_voltages[i]=(adc_channel[i]*0.976)+1000;   //
    }

    for (int i=CHANNELS/2; i<CHANNELS/2+CELLS; i++){
      cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);
    }

    //getting board temperature from analog pin
    board_temp= res.getTemperature(analogRead(_ONBOARD_NTC_PIN));
    //getting acquisition instant
    curr_time = millis() - startup_time;
    //print_all(cell_voltages, cell_temperatures, curr_time, 0, board_temp);

    if (board_temp > BT_TH){  //board temp check
      myPSU.stopCharging();
    }

    // balancing control
    prev_balance_reg = balance_reg;
    for( i=0 ; i <CELLS; i++){
      if (cell_voltages[i] > V_TH){
        balance_reg = balance_reg | (1 << i);
      }
    }

    if (balance_reg != prev_balance_reg){  //if balance_reg changed
      myAD.balance_all(balance_reg, 0);     //eternal balancing
    }

    // temperatures control
    for( i=0 ; i < CELLS; i++){
      if (cell_temperatures[i] > T_TH){
        myPSU.stopCharging();
      }
    }

    //se tutte tensioni sopra la soglia allora stopCharging
    if (balance_reg == full_balance_reg ){
      myPSU.stopCharging();
    }

  }

}

///////////////////////////////////////////////////////////////////////////////
//AUXILIARY FUNCTIONS
///////////////////////////////////////////////////////////////////////////////

// PRINT TEXT ITEM OF A TABLE OF ALL MEASUREMENTS
//////////////////////////////////////////////////
void print_table() {
    for(int i =0 ; i < CHANNELS ; i++ )
  {
    if ((i/(CELLS) %2) == 0 )
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
Serial.print("CHARGING STATE\t");
Serial.print("\n");
startup_time = millis();
}


// PRINT ONE ROW OF A TABLE OF ALL MEASUREMENTS
//////////////////////////////////////////////
void print_all (uint16_t * cell_voltages, uint16_t * cell_temperatures, long int curr_time, int cell_current, int16_t board_temp) {
   //calcolo temperatura degli ntc e rimetto dentro adc-channel
  Serial.print(curr_time, DEC);
  Serial.print("\t");
  for(int i =0 ; i < CELLS ; i++ )    // print voltages and temperature
  {
    Serial.print(cell_voltages[i], DEC);
    Serial.print("\t");
  }
  for(int i =0 ; i < CELLS ; i++ )   // print voltages and temperature
  {
    Serial.print(cell_temperatures[i], DEC);
    Serial.print("\t");
  }
  Serial.print(board_temp, DEC);       //print board temperature
  Serial.print("\t");
  Serial.print(cell_current,BIN);
  Serial.print("\n");
}
