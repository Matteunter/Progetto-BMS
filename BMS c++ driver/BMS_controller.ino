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




#ifndef _SS
#define _SS 10
#endif

#define _EN_PIN	5
#define C_LIM  0.5
#define V_LIM  13
#define V_TH  4


#define T_TH  50
#define ONBOARD_NTC_PIN A4
#define BT_TH 60




long int startup_time;
AD7280 myAD;                  //1 AD class allocation
PSU    myPSU;
THERMISTOR res;              // 1 onboard resistor class allocation
THERMISTOR ntc;            // 6 cell resistor class allocation
//CURRENT_SENSOR cell_curr;

int board_temp;     //temperatura della board


byte a;





void setup() {
  digitalWrite(_EN_PIN, HIGH);
  Serial.begin(38400);
  myAD.init(_SS);        //initialize slave AD on pin 10
  res.init(ONBOARD_NTC);     //initialize resistor as onboard ntc
  ntc.init(CELL_NTC);      //initialize resistors ad cell ntc
  //cell_curr.init(ACS712);   //setup Hall Effect Sensor
}









void loop() {

  int sum;
  int k=0;
  int i=0;
  uint16_t adc_channel[myAD.ADinst.scan_cnt];
  uint16_t cell_temperatures[myAD.ADinst.scan_cnt];
  uint16_t cell_voltages[myAD.ADinst.scan_cnt];
  byte balance_reg = 0b00000000;
  myAD.balance_all(balance_reg, 0);
  byte prev_balance_reg;

  byte full_balance_reg = 0b00000000;


  for (i = 0; i<myAD.ADinst.scan_cnt; i++ ){
    full_balance_reg = full_balance_reg | (1<<i);
  }


  do {
  //reading cell Values

  sum = myAD.read_all( myAD.ADinst.scan_cnt, &adc_channel[0]);

  //separate channels between voltages and temperatures


  for (i=0;i<myAD.ADinst.scan_cnt/2; i++){
    cell_voltages[i]=(adc_channel[i]*0.976)+1000;   //
  }

  for (i=myAD.ADinst.scan_cnt/2+1;i<myAD.ADinst.scan_cnt; i++){
    cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);
  }

  //getting board temperature from analog pin

  board_temp= res.getTemperature(analogRead(ONBOARD_NTC_PIN));

  k++;    //

  if (k == myAD.ADinst.scan_cnt/2) k = 0;

  //wait the end of discharge or overheating

  } while ((sum > (myAD.ADinst.scan_cnt/2 * V_TH))||(cell_temperatures[k]> T_TH));



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

    myAD.read_all( myAD.ADinst.scan_cnt, &adc_channel[0]);

    //separate channels between voltages and temperatures

    for (i=0;i<myAD.ADinst.scan_cnt/2; i++){
      cell_voltages[i]=(adc_channel[i]*0.976)+1000;   //
    }

    for (i=myAD.ADinst.scan_cnt/2+1;i<myAD.ADinst.scan_cnt; i++){
      cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);
    }

    //getting board temperature from analog pin
    board_temp= res.getTemperature(analogRead(ONBOARD_NTC_PIN));
    if (board_temp > BT_TH){  //board temp check
      myPSU.stopCharging();
    }

    // balancing control
    prev_balance_reg = balance_reg;
    for( i=0 ; i < myAD.ADinst.scan_cnt/2; i++){
      if (cell_voltages[i] > V_TH){
        balance_reg = balance_reg | (1 << i);
      }
    }

    if (balance_reg != prev_balance_reg){  //if balance_reg changed
      myAD.balance_all(balance_reg, 0);     //eternal balancing
    }

    // temperatures control
    for( i=0 ; i < myAD.ADinst.scan_cnt/2; i++){
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
