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

template <IsLDESeriesSensorType S>
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
    //SPIFrame_t ReadPressureData or stored as member within class, currentSPIResponse
    double GetPressure() const;

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
};

NuerteyLDESeriesDevice<S>::NuerteyLDESeriesDevice(PinName mosi,
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

NuerteyLDESeriesDevice<S>::~NuerteyLDESeriesDevice()
{
}

void NuerteyLDESeriesDevice<S>::InitiateDataTransfer()
{
    
}

double NuerteyLDESeriesDevice<S>::GetPressure() const
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
