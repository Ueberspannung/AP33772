#include <Arduino.h>
#include <Wire.h>
#include "AP33772.h"

						

void AP33772_c::init(uint8_t int_pin)						// set int pin and do initial read
{
	uint8_t i;
	
	this->int_pin=int_pin;
	if (int_pin!=0xFF)
		pinMode(int_pin,INPUT);
		
	set_derating_temperature_threshold_C(DEFAULT_DRT);
	set_overtemperature_threshold_C(DEFAULT_OTP);
	set_interrupt_mask(DEFAULT_MSK);
	set_overcurrent_threshold_mA(DEFAULT_OCP);

	read_voltage_mV();
	read_current_mA();
	read_temperature_C();

}

bool AP33772_c::test_chip(void)								// check if AP33772 is present   
{	//	test_chip
	Wire.beginTransmission(AP33772_I2C_ADD);
	return Wire.endTransmission()==0;
}	//	test_chip

bool AP33772_c::test_int_pin(void)							// check if int request has been set
{	// test_int_pin
	return digitalRead(int_pin);
}	// test_int_pin
	
uint8_t AP33772_c::read_status(void)						// read status register
{	// read_status
	last_status.data=i2c_read_uint8(STATUS_REG);
	return last_status.data;
}	// read_status

uint8_t AP33772_c::get_last_status(void)					// get last status flags
{
	return last_status.data;
}

uint8_t AP33772_c::read_interrupt_mask(void)				// read interrupt mask register
{	// read_interrupt_mask
	return i2c_read_uint8(INT_MASK_REG);
}	// read_interrupt_mask

void AP33772_c::set_interrupt_mask(uint8_t int_mask)		// write interrupt mask register
{	// set_interrupt_mask
	i2c_write(INT_MASK_REG,int_mask);
}	// set_interrupt_mask

void AP33772_c::read_source_pdos(void)						// read advertised PDOs into buffer
{	// read_source_pdos
	uint8_t nCnt;
	uint8_t *pData=(uint8_t*)&source_pdo_data[1];			// pos. 0 is used to store last rdo

	nCnt=4*MAX_SOURCE_PDO;									// read 7 source PDOs
	i2c_read(SRCPDO_REG,4*MAX_SOURCE_PDO);					// all 7 Source PDO are stored consecutively
															// starting ad SRCPOD_REG, last byte is PDO Cnt			
	while (nCnt--) *pData++=Wire.read();

	source_pdo_cnt=i2c_read_uint8(PDONUM_REG);				// read number of source PDOs
	
}	// read_source_pdos

uint8_t AP33772_c::get_pdo_cnt(void)						// get number of advertised PDOs
{	// get_pdo_cnt
	return source_pdo_cnt;
}	// get_pdo_cnt

uint32_t AP33772_c::get_pdo(uint8_t pdo_num)				// get specific PDO, #0 ist used for last RDO
{	// get_pdo
	return source_pdo_data[pdo_num];
}	// get_pdo

void AP33772_c::set_rdo(uint32_t RDO)
{	// set_rdo
	source_pdo_data[RDO_POS]=RDO;
	i2c_write(RDO_REG,RDO);
}	// set_rdo

uint32_t AP33772_c::get_last_rdo(void)
{	// get_last_rdo
	return source_pdo_data[RDO_POS];
}	// get_last_rdo


uint16_t AP33772_c::read_voltage_mV(void)					// read voltage register and return voltage in mV
{	// read_voltage_mV
	temp_data_voltage=i2c_read_uint8(VOLTAGE_REG);
	return get_voltage_measurement_mV();
}	// read_voltage_mV

uint16_t AP33772_c::read_current_mA(void)					// read current register and return current in mA
{	// read_current_mA
	temp_data_current=i2c_read_uint8(CURRENT_REG);
	return get_current_measurement_mA();
}	// read_current_mA

int8_t AP33772_c::read_temperature_C(void)				// read temperature register and return temperatur in °C
{	// read_temperadture_C
	temp_data_temperature=(int8_t)i2c_read_uint8(TEMP_REG);
	return get_temperature_measurement_C();
}	// read_temperadture_C

uint16_t AP33772_c::get_voltage_measurement_mV(void)		// get voltage in mV from temp. storage 
{	// get_voltage_measurement_mV
	return ((uint16_t)temp_data_voltage)*VOLTAGE_SCALE;
}	// get_voltage_measurement_mV

uint16_t AP33772_c::get_current_measurement_mA(void)		// get current in mA from temp. storage
{	// get_current_measurement_mA
	return ((uint16_t)temp_data_current)*CURRENT_SCALE;
}	// get_current_measurement_mA


int8_t AP33772_c::get_temperature_measurement_C(void)		// get current in °C from temp. storage
{	// get_temperature_measurement_C
	return temp_data_temperature;
}	// get_temperature_measurement_C

uint16_t AP33772_c::read_overcurrent_threshold_mA(void)						// read overcurrent threshold reg and convert to mA
{	// read_overcurrent_threshold_mA
	return ((uint16_t)i2c_read_uint8(OCPTHR_REG))*OVERCURRENT_SCALE;
}	// read_overcurrent_threshold_mA

uint8_t AP33772_c::read_overtemperature_threshold(void)						// read overtemperatur setting for helath monitoring
{	// read_overtemperature_threshold
	return i2c_read_uint8(OTPTHR_REG);
}	// read_overtemperature_threshold

uint8_t AP33772_c::read_derating_temperature_threshold(void)				// read derating setting for health monitoring
{	// read_derating_temperature_threshold
	return i2c_read_uint8(DRTHR_REG); 
}	// read_derating_temperature_threshold

uint16_t AP33772_c::read_temperature_curve(AP33772_c::temperature_et temp)	// read temperature curve of ntc
{	// read_temperature_curve
	return i2c_read_uint16(TR25_REG+temp);
}	// read_temperature_curve

void AP33772_c::set_overcurrent_threshold_mA(uint16_t mA)						// set overcurrent threshold
{	// set_overcurrent_threshold_mA
	mA/=OVERCURRENT_SCALE;
	i2c_write(OCPTHR_REG,(uint8_t)mA);
}	// set_overcurrent_threshold_mA

void AP33772_c::set_overtemperature_threshold_C(uint8_t otp)					// set overtemperature threshold
{	// set_overtemperature_threshold_C
	i2c_write(OTPTHR_REG,otp);
}	// set_overtemperature_threshold_C

void AP33772_c::set_derating_temperature_threshold_C(uint8_t drtp)				// set derating threshold
{	// set_derating_temperature_threshold_C
	i2c_write(DRTHR_REG,drtp);
}	// set_derating_temperature_threshold_C

void AP33772_c::set_temperature_curve(AP33772_c::temperature_et temp, uint16_t R)	// set temperature curve of ntc
{	// set_temperature_curve
	i2c_write(TR25_REG+temp,R);
}	// set_temperature_curve

void AP33772_c::reset(void)
{	// reset
	set_rdo(RESET_RDO);
}	// reset						

/*******************************************************************************
 * 			private
 *******************************************************************************/
 

void AP33772_c::i2c_write(uint8_t reg_addr, uint8_t data)
{	// i2c_write uint8 to register
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.write(data);
	Wire.endTransmission();
}	// i2c_write uint8 to register


void AP33772_c::i2c_write(uint8_t reg_addr, uint16_t data)
{	// write uint16 to register
	uint8_t *pData=(uint8_t*)&data;
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.write(*pData++);
	Wire.write(*pData);
	Wire.endTransmission();
}	// write uint16 to register

void AP33772_c:: i2c_write(uint8_t reg_addr, uint32_t data)
{	// write uint32 to register
	uint8_t *pData=(uint8_t*)&data;
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.write(*pData++);
	Wire.write(*pData++);
	Wire.write(*pData++);
	Wire.write(*pData);
	Wire.endTransmission();
}	// write uint32 to register

uint8_t AP33772_c::i2c_read_uint8(uint8_t reg_addr)
{	// read one byte from specified register
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.endTransmission(false);
	Wire.requestFrom(AP33772_I2C_ADD,1);
	return Wire.read();
}	// read one byte from specified register

uint8_t AP33772_c::i2c_read(uint8_t reg_addr, uint8_t cnt)
{	// request cnt bytes from specified register
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.endTransmission(false);
	return Wire.requestFrom(AP33772_I2C_ADD,cnt);	
}	// request cnt bytes from specified register

uint16_t AP33772_c::i2c_read_uint16(uint8_t reg_addr)
{	// request uint16 from specified register
	uint16_t data;
	Wire.beginTransmission(AP33772_I2C_ADD);
	Wire.write(reg_addr);
	Wire.endTransmission(false);
	Wire.requestFrom(AP33772_I2C_ADD,2);
	data=Wire.read();
	*(((uint8_t*)&data)+1)=Wire.read();
	return data;	
}	// request uint16 from specified register
