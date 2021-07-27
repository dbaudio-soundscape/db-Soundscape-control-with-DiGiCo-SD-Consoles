/*
===============================================================================

Copyright (C) 2019 d&b audiotechnik GmbH & Co. KG. All Rights Reserved.

This file is part of RemoteProtocolBridge.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY d&b audiotechnik GmbH & Co. KG "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "RemoteProtocolBridgeCommon.h"
#include "ProcessingEngine.h"

// Fwd. Declarations
class MainRemoteProtocolBridgeComponent;
class GlobalConfigWindow;
class LoggingWindow;
class NodeComponent;


/**
 * Class MainRemoteProtocolBridgeComponent is the central component within our application window 
 * containing everything else regarding ui elements.
 */
class MainRemoteProtocolBridgeComponent   : public Component,
						public Button::Listener
{
public:
    //==============================================================================
    MainRemoteProtocolBridgeComponent();
    ~MainRemoteProtocolBridgeComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	//==============================================================================
	void childWindowCloseTriggered(DialogWindow* childWindow);

	//==============================================================================
	void DumpUItoConfig();
    void RefreshUIfromConfig();

	//==============================================================================
	ProcessingEngineConfig* GetConfig();
	ProcessingEngine* GetEngine();

private:
    //==============================================================================
	std::map<NodeId, std::unique_ptr<NodeComponent>>	m_NodeBoxes;				/**< Map holding node components for all active bridging nodes. */

	std::unique_ptr<ImageButton>						m_AddNodeButton;			/**< Button to add a node. */
	std::unique_ptr<ImageButton>						m_RemoveNodeButton;			/**< Button to remove a node. */

	std::unique_ptr<Label>								m_NameAndVersionLabel;		/**< Label to show minimal app name and version info. */

	std::unique_ptr<TextButton>							m_TriggerOpenConfigButton;	/**< Button to trigger opening configuration. */
	std::unique_ptr<TextButton>							m_TriggerOpenLoggingButton;	/**< Button to trigger opening logging. */
	std::unique_ptr<TextButton>							m_EngineStartStopButton;	/**< Button to toggle engine start/stop. */

	std::unique_ptr<GlobalConfigWindow>					m_ConfigDialog;				/**< Pointer to configuration dialog instance (created on demand). */
	std::unique_ptr<LoggingWindow>						m_LoggingDialog;			/**< Pointer to logging dialog instance (created on demand). */

	ProcessingEngine									m_engine;					/**< The processig engine of RemoteProtocolBridge. */
	ProcessingEngineConfig								m_config;					/**< The configuration object for engine. */

	void buttonClicked(Button* button) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainRemoteProtocolBridgeComponent)
};
