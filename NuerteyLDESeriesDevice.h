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

// SPI communication transfers data between the SPI master and 
// registers of the LDE Series ASIC. The LDE Series always operates as a 
// slave device in masterslave operation mode.

// The SPI communication herein implemented follows a Master/Slave 
// paradigm:
// 
// NUCLEO-F767ZI MCU=Master (MOSI output line), LDE=Slave (MISO output line) 

// =====================================================================
// SPI interface pins
//
// Pin    Pin Name                  Communication
//                                  
// CSB    Chip Select (active low)  MCU => LDE
// SCK    Serial Clock              MCU => LDE
// MOSI   Master Out Slave In       MCU => LDE
// MISO   Master In Slave Out       LDE => MCU
// =====================================================================

template <IsLDESeriesSensorType S>
class NuerteyLDESeriesDevice
{        
    static constexpr uint8_t DEFAULT_BYTE_ORDER = 0;  // A value of zero indicates MSB-first.
    
    // \" 
    // External clock frequency 
    //
    //     fECLK (VCKSEL=0) Min. 0.2 MHz     Max. 5 MHz
    // \"
    static constexpr uint32_t DEFAULT_FREQUENCY = 5000000;
    
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

    //void InitiateDataTransfer();
    //SPIFrame_t ReadPressureData or stored as member within class, currentSPIResponse
    double GetPressure() const;
    double GetTemperature() const;
        
    template<typename T>
    double GetTemperature() const;
        
    uint8_t  GetMode() const { return m_Mode; }
    uint8_t  GetByteOrder() const { return m_ByteOrder; }
    uint8_t  GetBitsPerWord() const { return m_BitsPerWord; }
    uint32_t GetFrequency() const { return m_Frequency; };

protected:
    void FullDuplexTransfer(const SPIFrame_t& cBuffer, SPIFrame_t& rBuffer);
    
    double ConvertTemperature(const int16_t& temperature) const;    
    
    template<typename T>
    double ConvertTemperature(const int16_t& temperature) const;
    
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

double NuerteyLDESeriesDevice<S>::GetPressure() const
{
    // Initiate pressure data transfer:
    
}

double NuerteyLDESeriesDevice<S>::GetTemperature() const
{
    
}

template <typename T>
double NuerteyLDESeriesDevice<S>::GetTemperature() const
{
    static_assert((std::is_same_v<T, Celsius_t>
                || std::is_same_v<T, Fahrenheit_t>
                || std::is_same_v<T, Kelvin_t>),
    "Hey! Temperature scale MUST be one of the following types: \
                \n\tCelsius_t\n\tFahrenheit_t \n\tKelvin_t");
                    
    return ConvertTemperature<T>(std::get<2>(std::get<4>(g_TheSensorData)));
}

void NuerteyLDESeriesDevice<S>::FullDuplexTransfer(const SPIFrame_t& cBuffer,
                                                      SPIFrame_t& rBuffer)
{   
    // Do not presume that the users of this OS-abstraction are well-behaved.
    rBuffer.fill(0);

    //DisplayFrame(cBuffer);

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
    
    // Deassert the Slave Select line, releasing exclusive access to the
    // SPI bus. Chip select is active low hence cs = 1 here.  Note that
    // write already internally deselects and mutex unlocks the SPI bus.
    //m_TheSPIBus.deselect();   
    
    if (bytesWritten != cBuffer.size())
    {
        printf("Error! SPI Command Frame - Incorrect number of bytes transmitted\n");
    } 
}


double NuerteyLDESeriesDevice<S>::ConvertTemperature(const int16_t& temperature) const
{
    double result{0.0};
    
    // \" Temperature is converted to °C with following equation:
    // 
    // Temperature [°C] = -273 + (TEMP / 18.9),
    // 
    // where TEMP is temperature sensor output register content in decimal format. \"
    result = static_cast<double>(-273) 
           + (static_cast<double>(temperature) 
            / static_cast<double>(18.9)); // Convert 2's complement to °C. 
    
    return result;
}

template<typename T>
double NuerteyLDESeriesDevice<S>::ConvertTemperature(const int16_t& temperature) const
{
    static_assert((std::is_same_v<T, Celsius_t>
                || std::is_same_v<T, Fahrenheit_t>
                || std::is_same_v<T, Kelvin_t>),
    "Hey! Temperature scale MUST be one of the following types: \
                \n\tCelsius_t\n\tFahrenheit_t \n\tKelvin_t");
                    
    auto result = ConvertTemperature(temperature);
                    
    if constexpr (std::is_same_v<T, Celsius_t>)
    { 
        // noop.
    }
    else if constexpr (std::is_same_v<T, Fahrenheit_t>)
    {
        // Since we must be wary of precision loss, pre-cast the operands:
        result = (result * static_cast<double>(9) 
                / static_cast<double>(5)) 
                + static_cast<double>(32); // Convert °C to °F.
    }
    else if constexpr (std::is_same_v<T, Kelvin_t>)
    {
        // Since we must be wary of precision loss, pre-cast the operands:
        result = result + static_cast<double>(273); // Convert °C to K.
    }
           
    return result;    
}
