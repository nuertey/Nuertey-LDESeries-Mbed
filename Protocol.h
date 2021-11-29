/***********************************************************************
* @file      Protocol.h
*
*    Type-driven SPI framing protocol for the First Sensor LDE Series – 
*    digital low differential pressure sensors.
* 
*    For ease of use, power, flexibility and readability of the code,  
*    the protocol has been written in a modern C++ (C++20) 
*    metaprogrammed and templatized class-encapsulated idiom. 
* 
*    A goal of the design is to encourage and promote zero-cost 
*    abstractions, even and especially so, in the embedded realms. Note
*    that care has been taken to deduce and glean these abstractions 
*    from the problem-domain itself. And in the doing so, care has also 
*    been taken to shape the abstractions logically.
* 
* @brief   
* 
* @note    
*
* @warning  
*
* @author  Nuertey Odzeyem
* 
* @date    November 28, 2021
*
* @copyright Copyright (c) 2021 Nuertey Odzeyem. All Rights Reserved.
***********************************************************************/
#pragma once

/// Note that from hence, relevant sections of the 'DS_Standard-LDE_E_11815.pdf'
// are appropriately quoted (\" ... \") as needed. These are intended to
// serve as a sort of Customer requirement repository and to evidence 
// traceability.
//
// https://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf

#include <span> // std::span (C++20) is a safer alternative to separated pointer/size.
#include <bitset>
#include <cmath>

#include "Utilities.h"

// Metaprogramming types to distinguish particular LDE series 
// pressure sensor incarnation:
struct LDE_S025_U_t {};
struct LDE_S050_U_t {};
struct LDE_S100_U_t {};
struct LDE_S250_U_t {}; // Example, LDES250UF6S. const int SCALE_FACTOR = 120;
struct LDE_S500_U_t {};
struct LDE_S025_B_t {};
struct LDE_S050_B_t {};
struct LDE_S100_B_t {};
struct LDE_S250_B_t {};
struct LDE_S500_B_t {};

// A concept is a named set of requirements. The definition of a 
// concept must appear at namespace scope. 
//
// The intent of concepts is to model semantic categories (Number,
// Range, RegularFunction) rather than syntactic restrictions
// (HasPlus, Array). According to ISO C++ core guideline T.20,
// "The ability to specify meaningful semantics is a defining
// characteristic of a true concept, as opposed to a syntactic
// constraint."
template<typename S>
concept IsLDESeriesSensorType = (std::is_same_v<S, LDE_S025_U_t>
                              || std::is_same_v<S, LDE_S050_U_t>
                              || std::is_same_v<S, LDE_S100_U_t>
                              || std::is_same_v<S, LDE_S250_U_t>
                              || std::is_same_v<S, LDE_S500_U_t>
                              || std::is_same_v<S, LDE_S025_B_t>
                              || std::is_same_v<S, LDE_S050_B_t>
                              || std::is_same_v<S, LDE_S100_B_t>
                              || std::is_same_v<S, LDE_S250_B_t>
                              || std::is_same_v<S, LDE_S500_B_t>);

// \" The LDE serial interface is a high-speed synchronous data input 
// and output communication port. The serial interface operates using 
// a standard 4-wire SPI bus. \"
constexpr size_t NUMBER_OF_BITS = 8;

// Convenience aliases:
using EightBits_t       = std::bitset<NUMBER_OF_BITS>;
using SixteenBits_t     = std::bitset<16>;

namespace ProtocolDefinitions
{    
    enum class MemoryBank_t : uint16_t
    {
        BANK_0 = 0,
        BANK_1 = 1 
    };
    
    constexpr uint8_t LDE_SERIES_SPI_DUMMY_BYTE{0xFF}; // ??? 0x00?

    // Another benefit of such an approach is, our ScalingFactorMap is 
    // statically generated at compile-time hence useable in constexpr
    // contexts.    
    template <typename S>
    struct ScalingFactorMap { static constexpr double VALUE; };
    
    template <typename S>
    constexpr double ScalingFactorMap<S>::VALUE = 0.0; // Default static object initialization.
    
    // Partial template specializations mimics core 'map' functionality.
    // Good and quiet thought makes programming fun and creative :).
    template <>
    constexpr double ScalingFactorMap<LDE_S025_U_t>::VALUE = 1200.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S050_U_t>::VALUE =  600.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S100_U_t>::VALUE =  300.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S250_U_t>::VALUE =  120.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S500_U_t>::VALUE =   60.0;
    
    template <>
    constexpr double ScalingFactorMap<LDE_S025_B_t>::VALUE = 1200.0;

    template <>
    constexpr double ScalingFactorMap<LDE_S050_B_t>::VALUE =  600.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S100_B_t>::VALUE =  300.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S250_B_t>::VALUE =  120.0;
                                                              
    template <>                                               
    constexpr double ScalingFactorMap<LDE_S500_B_t>::VALUE =   60.0;
            
    // Concept usage within a constexpr conditional statement:                      
    template <typename S>
        requires IsLDESeriesSensorType<S>
    constexpr auto GetScalingFactor()
    {
        constexpr auto LDE_SERIES_SCALING_FACTOR = ScalingFactorMap<S>::VALUE;        
        return LDE_SERIES_SCALING_FACTOR;
    }
    
    // \" 8-bit register for component identification
    //
    // Component ID [7:0] = C1h \"
    constexpr uint8_t WHO_AM_I{0xC1};
    
    constexpr SPICommandFrame_t READ_ACCELERATION_X_AXIS  {0x04, 0x00, 0x00, 0xF7};
    constexpr SPICommandFrame_t READ_ACCELERATION_Y_AXIS  {0x08, 0x00, 0x00, 0xFD};
    constexpr SPICommandFrame_t READ_ACCELERATION_Z_AXIS  {0x0C, 0x00, 0x00, 0xFB};
    constexpr SPICommandFrame_t READ_SELF_TEST_OUTPUT     {0x10, 0x00, 0x00, 0xE9};
    constexpr SPICommandFrame_t ENABLE_ANGLE_OUTPUTS      {0xB0, 0x00, 0x1F, 0x6F};
    constexpr SPICommandFrame_t READ_ANGLE_X_AXIS         {0x24, 0x00, 0x00, 0xC7};
    constexpr SPICommandFrame_t READ_ANGLE_Y_AXIS         {0x28, 0x00, 0x00, 0xCD};
    constexpr SPICommandFrame_t READ_ANGLE_Z_AXIS         {0x2C, 0x00, 0x00, 0xCB};
    constexpr SPICommandFrame_t READ_TEMPERATURE          {0x14, 0x00, 0x00, 0xEF};
    constexpr SPICommandFrame_t READ_STATUS_SUMMARY       {0x18, 0x00, 0x00, 0xE5};
    constexpr SPICommandFrame_t READ_ERROR_FLAG_1         {0x1C, 0x00, 0x00, 0xE3};
    constexpr SPICommandFrame_t READ_ERROR_FLAG_2         {0x20, 0x00, 0x00, 0xC1};
    constexpr SPICommandFrame_t READ_COMMAND              {0x34, 0x00, 0x00, 0xDF};
    constexpr SPICommandFrame_t CHANGE_TO_MODE_1          {0xB4, 0x00, 0x00, 0x1F};
    constexpr SPICommandFrame_t CHANGE_TO_MODE_2          {0xB4, 0x00, 0x01, 0x02};
    constexpr SPICommandFrame_t CHANGE_TO_MODE_3          {0xB4, 0x00, 0x02, 0x25};
    constexpr SPICommandFrame_t CHANGE_TO_MODE_4          {0xB4, 0x00, 0x03, 0x38};
    constexpr SPICommandFrame_t SET_POWERDOWN_MODE        {0xB4, 0x00, 0x04, 0x6B};
    constexpr SPICommandFrame_t WAKEUP_FROM_POWERDOWN_MODE{0xB4, 0x00, 0x00, 0x1F};
    constexpr SPICommandFrame_t SOFTWARE_RESET            {0xB4, 0x00, 0x20, 0x98};
    constexpr SPICommandFrame_t READ_WHO_AM_I             {0x40, 0x00, 0x00, 0x91};
    constexpr SPICommandFrame_t READ_SERIAL_1             {0x64, 0x00, 0x00, 0xA7};
    constexpr SPICommandFrame_t READ_SERIAL_2             {0x68, 0x00, 0x00, 0xAD};
    constexpr SPICommandFrame_t READ_CURRENT_BANK         {0x7C, 0x00, 0x00, 0xB3};
    constexpr SPICommandFrame_t SWITCH_TO_BANK_0          {0xFC, 0x00, 0x00, 0x73};
    constexpr SPICommandFrame_t SWITCH_TO_BANK_1          {0xFC, 0x00, 0x01, 0x6E};

    inline void DisplaySPIFrame(const uint8_t& frame)
    {
        std::ostringstream oss;

        oss << "\n\t0x";
        oss << std::setfill('0') << std::setw(2) << std::hex 
            << std::uppercase << static_cast<unsigned>(frame);
        oss << '\n';
        
        printf("%s\n", oss.str().c_str());
    }

    template <typename T>    
    inline SPIMISOFrame_t<T> Deserialize(const SPICommandFrame_t& frame)
    {
        uint8_t  operationCodeReadWrite = (frame.at(0) >> 7);
        uint8_t  operationCodeAddress   = (frame.at(0) >> 2) & 0x1F;
        uint8_t  returnStatusMISO       = (frame.at(0) & 0x03);
        T        sensorData             = ((frame.at(1) << 8) | frame.at(2));
        uint8_t  checksum               = frame.at(3);

        return std::make_tuple(operationCodeReadWrite, 
                               operationCodeAddress, 
                               returnStatusMISO,
                               sensorData, 
                               checksum);
    }
    
} // End of namespace ProtocolDefinitions.
