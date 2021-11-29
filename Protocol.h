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

// \" The LDE serial interface is a high-speed synchronous data input 
// and output communication port. The serial interface operates using 
// a standard 4-wire SPI bus. \"
constexpr size_t NUMBER_OF_BITS = 8;
constexpr auto   NUMBER_OF_SPI_COMMAND_FRAME_BYTES = 4;

// Convenience aliases:
using EightBits_t       = std::bitset<NUMBER_OF_BITS>;
using SixteenBits_t     = std::bitset<16>;
using SPICommandFrame_t = std::array<unsigned char, NUMBER_OF_SPI_COMMAND_FRAME_BYTES>;

template <typename T>
using SPIMISOFrame_t    = std::tuple<uint8_t,  // Operation Code, Read/Write
                                     uint8_t,  // Operation Code, Address
                                     uint8_t,  // Return Status (MISO)
                                     T,        // Sensor Data
                                     uint8_t>; // Checksum

namespace ProtocolDefinitions
{

    
    enum class MemoryBank_t : uint16_t
    {
        BANK_0 = 0,
        BANK_1 = 1 
    };
    
    constexpr uint8_t LDE_SERIES_SPI_DUMMY_BYTE{0xFF}; // ??? 0x00?

    // Another benefit of such an approach is, our ScalingFactor_t is 
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
                                  || std::is_same_v<S, LDE_S500_B_t>)
    constexpr double GetScalingFactor()
    {
        constexpr double LDE_SERIES_SCALING_FACTOR = ScalingFactorMap<T>::VALUE;        
        return LDE_SERIES_SCALING_FACTOR;
    }
    
    // \" 8-bit register for component identification
    //
    // Component ID [7:0] = C1h \"
    constexpr uint8_t WHO_AM_I{0xC1};
    
    constexpr SPICommandFrame_t RETURN_STATUS_MASK{0x03, 0x00, 0x00, 0x00};
    
    // =================================================================
    // /" Table 14 Operations And Their Equivalent SPI Frames /"
    //
    // Note that other than these below, no other command frame values
    // are allowed. 
    // =================================================================
    
    // Pertaining to the SPI CRC on the MOSI line, note that these SPI
    // command frames specified below have already had a precomputed CRC
    // byte appended. It is indeed the last byte, SPI [7:0] (MSB first).
    //
    // SPI Frame Specification Detail:
    //
    // \" OP (RW + ADDR)[31:26] + RS[25:24] + DATA[23:8] + CRC[7:0] \"
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

    inline void DisplayFrame(const SPICommandFrame_t& frame)
    {
        if (!frame.empty())
        {
            std::ostringstream oss;

            oss << "\n\t0x";
            for (auto& byte : frame)
            {
                oss << std::setfill('0') << std::setw(2) << std::hex 
                    << std::uppercase << static_cast<unsigned>(byte);
            }
            oss << '\n';
            
            printf("%s\n", oss.str().c_str());
        }
    }

    template <typename T>
    inline void AssertValidSPICommandFrame(const T& frame)
    {
        static_assert((std::is_same_v<T, SPICommandFrame_t>),
            "Hey! Frame argument MUST be of type: SPICommandFrame_t");
                    
        if ((READ_ACCELERATION_X_AXIS   != frame)
         && (READ_ACCELERATION_Y_AXIS   != frame)
         && (READ_ACCELERATION_Z_AXIS   != frame)
         && (READ_SELF_TEST_OUTPUT      != frame)
         && (ENABLE_ANGLE_OUTPUTS       != frame)
         && (READ_ANGLE_X_AXIS          != frame)
         && (READ_ANGLE_Y_AXIS          != frame)
         && (READ_ANGLE_Z_AXIS          != frame)
         && (READ_TEMPERATURE           != frame)
         && (READ_STATUS_SUMMARY        != frame)
         && (READ_ERROR_FLAG_1          != frame)
         && (READ_ERROR_FLAG_2          != frame)
         && (READ_COMMAND               != frame)
         && (CHANGE_TO_MODE_1           != frame)
         && (CHANGE_TO_MODE_2           != frame)
         && (CHANGE_TO_MODE_3           != frame)
         && (CHANGE_TO_MODE_4           != frame)
         && (SET_POWERDOWN_MODE         != frame)
         && (WAKEUP_FROM_POWERDOWN_MODE != frame)
         && (SOFTWARE_RESET             != frame)
         && (READ_WHO_AM_I              != frame)
         && (READ_SERIAL_1              != frame)
         && (READ_SERIAL_2              != frame)
         && (READ_CURRENT_BANK          != frame)
         && (SWITCH_TO_BANK_0           != frame)
         && (SWITCH_TO_BANK_1           != frame))
        {
            DisplayFrame(frame);
            assert(((void)"This SPI Command Frame is NOT valid/allowed!", (false)));
        }
    }
    
    // Copied verbatim from 'datasheet_scl3300-d01.pdf':
    //
    // https://www.murata.com/-/media/webrenewal/products/sensor/pdf/datasheet/datasheet_scl3300-d01.ashx?la=en-sg
    inline uint8_t CalculateCRC(uint8_t BitValue, uint8_t eightCRC)
    {
        uint8_t Temp;
        Temp = static_cast<uint8_t>(eightCRC & 0x80);
        if (BitValue == 0x01)
        {
            Temp ^= 0x80;
        }
        eightCRC <<= 1;
        if (Temp > 0)
        {
            eightCRC ^= 0x1D;
        }
        return eightCRC;
    }

    // Copied verbatim from 'datasheet_scl3300-d01.pdf':
    //
    // https://www.murata.com/-/media/webrenewal/products/sensor/pdf/datasheet/datasheet_scl3300-d01.ashx?la=en-sg
    inline uint8_t CalculateCRC(uint32_t frame)
    {
        // Calculate CRC for 24 MSB's of the 32 bit dword
        // (8 LSB's are the CRC field and are not included in CRC calculation)
        uint8_t BitIndex{0};
        uint8_t BitValue{0};
        uint8_t eightCRC{0};
        
        eightCRC = 0xFF;
        for (BitIndex = 31; BitIndex > 7; BitIndex--)
        {
            BitValue = static_cast<uint8_t>((frame >> BitIndex) & 0x01);
            eightCRC = CalculateCRC(BitValue, eightCRC);
        }
        eightCRC = static_cast<uint8_t>(~eightCRC);
        
        return eightCRC;
    }
    
    inline uint8_t CalculateCRC(const SPICommandFrame_t& frame)
    {
        uint32_t theFlatFrame = (frame.at(0) << 24) 
                              + (frame.at(1) << 16)
                              + (frame.at(2) << 8) 
                              + frame.at(3);
                              
        return CalculateCRC(theFlatFrame);
    }
    
    inline uint8_t GetReturnStatus(const SPICommandFrame_t& frame)
    {
        return (frame.at(0) & RETURN_STATUS_MASK.at(0));
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
