# Nuertey-SCL3300-Mbed - Type-Driven SPI Framing Protocol For Murata Manufacturing Co. Ltd.'s SCL3300 3-Axis Inclinometer.

SCL3300 - 3-axis inclinometer with angle output and digital SPI interface. 

This sensor driver is targetted for the ARM Mbed platform. For ease of use, power, flexibility and readability of the code, the protocol has been written in a modern C++ (C++17/C++20) metaprogrammed and templatized class-encapsulated idiom. 

A goal of the design is to encourage and promote zero-cost abstractions, even and especially so, in the embedded realms. Note that care has been taken to deduce and glean those abstractions from the problem-domain itself. And in the doing so, care has also been taken to shape the abstractions logically.

![SCL3300 Sensor Driver Class Diagram](https://github.com/nuertey/RandomArtifacts/blob/master/NuerteySCL3300Device2.png?raw=true)

The SCL3300 is targeted at applications demanding high stability and accuracy with tough environmental requirements. Typical applications would include:
 
  - Leveling
  - Tilt sensing
  - Machine control
  - Structural health monitoring
  - Inertial measurement units (IMUs)
  - Robotics
  - Positioning and guidance system
 
Key features of the sensor are as follows:
 
  - 3-axis (XYZ) inclinometer
  - User selectable measurement modes:
    - 3000 LSB/g with 70 Hz LPF
    - 6000 LSB/g with 40 Hz LPF
    - 12000 LSB/g with 10 Hz LPF
  - Angle output resolution 0.0055°/LSB
  - −40°C…+125°C operating range
  - 3.0V…3.6V supply voltage
  - SPI digital interface
  - Ultra-low 0.001 °/√Hz noise density
  - Excellent offset stability
  - Size 8.6 x 7.6 x 3.3 mm (l × w × h)
  - Proven capacitive 3D-MEMS technology

## DEPENDENCIES - CODING LANGUAGE/OS/COMPILATION TARGET/COMPILER:
  - C++20
  - mbed-os-6.15.1
  - NUCLEO F767ZI
  - GCC ARM 10.3.1
    - arm-none-eabi-g++ (GNU Arm Embedded Toolchain 10.3-2021.10) 10.3.1 20210824 (release)
    - gcc-arm-none-eabi-10.3-2021.10/bin

```console 
(py37-venv) nuertey@nuertey-PC:/.../Nuertey-SCL3300-Mbed$ mbed config -L

[mbed] Global config:
GCC_ARM_PATH=.../opt/gcc-arm-none-eabi-10.3-2021.10/bin

[mbed] Local config (/.../Nuertey-SCL3300-Mbed):
TARGET=nucleo_f767zi
TOOLCHAIN=GCC_ARM
```
 
## Compilation Output (Mbed CLI 1)

```console
...
Compile [ 99.8%]: MBRBlockDevice.cpp
[Warning] mpu_armv7.h@196,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] mpu_armv7.h@208,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] mpu_armv7.h@210,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] cachel1_armv7.h@67,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] cachel1_armv7.h@83,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] cachel1_armv7.h@169,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
[Warning] cachel1_armv7.h@191,14: compound assignment with 'volatile'-qualified left operand is deprecated [-Wvolatile]
Compile [ 99.9%]: hal_tick_overrides.c
Compile [100.0%]: stm32f7xx_hal_pcd_ex.c
Link: Nuertey-SCL3300-Mbed
Elf2Bin: Nuertey-SCL3300-Mbed

| Module               |           .text |       .data |          .bss |
|----------------------|-----------------|-------------|---------------|
| NuerteyNTPClient.o   |     4042(+4042) |       4(+4) |     101(+101) |
| Utilities.o          |     9592(+9592) |       4(+4) |     449(+449) |
| [fill]               |       314(+314) |     22(+22) |       78(+78) |
| [lib]/c.a            |   82560(+82560) | 2574(+2574) |       97(+97) |
| [lib]/gcc.a          |     7416(+7416) |       0(+0) |         0(+0) |
| [lib]/m.a            |     4040(+4040) |       0(+0) |         0(+0) |
| [lib]/misc           |       188(+188) |       4(+4) |       28(+28) |
| [lib]/nosys.a        |         32(+32) |       0(+0) |         0(+0) |
| [lib]/stdc++.a       | 174312(+174312) |   145(+145) |   5720(+5720) |
| main.o               |   15720(+15720) |     11(+11) |     713(+713) |
| mbed-os/cmsis        |     9890(+9890) |   168(+168) | 14400(+14400) |
| mbed-os/connectivity |   53284(+53284) |   103(+103) | 23535(+23535) |
| mbed-os/drivers      |     1146(+1146) |       0(+0) |   1852(+1852) |
| mbed-os/events       |     1776(+1776) |       0(+0) |   3104(+3104) |
| mbed-os/hal          |     1528(+1528) |       8(+8) |     114(+114) |
| mbed-os/platform     |     6676(+6676) |   340(+340) |     425(+425) |
| mbed-os/rtos         |     1276(+1276) |       0(+0) |         8(+8) |
| mbed-os/targets      |   17476(+17476) |       9(+9) |   1352(+1352) |
| Subtotals            | 391268(+391268) | 3392(+3392) | 51976(+51976) |
Total Static RAM memory (data + bss): 55368(+55368) bytes
Total Flash memory (text + data): 394660(+394660) bytes

Image: ./BUILD/NUCLEO_F767ZI/GCC_ARM-MY_PROFILE/Nuertey-SCL3300-Mbed.bin
```

## Tested Target (and Peripheral):

Lacking an actual SCL3300 sensor on my workbench for testing, I am left to appeal to you OEMs or better-equipped IoT hobbyists out there. If you have a spare SCL3300 dev board that can be connected to my STM32F767-ZI [Nucleo-144], kindly send me an email at nuertey_odzeyem@hotmail.com 


## License
MIT License

Copyright (c) 2021 Nuertey Odzeyem

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
