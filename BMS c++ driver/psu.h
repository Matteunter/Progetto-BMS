
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
