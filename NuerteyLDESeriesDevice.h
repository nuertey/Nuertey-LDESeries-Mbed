/***********************************************************************
* @file      NuerteyLDESeriesDevice.h
*
*    This sensor driver is targetted for the ARM Mbed platform and 
*    encapsulates the digital SPI interface and accompanying protocol 
*    as presented by the LDE Series of digital low differential pressure
*    sensors. 
* 
*    From its datasheet, the First Sensor LDE Series sensor is 
*    characterized as:
* 
*    "The LDE differential low pressure sensors are based on thermal flow
*    measurement of gas through a micro-flow channel integrated within 
*    the sensor chip. The innovative LDE technology features superior 
*    sensitivity especially for ultra low pressures. The extremely low 
*    gas flow through the sensor ensures high immunity to dust 
*    contamination, humidity and long tubing compared to other flow-based
*    pressure sensors."
* 
* - https://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf
*
* @brief   
* 
* @note  Quoting the above datasheet further:
* 
*    Features
* 
*     – Ultra-low pressure ranges from 25 to 500 Pa (0.1 to 2 in H2O) 
*     – Pressure sensor based on thermal microflow measurement
*     – High flow impedance
*     – very low flow-through leakage
*     – high immunity to dust and humidity
*     – no loss in sensitivity using long tubing
*     – Calibrated and temperature compensated
*     – Unique offset autozeroing feature ensuring superb long-term stability
*     – Offset accuracy better than 0.2% FS
*     – Total accuracy better than 0.5% FS typical
*     – On-chip temperature sensor
*     – Analog output and digital SPI interface
*     – No position sensitivity
* 
*   Certificates
* 
*     – Quality Management System according to EN ISO 13485 and EN ISO 9001
*     – RoHS and REACH compliant
* 
*   Media compatibility
* 
*     – Air and other non-corrosive gases
* 
*   Applications
* 
*     Medical
*     – Ventilators
*     – Spirometers
*     – CPAP
*     – Sleep diagnostic equipment
*     – Nebulizers
*     – Oxygen conservers/concentrators
*     – Insufflators/endoscopy
* 
*     Industrial
*     – HVAC
*     – VAV
*     – Filter monitoring
*     – Burner control
*     – Fuel cells
*     – Gas leak detection
*     – Gas metering
*     – Fume hood
*     – Instrumentation
*     – Security systems
*
* @warning
*
* @author    Nuertey Odzeyem
* 
* @date      November 28, 2021
*
* @copyright Copyright (c) 2021 Nuertey Odzeyem. All Rights Reserved.
***********************************************************************/
#pragma once

// Note that from hence, relevant sections of the 'DS_Standard-LDE_E_11815.pdf'
// are appropriately quoted (\" ... \") as needed. These are intended to
// serve as a sort of Customer requirement repository and to evidence 
// traceability.
//
// https://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf

// \"
// Pressure sensor characteristics
// 
// Part no.     Operating pressure                        Proof pressure (5) Burst pressure (5)
//                                                                          
// LDES025U...  0...25 Pa / 0...0.25 mbar (0.1 in H2O)                       
// LDES050U...  0...50 Pa / 0...0.5 mbar (0.2 in H2O)                        
// LDES100U...  0...100 Pa / 0...1 mbar (0.4 in H2O)                         
// LDES250U...  0...250 Pa / 0...2.5 mbar (1 in H2O)                         
// LDES500U...  0...500 Pa / 0...5 mbar (2 in H2O)        2 bar (30 psi)     5 bar (75 psi)
// LDES025B...  0...±25 Pa / 0...±0.25 mbar (±0.1 in H2O)
// LDES050B...  0...±50 Pa / 0...±0.5 mbar (±0.2 in H2O)
// LDES100B...  0...±100 Pa / 0...±1 mbar (±0.4 in H2O)
// LDES250B...  0...±250 Pa / 0...±2.5 mbar (±1 in H2O)
// LDES500B...  0...±500 Pa / 0...±5 mbar (±2 in H2O)
//
// ...
//
// (5) The max. common mode pressure is 5 bar. \"

// \"
// Gas correction factors (6)
//
// Gas type                Correction factor
//
// Dry air                 1.0
// Oxygen (O2)             1.07
// Nitrogen (N2)           0.97
// Argon (Ar)              0.98
// Carbon dioxide (CO2)    0.56
//
// ...
//
// (6) For example with a LDES500... sensor measuring CO2 gas, at full-scale output
// the actual pressure will be:
//
// ΔPeff = ΔPSensor x gas correction factor = 500 Pa x 0.56 = 280 Pa
//
// ΔPeff = True differential pressure
// ΔP Sensor= Differential pressure as indicated by output signal
// \"

// \"
// LDE...6... Performance characteristics (7)
//
// (VS=5.0 VDC, TA=20 °C, PAbs=1 bara, calibrated in air, analog and 
// digital output signals are non-ratiometric to VS)
//
// 25 Pa and 50 Pa devices
//
// ...
//
// Power-on time 25 ms.
//
// ...
//
// (7) The sensor is calibrated with a common mode pressure of 1 bar absolute. 
// Due to the mass flow based measuring principle, variations in absolute common
// mode pressure need to be compensated according to the following formula:
//
// ΔPeff = ΔPSensor x 1 bara/Pabs
//
// ΔPeff = True differential pressure
// ΔPSensor = Differential pressure as indicated by output voltage
// Pabs = Current absolute common mode pressure
//
// ...
// 
// Digital output
//
// Parameter                                             Typ.  Unit
//
// Scale factor (digital output) (10) 0...25/0...±25 Pa  1200  counts/Pa
//                                    0...50/0...±50 Pa   600  counts/Pa
// ...
//
// (10) The digital output signal is a signed, two complement integer.
// Negative pressures will result in a negative output. \"


// \"
// SPI – Serial Peripheral Interface
//
// Note: it is important to adhere to the communication protocol in 
// order to avoid damage to the sensor.
// \"

#include <system_error>
#include "Protocol.h" 

using namespace Utilities;
using namespace ProtocolDefinitions;

// \"Table 7 describes the DC characteristics of SCL3300-D01 sensor SPI I/O pins. Supply
// voltage is 3.3 V unless otherwise specified. Current flowing into the circuit has a positive
// value.
//
// Table 7 SPI DC Characteristics
// ...
// Serial Clock SCK (Pull Down)
// ...
// Chip Select CSB (Pull Up), low active
// ...
// Serial Data Input MOSI (Pull Down)
// ...
// Serial Data Output MISO (Tri State)
// ...

// \" SPI communication transfers data between the SPI master and 
// registers of the SCL3300 ASIC. The SCL3300 always operates as a 
// slave device in masterslave operation mode. \"

// The SPI communication herein implemented follows a Master/Slave 
// paradigm:
// 
// NUCLEO-F767ZI MCU=Master (MOSI output line), SCL3300=Slave (MISO output line) 

// =====================================================================
// \" Table 12 SPI interface pins
//
// Pin    Pin Name                  Communication
//                                  
// CSB    Chip Select (active low)  MCU => SCL3300
// SCK    Serial Clock              MCU => SCL3300
// MOSI   Master Out Slave In       MCU => SCL3300
// MISO   Master In Slave Out       SCL3300 => MCU
// \"
// =====================================================================

enum class [[nodiscard]] SensorStatus_t : int8_t
{
    SUCCESS                                                  =   0,
    
    // Should never happen due to provision of proactive static assert,
    // ProtocolDefinitions::AssertValidSPICommandFrame<T>(). Still, 
    // trying to be comprehensive. 
    ERROR_INVALID_COMMAND_FRAME                              =  -1, 
                                                                
    ERROR_INCORRECT_NUMBER_OF_BYTES_WRITTEN                  =  -2,
    ERROR_COMMUNICATION_FAILURE_BAD_CHECKSUM                 =  -3,
    ERROR_INVALID_RESPONSE_FRAME                             =  -4,
    ERROR_OPCODE_READ_WRITE_MISMATCH                         =  -5,
    ERROR_RETURN_STATUS_STARTUP_IN_PROGRESS                  =  -6,
    ERROR_RETURN_STATUS_SELF_TEST_RUNNING                    =  -7,
    ERROR_RETURN_STATUS_ERROR_FLAGS_ACTIVE                   =  -8,
    ERROR_STO_SIGNAL_EXCEEDS_THRESHOLD                       =  -9,
    
    // \" Component failure can be suspected if the STO signal exceeds
    // the threshold level continuously after performing component hard
    // reset in static (no vibration) condition. \"
    ERROR_STO_SIGNAL_COMPONENT_FAILURE_DETECTED              = -10,
    
    ERROR_STATUS_REGISTER_PIN_CONTINUITY                     = -11,
    ERROR_STATUS_REGISTER_MODE_CHANGED                       = -12,
    ERROR_STATUS_REGISTER_DEVICE_POWERED_DOWN                = -13,
    ERROR_STATUS_REGISTER_NON_VOLATILE_MEMORY_ERRORED        = -14,
    ERROR_STATUS_REGISTER_SAFE_VOLTAGE_LEVELS_EXCEEDED       = -15,
    ERROR_STATUS_REGISTER_TEMPERATURE_SIGNAL_PATH_SATURATED  = -16,
    ERROR_STATUS_REGISTER_ACCELERATION_SIGNAL_PATH_SATURATED = -17,
    ERROR_STATUS_REGISTER_CLOCK_ERRORED                      = -18,
    ERROR_STATUS_REGISTER_DIGITAL_BLOCK_ERRORED_TYPE_2       = -19,
    ERROR_STATUS_REGISTER_DIGITAL_BLOCK_ERRORED_TYPE_1       = -20   
};

// Register for implicit conversion to error_code:
//
// For the SensorStatus_t enumerators to be usable as error_code constants,
// enable the conversion constructor using the is_error_code_enum type trait:
namespace std
{
    template <>
    struct is_error_code_enum<SensorStatus_t> : std::true_type {};
}

class SCL3300ErrorCategory : public std::error_category
{
public:
    virtual const char* name() const noexcept override;
    virtual std::string message(int ev) const override;
};

const char* SCL3300ErrorCategory::name() const noexcept
{
    return "SCL3300-Sensor-Mbed";
}

std::string SCL3300ErrorCategory::message(int ev) const
{
    switch (ToEnum<SensorStatus_t>(ev))
    {
        case SensorStatus_t::SUCCESS:
            return "Success - no errors";
            
        case SensorStatus_t::ERROR_INVALID_COMMAND_FRAME:
            return "SPI Command Frame invalid or has incorrect frame CRC";

        case SensorStatus_t::ERROR_INCORRECT_NUMBER_OF_BYTES_WRITTEN:
            return "SPI Command Frame - Incorrect number of bytes transmitted";

        case SensorStatus_t::ERROR_COMMUNICATION_FAILURE_BAD_CHECKSUM:
            return "SPI Response Frame checksum failure";

        case SensorStatus_t::ERROR_INVALID_RESPONSE_FRAME:
            return "SPI Response Frame Opcode ADDRESS does NOT match transmitted command frame";

        case SensorStatus_t::ERROR_OPCODE_READ_WRITE_MISMATCH:
            return "SPI Response Frame Opcode READ/WRITE does NOT match transmitted command frame";

        case SensorStatus_t::ERROR_RETURN_STATUS_STARTUP_IN_PROGRESS:
            return "Return Status indicates Startup is in progress";

        case SensorStatus_t::ERROR_RETURN_STATUS_SELF_TEST_RUNNING:
            return "Return Status indicates Self-Test is running";            
            
        case SensorStatus_t::ERROR_RETURN_STATUS_ERROR_FLAGS_ACTIVE:
            return "Error flag (or flags) are active in Status Summary register";
            
        case SensorStatus_t::ERROR_STO_SIGNAL_EXCEEDS_THRESHOLD:
            return "Self-Test Output signal exceeds threshold";

        case SensorStatus_t::ERROR_STO_SIGNAL_COMPONENT_FAILURE_DETECTED:
            return "Component failure - STO signal has exceeded threshold multiple times";

        case SensorStatus_t::ERROR_STATUS_REGISTER_PIN_CONTINUITY:
            return "Component internal connection error";

        case SensorStatus_t::ERROR_STATUS_REGISTER_MODE_CHANGED:
            return "Operation mode changed - If unrequested, SW or HW reset needed";

        case SensorStatus_t::ERROR_STATUS_REGISTER_DEVICE_POWERED_DOWN:
            return "Device in powered down mode - SW or HW reset needed";

        case SensorStatus_t::ERROR_STATUS_REGISTER_NON_VOLATILE_MEMORY_ERRORED:
            return "Error in non-volatile memory - SW or HW reset needed";

        case SensorStatus_t::ERROR_STATUS_REGISTER_SAFE_VOLTAGE_LEVELS_EXCEEDED:
            return "Start-up indication or External voltage levels too extreme - SW or HW reset needed";  
            
        case SensorStatus_t::ERROR_STATUS_REGISTER_TEMPERATURE_SIGNAL_PATH_SATURATED:
            return "Temperature signal path saturated - External temperatures too extreme";

        case SensorStatus_t::ERROR_STATUS_REGISTER_ACCELERATION_SIGNAL_PATH_SATURATED:
            return "Acceleration signal path saturated - Acceleration too high! Readings not usable";

        case SensorStatus_t::ERROR_STATUS_REGISTER_CLOCK_ERRORED:
            return "Clock error - SW or HW reset needed";

        case SensorStatus_t::ERROR_STATUS_REGISTER_DIGITAL_BLOCK_ERRORED_TYPE_2:
            return "Digital block error type 2 - SW or HW reset needed";

        case SensorStatus_t::ERROR_STATUS_REGISTER_DIGITAL_BLOCK_ERRORED_TYPE_1:
            return "Digital block error type 1 - SW or HW reset needed";   
                        
        default:
            return "(unrecognized error)";
    }
}

inline const std::error_category& scl3300_error_category()
{
    static SCL3300ErrorCategory instance;
    return instance;
}

inline auto make_error_code(SensorStatus_t e)
{
    return std::error_code(ToUnderlyingType(e), scl3300_error_category());
}

inline auto make_error_condition(SensorStatus_t e)
{
    return std::error_condition(ToUnderlyingType(e), scl3300_error_category());
}

// =====================================================================
enum class ErrorFlag1Reason_t : uint16_t
{
    SUCCESS_NO_ERROR =     0,
    MEM              =     1,
    AFE_SAT_BIT_1    =     2,
    AFE_SAT_BIT_2    =     4,
    AFE_SAT_BIT_3    =     8,
    AFE_SAT_BIT_4    =    16,
    AFE_SAT_BIT_5    =    32,
    AFE_SAT_BIT_6    =    64,
    AFE_SAT_BIT_7    =   128,
    AFE_SAT_BIT_8    =   256,
    AFE_SAT_BIT_9    =   512,
    AFE_SAT_BIT_10   =  1024,
    ADC_SAT          =  2048,
    RESERVED_1       =  4096,
    RESERVED_2       =  8192,
    RESERVED_3       = 16384,
    RESERVED_4       = 32768
};

using ErrorFlag1ReasonMap_t = std::map<ErrorFlag1Reason_t, std::string>;
using IndexElementFlag1_t   = ErrorFlag1ReasonMap_t::value_type;

inline static auto make_error_flag1_reason_map()
{
    ErrorFlag1ReasonMap_t rMap;
    
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::SUCCESS_NO_ERROR, std::string("\"No errors present\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::MEM, std::string("\"Error in non-volatile memory\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_1, std::string("\"Signal saturated at C2V - Bit 1\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_2, std::string("\"Signal saturated at C2V - Bit 2\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_3, std::string("\"Signal saturated at C2V - Bit 3\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_4, std::string("\"Signal saturated at C2V - Bit 4\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_5, std::string("\"Signal saturated at C2V - Bit 5\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_6, std::string("\"Signal saturated at C2V - Bit 6\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_7, std::string("\"Signal saturated at C2V - Bit 7\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_8, std::string("\"Signal saturated at C2V - Bit 8\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_9, std::string("\"Signal saturated at C2V - Bit 9\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::AFE_SAT_BIT_10, std::string("\"Signal saturated at C2V - Bit 10\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::ADC_SAT, std::string("\"Signal saturated at A2D\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::RESERVED_1, std::string("\"Reserved - Bit 1\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::RESERVED_2, std::string("\"Reserved - Bit 2\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::RESERVED_3, std::string("\"Reserved - Bit 3\"")));
    rMap.insert(IndexElementFlag1_t(ErrorFlag1Reason_t::RESERVED_4, std::string("\"Reserved - Bit 4\"")));

    return rMap;
}

static ErrorFlag1ReasonMap_t gs_ErrorFlag1ReasonMap = make_error_flag1_reason_map();

inline std::string ToString(const ErrorFlag1Reason_t & key)
{
    return (gs_ErrorFlag1ReasonMap.at(key));
}

// =====================================================================
enum class ErrorFlag2Reason_t : uint16_t
{
    SUCCESS_NO_ERROR =     0,
    CLK              =     1,
    TEMP_SAT         =     2,
    APWR_2           =     4,
    VREF             =     8,
    DPWR             =    16,
    APWR             =    32,
    RESERVED_6       =    64,
    MEMORY_CRC       =   128,
    PD               =   256,
    MODE_CHANGE      =   512,
    RESERVED_10      =  1024,
    VDD              =  2048,
    AGND             =  4096,
    A_EXT_C          =  8192,
    D_EXT_C          = 16384,
    RESERVED_15      = 32768
};

using ErrorFlag2ReasonMap_t = std::map<ErrorFlag2Reason_t, std::string>;
using IndexElementFlag2_t   = ErrorFlag2ReasonMap_t::value_type;

inline static auto make_error_flag2_reason_map()
{
    ErrorFlag2ReasonMap_t rMap;
    
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::SUCCESS_NO_ERROR, std::string("\"No errors present\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::CLK, std::string("\"Clock error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::TEMP_SAT, std::string("\"Temperature signal path saturated\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::APWR_2, std::string("\"Analog power error 2\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::VREF, std::string("\"Reference voltage error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::DPWR, std::string("\"Digital power error - SW or HW reset needed\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::APWR, std::string("\"Analog power error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::RESERVED_6, std::string("\"Reserved - Bit 6\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::MEMORY_CRC, std::string("\"Memory CRC check failed\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::PD, std::string("\"Device in power down mode\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::MODE_CHANGE, std::string("\"Operation mode changed by user\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::RESERVED_10, std::string("\"Reserved - Bit 10\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::VDD, std::string("\"Supply voltage error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::AGND, std::string("\"Analog ground connection error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::A_EXT_C, std::string("\"A - External capacitor connection error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::D_EXT_C, std::string("\"D - External capacitor connection error\"")));
    rMap.insert(IndexElementFlag2_t(ErrorFlag2Reason_t::RESERVED_15, std::string("\"Reserved - Bit 15\"")));

    return rMap;
}

static ErrorFlag2ReasonMap_t gs_ErrorFlag2ReasonMap = make_error_flag2_reason_map();

inline std::string ToString(const ErrorFlag2Reason_t & key)
{
    return (gs_ErrorFlag2ReasonMap.at(key));
}

// =====================================================================
enum class CommandRegisterValue_t : uint16_t
{
    MODE_1              =     0,
    MODE_2              =     1,
    MODE_3              =     2,
    MODE_4              =     3,
    PD                  =     4,
    FACTORY_USE_BIT_3   =     8,
    FACTORY_USE_BIT_4   =    16,
    SW_RST              =    32,
    FACTORY_USE_BIT_6   =    64,
    FACTORY_USE_BIT_7   =   128,
    RESERVED_BIT_8      =   256,
    RESERVED_BIT_9      =   512,
    RESERVED_BIT_10     =  1024,
    RESERVED_BIT_11     =  2048,
    RESERVED_BIT_12     =  4096,
    RESERVED_BIT_13     =  8192,
    RESERVED_BIT_14     = 16384,
    RESERVED_BIT_15     = 32768
};

using CommandRegisterValueMap_t = std::map<CommandRegisterValue_t, std::string>;
using IndexElementCommand_t     = CommandRegisterValueMap_t::value_type;

inline static auto make_command_register_value_map()
{
    CommandRegisterValueMap_t cMap;

    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::MODE_1, std::string("\"MODE_1 -> SCL3300 Operation Mode 1\"")));    
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::MODE_2, std::string("\"MODE_2 -> SCL3300 Operation Mode 2\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::MODE_3, std::string("\"MODE_3 -> SCL3300 Operation Mode 3\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::MODE_4, std::string("\"MODE_4 -> SCL3300 Operation Mode 4\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::PD, std::string("\"PD -> Power Down\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::FACTORY_USE_BIT_3, std::string("\"FACTORY_USE -> Factory use - Bit 3\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::FACTORY_USE_BIT_4, std::string("\"FACTORY_USE -> Factory use - Bit 4\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::SW_RST, std::string("\"SW_RST -> Software (SW) Reset\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::FACTORY_USE_BIT_6, std::string("\"FACTORY_USE -> Factory use - Bit 6\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::FACTORY_USE_BIT_7, std::string("\"FACTORY_USE -> Factory use - Bit 7\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_8, std::string("\"RESERVED -> Reserved - Bit 8\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_9, std::string("\"RESERVED -> Reserved - Bit 9\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_10, std::string("\"RESERVED -> Reserved - Bit 10\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_11, std::string("\"RESERVED -> Reserved - Bit 11\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_12, std::string("\"RESERVED -> Reserved - Bit 12\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_13, std::string("\"RESERVED -> Reserved - Bit 13\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_14, std::string("\"RESERVED -> Reserved - Bit 14\"")));
    cMap.insert(IndexElementCommand_t(CommandRegisterValue_t::RESERVED_BIT_15, std::string("\"RESERVED -> Reserved - Bit 15\"")));

    return cMap;
}

static CommandRegisterValueMap_t gs_CommandRegisterValueMap = make_command_register_value_map();

inline std::string ToString(const CommandRegisterValue_t & key)
{
    return (gs_CommandRegisterValueMap.at(key));
}

// =====================================================================

// Metaprogramming types to distinguish sensor temperature scales:
struct Celsius_t {};
struct Fahrenheit_t {};
struct Kelvin_t {};

// \" Sensor ODR in normal operation mode is 2000 Hz. Registers are updated in every
// 0.5 ms and if all data is not read the full noise performance of sensor is not met.
//
// In order to achieve optimal performance, it is recommended that during normal
// operation acceleration outputs ACCX, ACCY, ACCZ are read in every cycle using
// sensor ODR. It is necessary to read STATUS register only if return status (RS) indicates
// error. \"

template<typename T>
using SensorAttribute_t = std::tuple<SPICommandFrame_t, SPICommandFrame_t, T, std::string>;

// Tuple to hold raw sensor data. We must ensure to populate all 
// these each time we read a set of sensor data:
//
//    // Culprit register values of interest. Sourced from 
//    // 'datasheet_scl3300-d01.pdf' section:
//    //
//    // /" 6.1 Sensor Data Block
//    //
//    // Table 18 Sensor data block description \"
//    SensorAttribute_t<int16_t >    m_AccelerationXAxis;
//    SensorAttribute_t<int16_t >    m_AccelerationYAxis;
//    SensorAttribute_t<int16_t >    m_AccelerationZAxis;
//    SensorAttribute_t<int16_t >    m_SelfTestOutput;
//    SensorAttribute_t<int16_t >    m_Temperature;
//    SensorAttribute_t<int16_t >    m_AngleXAxis;
//    SensorAttribute_t<int16_t >    m_AngleYAxis;
//    SensorAttribute_t<int16_t >    m_AngleZAxis;
//    SensorAttribute_t<uint16_t>    m_StatusSummary;
//    SensorAttribute_t<uint16_t>    m_WhoAmI;
using SCL3300SensorData_t = std::tuple<SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<int16_t >,
                                       SensorAttribute_t<uint16_t>,
                                       SensorAttribute_t<uint16_t> >;

// \" 6 Register Definition
//
// SCL3300-D01 contains two user switchable register banks. Default
// register bank is #0.
//
// One should have register bank #0 always active, unless data from
// bank #1 is required. After reading data from bank #1 is finished,
// one should switch back to bank #0 to ensure no accidental 
// read / writes in unwanted registers. See 6.9 SELBANK for more
// information for selecting active register bank. Table 18 shows
// overview of register banks and register addresses. \"

// \" User should not access Reserved nor Factory Use registers.
// Power-cycle, reset and power down mode will reset all written
// settings. \"
SCL3300SensorData_t g_TheSensorData{
    std::make_tuple(SWITCH_TO_BANK_1, READ_ACCELERATION_X_AXIS, 0, std::string("READ_ACCELERATION_X_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_1, READ_ACCELERATION_Y_AXIS, 0, std::string("READ_ACCELERATION_Y_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_1, READ_ACCELERATION_Z_AXIS, 0, std::string("READ_ACCELERATION_Z_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_1, READ_SELF_TEST_OUTPUT,    0, std::string("READ_SELF_TEST_OUTPUT")),
    std::make_tuple(SWITCH_TO_BANK_1, READ_TEMPERATURE,         0, std::string("READ_TEMPERATURE")),
    std::make_tuple(SWITCH_TO_BANK_0, READ_ANGLE_X_AXIS,        0, std::string("READ_ANGLE_X_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_0, READ_ANGLE_Y_AXIS,        0, std::string("READ_ANGLE_Y_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_0, READ_ANGLE_Z_AXIS,        0, std::string("READ_ANGLE_Z_AXIS")),
    std::make_tuple(SWITCH_TO_BANK_1, READ_STATUS_SUMMARY,      0, std::string("READ_STATUS_SUMMARY")),
    std::make_tuple(SWITCH_TO_BANK_0, READ_WHO_AM_I,            0, std::string("READ_WHO_AM_I"))};

class NuerteyLDESeriesDevice
{        
    static constexpr uint8_t DEFAULT_BYTE_ORDER = 0;  // A value of zero indicates MSB-first.
    
    // \" SPI communication may affect the noise level. Used SPI clock 
    // should be carefully validated. Recommended SPI clock is 2 MHz -
    // 4 MHz to achieve the best performance. \"
    static constexpr uint32_t DEFAULT_FREQUENCY = 4000000;

    // \" Figure 7 Timing diagram of SPI communication.
    // 
    // Table 8 SPI AC electrical characteristics.
    // 
    // Symbol Description                                         Min. Unit
    //
    // ...
    // TLH    Time between SPI cycles, CSB at high level (90%)    10   us  \"    
    static constexpr uint8_t  MINIMUM_TIME_BETWEEN_SPI_CYCLES_MICROSECS = 10;
    
public:
    // \" 3-wire SPI connection is not supported. \"
    NuerteyLDESeriesDevice(
        PinName mosi,
        PinName miso,
        PinName sclk,
        PinName ssel,
        const uint8_t& mode = 0,
        const uint8_t& byteOrder = DEFAULT_BYTE_ORDER,
        const uint8_t& bitsPerWord = NUMBER_OF_BITS,
        const uint32_t& frequency = DEFAULT_FREQUENCY);

    NuerteyLDESeriesDevice(const NuerteyLDESeriesDevice&) = delete;
    NuerteyLDESeriesDevice& operator=(const NuerteyLDESeriesDevice&) = delete;

    virtual ~NuerteyLDESeriesDevice();

    void InitiateDataTransfer();

    void LaunchStartupSequence();
    void LaunchNormalOperationSequence();
    
    std::error_code LaunchSelfTestMonitoring();

    void ReadSensorData(auto& item);
    void ReadAllSensorData();

    std::error_code ClearStatusSummaryRegister();

    template <typename T>
    std::error_code ValidateSPIResponseFrame(T& sensorData,
                                const SPICommandFrame_t& commandFrame,
                                const SPICommandFrame_t& responseFrame);
                    
    std::error_code ValidateCRC(const SPICommandFrame_t& frame);
    
    std::error_code FullDuplexTransfer(const SPICommandFrame_t& cBuffer, 
                                             SPICommandFrame_t& rBuffer);
    
    // Gets work on already retrieved SCL3300SensorData_t.
    double GetAccelerationXAxis() const;
    double GetAccelerationYAxis() const;
    double GetAccelerationZAxis() const;
    double GetAngleXAxis() const;
    double GetAngleYAxis() const;
    double GetAngleZAxis() const;
    
    template<typename T>
    double GetTemperature() const;
    
    std::error_code GetSelfTestOutputErrorCode() const;
    std::error_code GetStatusSummaryErrorCode() const;

    // C++20 concepts:    
    template <typename E>
        requires (std::is_same_v<E, ErrorFlag1Reason_t> || std::is_same_v<E, ErrorFlag2Reason_t>)
    void PrintErrorFlagReason(const uint16_t& errorFlag, const E& reason) const;

    // Reads employ SPI to actually retrieve fresh data from the device.    
    std::error_code ReadErrorFlag1Reason(uint16_t& errorFlag,
                                         ErrorFlag1Reason_t& reason);
    std::error_code ReadErrorFlag2Reason(uint16_t& errorFlag, 
                                         ErrorFlag2Reason_t& reason);
    std::error_code ReadSerialNumber(std::string& serialNumber);
    std::error_code ReadCurrentBank(MemoryBank_t& bank);
    
    void PrintCommandRegisterValues(const uint16_t& commandValue) const;
    std::error_code ReadCommandRegister(SixteenBits_t& bitValue);

    template <SPICommandFrame_t V>
    std::error_code SwitchToBank();
    
    void SwitchToBank0();
    void SwitchToBank1();

    template <SPICommandFrame_t V>
    std::error_code WriteCommandOperation();
    
    std::error_code EnableAngleOutputs();
    
    void InitiateResetIfErrorCode(const std::error_code& errorCode);
    void InitiateResetIfErrorFlag2(const ErrorFlag2Reason_t& reason);
    
    void ChangeToMode1();
    void ChangeToMode2();
    void ChangeToMode3();
    void ChangeToMode4();
    void PowerDown();
    void WakeupFromPowerDown();
    void SoftwareReset();
    
    void AssertWhoAmI() const;
    
    uint8_t  GetMode() const { return m_Mode; }
    uint8_t  GetByteOrder() const { return m_ByteOrder; }
    uint8_t  GetBitsPerWord() const { return m_BitsPerWord; }
    uint32_t GetFrequency() const { return m_Frequency; };

protected:
    double ConvertAcceleration(const int16_t& accelaration) const;
    double ConvertAngle(const int16_t& angle) const;
    double ConvertTemperature(const int16_t& temperature) const;    
    
    template<typename T>
    double ConvertTemperature(const int16_t& temperature) const;
    
    std::error_code ConvertStatusSummaryToErrorCode(const uint16_t& status) const;
    std::error_code ConvertSTOToErrorCode(const int16_t& sto) const;
    
    ErrorFlag1Reason_t ConvertErrorFlag1ToReason(const uint16_t& errorFlag) const;
    ErrorFlag2Reason_t ConvertErrorFlag2ToReason(const uint16_t& errorFlag) const;
    
    std::string ComposeSerialNumber(const uint16_t& serial1LSB, 
                                    const uint16_t& serial2MSB) const;
    
private:               
    SPI                                m_TheSPIBus;
    uint8_t                            m_Mode;
    uint8_t                            m_ByteOrder;
    uint8_t                            m_BitsPerWord;
    uint32_t                           m_Frequency;
    OperationMode_t                    m_InclinometerMode;
    bool                               m_PoweredDownMode;
    NucleoF767ZIClock_t::time_point    m_LastSPITransferTime;
};

NuerteyLDESeriesDevice::NuerteyLDESeriesDevice(PinName mosi,
                                           PinName miso,
                                           PinName sclk,
                                           PinName ssel,
                                           const uint8_t& mode,
                                           const uint8_t& byteOrder,
                                           const uint8_t& bitsPerWord,
                                           const uint32_t& frequency)
    // The usual alternate constructor passes the SSEL pin selection to 
    // the target HAL. However, as not all MCU targets support SSEL, that 
    // constructor should NOT be relied upon in portable code. Rather, 
    // use the alternative constructor as per the below. It manipulates 
    // the SSEL pin as a GPIO output using a DigitalOut object. This 
    // should work on any target, and permits the use of select() and 
    // deselect() methods to keep the pin asserted between transfers.
    : m_TheSPIBus(mosi, miso, sclk, ssel, mbed::use_gpio_ssel)
    , m_Mode(mode)
    , m_ByteOrder(byteOrder)
    , m_BitsPerWord(bitsPerWord)
    , m_Frequency(frequency)
    , m_InclinometerMode(OperationMode_t::MODE_1) // \" (default) 1.8g full-scale 40 Hz 1st order low pass filter \"
    , m_PoweredDownMode(false)
    , m_LastSPITransferTime(NucleoF767ZIClock_t::now()) // Just a placeholder for construction/initialization.
{
    // \" The LDE device runs in SPI mode 0, which requires the clock 
    // line SCLK to idle low (CPOL = 0), and for data to be sampled on
    // the leading clock edge (CPHA = 0). \"
    
    // By default, the SPI bus is configured at the Mbed layer with 
    // format set to 8-bits, mode 0, and a clock frequency of 1MHz.

    // /** Configure the data transmission format.
    //  *
    //  *  @param bits Number of bits per SPI frame (4 - 32, target dependent).
    //  *  @param mode Clock polarity and phase mode (0 - 3).
    //  *
    //  * @code
    //  * mode | POL PHA
    //  * -----+--------
    //  *   0  |  0   0
    //  *   1  |  0   1
    //  *   2  |  1   0
    //  *   3  |  1   1
    //  * @endcode
    //  */
    // void format(int bits, int mode = 0);
    // 
    // /** Set the SPI bus clock frequency.
    //  *
    //  *  @param hz Clock frequency in Hz (default = 1MHz).
    //  */
    // void frequency(int hz = 1000000);
    m_TheSPIBus.format(m_BitsPerWord, m_Mode);
    m_TheSPIBus.frequency(m_Frequency);
    
    // /** Set default write data.
    // * SPI requires the master to send some data during a read operation.
    // * Different devices may require different default byte values.
    // * For example: A SD Card requires default bytes to be 0xFF.
    // *
    // * @param data Default character to be transmitted during a read operation.
    // */
    // void set_default_write_value(char data);
    
    set_default_write_value(reinterpret_cast<char>(LDE_SERIES_SPI_DUMMY_BYTE));
}

NuerteyLDESeriesDevice::~NuerteyLDESeriesDevice()
{
}

template <typename T>
std::error_code NuerteyLDESeriesDevice::ValidateSPIResponseFrame(T& sensorData,
                                const SPICommandFrame_t& commandFrame,
                                const SPICommandFrame_t& responseFrame)
{
    std::error_code result{};
    
    result = ValidateCRC(responseFrame);
    if (!result)
    {
        // Prefer C++17 structured bindings over std::tie() and std::ignore.
        // Updated compilers guarantee us the suppression of warnings on
        // the ignored tuple elements.
        auto [commandOpCodeReadWrite, 
              commandOpCodeAddress, 
              ignoredVariable1,
              ignoredVariable2, 
              ignoredVariable3] = Deserialize<T>(commandFrame);        

        auto [receivedOpCodeReadWrite, 
              receivedOpCodeAddress, 
              returnStatusMISO,
              receivedSensorData, 
              ignoredVariable4] = Deserialize<T>(responseFrame);        

        if (returnStatusMISO != ToUnderlyingType(ReturnStatus_t::ERROR))
        {
            if (commandFrame == READ_STATUS_SUMMARY)
            {
                static bool startupIndication = true;
                
                if (returnStatusMISO == ToUnderlyingType(ReturnStatus_t::NORMAL_OPERATION_NO_FLAGS))
                {
                    if (startupIndication)
                    {
                        startupIndication = false;
                        
                        printf("Success! %s: \n\t[%d] -> First response where STATUS has been \
                                cleared. RS bits are indicating proper start-up.\n", 
                            __PRETTY_FUNCTION__,
                            returnStatusMISO);
                    }
                }
                else
                {
                    printf("Warning! Start-up has not been performed correctly.\n");
                }
            }
            
            if (receivedOpCodeAddress == commandOpCodeAddress)
            {
                if (receivedOpCodeReadWrite == commandOpCodeReadWrite)
                {
                    sensorData = receivedSensorData; 
                }
                else
                {
                    result = make_error_code(SensorStatus_t::ERROR_OPCODE_READ_WRITE_MISMATCH);
                }
            }
            else
            {
                result = make_error_code(SensorStatus_t::ERROR_INVALID_RESPONSE_FRAME);
            }
        }        
        else
        {
            if (commandFrame == READ_STATUS_SUMMARY)
            {
                // \" Error flag (or flags) are active in Status Summary register... \"
                // 
                // This is expected to occur during startup hence fake the SensorStatus_t:
                //
                // \" Read STATUS. ‘11’ Clear status summary. Reset status summary \"
                result = make_error_code(SensorStatus_t::ERROR_RETURN_STATUS_STARTUP_IN_PROGRESS);              
            }
            else
            {
                // \" In case of wrong CRC in MOSI write/read, RS bits “11” 
                // are set in the next SPI response, STATUS register is not 
                // changed, and write command must be discarded. \"
                
                // Should never happen due to provision of proactive static
                // assert, ProtocolDefinitions::AssertValidSPICommandFrame<T>().
                // Still, if the sensor responds that it is so, react on it.
                result = make_error_code(SensorStatus_t::ERROR_INVALID_COMMAND_FRAME); 
            }
        }
    }
    
    return result;
}

std::error_code NuerteyLDESeriesDevice::ValidateCRC(const SPICommandFrame_t& frame)
{
    std::error_code result{};
    
    // \" For SPI transmission error detection a Cyclic Redundancy 
    // Check (CRC) is implemented, for details see Table 16. \"
    auto receivedCRC = frame.at(3);
    auto expectedCRC = CalculateCRC(frame);
    
    if (receivedCRC != expectedCRC)
    {
        // \" If CRC in MISO SPI response is incorrect, communication 
        // failure [has] occurred. \"
        result = make_error_code(SensorStatus_t::ERROR_COMMUNICATION_FAILURE_BAD_CHECKSUM);
    }
    
    return result;        
}

std::error_code NuerteyLDESeriesDevice::FullDuplexTransfer(
           const SPICommandFrame_t& cBuffer, SPICommandFrame_t& rBuffer)
{
    std::error_code result{};
    
    // Any benign housekeeping (without any side-effects), can be 
    // accomplished here so that by the time we get to the busy-wait
    // statement below, we are likely guaranteed that we will never 
    // actually busy-wait.
    
    // Do not presume that the users of this OS-abstraction are well-behaved.
    rBuffer.fill(0);

    //DisplayFrame(cBuffer);
    
    if (cBuffer == SWITCH_TO_BANK_0)
    {
        printf("Switching the SCL3300 sensor operations to memory bank 0...\n");
    }
    else if (cBuffer == SWITCH_TO_BANK_1)
    {
        printf("Switching the SCL3300 sensor operations to memory bank 1...\n");       
    }
    
    // Enforce the 10 us SPI transfer interval requirement with my 
    // customized Clock_t:
    auto currentTime = NucleoF767ZIClock_t::now();

    // \"NOTE: For sensor operation, time between consecutive SPI requests (i.e. CSB
    // high) must be at least 10 µs. If less than 10 µs is used, output data will be
    // corrupted. \"
    
    // Note to only escape the <chrono> system with '.count()' only when
    // you absolutely must. Also, note that the statement below 
    // constitutes a busy-wait.
    while (std::chrono::duration_cast<MicroSecs_t>(currentTime - m_LastSPITransferTime).count()
         < MINIMUM_TIME_BETWEEN_SPI_CYCLES_MICROSECS)
    {
    };

    // Assert the Slave Select line, acquiring exclusive access to the
    // SPI bus. Chip select is active low hence cs = 0 here. Note that
    // write already internally mutex locks and selects the SPI bus.
    //m_TheSPIBus.select();
    
    // Write to the SPI Slave and obtain the response.
    //
    // The total number of bytes sent and received will be the maximum
    // of tx_length and rx_length. The bytes written will be padded with
    // the value 0xff. Further note that the number of bytes to either
    // write or read, may be zero, without raising any exceptions.
    std::size_t bytesWritten = m_TheSPIBus.write(reinterpret_cast<const char*>(cBuffer.data()),
                                                 cBuffer.size(),
                                                 reinterpret_cast<char*>(rBuffer.data()), 
                                                 rBuffer.size());

    m_LastSPITransferTime = NucleoF767ZIClock_t::now();
    
    // Deassert the Slave Select line, releasing exclusive access to the
    // SPI bus. Chip select is active low hence cs = 1 here.  Note that
    // write already internally deselects and mutex unlocks the SPI bus.
    //m_TheSPIBus.deselect();   
    
    // We will ignore SPI reception for now, as from our standpoint and 
    // at this particular moment in time, we only care about SPI 
    // transmission success. Reception will be validated elsewhere.
    if (bytesWritten != cBuffer.size())
    {
        result = make_error_code(SensorStatus_t::ERROR_INCORRECT_NUMBER_OF_BYTES_WRITTEN);
    }
    
    return result;    
}

void NuerteyLDESeriesDevice::WakeupFromPowerDown()
{
    printf("Waking up the SCL3300 sensor from PowerDown mode...\n");
    m_PoweredDownMode = false;
    WriteCommandOperation<WAKEUP_FROM_POWERDOWN_MODE>();    
}

void NuerteyLDESeriesDevice::SoftwareReset()
{
    // \"Software (SW) reset is done with SPI operation (see 5.1.4). 
    // Hardware (HW) reset is done by power cycling the sensor. If these
    // do not reset the error, then possible component error has occurred
    // and system needs to be shut down and part returned to supplier. \"
    printf("Software resetting the SCL3300 sensor...\n");
    WriteCommandOperation<SOFTWARE_RESET>();
}

void NuerteyLDESeriesDevice::AssertWhoAmI() const
{ 
    // \" WHOAMI is a 8-bit register for component identification. 
    // Returned value is C1h.
    //
    // Note: as returned value is fixed, this can be used to ensure SPI
    // communication is working correctly. \"
    uint8_t retrievedValue = (std::get<2>(std::get<9>(g_TheSensorData)) & 0xFF);
    
    assert(((void)"WHOAMI component identification incorrect! SPI \
                   communication must NOT be working correctly!", 
        (retrievedValue == WHO_AM_I)));
}
