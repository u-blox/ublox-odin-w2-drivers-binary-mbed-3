# u-blox ODIN-W2 Wi-Fi, Bluetooth and Ethernet drivers 

This is a pre-compiled binary module including u-blox Wi-Fi, Bluetooth and Ethernet drivers for [mbed OS](https://mbed.com).

The binary in intended for a u-blox ODIN-W2 module.

Please note that this document is not intended as a complete system description. It is intended to be an overview and for details the header files and sample project must be used.

**NOTE 1: Bluetooth low energy is not yet included in the driver module.**

**NOTE 2: The Ethernet interface is currently only experimental since it's not possible to set any configuration of an external PHY chip.**

## Related documents
[ODIN-W2 Data Sheet](https://www.u-blox.com/sites/default/files/ODIN-W2_DataSheet_%28UBX-14039949%29.pdf)  
[https://github.com/u-blox/sal-stack-lwip-ublox-odin-w2](https://github.com/u-blox/sal-stack-lwip-ublox-odin-w2) - IP stack(lwIP) for ODIN-W2  
[https://github.com/u-blox/ublox-odin-w2-lwip-adapt](https://github.com/u-blox/ublox-odin-w2-lwip-adapt) - Adaptation layer between lwIP and u-blox Wi-Fi and Bluetooth drivers  
[https://github.com/u-blox/target-ublox-odin-w2-gcc](https://github.com/u-blox/target-ublox-odin-w2-gcc) - Target description including pin map for ODIN-W2 module  
[https://github.com/u-blox/target-ublox-evk-odin-w2-gcc](https://github.com/u-blox/target-ublox-evk-odin-w2-gcc) - Target description including pin map for EVK-ODIN-W2  
[https://github.com/u-blox/mbed-examples-odin-w2](https://github.com/u-blox/mbed-examples-odin-w2) - Example applications  

## Overview

The ODIN-W2 module supports a variety of interfaces such as WLAN, Bluetooth, Bluetooth low energy, RMII (Ethernet), SPI, UART, CAN, I2C, GPIOs, Analog input pins, and JTAG/SWD. Many of the interfaces and IO pins are multiplexed. For details see the [ODIN-W2 Data Sheet](https://www.u-blox.com/sites/default/files/ODIN-W2_DataSheet_%28UBX-14039949%29.pdf).

The embedded Bluetooth Stack and the embedded WLAN driver are optimized for small embedded industrial systems with high requirements on performance and robustness. The Bluetooth stack contains the classic SPP, DUN, PAN, DID and GAP profiles and the low energy GATT, GAP and u-blox Serial Port Service. The Wi-Fi driver contains station as well as access point. A supplicant is also included.

![](mbed_odin_w2.png)

The exported components and corresponding files in the u-blox ODIN-W2 driver is shortly described below.

### Common
- **Main**(cb\_main.h) - Initialization of Wi-Fi, Bluetooth and Ethernet drivers
- cb\_comdefs.h - Commonly used definitions like TRUE/FALSE
- cb\_status.h - Common status codes

### Bluetooth
- **BT Manager**(cb\_bt\_man.h) - Bluetooth Generic Access Profile(GAP) functionality like inquiry, device name etc
- **BT Connection Manager**(cb\_bt\_conn\_man.h) - Setting up and tearing down Bluetooth SPP/PAN/DUN connections
- **BT Security Manager**(cb\_bt\_sec\_man.h) - Security manager that handles pairing and link keys. **NOTE, this API is subject to change for the next release.**
- **BT PAN**(cb\_bt\_pan.h) - Personal Area Network Profile(PAN) for sending Ethernet frames over Classic Bluetooth
- **BT Serial**(cb\_bt\_serial.h) - Serial Port Profile(SPP) based on RFCOMM for sending and receiving transparent data. Also supports ub-bloc Serial Port Servie for low energy and Dial-Up Network(DUN).
- bt\_types.h - Common Bluetooth types that are used be several components
- cb\_bt\_utils.h - Utility functions like comparing Bluetooth addresses

For more info about the Bluetooth components look [here](readme_bluetooth.md).

### Wi-Fi
- **WLAN API**(cb\_wlan.h) - Scanning, connection setup, maintenance and termination
- cb\_wlan\_types.h - WLAN types
- cb\_wlan\_target\_data.h - WLAN target that handles packetization of Ethernet frames
- cb\_platform\_basic\_types.h - Common definitions for a GCC compatible compiler
- cb\_port\_types.h - WLAN types
- cb\_types.h - Common types

For more info about the Wi-Fi component look [here](readme_wifi.md).

### Ethernet
- **Ethernet API**(cb\_ethernet.h) - Ethernet driver. NOTE, this API is subject to change for the next release.

### TCP/IP stack
Any TCP/IP stack can be used together with the driver but a ready to use [lwIP stack](http://savannah.nongnu.org/projects/lwip/) has been tested and is provided as a separate module [here](https://github.com/u-blox/sal-stack-lwip-ublox-odin-w2). It has been slightly modified to better suite the Wi-Fi driver.

To make it easier to customize the usage of the lwIP stack an adaptation module has been added and can be found [here](https://github.com/u-blox/ublox-odin-w2-lwip-adapt). For example, if layer 2 routing is needed it is highly recommended to fork the adaptation module or completely replace it.

### mbed OS
This is the ARM mbed OS framework and is a collection of OS-related modules and includes the [minar scheduler](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/Full_Guide/MINAR/), control of GPIOs, UART, SPI, I2C, security etc. A thorough description can be found [here](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/)

### ST firmware library
A subset of the functionality provided by the ST firmware library is accessible from mbed OS. If more control is needed it's also possible to access the drivers directly [here](https://github.com/ARMmbed/mbed-hal-st-stm32cubef4). Note that it must be used with care since any misusage might break the driver and/or mbed OS.

### Deprecated API
- cb_assert.h - use mbed assert instead
- cb_hw.h - use mbed functions instead
- cb_timer.h - use mbed functions instead

The module fulfills the ETSI regulations and modular approved for FCC and IC. It is also Bluetooth qualified as a Bluetooth controller subsystem. The embedded Bluetooth stack is pre-qualified as a Bluetooth host subsystem. This allows for customer specific Bluetooth applications developed directly for the STM32F439 microcontroller.

## Development and Debugging Environment
The build tool used for compiling and linking the driver is [yotta](https://www.mbed.com/en/development/software/mbed-yotta/).

When linking an application with the driver, all modules that the driver depends on must be of a specific version. The reason for this is that the driver can only be guaranteed to work with the fixed versions that we have verified during release tests.

Examples of how to set-up and debug an application can be found here [https://github.com/u-blox/mbed-examples-odin-w2](https://github.com/u-blox/mbed-examples-odin-w2).

## Flash memory configuration

![](mbed_odin_w2_flash.png)

The flash memory configuration above shows how the utilization can look like when two 128k sectors are used as non-volatile storage of link keys. How much space the ARM mbed OS and ODIN-W2 drivers occupy is very dependent on how much functionality is used. The [Wi-Fi example](https://github.com/u-blox/mbed-examples-odin-w2) will occupy ~1.2MByte.

No boot, starting at address 0, is used in this flash configuration as this is typically overwritten when using the ST-LINK mass storage flashing on the EVK-ODIN-W2.

**NOTE 3: The One Time Programmable(OTP) area is reserved for the ODIN-W2 drivers and must not be written to. The OTP area keeps all the MAC addresses.**

## RAM
The ODIN-W2 drivers use both static RAM and dynamically allocated heap memory via the mbed OS module [ualloc](https://github.com/ARMmbed/ualloc). The heap usage is heavily dependent on the use case.

## Hardware watchdog
TBD

## Power management
TBD

## Hardware resources
Both the Bluetooth stack and the Wi-Fi driver use parts of the hardware. The table below shows the dependencies to those resources.

| HW resource           | Component             | Description                                                                                                                                                                                                                                                                           |
|-----------------------|-----------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| GPIO                  | ODIN-W2               | Only IO pins that are included in the ODIN-W2 target can be used.                                                                                                                                                                                                                     |
| System tick interrupt | cbTimer, HAL          | The system tick is used by the timer component and the also by st fw lib.                                                                                                                                                                                                             |
| TIM2,TIM3,TIM4        | cbHW, cbUART          | Timers used for UART timing.To be able to use additional HW timers and still use the functionality provided by st fw lib the init, de-init and interrupt handling routines in cbHW must be updated. (HAL_TIM_Base_MspInit, HAL_TIM_Base_MspDeInit and HAL_TIM_PeriodElapsedCallback)  |
| DMA2, DMA7,DMA5, DMA6 | cbUART                | DMA stream 2 and 7 are used by the external SPA UART and DMA stream 5 and 6 is used by the Bluetooth HCI UART                                                                                                                                                                         |
| USART1,USART2         | cbUART                |                                                                                                                                                                                                                                                                                       |
| IWDG                  | cbWD                  | .The independent watchdog is setup using cbWD. Disabled by default                                                                                                                                                                                                                    |
| Ethernet              |                       |                                                                                                                                                                                                                                                                                       |
| SDIO                  |                       |                                                                                                                                                                                                                                                                                       |
| Flash memory          | boot, cbNVDS, cbFlash | Sectors 0-3 reserved for boot and production parameters.Sectors 22-23 reserved to NVDS parameter storage. Platform functionality will only use a fraction of the available parameters storage capacity and the application can use the rest. OTP is reserved for the ODIN-W2 drivers. |
| BKP Register 0        |                       | Backup register 0 used when jumping from application to boot for FW upgrade                                                                                                                                                                                                           |

