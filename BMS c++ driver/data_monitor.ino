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
byte current_voltage;

uint16_t adc_channel[CHANNELS];
uint16_t cell_temperatures[CHANNELS];
uint16_t cell_voltages[CHANNELS];
int battery_voltage_mv=0;


char received[50];
char command[25];
char value[25];
int value_int;





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

                int i = 0;
                a = Serial.read();
                while (a != '\n') {
                        received[i]= a;
                        a = Serial.read();
                        i++;
                }

                received[i] = '\n';



                i = 0;

                while (received[i] != SEPARATOR){
                        command[i] = received[i];
                        i++;
                }

                command[i] = '\0';


                int k =0;
                i++;
                while (received[i] != '\n') {
                        value[k] = received[i];
                        i++;
                        k++;

                }
                value[k]= '\0';


                // Serial.print(command);
                // Serial.print('\t');
                // Serial.print(SEPARATOR);
                // Serial.print('\t');

                // Serial.print('\n');

                sscanf(value, "%d", &value_int);
                Serial.print(value_int, DEC);


                myAD.read_all( CHANNELS, &adc_channel[0]);
                board_temp= res.getTemperature(analogRead(A4));
                curr_time = millis()- startup_time;


                for (i=0;i<CHANNELS/2; i++){
                cell_voltages[i]=(adc_channel[i]*0.976)+1000;
                battery_voltage_mv = battery_voltage_mv + cell_voltages[i] ;
                }

                for (i=CHANNELS/2+1;i<CHANNELS; i++){
                cell_temperatures[i]=ntc.getTemperature(adc_channel[i]);
                }

                board_temp= res.getTemperature(analogRead(_ONBOARD_NTC_PIN));




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

                else if (strcmp(command, "TCELL") == 0) {
                        for (i=0; i< CHANNELS/2; i++ ) {
                                Serial.print(cell_temperatures[i]);
                                Serial.print(SEPARATOR);

                        }
                        Serial.print('\n');

                }
                else if (strcmp(command, "RBCELL") == 0) {

                }
                else if (strcmp(command, "SBCELL ") == 0) {

                }


        }


        delay(2000);



  }
