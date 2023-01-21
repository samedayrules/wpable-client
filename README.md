# WPA Supplicant Configuration over Bluetooth
Configuring the wireless interface on a newly deployed Raspberry Pi typically involves: 1) modifying system files on the Pi's SD card prior to installation, 2) connecting to the Pi via a monitor/keyboard and manually modifying system files, or 3) some other mechanism that requires relatively low-level access to and detailed knowledge of the Linux operating system. On the Raspberry Pi, the wireless network is often managed via changes to the [WPA Supplicant](https://wiki.archlinux.org/title/wpa_supplicant) system file that specifies network connection parameters.

This repository provides server and client applications that configure the WPA Supplicant wireless interface (e.g., wlan0) of Linux-based systems such as the Raspberry Pi using a Bluetooth Low Energy (BLE) connection. The **server software** accepts standard WPA Supplicant parameters from the client and applies those parameters to the WPA Supplicant system file on the server device. It then recofigures the interface using those parameters. The **client software** allows the user to customize the wireless interface settings and then apply them to the server device over Blutooth. The user does not have to manually modify the server operating system files or physically attach (cables) to the server device to connect to the wireless network.

This approach benefits product developers who supply the Raspberry Pi or similar Linux-based processing systems as part of their product line as the end user does not have to manually modify system files in order to connect the product to their home wireless network. The developer installs the server software on the Raspberry Pi, and the user installs the client software on their home computer - the user then runs the client software to connect the Raspberry Pi to their wireless network.

## Bluetooth Terminology
Excellent overview of BLE terminilogy provided by this [Adafruit article](https://learn.adafruit.com/introduction-to-bluetooth-low-energy).

**GAP: Generic Access Profile**

Controls connections and advertising in Bluetooth. 

**GATT: Generic ATTribute Profile**

Defines the way that two BLE devices transfer data back and forth using concepts called Services and Characteristics. It makes use of a generic
data protocol called the Attribute Protocol (ATT), which is used to store Services, Characteristics and related data in a simple lookup table
using 16-bit IDs for each entry in the table.

**Peripheral Device**

The peripheral device is known as the GATT Server, which holds the ATT lookup data and service and characteristic definitions.

**Central Device**

The central (main) device is known as the GATT Client (the phone/tablet), which sends requests to the server.

## Bluetooth Specifications

[Specifications and Test Documents List](https://www.bluetooth.com/specifications/specs/)

## Server Application

Server documentation is located here: [WPA BLE Supplicant Server](https://github.com/samedayrules/wpable_server)

## Client Application

The client application is intended to be available on various platforms (e.g., Windows, Apple iOS, Android OS). Currently, only the Windows version of the client is available.

The client software uses the [SimpleBLE library](https://simpleble.readthedocs.io/en/latest/index.html) as a cross-platform solution. General build instructions for the SimpleBLE library are [here](https://simpleble.readthedocs.io/en/latest/simpleble/usage.html), but they are not complete. The library source files can be downloaded from GitHub [here](https://github.com/OpenBluetoothToolbox/SimpleBLE).

### Windows 10

The client application is available on the [Microsoft Windows App Store](https://www.microsoft.com/store/apps/9PNCS683X3KR). Search for the title **Connect My Pi To WiFi**.

The client was developed under Microsoft Visual Studio Community 2022 (64-bit) - Current Version 17.4.3 as a "Project for a single page C++/WinRT Universal Windows Platform (UWP) app with no predefined layout".

If you choose to build the application from source, then the following sections apply.

#### SimpleBLE Library

The SimpleBle library requires:

1. [Microsoft Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/community/)
2. [Microsoft Windows SDK Version 10.0.19041.0 "or higher" Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/sdk-archive/)
3. [CMake version 3.25](https://cmake.org/download/)

**Note:**

Windows SDK versions 10.0.22621.755 and 10.0.22000.194 target Windows 11 and both cause C++/WinRT version mismatch errors during link of MS Visual Studio Community 2022 solution build.

To build the SimpleBLE library under Windows (assuming the SimpleBLE software has been downloaded into the directory `SimpleBLE`), issue the following commands using a command shell that has **System Administrator** privileges:

`C:\> cd \SimpleBLE\simpleble`

`C:\SimpleBLE\simpleble> mkdir build`

`C:\SimpleBLE\simpleble> cd build`

`C:\SimpleBLE\simpleble\build> cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_SYSTEM_VERSION=10.0.19041.0`

`C:\SimpleBLE\simpleble\build> cmake --build . -j7 --config Release`

`C:\SimpleBLE\simpleble\build> cmake --install . --config Release`

The `install` target does not copy the libraries to the intended directory, so you need to issue the following commands to do so:

`C:\SimpleBLE\simpleble\build> mkdir C:\"Program Files (x86)"\simpleble\lib\Release`

`C:\SimpleBLE\simpleble\build> copy lib\Release\*.lib C:\"Program Files (x86)"\simpleble\lib\Release\*.lib`

#### In Microsoft Visual Studio:

1. Add the include directory for the SimpleBLE library:

    `Project Solution > Properties > Configuration Properties > C/C++ > General > Additional Include Directories`

2. Add the library directory for the SimpleBLE library:

    `Project Solution > Properties > Configuration Properties > Linker > General > Additional Library Directories`

3. Add the library to the project solution:

    `Project Solution > Properties > Configuration Properties > Linker > Input > Additional Dependencies`

4. Set target Windows SDK version:
 
    `Project Solution > Properties > Configuration Properties > General > Windows SDK Version`
