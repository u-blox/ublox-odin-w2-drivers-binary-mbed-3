# c029/binary

This is a binary module including u-blox Bluetooth and WiFi stack for [mbed OS](https://mbed/com).

## Structure of This Module

```
   |__ sal-stack-ublox-odin-w2
   |    \__ bt_types.h
   |    \__ cb_assert.h
   |    \__ cb_bt_conn_man.h
   |    \__ cb_bt_man.h
   |    \__ cb_bt_sec_man.h
   |    \__ cb_bt_serial.h
   |    \__ cb_bt_test_man.h
   |    \__ cb_bt_utils.h
   |    \__ cb_comdefs.h
   |    \__ cb_dev_info.h
   |    \__ cb_main.h
   |    \__ cb_platform_basic_types.h
   |    \__ cb_types.h
   |    \__ cb_wlan.h
   |    \__ cb_wlan_os.h
   |    \__ cb_wlan_types.h
   |    \__ lwipv4_init.h
   |    \__ WlanInterface.h
   |__ source
   |    |__ link-lib.cmake
   |    \__ sal-stack-ublox-odin-w2-binary.a
   |__ module.json
   |__ LICENSE
   \__ README.md
```

## Required hardware

## Dependencies

|_ mbed-hal-st-stm32cubef4 0.3.0
| \_ mbed-hal-st-stm32f4 1.1.0
|   |_ uvisor-lib 1.0.9
|   | \_ cmsis-core 1.1.0
|   |   \_ cmsis-core-st 1.0.0
|   |_ mbed-hal 1.2.0
|   | |_ mbed-drivers 0.11.3
|   | | |_ ualloc 1.0.2
|   | | | \_ dlmalloc 1.0.0
|   | | |_ minar 1.0.1
|   | | | \_ minar-platform 1.0.0
|   | | |   \_ minar-platform-mbed
|   | | |_ core-util 1.0.1
|   | | \_ compiler-polyfill 1.1.1
|   | \_ mbed-hal-st 1.0.0 yotta_modules\mbed-hal-st
|   \_ mbed-hal-st-stm32f429zi 1.0.5
|_ sal 1.0.2
|_ cmsis-core-stm32f4 1.0.4
| \_ cmsis-core-stm32f429xi 1.0.3
|_ sockets 1.0.2
\_ sal-stack-ublox-odin-w2-binary 0.0.1

## Using This Module

Add this module as a dependency to another module's module.json file

"dependencies": {
  "sal-stack-ublox-odin-w2-binary": "*"
}

and then 

include "sal-stack-ublox-odin-w2/*.h"

Targets:
ublox-c029-gcc
ublox-odin-w2-gcc

## Known limitations

## Known problems

## Future improvements
