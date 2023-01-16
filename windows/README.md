## Client Application

The client software uses the [SimpleBLE library](https://simpleble.readthedocs.io/en/latest/index.html) as a cross-platform solution. General build instructions are [here](https://simpleble.readthedocs.io/en/latest/simpleble/usage.html), but they are not complete. The library source files can be downloaded from GitHub [here](https://github.com/OpenBluetoothToolbox/SimpleBLE).

The client was developed under Microsoft Visual Studio Community 2022 (64-bit) - Current Version 17.4.3 as a "Project for a single page C++/WinRT Universal Windows Platform (UWP) app with no predefined layout".

### Windows 10

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

`C:\SimpleBLE\simpleble\build> cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 -DCMAKE_SYSTEM_VERSION=10.0.20348.0`

`C:\SimpleBLE\simpleble\build> cmake --build . -j7 --config Release`

`C:\SimpleBLE\simpleble\build> cmake --install . --config Release`

The `install` target does not copy the libraries to the intended directory, so you need to issue the following commands to do so:

`C:\SimpleBLE\simpleble\build> mkdir C:\"Program Files (x86)"\simpleble\lib\Release`

`C:\SimpleBLE\simpleble\build> copy lib\Release\*.lib C:\"Program Files (x86)"\simpleble\lib\Release\*.lib`

### In Microsoft Visual Studio:

1. Add the include directory for the SimpleBLE library:

    `Project Solution > Properties > Configuration Properties > C/C++ > General > Additional Include Directories`

2. Add the library directory for the SimpleBLE library:

    `Project Solution > Properties > Configuration Properties > Linker > General > Additional Library Directories`

3. Add the library to the project solution:

    `Project Solution > Properties > Configuration Properties > Linker > Input > Additional Dependencies`

4. Set target Windows SDK version:
 
    `Project Solution > Properties > Configuration Properties > General > Windows SDK Version`
