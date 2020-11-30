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

#include "../RemoteProtocolBridgeCommon.h"
#include "../ProcessingEngine.h"

// Fwd. Declarations
class MainRemoteProtocolBridgeComponent;
class GlobalConfigWindow;

/**
 * Class GlobalConfigComponent is a container used to hold the GUI controls for modifying the app configuration.
 */
class GlobalConfigComponent :	public Component,
							public TextEditor::Listener,
							public Button::Listener
{
public:
	GlobalConfigComponent();
	~GlobalConfigComponent();

	//==============================================================================
	bool DumpEngineStartOnAppStart();
	bool DumpTrafficLoggingAllowed();
	void SetEngineStartOnAppStart(bool start);
	void SetTrafficLoggingAllowed(bool allowed);

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize();

	//==============================================================================
	void AddListener(GlobalConfigWindow* listener);

private:
	void paint(Graphics&) override;
	void resized() override;

	void textEditorFocusLost(TextEditor &) override;
	void textEditorReturnKeyPressed(TextEditor &) override;

	void buttonClicked(Button* button) override;

private:
	GlobalConfigWindow*			m_parentListener;				/**< Parent that needs to be notified when this window self-destroys. */

	std::unique_ptr<Label>			m_AllowTrafficLoggingLabel;		/**< Name label for traffic logging check. */
	std::unique_ptr<Label>			m_EnableEngineOnAppStartLabel;	/**< Enable checkbox for traffic logging. */
	std::unique_ptr<ToggleButton>	m_AllowTrafficLoggingCheck;		/**< Name label for engine autostart check. */
	std::unique_ptr<ToggleButton>	m_EnableEngineOnAppStartCheck;	/**< Enable checkbox for engine autostart. */

	std::unique_ptr<TextButton>		m_applyConfigButton;			/**< Button to apply edited values to configuration. */
};

/**
 * Class GlobalConfigWindow provides a window for editing app configuration
 */
class GlobalConfigWindow : public DialogWindow
{
public:
	//==============================================================================
	GlobalConfigWindow(const String& name,
		Colour backgroundColour,
		bool escapeKeyTriggersCloseButton,
		bool addToDesktop = true);
	~GlobalConfigWindow();

	//==============================================================================
	bool DumpConfig(ProcessingEngineConfig& config);
	void SetConfig(const ProcessingEngineConfig& config);
	void OnEditingFinished();

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize();

	//==============================================================================
	void AddListener(MainRemoteProtocolBridgeComponent* listener);

private:
	void closeButtonPressed() override;

	std::unique_ptr<GlobalConfigComponent>	m_configComponent;	/**< Actual config content component to reside in window. */
	MainRemoteProtocolBridgeComponent*		m_parentListener;	/**< Parent that needs to be notified when this window self-destroys. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlobalConfigWindow)
};
