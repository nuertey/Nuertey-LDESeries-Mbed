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

// Note that from hence, relevant sections of the 'DS_Standard-LDE_E_11815.pdf'
// are appropriately quoted (\" ... \") as needed. These are intended to
// serve as a sort of Customer requirement repository and to evidence 
// traceability.
//
// https://www.first-sensor.com/cms/upload/datasheets/DS_Standard-LDE_E_11815.pdf

#include <span> // std::span (C++20) is a safer alternative to separated pointer/size.
#include <bitset>
#include <cmath>

#include "Utilities.h"

// Metaprogramming types to distinguish the particular LDE series 
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

// \" The entire 16 bit content of the LDE register is then read out on
// the MISO pin, MSB first, by applying 16 successive clock pulses to 
// SCLK with /CS asserted low. \"
constexpr auto   NUMBER_OF_SPI_FRAME_BYTES = 2;

// Convenience aliases:
using EightBits_t   = std::bitset<NUMBER_OF_BITS>;
using SixteenBits_t = std::bitset<16>;
using SPIFrame_t    = std::array<unsigned char, NUMBER_OF_SPI_FRAME_BYTES>;

template<typename T>
concept IsLDESeriesSPIFrameType = ((std::is_integral_v<T> && (sizeof(T) == 1))
                                || TrueTypesEquivalent_v<T, SPIFrame_t>);
                              
namespace ProtocolDefinitions
{    

    
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
    template <IsLDESeriesSensorType S>
    constexpr auto GetScalingFactor()
    {
        constexpr auto LDE_SERIES_SCALING_FACTOR = ScalingFactorMap<S>::VALUE;        
        return LDE_SERIES_SCALING_FACTOR;
    }
    
    // \"
    // Data read – pressure
    //
    // When powered on, the sensor begins to continuously measure pressure.
    // To initiate data transfer from the sensor, the following three unique
    // bytes must be written sequentially, MSB first, to the MOSI pin (see
    // Figure 5):
    // \"
    constexpr uint8_t POLL_CURRENT_PRESSURE_MEASUREMENT{0x2D};
    constexpr uint8_t SEND_RESULT_TO_DATA_REGISTER     {0x14};
    constexpr uint8_t READ_DATA_REGISTER               {0x98};

    // \"
    // The entire 16 bit content of the LDE register is then read out on
    // the MISO pin, MSB first, by applying 16 successive clock pulses
    // to SCLK with /CS asserted low. Note that the value of the LSB is
    // held at zero for internal signal processing purposes. This is 
    // below the noise threshold of the sensor and thus its fixed value
    // does not affect sensor performance and accuracy. \"
    constexpr SPIFrame_t LDE_SERIES_SPI_DUMMY_FRAME{0x00, 0x00};

    template <IsLDESeriesSPIFrameType T>
    inline void DisplaySPIFrame(const T& frame)
    {        
        if constexpr(TrueTypesEquivalent_v<T, SPIFrame_t>)
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
        else
        {
            std::ostringstream oss;

            oss << "\n\t0x";
            oss << std::setfill('0') << std::setw(2) << std::hex 
                << std::uppercase << static_cast<unsigned>(frame);    
            oss << '\n';
            
            printf("%s\n", oss.str().c_str());
        }
    }

    inline int16_t Deserialize(const SPIFrame_t& frame)
    {
        // \" (10) The digital output signal is a signed, two complement
        // integer. Negative pressures will result in a negative output. \"
        int16_t sensorData = ((frame.at(1) << 8) | frame.at(2));

        return sensorData;
    }
    
} // End of namespace ProtocolDefinitions.
