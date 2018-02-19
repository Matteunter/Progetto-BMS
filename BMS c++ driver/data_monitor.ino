#include <SPI.h>
#include <stdint.h>
#include <Arduino.h>
#include <stdio.h>
#include "AD7280.h"
#include "thermistor.h"
#include "current_sensor.h"
#include "ADserial.h"



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
uint16_t cell_temperatures[CELLS];
uint16_t cell_voltages[CELLS];
int battery_voltage_mv=0;


char received[50];
char command[25];
char value[25];
int value_int;
byte value_bin;
int balance_reg;





void setup() {
        digitalWrite(_EN_PIN, HIGH);
        myAD.init(_SS);        //initialize slave AD on pin 10


        startup_time = millis();
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

                sscanf(value, "%d", &value_int);        //convert value string to integer
                //Serial.print(value_int, DEC);


                myAD.read_all(CHANNELS, &adc_channel[0]);          //read all channels



                curr_time = millis()- startup_time;

                //separate voltages from temperatures
                for (i=0;i<CELLS; i++){
                cell_voltages[i]=(adc_channel[i]*0.976)+1000;
                battery_voltage_mv = battery_voltage_mv + cell_voltages[i] ;    //calc total voltage across the battery
                }
                k = 0;
                uint32_t temp;
                for (i=CHANNELS/2; i<CHANNELS/2+CELLS; i++){
                    cell_temperatures[k] = ntc.getTemperature(adc_channel[i]);        //INSERTTT   convert cell temperature to degrees
                    k++;
                }

                board_temp= res.getTemperature(analogRead(_ONBOARD_NTC_PIN));       //board temperature acquisition and convertion


                // TO PRINT VOLTAGES

                if (strcmp(command, "mVCELL") == 0) {
                        if (strcmp(value, "A")==0){
                            for (i=0; i< CHANNELS/2; i++ ) {
                                    Serial.print(cell_voltages[i], DEC);
                                    Serial.print(SEPARATOR);
                            }
                            Serial.print('\n');
                        }
                        else if ((value_int <= CELLS)&&(value_int > 0)) {
                            Serial.print(cell_voltages[value_int]);
                            Serial.print('\n');
                        }
                        else    Serial.print("ERROR");
                }

                //TO PRINT TEMPERATURES

                else if (strcmp(command, "TCELL") == 0) {

                                if (strcmp(value, "A")==0){
                                        for (i=0; i< CELLS; i++ ) {
                                                Serial.print((cell_temperatures[i]), DEC);
                                                Serial.print(SEPARATOR);
                                        }
                                        Serial.print('\n');
                                }
                                else if ((value_int <= CELLS)&&(value_int > 0)) {
                                    Serial.println(cell_temperatures[value_int], DEC);
                                }
                        else    Serial.print("ERROR");
                }


                //TO PRINT BALANCING MOSFET STATUS


                else if (strcmp(command, "RBCELL") == 0) {
                        Serial.print("PROVA1");
                        balance_reg = myAD.readreg(0, 0x14);            //read from balance register
                        Serial.print(balance_reg, BIN);


                }






                //TO ENABLE BALANCING FROM CELL BITMASK


                else if (strcmp(command, "SBCELL") == 0) {
                        Serial.println(value);
                        int error=0;
                        value_bin= 0x00;
                        for (int i=0; value[i]!=NULL; i++){
                                if (value[i]== '1') {
                                        value_bin = value_bin | (0b100000 >> i);
                                }
                                else {
                                        if (value[i] != '0') error =1;

                                }
                        }
                        if (error == 1) Serial.println("WRONG CELL BALANCING BITMASK");
                        else myAD.balance_all(value_bin, 0 );

                }

        }




        delay(2000);
        return;
}
