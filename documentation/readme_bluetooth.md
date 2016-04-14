# u-blox Embedded Bluetooth Stack
The u-blox Embedded Bluetooth Stack is optimized for small embedded industrial systems with high requirements on performance and robustness. It is also pre-qualified for Bluetooth 4.0+EDR and contains the following classic profiles; SPP, DUN, PAN (role PANU and NAP,  DID and GAP. For Bluetooth low energy GAP, GATT and DIS are pre-qualified.
Direct function calls and callbacks are used as much as possible and message passing and critical sections are used sparsely. All Bluetooth stack processes are running in the same context therefore mutual exclusions are not required for the different callbacks from the stack to the application.
The block diagram below shows the different modules in the stack.

![](mbed_ble_c_stack_system_overview.png)

The top modules cbBM, cbBSM, cbBCM, cbBSE, cbBSL, cbGATT and cbBTPAN above are exported via the API and they are described below. The other modules are internal and thus not accessible by the application. The functions/callbacks are described in the header files.

## General initialization
The Bluetooth stack is started by calling cbMAIN\_initBt. Note that cbBM_init must not be called from the application since this is done from inside cbMAIN\_initBt.
After cbBM_init has been called the application must wait for the complete callback before issuing any other function calls to the driver.

## Bluetooth Manager(cbBM)
The Bluetooth Manager takes care of static configuration of the Bluetooth stack e.g. local name, class of device, connectable modes etc. Procedures for inquiry and device name are also found here.
## Bluetooth Connection Manager(cbBCM)
This module contains functionality for establishing and tear down a connection to a remote Bluetooth device. The connection can be of different types, ACL connection, low energy connection or profile connection e.g. SPP.

Several connections can be active in parallel. They are separated by the use of a handle which is provided by the stack upon outgoing/incoming connection.

When an ACL/profile connection is set up, security might be needed depending on what Bluetooth version and security requirements the remote and local side is using. The security requirements depend on what protocol/profile is being used. For example, when using SDP it is likely that no security is needed while setting up RFCOMM will involve security. See Bluetooth Security chapter for more info on how security works.

Once a connection has been established, data can be sent to and from the remote device using the Bluetooth Serial(cbBSE) component.
This component also takes care of registering SDP database records and service searching.

An SDP client is a device that is searching for remote services while an SDP server is holding a service record to be found by remote devices. Therefore, if a device acts only as a client, it does not need to register an SDP service record. A single device can function both as an SDP server and as an SDP client.

Note that service records can only be added, not removed. When the device is reset all service records are removed.

The sequence diagram below shows the calls and events used in a successful connection setup and disconnect of a Bluetooth Serial Profile link. The connection is initiated by the application using the Connection Manager and when the connection is established the application receives the connect event callback and opens a corresponding data channel in Bluetooth Serial. When a disconnect occurs the application receives a disconnect event from the Connection Manager.

![](mbed_bt_spp_conn_setup.png)

## Bluetooth Serial(cbBSE)
The Bluetooth Serial component provides a generic data packet interface for SPP and DUN. After a connection has been set up using the Connection Manager a data packet channel can be opened. The data packet channel will be automatically closed when a disconnection occurs. The sequence diagram below shows how data is sent to and received from a remote device using a Bluetooth Serial Port link. For each write operation a write confirmation is received. When data is received from the remote device the application receives a data available event. When the application receives the data available event it fetches the data using the cbBSE_getReadbuf function and then, when the data has been processed calls the cbBSE_readBufConsumed function to notify that the underlying buffers can be reused. Note that the sample code for connection establishment and data transfer is available in the sample applications.

![](mbed_bt_spp_data_transfer.png)

## Bluetooth Security(cbBSM)
** NOTE: The current security API is subject to change.**
TBA

## Bluetooth Personal-Area Network(cbBTPAN)
The PAN profile is used for sending and receiving ethernet frames over a Bluetooth connection. This is typically supported by mobile phones and laptops.

The PAN profile defines three roles:
- PAN User (PANU) and PANU Service - This is the Bluetooth device that uses either the NAP or the GN service.
- Network Access Point (NAP) and NAP Service – A Bluetooth device that supports the NAP service is a Bluetooth device that provides some of the features of an Ethernet bridge to support network services.
- Group Ad-hoc Network (GN) and GN Service – A Bluetooth device that supports the GN service is able to forward Ethernet packets to each of the connected Bluetooth devices, the PAN users, as needed.

PANU and NAP are supported by the ODIN-W2 drivers.

## Bluetooth low energy Serial Port Service(cbBSL)
The Bluetooth low energy Serial component provides a generic data packet interface on top of GATT. After a connection has been set up using the Connection Manager a data packet channel can be opened. The data packet channel will be automatically closed when a disconnection occurs. The usage is similar to cbBSE.

## Bluetooth low energy Generic Attribute Profile(cbBTGATT)
TBA

## Bluetooth Test Manager(cbBTM)
This component is used for testing different radio characteristics and assure that radio approvals are followed.

## Bluetooth Qualification
The stack is pre-qualified for the following profiles and protocols:

- SPP  
- DUN  
- PAN (Role PANU and NAP)  
- BNEP  
- GAP  
- SDP  
- RFCOMM  
- L2CAP  
- HCI  
- DID  
- DIS  
- SM  
- ATT  
- GATT  

For more information on qualification please see the ODIN-W2 datasheet and System Integration Manual:  
[https://www.u-blox.com/en/product/odin-w2-series](https://www.u-blox.com/en/product/odin-w2-series)
