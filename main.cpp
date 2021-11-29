/***********************************************************************
* @file      main.cpp
*
*    An ARM Mbed application that illustrates how a NUCLEO-F767ZI can be 
*    connected to a Murata SCL3300 Inclinometer Sensor Device.
*
* @brief   Test Murata SCL3300 Inclinometer Sensor Device.
* 
* @note    
*
* @warning Note that the I/O pins of STM32 NUCLEO-F767ZI are 3.3 V 
*          compatible instead of 5 V for say, the Arduino Uno V3 
*          microcontroller.
* 
*          Furthermore, the STM32 GPIO pins are not numbered 1-64; rather, 
*          they are named after the MCU IO port that they are controlled
*          by. Hence PA_5 is pin 5 on port A. This means that physical
*          pin location may not be related to the pin name. Consult 
*          the "Extension connectors" sub-chapter of the "Hardware 
*          layout and configuration" chapter of the STM32 Nucleo-144 
*          boards UM1974 User manual (en.DM00244518.pdf) to know where
*          each pin is located. Note that all those pins shown can be 
*          used as GPIOs, however most of them also have alternative 
*          functions which are indicated on those diagrams.
*
* @author    Nuertey Odzeyem
* 
* @date      November 28th, 2021
*
* @copyright Copyright (c) 2021 Nuertey Odzeyem. All Rights Reserved.
***********************************************************************/
#include "NuerteyLDESeriesDevice.h"

#define LED_ON  1
#define LED_OFF 0

// \" Specification notes (cont.)
//
// (17) For correct operation of LDE…3... devices, the device driving
// the SPI bus must have a minimum drive capability of ±2 mA. \"

// \" Care should be taken to ensure that the sensor is properly 
// connected to the master microcontroller. Refer to the manufacturer's
// datasheet for more information regarding physical connections. \"

// \" Application circuit
//
// The use of pull-up resistors is generally unnecessary for SPI as most
// master devices are configured for push-pull mode. If pull-up resistors
// are required for use with 3 V LDE devices, howeer, they should be 
// greater than 50 kW.
//
// ...
//
// If these series resistors are used, they must be physically placed as
// close as possible to the pins of the master and slave devices. \"

// Connector: CN7 
// Pin      : 14 
// Pin Name : D11       * Arduino-equivalent pin name
// STM32 Pin: PA7
// Signal   : SPI_A_MOSI/TIM_E_PWM1
//
// Connector: CN7 
// Pin      : 12 
// Pin Name : D12       * Arduino-equivalent pin name
// STM32 Pin: PA6
// Signal   : SPI_A_MISO 
//
// Connector: CN7 
// Pin      : 10 
// Pin Name : D13        * Arduino-equivalent pin name
// STM32 Pin: PA5
// Signal   : SPI_A_SCK
//
// Connector: CN7 
// Pin      : 16 
// Pin Name : D10       * Arduino-equivalent pin name
// STM32 Pin: PD14
// Signal   : SPI_A_CS/TIM_B_PWM3

// TBD, use fake pins for now just to enable compilation.
//
//        PinName mosi
//        PinName miso
//        PinName sclk
//        PinName ssel
NuerteyLDESeriesDevice g_LDESeriesDevice(D11, D12, D13, D10); 
        
// As per my ARM NUCLEO-F767ZI specs:        
DigitalOut        g_LEDGreen(LED1);
DigitalOut        g_LEDBlue(LED2);
DigitalOut        g_LEDRed(LED3);

// Do not return from main() as in Embedded Systems, there is nothing
// (conceptually) to return to. A crash will occur otherwise!
int main()
{
    printf("\r\n\r\nNuertey-LDESeries-Mbed - Beginning... \r\n\r\n");
    
    // Indicate with LEDs that we are commencing.
    g_LEDBlue = LED_ON;
    g_LEDGreen = LED_ON;

    if (Utilities::InitializeGlobalResources())
    {
        printf("\r\n%s\r\n", Utilities::g_NetworkInterfaceInfo.c_str());
        printf("\r\n%s\r\n", Utilities::g_SystemProfile.c_str());
        printf("\r\n%s\r\n", Utilities::g_BaseRegisterValues.c_str());
        printf("\r\n%s\r\n", Utilities::g_HeapStatistics.c_str());

        // Allow the sensor device time to stabilize from powering on 
        // and time enough for it to accumulate continuously measuring
        // temperature and pressure. Ergo:
        //
        // \" Power-on time 25 ms. \"
        //
        // \" When powered on, the sensor begins to continuously measure
        // pressure. \"
        ThisThread::sleep_for(25ms);        

        // Poll and query temperature and pressure measurements from LDE
        // sensor part number, LDES250BF6S, for example:
        printf("%s Pa\n\n", TruncateAndToString<double>(g_LDESeriesDevice.GetPressure<LDE_S250_B_t>()).c_str());
        
        printf("%s °C\n", TruncateAndToString<double>(g_LDESeriesDevice.GetTemperature<LDE_S250_B_t, Celsius_t>()).c_str());
        printf("%s °F\n", TruncateAndToString<double>(g_LDESeriesDevice.GetTemperature<LDE_S250_B_t, Fahrenheit_t>()).c_str());
        printf("%s K\n",  TruncateAndToString<double>(g_LDESeriesDevice.GetTemperature<LDE_S250_B_t, Kelvin_t>()).c_str());

        // Allow the user the chance to view the results:
        ThisThread::sleep_for(5s);
    
        Utilities::ReleaseGlobalResources();
    }
    else
    {
        printf("\r\n\r\nError! Initialization of Global Resources Failed!\n");
    }

    g_LEDGreen = LED_OFF;
    g_LEDBlue = LED_OFF;
    printf("\r\n\r\nNuertey-LDESeries-Mbed Application - Exiting.\r\n\r\n");
}
