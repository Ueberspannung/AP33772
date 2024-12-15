#ifndef __AP33772_H_
#define __AP33772_H_

#include <stdint.h>

/**********************************************************************************
 * 				AP33772 USB PD SINK CONTROLLER (Diodes Incorporeated)
 **********************************************************************************
 * Register Acces Class
 * 
 * Optimized for little endian machines
 * 
 * PDOs / RDOs are represented aus 32 bit unsigned integer
 * PDO # 0 is used to store th last RDO 
 * 
 * Current Measurement ist only supported with default shunt (R010), the scale is
 * fixed to 24 mA 
 * 
 * Author 			Ueberspannung
 * Inital Version	24/05/10
 * Current Version
 *********************************************************************************/

class AP33772_c 
{
	public:
		static const uint8_t DERATING_INT_FLAG			=	0x80;	
		static const uint8_t OVERTEMPERATURE_INT_FLAG	=	0x40;
		static const uint8_t OVERCURRENT_INT_FLAG		=	0x20;
		static const uint8_t OVERVOLTAGE_INT_FLAG		=	0x10;
		static const uint8_t NEWPDO_INT_FLAG			=	0x04;
		static const uint8_t SUCCES_INT_FLAG			=	0x02;
		static const uint8_t READY_INT_FLAG				=	0x01;

		typedef enum:uint8_t { R25, R50, R75, R100 } temperature_et;

		typedef struct status_flags_s { uint8_t	READY 	:1;
										uint8_t	SUCCESS	:1;
										uint8_t	NEWPDO	:1;
										uint8_t	Reserved:1;
										uint8_t	OVP		:1;
										uint8_t	OCP		:1;
										uint8_t	OTP		:1;
										uint8_t	DR		:1;
										} status_flags_st;
		typedef union { status_flags_st flags;
						uint8_t			data;
					} status_t;

	
		AP33772_c(void):int_pin(0xFF),
						source_pdo_cnt(0),
						source_pdo_data{0,0,0,0,0,0,0,0},
						temp_data_voltage(0),
						temp_data_current(0),
						temp_data_temperature(0) {last_status.data=0;}

	
		void init(uint8_t int_pin);						// set int pin and do initial read
		
		bool test_chip(void);							// check if AP33772 is present   
		bool test_int_pin(void);						// check if int request has been set
			
		uint8_t read_status(void);						// read status register
		uint8_t get_last_status(void);					// get last status flags
		
		uint8_t read_interrupt_mask(void);				// read interrupt mask register
		void set_interrupt_mask(uint8_t int_mask);		// write interrupt mask register

		void read_source_pdos(void);					// read advertised PDOs into buffer
		uint8_t get_pdo_cnt(void);						// get number of advertised PDOs
		uint32_t get_pdo(uint8_t pdo_num);				// get specific PDO, #0 ist used for last RDO
	
		void set_rdo(uint32_t RDO);						// set RDO -> request pdo
		uint32_t get_last_rdo(void);                    // get last requested data

		uint16_t read_voltage_mV(void);					// read voltage register and return voltage in mV
		uint16_t read_current_mA(void);					// read current register and return current in mA
		int8_t read_temperature_C(void);				// read temperature register and return temperatur in °C
		
		uint16_t get_voltage_measurement_mV(void);		// get voltage in mV from temp. storage 
		uint16_t get_current_measurement_mA(void);		// get current in mA from temp. storage
		int8_t get_temperature_measurement_C(void);		// get current in °C from temp. storage

		
		uint16_t read_overcurrent_threshold_mA(void);			// read overcurrent threshold reg and convert to mA
		uint8_t read_overtemperature_threshold(void);			// read overtemperatur setting for helath monitoring
		uint8_t read_derating_temperature_threshold(void);		// read derating setting for health monitoring
		uint16_t read_temperature_curve(temperature_et temp);	// read temperature curve of ntc

		void set_overcurrent_threshold_mA(uint16_t mA);					// set overcurrent threshold
		void set_overtemperature_threshold_C(uint8_t otp);				// set overtemperature threshold
		void set_derating_temperature_threshold_C(uint8_t drtp);		// set derating threshold
		void set_temperature_curve(temperature_et temp, uint16_t R);	// set temperature curve of ntc
		
		void reset(void);								
	private:
	
		static const uint8_t	AP33772_I2C_ADD = 0x51;
		
		static const uint8_t	SRCPDO_REG	= 	0x00;	// Source PDO register, 28 bytes (7x4 bytes)
		static const uint8_t	PDONUM_REG 	=	0x1C;	// Number of valid Source PDOs
		static const uint8_t    STATUS_REG	=	0x1D;	// Status bits
		static const uint8_t    INT_MASK_REG=	0x1E;	// Interrupt Mask Register
		static const uint8_t    VOLTAGE_REG	=	0x20;	// measured Voltage	LSB 80mV
		static const uint8_t    CURRENT_REG =	0x21;	// measured Current	LSB 24mA @ 10mOhm
		static const uint8_t    TEMP_REG	=	0x22;	// measured Temperature (NTC) in °C
		static const uint8_t	OCPTHR_REG	=	0x23;	// over current protection threshold in °C
		static const uint8_t	OTPTHR_REG	=	0x24;	// over temperature protection threshold in °C
		static const uint8_t	DRTHR_REG	=	0x25;	// derating temperature threshold in °C
		static const uint8_t	TR25_REG	=	0x28;	// NTC resistance @ 25°C, 16bit in Ohms
		static const uint8_t	TR50_REG	=	0x2A;	// NTC resistance @ 50°C, 16bit in Ohms
		static const uint8_t	TR75_REG	=	0x2C;	// NTC resistance @ 75°C, 16bit in Ohms
		static const uint8_t	TR100_REG	=	0x2E;	// NTC resistance @ 100°C, 16bit in Ohms
		static const uint8_t    RDO_REG		=	0x30;	// Request Data object 
		
		static const uint8_t	MAX_SOURCE_PDO	= 7;	// max. number of SPR Profiles
		static const uint8_t	VOLTAGE_SCALE	= 80;	// 80 mV per voltage digit
		static const uint8_t	CURRENT_SCALE	= 24;	// 24 mA per current digit
		static const uint8_t	OVERCURRENT_SCALE= 50;	// 50 mA per OCP digit (trigger = OCP * 110%)

		static const uint16_t	DEFAULT_OCP	= 12750;	// disable ocp, 255*50mA	
		static const uint8_t	DEFAULT_OTP	=	127;	// OTP disabled
		static const uint8_t	DEFAULT_DRT	=	127;	// DRT disabled
		static const uint8_t	DEFAULT_MSK =	READY_INT_FLAG;	// default: only Ready Int
		static const uint8_t	RDO_POS		=	0;		// pos #0 in source pdo data is last rdo
		static const uint32_t	RESET_RDO	= 	0;		// to reset chip request zero rdo
		
		uint8_t 	int_pin;
		uint8_t		source_pdo_cnt;
		uint32_t	source_pdo_data[MAX_SOURCE_PDO+1];
		status_t	last_status;
		uint8_t		temp_data_voltage;
		uint8_t		temp_data_current;
		int8_t		temp_data_temperature; 
		
		void i2c_write(uint8_t reg_addr, uint8_t data);
		void i2c_write(uint8_t reg_addr, uint16_t data);
		void i2c_write(uint8_t reg_addr, uint32_t data);
		uint8_t i2c_read_uint8(uint8_t reg_addr);
		uint16_t i2c_read_uint16(uint8_t reg_addr);
		uint8_t i2c_read(uint8_t reg_addr, uint8_t cnt);
		
};

#endif // __AP33772_H_











