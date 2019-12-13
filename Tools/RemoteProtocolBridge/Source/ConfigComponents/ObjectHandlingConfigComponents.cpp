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

#include "ObjectHandlingConfigComponents.h"

#include "../NodeComponent.h"
#include "../Common.h"

//==============================================================================
// Class ObjectHandlingConfigComponent_Abstract
//==============================================================================

/**
 * @fn void ProcessingEngineConfig::ObjectHandlingData ObjectHandlingConfigComponent_Abstract::DumpObjectHandlingData()
 * @return The dumped set of object handling configuration data.
 * Pure virtual function to be implemented by data handling configuration components to dump the active configuration.
 */

/**
 * @fn void ObjectHandlingConfigComponent_Abstract::FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData)
 * @param ohData	
 * Pure virtual function to be implemented by data handling config component objects to insert a set of object handling configuration data.
 */

 /**
  * @fn const std::pair<int, int>  ObjectHandlingConfigComponent_Abstract::GetSuggestedSize()
  * @return The components favoured xy size
  * Pure virtual function to be implemented by data handling config component objects to suggest a favoured size.
  */

/**
 * Class constructor.
 */
ObjectHandlingConfigComponent_Abstract::ObjectHandlingConfigComponent_Abstract(ObjectHandlingMode mode)
{
	m_parentListener = 0;
	m_mode = mode;
}

/**
 * Class destructor.
 */
ObjectHandlingConfigComponent_Abstract::~ObjectHandlingConfigComponent_Abstract()
{
}

/**
 * Reimplemented paint method that fills background with solid color
 *
 * @param g	Graphics painting object to use for filling background
 */
void ObjectHandlingConfigComponent_Abstract::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void ObjectHandlingConfigComponent_Abstract::buttonClicked(Button* button)
{
	if (button == m_applyConfigButton.get())
	{
		if (m_parentListener)
			m_parentListener->OnEditingFinished();
	}
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void ObjectHandlingConfigComponent_Abstract::AddListener(ObjectHandlingConfigWindow* listener)
{
	m_parentListener = listener;
}


//==============================================================================
// Class OHNoConfigComponent
//==============================================================================
/**
 * Class constructor.
 */
OHNoConfigComponent::OHNoConfigComponent(ObjectHandlingMode mode)
	: ObjectHandlingConfigComponent_Abstract(mode)
{
	m_Headline = std::make_unique<Label>();
	m_Headline->setText("This object handling mode does not require configuration.", dontSendNotification);
	addAndMakeVisible(m_Headline.get());

	m_applyConfigButton = std::make_unique<TextButton>("Ok");
	addAndMakeVisible(m_applyConfigButton.get());
	m_applyConfigButton->addListener(this);
}

/**
 * Class destructor.
 */
OHNoConfigComponent::~OHNoConfigComponent()
{
}

/**
 * Reimplemented to resize and re-postion controls on the overview window.
 */
void OHNoConfigComponent::resized()
{
	double usableWidth = (double)(getWidth() - 2 * UIS_Margin_s);

	// headline
	int yOffset = UIS_Margin_s;
	m_Headline->setBounds(UIS_Margin_m, yOffset, (int)usableWidth, UIS_ElmSize);

	// ok button
	yOffset += UIS_ElmSize + UIS_Margin_s;
	m_applyConfigButton->setBounds(Rectangle<int>((int)usableWidth - UIS_ButtonWidth, yOffset, UIS_ButtonWidth, UIS_ElmSize));
}

/**
 * Method to get the components' suggested size. This will be deprecated as soon as
 * the primitive UI is refactored and uses dynamic / proper layouting
 *
 * @return	The pair of int representing the suggested size for this component
 */
const std::pair<int, int> OHNoConfigComponent::GetSuggestedSize()
{
	int width = UIS_BasicConfigWidth;
	int height = 100;

	return std::pair<int, int>(width, height);
}

/**
 * Method to trigger dumping contents of configcomponent member
 * to list of objects to return to the app to initialize from
 *
 * @return	The list of objects to actively handle when running the engine.
 */
ProcessingEngineConfig::ObjectHandlingData OHNoConfigComponent::DumpObjectHandlingData()
{
	ProcessingEngineConfig::ObjectHandlingData ohData;
	ohData.Mode = m_mode;
	ohData.ACnt = 0;
	ohData.BCnt = 0;

	return ohData;
}

/**
 * Method to trigger filling contents of
 * configcomponent member with ObjectHandling data
 *
 * @param ohData	The data to set into UI elms.
 */
void OHNoConfigComponent::FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData)
{
	ignoreUnused(ohData);
}


//==============================================================================
// Class OHMultiplexAtoBConfigComponent
//==============================================================================
/**
 * Class constructor.
 */
OHMultiplexAtoBConfigComponent::OHMultiplexAtoBConfigComponent(ObjectHandlingMode mode)
	: ObjectHandlingConfigComponent_Abstract(mode)
{
	m_Headline = std::make_unique <Label>();
	m_Headline->setText("Multiplexing parameters:", dontSendNotification);
	addAndMakeVisible(m_Headline.get());

	m_CountAEdit = std::make_unique<TextEditor>();
	addAndMakeVisible(m_CountAEdit.get());
	m_CountAEdit->addListener(this);

	m_CountALabel = std::make_unique<Label>();
	m_CountALabel->setText("Ch. count per ProtocolA (n)", dontSendNotification);
	addAndMakeVisible(m_CountALabel.get());
	m_CountALabel->attachToComponent(m_CountAEdit.get(), true);

	m_CountBEdit = std::make_unique<TextEditor>();
	addAndMakeVisible(m_CountBEdit.get());
	m_CountBEdit->addListener(this);

	m_CountBLabel = std::make_unique<Label>();
	m_CountBLabel->setText("Ch. count per ProtocolB (m)", dontSendNotification);
	addAndMakeVisible(m_CountBLabel.get());
	m_CountBLabel->attachToComponent(m_CountBEdit.get(), true);

	m_applyConfigButton = std::make_unique<TextButton>("Ok");
	addAndMakeVisible(m_applyConfigButton.get());
	m_applyConfigButton->addListener(this);
}

/**
 * Class destructor.
 */
OHMultiplexAtoBConfigComponent::~OHMultiplexAtoBConfigComponent()
{
}

/**
 * Reimplemented to resize and re-postion controls on the overview window.
 */
void OHMultiplexAtoBConfigComponent::resized()
{
	double usableWidth = (double)(getWidth() - 2 * UIS_Margin_s);
	
	// active objects headline
	int yOffset = UIS_Margin_s;
	m_Headline->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, (int)usableWidth, UIS_ElmSize));

	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_CountAEdit->setBounds(Rectangle<int>(UIS_WideAttachedLabelWidth + UIS_Margin_s, yOffset, (int)usableWidth - UIS_WideAttachedLabelWidth - UIS_Margin_s, UIS_ElmSize));
	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_CountBEdit->setBounds(Rectangle<int>(UIS_WideAttachedLabelWidth + UIS_Margin_s, yOffset, (int)usableWidth - UIS_WideAttachedLabelWidth - UIS_Margin_s, UIS_ElmSize));

	// ok button
	yOffset += UIS_Margin_s + UIS_ElmSize + UIS_Margin_s;
	m_applyConfigButton->setBounds(Rectangle<int>((int)usableWidth - UIS_ButtonWidth, yOffset, UIS_ButtonWidth, UIS_ElmSize));
}

/**
 * Callback function for changes to our textEditors.
 * @param textEditor	The TextEditor object whose content has just changed.
 */
void OHMultiplexAtoBConfigComponent::textEditorFocusLost(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for Enter key presses on textEditors.
 * @param textEditor	The TextEditor object whose where enter key was pressed.
 */
void OHMultiplexAtoBConfigComponent::textEditorReturnKeyPressed(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Method to get the components' suggested size. This will be deprecated as soon as
 * the primitive UI is refactored and uses dynamic / proper layouting
 *
 * @return	The pair of int representing the suggested size for this component
 */
const std::pair<int, int> OHMultiplexAtoBConfigComponent::GetSuggestedSize()
{
	int width = UIS_OSCConfigWidth;
	int height =	UIS_Margin_s +
					2 * UIS_Margin_m + UIS_ElmSize +
					UIS_Margin_s + UIS_ElmSize +
					UIS_Margin_s + UIS_ElmSize +
					UIS_Margin_s + UIS_ElmSize + UIS_Margin_s +
					UIS_Margin_s + UIS_ElmSize;

	return std::pair<int, int>(width, height);
}

/**
 * Method to trigger dumping contents of configcomponent member
 * to list of objects to return to the app to initialize from
 *
 * @return	The list of objects to actively handle when running the engine.
 */
ProcessingEngineConfig::ObjectHandlingData OHMultiplexAtoBConfigComponent::DumpObjectHandlingData()
{
	ProcessingEngineConfig::ObjectHandlingData ohData;
	ohData.Mode = m_mode;
	if (m_CountAEdit)
	{
		ohData.ACnt = m_CountAEdit->getText().getIntValue();
	}
	if (m_CountBEdit)
	{
		ohData.BCnt = m_CountBEdit->getText().getIntValue();
	}

	return ohData;
}

/**
 * Method to trigger filling contents of
 * configcomponent member with ObjectHandling data
 *
 * @param ohData	The data to set into UI elms.
 */
void OHMultiplexAtoBConfigComponent::FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData)
{
	if (m_CountAEdit)
		m_CountAEdit->setText(String(ohData.ACnt), dontSendNotification);
	if (m_CountBEdit)
		m_CountBEdit->setText(String(ohData.BCnt), dontSendNotification);

}


// **************************************************************************************
//    class ObjectHandlingConfigWindow
// **************************************************************************************
/**
 * Constructor for the ObjectHandlingConfigWindow class. Internal contents are created here.
 *
 * @param name							The name of the window
 * @param backgroundColour				The background color of the window
 * @param escapeKeyTriggersCloseButton	Flag value to set behaviour of hitting escape key on window
 * @param NId							Node Id of the node this object handling config window implements editing for
 * @param mode							Object handling mode this config window shall provide editing for
 * @param addToDesktop					Flag value to define if window is to be standalone or embedded in other content
 */
ObjectHandlingConfigWindow::ObjectHandlingConfigWindow(const String &name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, NodeId NId,
													   ObjectHandlingMode mode, bool addToDesktop)
	: DialogWindow(name, backgroundColour, escapeKeyTriggersCloseButton, addToDesktop)
{
	m_parentListener = 0;

	m_NId = NId;

	switch (mode)
	{
	case ObjectHandlingMode::OHM_Mux_nA_to_mB:
		m_configComponent = std::make_unique<OHMultiplexAtoBConfigComponent>(mode);
		break;
	case ObjectHandlingMode::OHM_Bypass:
		// intentionally no break to run into default
	case ObjectHandlingMode::OHM_Remap_A_X_Y_to_B_XY:
		// intentionally no break to run into default
	case ObjectHandlingMode::OHM_Invalid:
		// intentionally no break to run into default
	default:
		m_configComponent = std::make_unique<OHNoConfigComponent>(mode);
		break;
	}

	// Component resizes automatically anyway, but need size > 0;
	m_configComponent->setBounds(Rectangle<int>(1, 1));
	m_configComponent->AddListener(this);

	setContentOwned(m_configComponent.get(), true);
}

/**
 * Destructor
 */
ObjectHandlingConfigWindow::~ObjectHandlingConfigWindow()
{

}

/**
 * Overloaded method that is called when window close button is pressed.
 * We enshure the window self-destroys here, but first notify the parent of it.
 */
void ObjectHandlingConfigWindow::closeButtonPressed()
{
	if (m_parentListener)
		m_parentListener->childWindowCloseTriggered(this);
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void ObjectHandlingConfigWindow::AddListener(NodeComponent* listener)
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
bool ObjectHandlingConfigWindow::DumpConfig(ProcessingEngineConfig& config)
{
	config.SetObjectHandlingData(m_NId, m_configComponent->DumpObjectHandlingData());

	return true;
}

/**
 * Proxy setter method to trigger filling contents of
 * configcomponent member with configuration contents
 *
 * @param config	The global configuration object.
 */
void ObjectHandlingConfigWindow::SetConfig(const ProcessingEngineConfig& config)
{
	m_configComponent->FillObjectHandlingData(config.GetObjectHandlingData(m_NId));
}

/**
 * Method to be called by child component to trigger closing down and applying
 * edited contents.
 */
void ObjectHandlingConfigWindow::OnEditingFinished()
{
	closeButtonPressed();
}

/**
 * Proxy method to get the windows' components' suggested size.
 *
 * @return	The pair of int representing the suggested size for the window
 */
const std::pair<int, int> ObjectHandlingConfigWindow::GetSuggestedSize()
{
	return m_configComponent->GetSuggestedSize();
}
