

#include "psu.h"
#include <Arduino.h>





//////////////////////////////////////////////////////////////////////////////
//  CONSTRUCTOR MEMBER
//////////////////////////////////////////////////////////////////////////////

PSU::PSU() {
}


//////////////////////////////////////////////////////////////////////////////
//	PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

bool PSU::setCurrent (float curr_max) {
  Serial.begin(38400);
  curr_state.current_lim = curr_max;
  Serial.print("CURRENT ");
  Serial.print(curr_max,3);
  Serial.print("\n");



    }
bool PSU::setVoltage (float volt_max){
  Serial.begin(38400);
  curr_state.voltage_lim = volt_max;
  Serial.print("VOLTAGE ");
  Serial.print(volt_max,2);
  Serial.print("\n");
}



void PSU::startCharging () {
  Serial.begin(38400);
  Serial.print("OUTPUT 1");
  Serial.print("\n");
  curr_state.output = 1;

}


void PSU::stopCharging (){
  Serial.begin(38400);
  Serial.print("OUTPUT 0");
  Serial.print("\n");
  curr_state.output = 0;
}

bool PSU::isCharging (){
  if (curr_state.output ==1) return 1;
  else return 0;
}





