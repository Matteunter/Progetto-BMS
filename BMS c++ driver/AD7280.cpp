/*****************************************************

File: AD7280.cpp
Authors: F.Garbuglia, M.Unterhorst
Created: Feb 2018
Credits:

*****************************************************/

#include <SPI.h>
#include <Arduino.h>
#include "ADserial.h"
#include "AD7280.h"




/////////////////////////////////////////////////////////////////////////////////
//	PRIVATE MEMBERS
/////////////////////////////////////////////////////////////////////////////////

static void AD7280::build_crc(uint8_t *crc_tab)
{
	uint8_t bit, crc;
	int16_t cnt, i;

	for (cnt = 0; cnt < 256; cnt++) {
		crc = cnt;
		for (i = 0; i < 8; i++) {
			bit = crc & HIGHBIT;
			crc <<= 1;
			if (bit)
				crc ^= POLYNOM;
		}
		crc_tab[cnt] = crc;
	}
}




uint8_t AD7280::calc_crc(uint8_t *crc_tab, uint32_t val)
{
	uint8_t crc;
	crc = crc_tab[(uint16_t)(val >> 16 & 0xFF)];
	crc = crc_tab[crc ^ (val >> 8 & 0xFF)];
	return  crc ^ (val & 0xFF);
}





int8_t AD7280::check_crc(uint32_t val)
{
	uint8_t crc = calc_crc(ADinst.crc_tab, val >> 10);

	if (crc != ((val >> 2) & 0xFF))
		return -1;

	return 0;
}



void AD7280::wait()
{
delay(ADinst.readback_delay_ms);
}




int8_t AD7280::chain_setup()				// Daisy chain setup returns the number of devices or error code.
{
	uint32_t val;
	uint8_t n;

	writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_LB, 1,			// To reset the AD7280a.
			AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			AD7280A_CTRL_LB_MUST_SET |
			AD7280A_CTRL_LB_SWRST |
			ADinst.ctrl_lb);

	writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_LB, 1,			// Lock to the new address and
			AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN |
			AD7280A_CTRL_LB_LOCK_DEV_ADDR |
			AD7280A_CTRL_LB_MUST_SET |
			ADinst.ctrl_lb);


	writereg(AD7280A_DEVADDR_MASTER, AD7280A_READ, 1,			// Set the read register of all the devices to AD7280A_CONTROL_LB
			AD7280A_CONTROL_LB << 2);

	for (n = 0; n <= AD7280A_MAX_CHAIN; n++) {
		read32(&val);

		if (val == 0) 							// If we get an Empty frame then we end the enumeration.
			return n - 1; 						// Minimum is 0 device excluding the master

		if (check_crc(val))
			return 200;

		if (n != AD7280A_DEVADDR(val >> 27))
			return 200;
	}

	return 255;
}









//	CONSTRUCTOR MEMBER

AD7280::AD7280(){
}



//////////////////////////////////////////////////////////////////////////////
//	PUBLIC MEMBERS
//////////////////////////////////////////////////////////////////////////////

int8_t AD7280::read32(uint32_t *val)
{
	*val = AD7280A_READ_TXVAL;	 //0xF800030A
	transferspi32(val, 10); 		// received value will be in <val>
	return 0;
}




int8_t AD7280::writereg(uint32_t devaddr, uint32_t addr, uint8_t all, uint32_t val)		// Send Data (val) to devices. Returns not relevant.
{
	uint32_t reg = (devaddr << 27 | addr << 21 |
			(val & 0xFF) << 13 | all << 12);

	reg |= calc_crc(ADinst.crc_tab, reg >> 11) << 3 | 0x2;
	reg = (uint32_t)(reg);

									//return spi_write(st->spi, &reg, 4);  Wrtite to SPI.
	transferspi32(&reg, 10);
	return	0;
}




int8_t AD7280::readreg(uint32_t devaddr,uint32_t addr)
{
  uint32_t tmp;


  writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,		/* turns off the read operation on all parts */
      AD7280A_CTRL_HB_CONV_INPUT_ALL |
      AD7280A_CTRL_HB_CONV_RES_READ_NO |
      ADinst.ctrl_hb);

  writereg(devaddr, AD7280A_CONTROL_HB, 0,				/* turns on the read operation on the addressed part */
      AD7280A_CTRL_HB_CONV_INPUT_ALL |
      AD7280A_CTRL_HB_CONV_RES_READ_ALL |
      ADinst.ctrl_hb);


  writereg(devaddr, AD7280A_READ, 0, addr << 2);			 /* Set the address of the register on the device to be read from */


  read32(&tmp);								 /* Valid read so that the value can be received*/

  //if (check_crc(tmp)) 								// Check the received CRC
    //return -1;									// if not, returns -1

  //if (((tmp >> 27) != devaddr) || (((tmp >> 21) & 0x3F) != addr))
    //return -1; 									// Check if the received device and register address is correct.

  return (tmp >> 13) & 0xFF;							// if crc and addressess are correct, returns the read value
}




uint16_t AD7280::read_channel(uint32_t devaddr,uint32_t addr)			    // Read from an Analog channel with start of Conversion as CS start. Returns the value of the ADC count received.
{
	int8_t ret;
	uint32_t tmp;


	ret = writereg(devaddr, AD7280A_READ, 0, addr << 2);			/* Write on Read Register of "the device" the Register address of channel*/


	ret = writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,		/* Write on Ctrl_HB of "all device" to convert all but not to read all.*/
			AD7280A_CTRL_HB_CONV_INPUT_ALL |
			AD7280A_CTRL_HB_CONV_RES_READ_NO |
			ADinst.ctrl_hb);

	ret = writereg(devaddr, AD7280A_CONTROL_HB, 0,				/* Write on Ctrl_HB of "the device" to convert all and to read all and to start the conversion as CS start*/
			AD7280A_CTRL_HB_CONV_INPUT_ALL |
			AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			AD7280A_CTRL_HB_CONV_START_CS |
			ADinst.ctrl_hb);

	wait();									/*Wait for the end of convertion*/


	read32(&tmp);						/*Valid write to get the channel value*/

	//if (check_crc(tmp)) // Check the received CRC
	//	return 0;

	//if (((tmp >> 27) != devaddr) || (((tmp >> 23) & 0xF) != addr))
	//	return 1; // Check if the received device and register address is correct.

	return (tmp >> 11) & 0xFFF;						// if crc and addressess are correct, returns the read value
}




uint32_t AD7280::read_all(uint32_t cnt, uint16_t *array)			/*Write to all the Control HB of all the devices, select all channels, read all, Start conversion at CS Start*/
{
	uint8_t  ret;
	uint32_t i;
	uint32_t tmp, sum = 0;

	ret = writereg(AD7280A_DEVADDR_MASTER, AD7280A_READ, 1,		/* Write to all the read registers of all the devices the address of first channel register i.e AD7280A_CELL_VOLTAGE_1*/
	 	AD7280A_CELL_VOLTAGE_1 << 2);

	int thermistor_config = 						/* Write on Ctrl_LB of "all device" to set termistor function.*/
		AD7280A_CTRL_LB_ACQ_TIME_1600ns |
		AD7280A_CTRL_LB_MUST_SET |
		AD7280A_CTRL_LB_THERMISTOR_EN |
		AD7280A_CTRL_LB_INC_DEV_ADDR |
		AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN |
		ADinst.ctrl_lb;
	//Serial.print("thermistor_config:");
	//Serial.print(thermistor_config);


	ret = writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_LB, 1,
			thermistor_config);

	ret = writereg(AD7280A_DEVADDR_MASTER, AD7280A_CONTROL_HB, 1,		/*Write to all the Control HB of all the devices, select all channels, read all, Start conversion at CS Start*/
			AD7280A_CTRL_HB_CONV_INPUT_ALL |
			AD7280A_CTRL_HB_CONV_RES_READ_ALL |
			AD7280A_CTRL_HB_CONV_START_CS |
      AD7280A_CTRL_HB_CONV_AVG_8|
			ADinst.ctrl_hb);

	wait();

	for (i = 0; i < cnt; i++) {
		read32(&tmp);

		if (check_crc(tmp))
			return 0xFFF;

			array[i] = ((tmp >> 11) & 0xFFF);
		/* only sum cell voltages */
		if (((tmp >> 23) & 0xF) <= AD7280A_CELL_VOLTAGE_6)
		{
			sum += ((tmp >> 11) & 0xFFF);
			//Serial.println(sum,HEX); //
		}
	}

	return sum;								//returns the sum of the total voltage across cells

}


void AD7280::cell_balance_enable(uint8_t cell_num, uint8_t timer_sec){

  if (timer_sec<71) timer_sec=71;      //minimum 71.5 seconds
  timer_sec = ((timer_sec / 71) << 3);    // timer in sec converted in 5 bit binary, 000 LSBs reserved

  uint8_t cell_code = 1 << (cell_num+1);      //code to enable selected cell, 00 LSBs reserved




  writereg(AD7280A_DEVADDR_MASTER, ((0x14)+cell_num), 1, timer_sec);

  writereg(AD7280A_DEVADDR_MASTER, 0x14, 1, cell_code);

  Serial.println("cell balance activated on cell");
  Serial.println(cell_num);



}


void AD7280::balance_all(byte cell_num, uint8_t timer_sec)
{
    if ((timer_sec<71)&&(timer_sec>1)) timer_sec=71;			//minimum 71.5 seconds
	timer_sec = ((timer_sec / 71) << 3);		// timer in sec converted in 5 bit binary, 000 LSBs reserved


  byte cell_num_LSB =0x00;
  int i=5;
  while(i>=0) {
    //Serial.println(cell_num>>i, BIN);
//    Serial.println((cell_num>>i)& 0x01, BIN);
    if (((cell_num>>i)&(0x01)) == 1){

    //writereg(AD7280A_DEVADDR_MASTER, (0x1A-i), 1, timer_sec);  //decomment if you need cell balance timers

    cell_num_LSB = cell_num_LSB | (0x01<<(5-i));

    //Serial.println(i);
    }

    i--;


  }
//  Serial.println(cell_num_LSB,BIN);
  writereg(AD7280A_DEVADDR_MASTER, 0x14, 1, cell_num_LSB<<2); //Cell balance

 return 0;

}









bool AD7280::init(int ss)
{
	ADinst.slave_select = ss;
	setup_spi32(ADinst.slave_select);
	Serial.begin(9600);
	//Serial.println("Setup SPI32 for AD7280a chip");
	ADinst.readback_delay_ms = 10; 	// 10 ms wait time.
	ADinst.cell_threshhigh = 0xFF;
	ADinst.aux_threshhigh = 0xFF;
	ADinst.ctrl_hb = 0x00;
	ADinst.ctrl_lb = 0x10; 		// D4 of Ctrl_lb must be 1 ( reserved Bit )
	ADinst.cell_threshlow = 0x00;
	ADinst.aux_threshlow = 0x00;

	int prova;

	build_crc(&ADinst.crc_tab[0]);
	ADinst.slave_num = chain_setup();	//find number of slave devices
	ADinst.scan_cnt = (ADinst.slave_num + 1) * AD7280A_NUM_CH;

	writereg(AD7280A_DEVADDR(1), AD7280A_CELL_BALANCE, 0, 0);	//disable cell balancing
	delay(10);
	writereg(AD7280A_DEVADDR(0), AD7280A_CELL_BALANCE, 0, 0);



  /*int i = 0;
  while(i<256){
	Serial.println(ADinst.crc_tab[i]);      //print crc_table
  i++;
  }
  */





	// chain setup

	// ADinst.scan_cnt = (ADinst.slave_num + 1) * AD7280A_NUM_CH;

	// Serial.println(ADinst.scan_cnt);

	return 0;




}
