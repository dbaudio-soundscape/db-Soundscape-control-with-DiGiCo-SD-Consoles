# Release notes for d&b Soundscape RemoteProtocolBridge for DiGiCo SD-Consoles

Copyright (C) 2017-2021, d&b audiotechnik GmbH & Co. KG

* "Features" are newly implemented features (or parts of) with the corresponding release
* "Bugfixes" are newly fixed bugs with the corresponding release
* "Know issues" are issues (inconveniences or bugs) known to exist in the corresponding release

## V0.4.5

### Features

* New object handling modes to allow value change based protocol bridging combined with multiplexing nA to m B protocols

## V0.4.4

### Features

* New object handling modes to allow only forward, reverse or value change based protocol bridging

### Bugfixes

* Fix for processing of last channel number in object handling mode to support multiple DS100 to single console (multiplex n A to m B protocol object handling mode)

## V0.4.1

### Features

* New object handling mode to support multiple DS100 to single console (multiplex n A to m B protocol object handling mode)
* First open source release

### Bugfixes

* General improvements

## V0.3.1

### Features

* New application icon 


### Bugfixes

* General improvements


### Known issues

* Parameter DelayMode cannot be changed with a knob on the console interface: 
- DelayMode is received correctly when changed via R1 (and can also be saved and recalled via the scene memory) 
- only can not be changed via an assigned knob on the console. 


## V0.3.0

* Initial release
* release "Guide" is based on
