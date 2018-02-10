
#ifndef PSU.h
#define PSU.h



#include <string.h>


struct state{
	int current;
	int voltage;
	int vin;
    bool output;
    bool cc;
    bool cv;
}



class PSU {

	public:
		PSU();

		bool init(uint8_t initial_current);
		bool getState();
		
		bool setCurrent ();
		bool setVoltage ();
		bool stopCharging ();


		









	private:

		state curr_state;
		

};