# u-blox ODIN-W2 Wifi and Bluetooth drivers 

This is a binary module including u-blox Bluetooth and WiFi stack for [mbed OS](https://mbed/com).

## Required hardware

u-blox ODIN-W2 module

## Get the source code
git clone https://github.com/u-blox/ublox-odin-w2-drivers-binary.git  
git clone https://github.com/u-blox/ublox-odin-w2-lwip-adapt.git  
git clone https://github.com/u-blox/sal-stack-lwip-ublox-odin-w2.git  
git clone https://github.com/u-blox/target-ublox-odin-w2-gcc.git

## Make yotta links from the modules above  
cd target-ublox-odin-w2-gcc  
yotta link-target  
cd..  

cd ublox-odin-w2-lwip-adapt  
yotta link  
cd..  

cd sal-stack-lwip-ublox-odin-w2  
yotta link  
cd..  

cd ublox-odin-w2-drivers-binary  
yotta link ublox-odin-w2-lwip-adapt  
yotta link sal-stack-lwip-ublox-odin-w2  
yotta link-target ublox-odin-w2-gcc  

## Set C029 target  
yotta target ublox-c029-gcc

## Building the tests
A config file is needed to be able to build the tests.
Include following to your config.json
```json
{
"test": {
    "wifi_ssid": "\"YOUR AP\"",
    "wifi_passphrase": "\"YOUR PASSWORD\"",
    "bt_remote_nap_addr": "0x11, 0x22, 0xAA, 0xBB, 0xCC, 0x00"
  }
}
```
Use the following to build: yotta build --config config.json

