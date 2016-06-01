# mbed-example-odin-w2-wifi-ap
Example to show how to use Wi-Fi access point on the ODIN-W2 for ARM mbed.

The application wifi-ap will demonstrate how to:

* Initialize Wi-Fi driver
* Start Wi-Fi driver
* Start lwIP
* Start access point mode with WPA2 security and DHCP server
* Provide an IP address to the connected Wi-Fi client
* Reply to ping requests

## Required hardware
* EVK-ODIN-W26 development board for the u-blox ODIN-W2 module
* USB micro cable
* Computer (or phone)

## Required software
* Yotta build environment, see *udp-time-client-wifi-example* section [*Required software*](https://github.com/u-blox/mbed-examples-odin-w2/tree/master/udp-time-client-wifi#required-software).  
* Fing - Network tool, to make ping request from an Android phone, can be downloaded from Google play.  

## Get the source code
See *udp-time-client-wifi-example* section [*Get the source code*](https://github.com/u-blox/mbed-examples-odin-w2/tree/master/udp-time-client-wifi#get-the-source-code).  

## Preparing configuration file
A configuration file can be used in this example application to change SSID, password, channel, address, netmask, gateway of the access point.   
Create the file "config.json" and place it in the root folder of your project.   

Include the following to your config.json and replace the default values:

```json
{
    "test": {
        "wifi_ap_ssid": "\"my_ssid\"",
        "wifi_ap_passphrase": "\"my_passphrase\"",
        "wifi_ap_channel": "6",
        "ip_static_addr": "\"192.168.0.1\"",
        "ip_static_netmask": "\"255.255.255.0\"",
        "ip_static_gateway": "\"192.168.0.1\"",
        "ip_static_dns_0": "\"0.0.0.0\"",
        "ip_static_dns_1": "\"0.0.0.0\""
    }
}
```

*Valid channels are 1-11,36,40,44,48*

## Configuring and building with Yotta
See *udp-time-client-wifi-example* section [*Configuring and building with Yotta*](https://github.com/u-blox/mbed-examples-odin-w2/tree/master/udp-time-client-wifi#configuring-and-building-with-yotta).  

## Running the application
When you plugin your EVK-ODIN-W2 board to your computer it should turn up as a USB mass storage device. To program the application binary (mbed-example-odin-w2-wifi-ap.bin) to your board simply copy and paste the application binary to your mbed USB mass storage device.

After successful programming reset your device and the application will start executing.

During execution some application status messages are printed to the debug UART of the EVK-ODIN-W2. When *cbWLAN\_STATUS\_AP\_UP* is displayed it is possible to connect a client to the access point. The client is successfully connected when status message *cbWLAN\_STATUS\_AP\_STA\_ADDED* is displayed. To test the connection do a ping request from the client to the access point (default IP address 192.168.0.1).

## Debugging
See *udp-time-client-wifi-example* section [*Debugging*](https://github.com/u-blox/mbed-examples-odin-w2/tree/master/udp-time-client-wifi#debugging).
