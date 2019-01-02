# Support for integration of DiGiCo Generic OSC

Copyright (C) 2017-2019, d&b audiotechnik GmbH

## What is integration of DiGiCo Generic OSC in d&b soundscape

d&b soundscape console integration for DiGiCo consoles can be realized with the DiGiCo Generic OSC feature and the d&b software bridge "RemoteProtocolBridge". 

A detailed description of the d&b soundscape system, and documentation of the functions, features and configuration options can be found in the "TI 501d&b Soundscape System design and operation" which you can download from the d&b Homepage: www.dbaudio.com

The DiGiCo´s Generic OSC feature enables DiGiCo SD consoles to remote control other devices via OSC from a control panel added to each input channel on the console surface.
This feature of the DiGiCo consoles, stand alone, can be used to control d&b soundscape parameters within the DS100 from the console surface. 

In addition, d&b provides the free software tool “RemoteProtocolBridge” for macOS and Windows that enables bidirectional communication and adjusts the parameter values between consoles and the DS100 in both directions. This ensures that the console always displays the actual status of the DS100, even if parameters on the DS100 got changed by other tools, such as the d&b R1 Remote control software, cause the software bridge will transfer this parameter changes to the console surface.

For more Infos about the d&b Soundscape System and Concept visit: 
www.dbaudio.com and www.dbsoundscape.com.

## Requirements for usage of d&b software bridge

The d&b software bridge tool "RemoteProtocolBridge" is available for both Windows and macOS operating systems as compiled binaries in this repository under Binary/Windows and Binary/MacOS.

### macOS

The archived *.app bundle can be extracted and executed on a macOS system. The user has to enforce opening of the app initially in SystemSettings->Security to bypass SafeGuard restrictions.

### Windows

The archived *.exe executable can be extracted and executed on a Windows system. It requires the MS VisualStudio runtime environment to be installed on the system.
The Visual C++ Redistributable Packages for Visual Studio 2013 can be downloaded from Microsoft website. For this see https://www.microsoft.com/en-US/download/details.aspx?id=40784