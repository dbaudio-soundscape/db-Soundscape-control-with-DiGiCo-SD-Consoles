# RemoteProtocolBridge

Copyright (C) 2017-2020, d&b audiotechnik GmbH & Co. KG

By downloading software from this site, you agree to the terms and conditions described in the End-User License Agreement (provided with EULA.md). If you do not agree with such terms and conditions do not download the software.

## What is the RemoteProtocolBridge?

The RemoteProtocolBridge is a software tool which acts as a bridge between transmitters/receivers of the same or different remote control protocols and either translates data between different formats or performs other tasks related to protocol specific requirements.
One of the major requirements is, for example, data polling for OSC, which is necessary to enable third party devices that do not support data polling themselves to update with DS100 data changes.

For more information about the d&b Soundscape system, go to www.dbaudio.com and www.dbsoundscape.com.

## Building the application

The RemoteProtocolBridge tool was developed using JUCE, a cross-platform C++ application framework. Download and install JUCE v5.4.5 and use the Projucer tool to open the provided RemoteProtocolBridge.jucer configuration file. This file defines export configurations for Microsoft Visual Studio 2017 and Apple Xcode.

The RemoteProtocolBridge.jucer project file defines exporters for MS Visual Studio 2017 (Windows) and XCode (macOS). On both target platforms, JUCE v5.4.5 needs to be installed to be able to build the project.

### Windows
To build RemoteProtocolBridge on Windows, first install Visual Studio 2017. Open the RemoteProtocolBridge.jucer file from this repository using JUCE's Projucer tool. In Projucer, select the exporter target 'Visual Studio 2017' and click on "Save and open in IDE". This generates or updates the required build files and opens Visual Studio 2017, in which you can build an run the tool.

### macOS
To build RemoteProtocolBridge on macOS, first install Xcode. Open the RemoteProtocolBridge.jucer file from this repository using JUCE's Projucer tool. In Projucer, select the exporter target 'Xcode' and click on "Save and open in IDE". This generates or updates the required build files and opens Xcode, from where you can build and run the tool.