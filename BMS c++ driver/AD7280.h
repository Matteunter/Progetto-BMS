
//
//		AD7280.h
//
//
//



// Class to decode AD7280 information using spi functions


#include <stdint.h>


// REGISTER DEFINITIONS AND CONSTANTS
#ifndef REGISTERS

#define AD7280A_ACQ_TIME_400ns			0
#define AD7280A_ACQ_TIME_800ns			1
#define AD7280A_ACQ_TIME_1200ns			2
#define AD7280A_ACQ_TIME_1600ns			3

#define AD7280A_CONV_AVG_DIS			0
#define AD7280A_CONV_AVG_2			1
#define AD7280A_CONV_AVG_4			2
#define AD7280A_CONV_AVG_8			3

#define AD7280A_ALERT_REMOVE_VIN5		(1 << 2)
#define AD7280A_ALERT_REMOVE_VIN4_VIN5		(2 << 2)
#define AD7280A_ALERT_REMOVE_AUX5		(1 << 0)
#define AD7280A_ALERT_REMOVE_AUX4_AUX5		(2 << 0)


#define AD7280A_CELL_VOLTAGE_1		0x0  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_2		0x1  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_3		0x2  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_4		0x3  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_5		0x4  /* D11 to D0, Read only */
#define AD7280A_CELL_VOLTAGE_6		0x5  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_1		0x6  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_2		0x7  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_3		0x8  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_4		0x9  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_5		0xA  /* D11 to D0, Read only */
#define AD7280A_AUX_ADC_6		0xB  /* D11 to D0, Read only */
#define AD7280A_SELF_TEST		0xC  /* D11 to D0, Read only */
#define AD7280A_CONTROL_HB		0xD  /* D15 to D8, Read/write */
#define AD7280A_CONTROL_LB		0xE  /* D7 to D0, Read/write */
#define AD7280A_CELL_OVERVOLTAGE	0xF  /* D7 to D0, Read/write */
#define AD7280A_CELL_UNDERVOLTAGE	0x10 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_OVERVOLTAGE	0x11 /* D7 to D0, Read/write */
#define AD7280A_AUX_ADC_UNDERVOLTAGE	0x12 /* D7 to D0, Read/write */
#define AD7280A_ALERT			0x13 /* D7 to D0, Read/write */
#define AD7280A_CELL_BALANCE		0x14 /* D7 to D0, Read/write */



#define AD7280A_CB1_TIMER		0x15 /* D7 to D0, Read/write */
#define AD7280A_CB2_TIMER		0x16 /* D7 to D0, Read/write */
#define AD7280A_CB3_TIMER		0x17 /* D7 to D0, Read/write */
#define AD7280A_CB4_TIMER		0x18 /* D7 to D0, Read/write */
#define AD7280A_CB5_TIMER		0x19 /* D7 to D0, Read/write */
#define AD7280A_CB6_TIMER		0x1A /* D7 to D0, Read/write */
#define AD7280A_PD_TIMER		0x1B /* D7 to D0, Read/write */
#define AD7280A_READ			0x1C /* D7 to D0, Read/write */
#define AD7280A_CNVST_CONTROL		0x1D /* D7 to D0, Read/write */

/* Bits and Masks */
#define AD7280A_CTRL_HB_CONV_INPUT_ALL			(0 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL_AUX1_3_4	(1 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_6CELL		(2 << 6)
#define AD7280A_CTRL_HB_CONV_INPUT_SELF_TEST		(3 << 6)
#define AD7280A_CTRL_HB_CONV_RES_READ_ALL		(0 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL_AUX1_3_4	(1 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_6CELL		(2 << 4)
#define AD7280A_CTRL_HB_CONV_RES_READ_NO		(3 << 4)
#define AD7280A_CTRL_HB_CONV_START_CNVST		(0 << 3)
#define AD7280A_CTRL_HB_CONV_START_CS			(1 << 3)
#define AD7280A_CTRL_HB_CONV_AVG_DIS			(0 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_2			(1 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_4			(2 << 1)
#define AD7280A_CTRL_HB_CONV_AVG_8			(3 << 1)
#define AD7280A_CTRL_HB_CONV_AVG(x)			((x) << 1)
#define AD7280A_CTRL_HB_PWRDN_SW			(1 << 0)

#define AD7280A_CTRL_LB_SWRST				(1 << 7)
#define AD7280A_CTRL_LB_ACQ_TIME_400ns			(0 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_800ns			(1 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1200ns			(2 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME_1600ns			(3 << 5)
#define AD7280A_CTRL_LB_ACQ_TIME(x)			((x) << 5)
#define AD7280A_CTRL_LB_MUST_SET			(1 << 4)
#define AD7280A_CTRL_LB_THERMISTOR_EN			(1 << 3)
#define AD7280A_CTRL_LB_LOCK_DEV_ADDR			(1 << 2)
#define AD7280A_CTRL_LB_INC_DEV_ADDR			(1 << 1)
#define AD7280A_CTRL_LB_DAISY_CHAIN_RB_EN		(1 << 0)

#define AD7280A_ALERT_GEN_STATIC_HIGH			(1 << 6)
#define AD7280A_ALERT_RELAY_SIG_CHAIN_DOWN		(3 << 6)

#define AD7280A_ALL_CELLS				(0x00AD << 16)

#define AD7280A_MAX_SPI_CLK_Hz		700000 /* < 1MHz */
#define AD7280A_MAX_CHAIN		8
#define AD7280A_CELLS_PER_DEV		6
#define AD7280A_BITS			12
#define AD7280A_NUM_CH			(AD7280A_AUX_ADC_6 - \
					AD7280A_CELL_VOLTAGE_1 + 1)

#define AD7280A_DEVADDR_MASTER		0
#define AD7280A_DEVADDR_ALL		0x1F
/* 5-bit device address is sent LSB first */
#define AD7280A_DEVADDR(addr)	(((addr & 0x1) << 4) | ((addr & 0x2) << 3) | \
				(addr & 0x4) | ((addr & 0x8) >> 3) | \
				((addr & 0x10) >> 4))
#define AD7280A_READ_TXVAL	0xF800030A

#define POLYNOM		0x2F
#define POLYNOM_ORDER	8
#define HIGHBIT		1 << (POLYNOM_ORDER - 1);

#endif

//TYPES

struct ad7280_state {
	int16_t			slave_num;
	int16_t			scan_cnt;
	uint8_t			readback_delay_ms;
	uint8_t			crc_tab[256];
	uint8_t			ctrl_hb;
	uint8_t			ctrl_lb;
	uint8_t			cell_threshhigh;
	uint8_t			cell_threshlow;
	uint8_t			aux_threshhigh;
	uint8_t			aux_threshlow;
	uint8_t			cb_mask[AD7280A_MAX_CHAIN];
	int			slave_select;
	struct spi_device *spi;
};

struct spi_device {
	int8_t spi;
};



//CLASS AD7280


class AD7280 {
	public:

		ad7280_state ADinst;						// current Ad state structure

		AD7280();			//constructor member
		
		bool init(int ss);			//initial setup
		
		int8_t writereg(uint32_t devaddr, uint32_t addr, uint8_t all, uint32_t val);		// to write an Ad's register
		
    		int8_t read32(uint32_t *val);								// to read an Ad's response to a previous operation

    		int8_t readreg(uint32_t devaddr, uint32_t addr);					// to read an Ad's register

		uint16_t read_channel(uint32_t devaddr, uint32_t addr);				// to read an Analog channel starting Convertion

		uint32_t read_all(uint32_t cnt, uint16_t *array);					// to read all channels starting Convertion


    void cell_balance_enable(uint8_t cell_num, uint8_t timer_sec);


	//private:
		
		
	 	void build_crc(uint8_t *crc_tab);				// to build crc table	

		uint8_t calc_crc(uint8_t *crc_tab, uint32_t val);		// to calculate the crc of a command

		int8_t check_crc(uint32_t val);					// to check if the crc of a received command is correct

		int8_t chain_setup();						// to enable Daisy Chain

		
		void wait();

   		
		
		
		
 		


};










