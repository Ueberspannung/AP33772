#ifndef __pdo_def_h__
#define __pdo_def_h__

/************************************************************************************
 * Bitfield structures according to USB PD Spec 3.1 
 * chapter 6.4.1 
 * chapter 6.4.2 Request Message
 ************************************************************************************/

enum PD_power_data_obj_type_t {   /* Power data object type */
    PD_PDO_TYPE_FIXED_SUPPLY    = 0,
    PD_PDO_TYPE_BATTERY         = 1,
    PD_PDO_TYPE_VARIABLE_SUPPLY = 2,
    PD_PDO_TYPE_AUGMENTED_PDO   = 3     /* USB PD 3.0 */
};
 

typedef struct RDO_DATA_S {
  union {
    struct {
      uint32_t maxCurrent			: 10;	//unit: 10mA
      uint32_t opCurrent			: 10;   //unit: 10mA
      uint32_t reserved				: 2;
      uint32_t eprModeCapable		: 1;
      uint32_t unchunkedMessages	: 1;
      uint32_t noUsbSuspend			: 1;
      uint32_t usbCommCapable		: 1;
      uint32_t capabilityMismatch	: 1;
      uint32_t giveBackFalg			: 1;
      uint32_t objPosition			: 4;
    } fixedRDO;
    struct {
      uint32_t minimumPower			: 10;	//unit: 250mW
      uint32_t operatingPower		: 10;	//unit: 250mW
      uint32_t reserved				: 2;
      uint32_t eprModeCapable		: 1;
      uint32_t unchunkedMessages	: 1;
      uint32_t noUsbSuspend			: 1;
      uint32_t usbCommCapable		: 1;
      uint32_t capabilityMismatch	: 1;
      uint32_t giveBackFalg			: 1;
      uint32_t objPosition			: 4;
    } batteryRDO;
    struct {
      uint32_t operatingCurrent		: 7;	//unit: 50mA
      uint32_t reserved_1			: 2;
      uint32_t outputVoltage		: 12;	//unit: 25mv with 2 LSB set to 00 -> effective 100mV
      uint32_t reserved_2			: 1;
      uint32_t eprModeCapable		: 1;
      uint32_t unchunkedMessages	: 1;
      uint32_t noUsbSuspend			: 1;
      uint32_t usbCommCapable		: 1;
      uint32_t capabilityMismatch	: 1;
      uint32_t reserved_3			: 1;
      uint32_t objPosition			: 4;
    } avsRDO; 								// adjusable voltage supply
    struct {
      uint32_t opCurrent			: 7;	//unit: 50mA
      uint32_t reserved_1			: 2;
      uint32_t outputVoltage		: 12;	//unit: 20mV
      uint32_t reserved_2			: 1;
      uint32_t eprModeCapable		: 1;
      uint32_t unchunkedMessages	: 1;
      uint32_t noUsbSuspend			: 1;
      uint32_t usbCommCapable		: 1;
      uint32_t capabilityMismatch	: 1;
      uint32_t giveBackFalg			: 1;
      uint32_t objPosition			: 4;
    } ppsARDO;								// programmable power supply (augmented RDO)
    struct {
      uint32_t data					: 28;
      uint32_t objPosition			: 4;
	} RDO;
    struct {
      uint8_t byte0;
      uint8_t byte1;
      uint8_t byte2;
      uint8_t byte3;
    };
	uint32_t data;
  };
} RDO_DATA_T;

typedef struct {
  union {
    struct {
		uint32_t maxCurrent			: 10;	// unit: 10mA
		uint32_t voltage			: 10;	// unit: 50mV
		uint32_t peakCurrent		: 2;	// 0b00: default, 0b01: 110%-150%, 0b10: 125%-200%, 0b11: 150%-200% 
		uint32_t reserved			: 1;
		uint32_t eprModeCapable		: 1;
		uint32_t unchunkedMessages	: 1;
		uint32_t dualRoleData		: 1;
		uint32_t usbCommCapable		: 1;	
		uint32_t unconstrainedPower	: 1;
		uint32_t usbSuspendSupported: 1;
		uint32_t dualRoleCapable	: 1;
		uint32_t pdoType			: 2;	// 0b00: fixed pdo
    } fixedPDO;
    struct {
		uint32_t maxPower			: 10; 	// unit: 250mW
		uint32_t minVoltage			: 10;	// unit: 50mV
		uint32_t maxVoltage			: 10;	// unit: 50mV
		uint32_t pdo_type			: 2;
	} batteryPDO;
    struct {
		uint32_t maxCurrent			: 10; 	// unit: 10mA
		uint32_t minVoltage			: 10;	// unit: 50mV
		uint32_t maxVoltage			: 10;	// unit: 50mV
		uint32_t pdo_type			: 2;
	} variableSupplyPDO;
    struct {
		uint32_t maxCurrent			: 7;     // unit: 50mA
		uint32_t reserved_1			: 1;
		uint32_t minVoltage			: 8;     // unit: 100mV
		uint32_t reserved_2			: 1;
		uint32_t maxVoltage			: 8;     // unit: 100mV
		uint32_t reserved_3			: 2;
		uint32_t ppsPowerLimited	: 1;
		uint32_t powerRange			: 2;	// 0b00: SPR, 0b01: EPR
		uint32_t pdoType			: 2;	// 0b11: pps augmented pdo
    } ppsAPDO;
    struct {
		uint32_t pdp				: 8;	// unit: 1W
		uint32_t minVoltage			: 8;	// unit: 100mV
		uint32_t reserved			: 1;
		uint32_t maxVoltage			: 9;	// unit: 100mV
		uint32_t peakCurrent		: 1;
		uint32_t powerRange			: 2;	// 0b00: SPR, 0b01: EPR
		uint32_t pdoType			: 2;	// 0b11: pps augmented pdo
    } eprAPDO;
    struct {
		uint32_t data				: 28;
		uint32_t powerRange			: 2;
		uint32_t pdoType			: 2;
	} PDO;
    struct {
      uint8_t byte0;
      uint8_t byte1;
      uint8_t byte2;
      uint8_t byte3;
    };
    uint32_t data;
  };
} PDO_DATA_T;



#endif // __pdo_def_h__