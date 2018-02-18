//////////////////////////////////////////////////////////////7
//DC-DC DIGITAL CONVERTER CONTROL CLASS
//MODEL: B3603
//https://www.elecrow.com/download/B3603%20User%20Manual.pdf
//
//This converter uses UART protocol to communicate with any other device
// Here are the main functions to control the behaviour of the converter.
//
//////////////////////////////////////////////////////////////
#ifndef PSU_h
#define PSU_h



#include <string.h>
#include <stdio.h>



struct state{
	float current_lim;
	float voltage_lim;
	//float  vin;
    bool output;
    //bool cc;
    //bool cv;
};



class PSU {

	public:
		PSU();

		//bool getState();

		bool setCurrent (float curr_max);
		bool setVoltage (float volt_max);
    	void startCharging ();
		void stopCharging ();
   		bool isCharging ();




	private:

		state curr_state;


};

#endif
