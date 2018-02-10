


//////////////////////////////////////////////////////////////////////////////
//	PUBLIC MEMBERS
//////////////////////////////////////////////////////////////////////////////

#ifndef PSU_STRINGS
#define SAVE_CMD	"SAVE"
#define LOAD_CMD	"LOAD"
#define ENABLE_CMD	"OUTPUT 1"
#define DISABLE_CMD	"OUTPUT 0"
#define STATUS_CMD	"STATUS"
#define 
#define
#define
#define
#define
#define
#define
#define







#endif



#include "PSU.h"


bool PSU::init(initial_current){
	char answer[];
	Serial.begin(38400);
	Serial.print("CURRENT ");
	Serial.println(initial_current, DEC);
	Serial.println("SAVE");
	Serial.println("OUTPUT 1");
	answer = Serial.read();

	if (strcmp(answer, "  ")!=1) 
		return false;


	return true;
}


bool getState(){
	char answer[50];
	answer = Serial.read()
	











		}



	}
	





}













bool setCurrent () {





}






bool setVoltage () {






}







bool stopCharging () {

}