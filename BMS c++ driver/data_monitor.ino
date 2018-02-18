#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>
#include "AD7280.h"
#include "thermistor.h"
#include "current_sensor.h"
#include <stdio.h>



#ifndef _SS
#define _SS 10

#endif

#define _EN_PIN	5
#define _ONBOARD_NTC_PIN A4
#define _CURRENT_OUT_PIN A0
#define _CURRENT_REF_PIN A1

#define CELLS 6
#define CHANNELS 12
#define SEPARATOR ' '

long int startup_time;
AD7280 myAD;                  //1 AD class allocation
THERMISTOR res;              // 1 onboard resistor class allocation
THERMISTOR ntc;            // 6 cell resistor class allocation
CURRENT_SENSOR curr_val;


byte a;

int cell_current;
long int curr_time;
int16_t board_temp;


uint16_t adc_channel[CHANNELS];
uint16_t cell_temperatures[CHANNELS];
uint16_t cell_voltages[CHANNELS];
int battery_voltage_mv=0;


char received[50];
char command[25];   //COMMAND LABEL
char value[25];
int value_int;
byte value_bin;
int balance_reg;





void setup() {
        digitalWrite(_EN_PIN, HIGH);
        myAD.init(_SS);        //initialize slave AD on pin 10

        startup_time = millis();
        return;
        Serial.begin(9600);

        res.init(ONBOARD_NTC);     //initialize resistor as onboard ntc
        ntc.init(CELL_NTC);      //initialize resistor as cell ntc
}


void loop() {


        //Serial.print(Serial.available());
        if (Serial.available() > 0){
                int i;
                //GET WHOLE COMMAND STRING FROM SERIAL
                a = Serial.read();
                for (int i =0; a != '\n'; i++) {
                        received[i]= a;
                        a = Serial.read();
                        i++;
                }
                received[i] = '\n';     //terminate received command

                i = 0;

                //SEPARATE COMMAND LABEL FROM VALUE
                for (;received[i] != SEPARATOR; i++){
                        command[i] = received[i];
                }

                command[i] = '\0';      //terminate label string


                int k =0;
                i++;
                for (k=0; received[i] != '\n'; k++) {
                        value[k] = received[i];
                        i++;
                }
                value[k]= '\0';         //terminate value string


                // Serial.print(command);
                // Serial.print('\t');
                // Serial.print(SEPARATOR);
                // Serial.print('\t');

                // Serial.print('\n');

                sscanf(value, "%d", &value_int);        //convert value string to integer
                Serial.print(value_int, DEC);


                myAD.read_all( CHANNELS, &adc_channel[0]);          //read all channels
                curr_time = millis()- startup_time;

                //separate voltages from temperatures
                for (i=0;i<CHANNELS/2; i++){
                cell_voltages[i]=(adc_channel[i]*0.976)+1000;
                battery_voltage_mv = battery_voltage_mv + cell_voltages[i] ;    //calc total voltage across the battery
                }

                for (i=CHANNELS/2+1;i<CHANNELS; i++){
                cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);        //convert cell temperature to degrees
                }

                board_temp= res.getTemperature(analogRead(_ONBOARD_NTC_PIN));       //board temperature acquisition and convertion


                // TO PRINT VOLTAGES

                if (strcmp(command, "mVCELL") == 0) {
                        if ((value_int <= CELLS)&&(value_int > 0)) {
                                if (strcmp(value, "A")==0){
                                        for (i=0; i< CHANNELS/2; i++ ) {
                                                Serial.print(cell_voltages[i]);
                                                Serial.print(SEPARATOR);
                                        }
                                }

                                else    Serial.print(cell_voltages[value_int]);
                                Serial.print('\n');
                        }
                        else    Serial.print("ERROR");
                }

                //TO PRINT TEMPERATURES

                else if (strcmp(command, "TCELL") == 0) {
                        if ((value_int <= CELLS)&&(value_int > 0)) {
                                if (strcmp(value, "A")==0){
                                        for (i=0; i< CHANNELS/2; i++ ) {
                                                Serial.print(cell_temperatures[i]);
                                                Serial.print(SEPARATOR);
                                        }
                                }

                                else    Serial.print(cell_temperatures[value_int]);
                                Serial.print('\n');
                        }
                        else    Serial.print("ERROR");
                }


                //TO PRINT BALANCING MOSFET STATUS

                }
                else if (strcmp(command, "RBCELL") == 0) {
                        balance_reg = myAD.readreg(0, 0x14);            //read from balance register
                        Serial.print(balance_reg, BIN);


                }






                //TO ENABLE BALANCING FROM CELL BITMASK


                else if (strcmp(command, "SBCELL ") == 0) {
                        value_bin= 0x00;
                        for (int i=0; value[i]!=NULL; i++){
                                if (value[i]= '1') {
                                        value_bin = value_bin | (1 << i);
                                }
                                else {
                                        if (value[i] == '0') value_bin = value_bin & (0 << i);
                                        else Serial.println("WRONG CELL BALANCING BITMASK");
                                }
                        }
                        myAD.balance_all(value_bin, 0 );

                }

                else Serial.print("UNKNOWN COMMAND");




        delay(1000);



  }
