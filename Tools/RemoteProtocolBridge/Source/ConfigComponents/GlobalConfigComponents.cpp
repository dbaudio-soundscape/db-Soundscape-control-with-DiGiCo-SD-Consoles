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

#include "GlobalConfigComponents.h"

#include "../MainRemoteProtocolBridgeComponent.h"
#include "../NodeComponent.h"
#include "../ProtocolComponent.h"
#include "../RemoteProtocolBridgeCommon.h"

//==============================================================================
// Class GlobalConfigComponent
//==============================================================================
/**
 * Class constructor.
 */
GlobalConfigComponent::GlobalConfigComponent()
{
	m_parentListener = 0;

	m_AllowTrafficLoggingCheck = std::make_unique<ToggleButton>();
	addAndMakeVisible(m_AllowTrafficLoggingCheck.get());

	m_AllowTrafficLoggingLabel = std::make_unique <Label>();
	addAndMakeVisible(m_AllowTrafficLoggingLabel.get());
	m_AllowTrafficLoggingLabel->setText("Allow protocol traffic logging window", dontSendNotification);
	m_AllowTrafficLoggingLabel->attachToComponent(m_AllowTrafficLoggingCheck.get(), true);
	
	m_EnableEngineOnAppStartCheck = std::make_unique <ToggleButton>();
	addAndMakeVisible(m_EnableEngineOnAppStartCheck.get());

	m_EnableEngineOnAppStartLabel = std::make_unique <Label>();
	addAndMakeVisible(m_EnableEngineOnAppStartLabel.get());
	m_EnableEngineOnAppStartLabel->setText("Automatically start engine on app start", dontSendNotification);
	m_EnableEngineOnAppStartLabel->attachToComponent(m_EnableEngineOnAppStartCheck.get(), true);

	m_applyConfigButton = std::make_unique <TextButton>("Ok");
	addAndMakeVisible(m_applyConfigButton.get());
	m_applyConfigButton->addListener(this);
}

/**
 * Class destructor.
 */
GlobalConfigComponent::~GlobalConfigComponent()
{

}

/**
 * Reimplemented paint method that fills background with solid color
 *
 * @param g	Graphics painting object to use for filling background
 */
void GlobalConfigComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

/**
 * Reimplemented to resize and re-postion controls on the overview window.
 */
void GlobalConfigComponent::resized()
{
	double usableWidth = (double)(getWidth() - 2 * UIS_Margin_s);

	// traffic logging / engine autostart toggles
	int yOffset = UIS_Margin_s;
	m_AllowTrafficLoggingCheck->setBounds(Rectangle<int>((int)usableWidth - UIS_ElmSize, yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));
	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_EnableEngineOnAppStartCheck->setBounds(Rectangle<int>((int)usableWidth - UIS_ElmSize, yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));

	// ok button
	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_applyConfigButton->setBounds(Rectangle<int>((int)usableWidth - UIS_ButtonWidth, yOffset, UIS_ButtonWidth, UIS_ElmSize));
}

/**
 * Callback function for changes to our textEditors.
 * @param textEditor	The TextEditor object whose content has just changed.
 */
void GlobalConfigComponent::textEditorFocusLost(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for Enter key presses on textEditors.
 * @param textEditor	The TextEditor object whose where enter key was pressed.
 */
void GlobalConfigComponent::textEditorReturnKeyPressed(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void GlobalConfigComponent::buttonClicked(Button* button)
{
	if(button == m_applyConfigButton.get())
	{
		if (m_parentListener)
			m_parentListener->OnEditingFinished();
	}
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void GlobalConfigComponent::AddListener(GlobalConfigWindow* listener)
{
	m_parentListener = listener;
}

/**
 * Method to trigger dumping of state of button for auto-engine-start on app start
 *
 * @return	True if engine should start automatically on app start.
 */
bool GlobalConfigComponent::DumpEngineStartOnAppStart()
{
	if (m_EnableEngineOnAppStartCheck)
		return m_EnableEngineOnAppStartCheck->getToggleState();
	else
		return false;
}

/**
 * Method to trigger dumping of state of button for allowing trafficlogging enable button on ui
 *
 * @return	True if button should be available on ui.
 */
bool GlobalConfigComponent::DumpTrafficLoggingAllowed()
{
	if (m_AllowTrafficLoggingCheck)
		return m_AllowTrafficLoggingCheck->getToggleState();
	else
		return false;
}

/**
 * Setter of state of button for auto-engine-start on app start
 *
 * @param start	True if engine should start automatically on app start.
 */
void GlobalConfigComponent::SetEngineStartOnAppStart(bool start)
{
	if (m_EnableEngineOnAppStartCheck)
		m_EnableEngineOnAppStartCheck->setToggleState(start, dontSendNotification);
}

/**
 * Setter of state of button for allowing trafficlogging enable button on ui
 *
 * @param allowed	True if button should be available on ui.
 */
void GlobalConfigComponent::SetTrafficLoggingAllowed(bool allowed)
{
	if (m_AllowTrafficLoggingCheck)
		m_AllowTrafficLoggingCheck->setToggleState(allowed, dontSendNotification);
}

/**
 * Method to get the components' suggested size. This will be deprecated as soon as
 * the primitive UI is refactored and uses dynamic / proper layouting
 *
 * @return	The pair of int representing the suggested size for this component
 */
const std::pair<int, int> GlobalConfigComponent::GetSuggestedSize()
{
	int width = UIS_GlobalConfigWidth;
	int height = UIS_Margin_s +
		UIS_ElmSize +
		UIS_Margin_s + UIS_ElmSize +
		UIS_ElmSize +
		UIS_Margin_s + UIS_ElmSize +
		UIS_ElmSize +
		UIS_Margin_s;

	return std::pair<int, int>(width, height);
}


// **************************************************************************************
//    class GlobalConfigWindow
// **************************************************************************************
/**
 * Constructor for the GlobalConfigWindow class. Internal contents are created here.
 *
 * @param name							The name of the window
 * @param backgroundColour				The background color of the window
 * @param escapeKeyTriggersCloseButton	Flag value to set behaviour of hitting escape key on window
 * @param addToDesktop					Flag value to define if window is to be standalone or embedded in other content
 */
GlobalConfigWindow::GlobalConfigWindow(const String& name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, bool addToDesktop)
	: DialogWindow(name, backgroundColour, escapeKeyTriggersCloseButton, addToDesktop)
{
	m_parentListener = 0;

	m_configComponent = std::make_unique<GlobalConfigComponent>();
	
	// Component resizes automatically anyway, but need size > 0;
	m_configComponent->setBounds(Rectangle<int>(1, 1));
	m_configComponent->AddListener(this);
	
	setContentOwned(m_configComponent.get(), true);
}

/**
 * Destructor
 */
GlobalConfigWindow::~GlobalConfigWindow()
{

}

/**
 * Overloaded method that is called when window close button is pressed.
 * We enshure the window self-destroys here, but first notify the parent of it.
 */
void GlobalConfigWindow::closeButtonPressed()
{
	if (m_parentListener)
		m_parentListener->childWindowCloseTriggered(this);
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void GlobalConfigWindow::AddListener(MainRemoteProtocolBridgeComponent* listener)
{
	m_parentListener = listener;
}

/**
 * Proxy method to trigger dumping contents of configcomponent member
 * to global config object
 *
 * @param config	The global configuration object to dump data to
 * @return	True on success
 */
bool GlobalConfigWindow::DumpConfig(ProcessingEngineConfig& config)
{
	//config.SetRemoteObjectsToActivate(m_configComponent->DumpActiveRemoteObjects());
	config.SetEngineStartOnAppStart(m_configComponent->DumpEngineStartOnAppStart());
	config.SetTrafficLoggingAllowed(m_configComponent->DumpTrafficLoggingAllowed());

	return true;
}

/**
 * Proxy setter method to trigger filling contents of
 * configcomponent member with configuration contents
 *
 * @param config	The global configuration object.
 */
void GlobalConfigWindow::SetConfig(const ProcessingEngineConfig& config)
{
	//m_configComponent->FillActiveRemoteObjects(config.GetRemoteObjectsToActivate());
	m_configComponent->SetEngineStartOnAppStart(config.IsEngineStartOnAppStart());
	m_configComponent->SetTrafficLoggingAllowed(config.IsTrafficLoggingAllowed());
}

/**
 * Method to be called by child component to trigger closing down and applying
 * edited contents.
 */
void GlobalConfigWindow::OnEditingFinished()
{
	closeButtonPressed();
}

/**
 * Proxy method to get the windows' components' suggested size.
 *
 * @return	The pair of int representing the suggested size for the window
 */
const std::pair<int, int> GlobalConfigWindow::GetSuggestedSize()
{
	return m_configComponent->GetSuggestedSize();
}
