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

#include "ProtocolComponent.h"

#include "NodeComponent.h"
#include "ConfigComponents/ProtocolConfigComponents.h"
#include "ProcessingEngineConfig.h"


// **************************************************************************************
//    class ProtocolGroupComponent
// **************************************************************************************
/**
 * Constructor
 */
ProtocolGroupComponent::ProtocolGroupComponent()
	: GroupComponent()
{
	m_parentComponent = 0;

	m_NodeId = 0;

	/******************************************************/
	m_AddProtocolButton = std::make_unique<ImageButton>();
	m_AddProtocolButton->addListener(this);
	addAndMakeVisible(m_AddProtocolButton.get());
	Image AddNormalImage = ImageCache::getFromMemory(BinaryData::AddNormalImage_png, BinaryData::AddNormalImage_pngSize);
	Image AddOverImage = ImageCache::getFromMemory(BinaryData::AddOverImage_png, BinaryData::AddOverImage_pngSize);
	Image AddDownImage = ImageCache::getFromMemory(BinaryData::AddDownImage_png, BinaryData::AddDownImage_pngSize);
	m_AddProtocolButton->setImages(false, true, true,
		AddNormalImage, 1.0, Colours::transparentBlack,
		AddOverImage, 1.0, Colours::transparentBlack,
		AddDownImage, 1.0, Colours::transparentBlack);

	m_RemoveProtocolButton = std::make_unique<ImageButton>();
	m_RemoveProtocolButton->addListener(this);
	addAndMakeVisible(m_RemoveProtocolButton.get());
	Image RemoveNormalImage = ImageCache::getFromMemory(BinaryData::RemoveNormalImage_png, BinaryData::RemoveNormalImage_pngSize);
	Image RemoveOverImage = ImageCache::getFromMemory(BinaryData::RemoveOverImage_png, BinaryData::RemoveOverImage_pngSize);
	Image RemoveDownImage = ImageCache::getFromMemory(BinaryData::RemoveDownImage_png, BinaryData::RemoveDownImage_pngSize);
	m_RemoveProtocolButton->setImages(false, true, true,
		RemoveNormalImage, 1.0, Colours::transparentBlack,
		RemoveOverImage, 1.0, Colours::transparentBlack,
		RemoveDownImage, 1.0, Colours::transparentBlack);

	/******************************************************/
}

/**
 * Destructor
 */
ProtocolGroupComponent::~ProtocolGroupComponent()
{
	jassert(m_ProtocolComponents.size() == m_ProtocolIds.size());

	for (const ProtocolId& PId : m_ProtocolIds)
	{
		removeChildComponent(m_ProtocolComponents[PId].get());
		m_ProtocolComponents[PId].reset();
		m_ProtocolComponents.erase(PId);
		m_ProtocolIds.removeAllInstancesOf(PId);
	}

	m_ProtocolComponents.clear();
}

/**
 * Overloaded method to resize contents
 */
void ProtocolGroupComponent::resized()
{
	GroupComponent::resized();

	/*Add/Remove Buttons*/
	int yPositionAddRemButtons = getHeight() - UIS_ElmSize - UIS_Margin_s;
	int xPositionAddRemButtons = getWidth() - UIS_ElmSize - UIS_Margin_s;
	if (m_AddProtocolButton)
		m_AddProtocolButton->setBounds(xPositionAddRemButtons, yPositionAddRemButtons, UIS_ElmSize - UIS_Margin_s, UIS_ElmSize - UIS_Margin_s);

	xPositionAddRemButtons -= UIS_ElmSize;
	if (m_RemoveProtocolButton)
		m_RemoveProtocolButton->setBounds(xPositionAddRemButtons, yPositionAddRemButtons, UIS_ElmSize - UIS_Margin_s, UIS_ElmSize - UIS_Margin_s);

	/*Dynamically sized elements*/
	int yOffset = UIS_Margin_m;
	for (std::map<ProtocolId, std::unique_ptr<ProtocolComponent>>::iterator piter = m_ProtocolComponents.begin(); piter != m_ProtocolComponents.end(); ++piter)
	{
		if(piter->second)
			piter->second->setBounds(UIS_Margin_s, UIS_Margin_s + yOffset, getWidth() - 2 * UIS_Margin_s, UIS_ElmSize);

		yOffset += UIS_ElmSize + UIS_Margin_s;
	}
}

/**
 * Method to gather data from ui input elements and dump them to given configuration object.
 *
 * @param config	The configuration object to be filled with contents from ui elements of this node
 */
void ProtocolGroupComponent::DumpUItoConfig(ProcessingEngineConfig& config)
{
	for (ProtocolId* PId = m_ProtocolIds.begin(); PId != m_ProtocolIds.end(); ++PId)
	{
		if (m_ProtocolComponents.count(*PId) && m_ProtocolComponents[*PId])
		{
			ProcessingEngineConfig::ProtocolData protocolData = config.GetProtocolData(m_NodeId, *PId);
			m_ProtocolComponents[*PId]->DumpUItoConfigData(protocolData);
			config.SetProtocolData(m_NodeId, *PId, protocolData);
		}
	}
}

/**
 * Method to update ui input elements with data to show from config
 *
 * @param parentNodeId	The node id of the parent node object
 * @param protocolIds	The array of ids of protocols to handle in this component
 * @param config		The configuration object to extract the data to show on ui from
 * @return	The calculated theoretically required size for this component
 */
int ProtocolGroupComponent::RefreshUIfromConfig(const NodeId& parentNodeId, const Array<ProtocolId> protocolIds, const ProcessingEngineConfig& config)
{
	m_NodeId = parentNodeId;

	// go through current ui node boxes to find out which ones need to be removed/destroyed
	Array<ProtocolId> PIdsToRemove;
	for (std::map<ProtocolId, std::unique_ptr<ProtocolComponent>>::iterator protocolIter = m_ProtocolComponents.begin(); protocolIter != m_ProtocolComponents.end(); protocolIter++)
		if (!protocolIds.contains(protocolIter->first))
			PIdsToRemove.add(protocolIter->first);

	for(const ProtocolId& PId : PIdsToRemove)
	{
		if (m_ProtocolComponents.count(PId))
		{
			removeChildComponent(m_ProtocolComponents[PId].get());
			m_ProtocolComponents[PId].reset();
			m_ProtocolComponents.erase(PId);
		}
		m_ProtocolIds.removeAllInstancesOf(PId);
	}

	// top margin v space
	int requiredHeight = UIS_Margin_m;

	// now go through all protocol ids currently in config and create those protocols
	// that do not already exist or simply update those that are present
	for (const ProtocolId& PId : protocolIds)
	{
		if (!m_ProtocolIds.contains(PId))
		{
			m_ProtocolIds.add(PId);

			ProtocolComponent* Protocol = new ProtocolComponent(m_NodeId, PId);
			Protocol->AddListener(this);
			addAndMakeVisible(Protocol);

			m_ProtocolComponents[PId] = std::unique_ptr<ProtocolComponent>(Protocol);
		}
		
		if(m_ProtocolIds.contains(PId) && m_ProtocolComponents.count(PId) && m_ProtocolComponents[PId])
		{
			requiredHeight += m_ProtocolComponents[PId]->RefreshUIfromConfigData(config.GetProtocolData(m_NodeId, PId));
		}
	}

	// margin v space
	requiredHeight += UIS_Margin_s;

	// reserve some v space for +- buttons
	requiredHeight += (UIS_ElmSize + UIS_Margin_m);

	// margin v space
	requiredHeight += UIS_Margin_m;

	return requiredHeight;
}

/**
 * Method to allow access to internal config.
 *
 * @return	Reference to the internal config object
 */
ProcessingEngineConfig* ProtocolGroupComponent::GetConfig()
{
	if (m_parentComponent)
		return m_parentComponent->GetConfig();
	else
	{
		jassert(false);
		return 0;
	}
}

/**
 * Method to allow access to internal engine.
 *
 * @return	Reference to the internal engine object
 */
ProcessingEngine* ProtocolGroupComponent::GetEngine()
{
	if (m_parentComponent)
		return m_parentComponent->GetEngine();
	else
	{
		jassert(false);
		return 0;
	}
}

/**
 * Getter for this protocol components' parent NodeId.
 *
 * @return	The NodeID
 */
NodeId ProtocolGroupComponent::GetNodeId()
{
	return m_NodeId;
}

/**
 * Getter for the ids of the protocols in this group component
 * 
 * @return	The list of protocol ids
 */
const Array<ProtocolId>& ProtocolGroupComponent::GetProtocolIds()
{
	return m_ProtocolIds;
}

/**
 * Method to add the parent listener to this instance of ProtocolGroupComponent.
 * This can afterwards be used for e.g. callbacks, etc.
 *
 * @param listener	The parent listener object to be used to invoke public methods from ('callback')
 */
void ProtocolGroupComponent::AddListener(NodeComponent* listener)
{
    m_parentComponent = listener;

	// In case a node id is set (!=0) we require it to be the listeners' node id, otherwise we have a severe misconfiguration!
	jassert((m_NodeId == 0) || (m_NodeId == listener->GetNodeId()));
}

/**
 * Overloaded method called by button objects on click events.
 * All internal button objects are registered to trigger this by calling
 * their ::addListener method with this object as argument
 *
 * @param button	The button object that has been clicked
 */
void ProtocolGroupComponent::buttonClicked(Button* button)
{
	if (m_parentComponent)
	{
		if (button == m_AddProtocolButton.get())
		{
			m_parentComponent->AddDefaultProtocol(this);
		}
		else if (button == m_RemoveProtocolButton.get())
		{
			m_parentComponent->RemoveProtocol(m_NodeId, m_ProtocolIds.getLast());
		}
	}
}

/**
 * Proxy method to forward triggering parent to start dumping to config
 */
void ProtocolGroupComponent::TriggerParentConfigDump()
{
	if (m_parentComponent)
		m_parentComponent->TriggerParentConfigDump();
}

/**
 * Proxy method to forward triggering parent to start refreshing from config
 */
void ProtocolGroupComponent::TriggerParentConfigRefresh()
{
	if (m_parentComponent)
		m_parentComponent->TriggerParentConfigRefresh();
}

/**
 * Proxy method to refresh the processing engine with current config
 * and restart it if it was running.
 */
bool ProtocolGroupComponent::RefreshEngine()
{
	if (m_parentComponent)
		return m_parentComponent->RefreshEngine();
	else
		return false;
}


// **************************************************************************************
//    class ProtocolComponent
// **************************************************************************************
/**
 * Constructor
 */
ProtocolComponent::ProtocolComponent(const NodeId& NId, const ProtocolId& PId)
	: Component()
{
	m_parentComponent = 0;

	m_NodeId = NId;
	m_ProtocolId = PId;

	/******************************************************/
	m_ProtocolDrop = std::make_unique<ComboBox>();
	m_ProtocolDrop->addListener(this);
	addAndMakeVisible(m_ProtocolDrop.get());
	m_ProtocolDrop->addItem(ProcessingEngineConfig::ProtocolTypeToString(PT_OSCProtocol), PT_OSCProtocol);
	//m_ProtocolDrop->addItem(ProcessingEngineConfig::ProtocolTypeToString(PT_OCAProtocol), PT_OCAProtocol); // not yet implemented, feel free to step in
	//m_ProtocolDrop->addItem(ProcessingEngineConfig::ProtocolTypeToString(PT_DummyMidiProtocol), PT_DummyMidiProtocol); // not yet implemented, feel free to step in
	m_ProtocolDrop->setColour(Label::textColourId, Colours::white);
	m_ProtocolDrop->setJustificationType(Justification::right);

	m_ProtocolLabel = std::make_unique<Label>();
	addAndMakeVisible(m_ProtocolLabel.get());
	m_ProtocolLabel->setText("Protocol " + String(PId), dontSendNotification);
	m_ProtocolLabel->setColour(Label::textColourId, Colours::white);
	m_ProtocolLabel->setJustificationType(Justification::right);
	m_ProtocolLabel->attachToComponent(m_ProtocolDrop.get(), true);

	m_IpEdit = std::make_unique<TextEditor>();
	m_IpEdit->addListener(this);
	addAndMakeVisible(m_IpEdit.get());
	m_IpEdit->setColour(Label::textColourId, Colours::white);

	m_ProtocolConfigEditButton = std::make_unique<TextButton>();
	m_ProtocolConfigEditButton->addListener(this);
	addAndMakeVisible(m_ProtocolConfigEditButton.get());
	m_ProtocolConfigEditButton->setButtonText("Configuration");
	m_ProtocolConfigEditButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
	m_ProtocolConfigEditButton->setColour(Label::textColourId, Colours::white);

	m_ProtocolConfigDialog = 0;

}

/**
 * Destructor
 */
ProtocolComponent::~ProtocolComponent()
{
}

/**
 * Overloaded method to resize contents
 */
void ProtocolComponent::resized()
{
	Component::resized();

	int xPos = getWidth() - UIS_ConfigButtonWidth - UIS_Margin_s;
	if (m_ProtocolConfigEditButton)
		m_ProtocolConfigEditButton->setBounds(xPos, 0, UIS_ConfigButtonWidth, UIS_ElmSize);

	int IpEditWidth = getWidth() - UIS_ProtocolLabelWidth - UIS_Margin_s - UIS_ProtocolDropWidth - UIS_Margin_s - UIS_ConfigButtonWidth - UIS_Margin_s;
	xPos = (UIS_ProtocolLabelWidth + UIS_Margin_s + UIS_ProtocolDropWidth);
	if (m_IpEdit)
		m_IpEdit->setBounds(xPos, 0, IpEditWidth, UIS_ElmSize);

	xPos = UIS_ProtocolLabelWidth;
	if (m_ProtocolDrop)
		m_ProtocolDrop->setBounds(UIS_ProtocolLabelWidth, 0, UIS_ProtocolDropWidth, UIS_ElmSize);
}

/**
 * Method to be called by child windows when closed, to enshure
 * button states and the internal
 * window object is invalidated to avoid accessviolation
 *
 * @param childWindow	The DialogWindow object that has been triggered to close
 */
void ProtocolComponent::childWindowCloseTriggered(DialogWindow* childWindow)
{
	if (m_parentComponent && m_ProtocolConfigDialog && childWindow == m_ProtocolConfigDialog.get())
	{
		ProcessingEngineConfig* config = m_parentComponent->GetConfig();

		if (config)
		{
			m_ProtocolConfigDialog->DumpConfig(*config);
			config->WriteConfiguration();

			m_parentComponent->RefreshEngine();
		}

		if (m_ProtocolConfigEditButton)
		{
			m_ProtocolConfigEditButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
			m_ProtocolConfigEditButton->setColour(Label::textColourId, Colours::white);
		}

		m_ProtocolConfigDialog.reset();
	}
}

/**
 * Method to dump the ui elemtents' current data to given data struct
 *
 * @param protocolData	The data struct to dump the ui values into
 */
void ProtocolComponent::DumpUItoConfigData(ProcessingEngineConfig::ProtocolData& protocolData)
{
	protocolData.Id = m_ProtocolId;

	if(m_IpEdit)
		protocolData.IpAddress = m_IpEdit->getText();
	if(m_ProtocolDrop)
		protocolData.Type = (ProtocolType)m_ProtocolDrop->getSelectedId();
}

/**
 * Method to update ui input elements with data to show from config
 *
 * @param protocolData	The config data structure to use to refresh what the ui displays
 * @return	The calculated theoretically required size for this component
 */
int ProtocolComponent::RefreshUIfromConfigData(const ProcessingEngineConfig::ProtocolData& protocolData)
{
	jassert(m_ProtocolId == protocolData.Id);

	if (m_ProtocolLabel)
		m_ProtocolLabel->setText("Protocol Id" + String(m_ProtocolId), dontSendNotification);
	if (m_ProtocolDrop)
		m_ProtocolDrop->setSelectedId(protocolData.Type, dontSendNotification);
	if (m_IpEdit)
		m_IpEdit->setText(protocolData.IpAddress);

	return UIS_ElmSize;
}

/**
 * Getter for this protocol components protocolId
 *
 * @return	This protocols' protocolId
 */
ProtocolId ProtocolComponent::GetProtocolId()
{
	return m_ProtocolId;
}

/**
 * Method to add the parent listener to this instance of ProtocolComponent.
 * This can afterwards be used for e.g. callbacks, etc.
 *
 * @param listener	The parent listener object to be used to invoke public methods from ('callback')
 */
void ProtocolComponent::AddListener(ProtocolGroupComponent* listener)
{
	m_parentComponent = listener;

	// In case a node id is set (!=0) we require it to be the listeners' node id, otherwise we have a severe misconfiguration!
	jassert((m_NodeId == 0) || (m_NodeId == listener->GetNodeId()));
}

/**
 * Overloaded method called by button objects on click events.
 * All internal button objects are registered to trigger this by calling
 * their ::addListener method with this object as argument
 *
 * @param button	The button object that has been clicked
 */
void ProtocolComponent::buttonClicked(Button* button)
{
	if (button == m_ProtocolConfigEditButton.get())
	{
		ToggleOpenCloseProtocolConfig(m_ProtocolConfigEditButton.get());
	}
}

/**
 * Overloaded method called by ComboBox objects on change events.
 * This is similar to ::buttonClicked but originates from inherited ComboBox::Listener
 *
 * @param comboBox	The comboBox object that has been changed
 */
void ProtocolComponent::comboBoxChanged(ComboBox* comboBox)
{
	ignoreUnused(comboBox);

	if (m_parentComponent)
	{
		m_parentComponent->TriggerParentConfigDump();
		m_parentComponent->RefreshEngine();
	}
}

/**
 * Overloaded method called by TextEditor objects on textchange events.
 * This is similar to ::buttonClicked but originates from inherited TextEditor::Listener
 *
 * @param textEdit	The textEdit object that has been changed
 */
void ProtocolComponent::textEditorTextChanged(TextEditor& textEdit)
{
	ignoreUnused(textEdit);
}

/**
 * Overloaded method called by TextEditor objects on keypress events.
 * This is similar to ::buttonClicked but originates from inherited TextEditor::Listener
 *
 * @param textEdit	The textEdit object that has received keypress
 */
void ProtocolComponent::textEditorReturnKeyPressed(TextEditor& textEdit)
{
	ignoreUnused(textEdit);

	if (m_parentComponent)
	{
		m_parentComponent->TriggerParentConfigDump();
		m_parentComponent->RefreshEngine();
	}
}

/**
 * Overloaded method called by TextEditor objects on keypress events.
 * This is similar to ::buttonClicked but originates from inherited TextEditor::Listener
 *
 * @param textEdit	The textEdit object that has received keypress
 */
void ProtocolComponent::textEditorEscapeKeyPressed(TextEditor& textEdit)
{
	ignoreUnused(textEdit);

	if (m_parentComponent)
		m_parentComponent->TriggerParentConfigRefresh();
}

/**
 * Overloaded method called by TextEditor objects on focuslost events.
 * This is similar to ::buttonClicked but originates from inherited TextEditor::Listener
 *
 * @param textEdit	The textEdit object that has lost focus
 */
void ProtocolComponent::textEditorFocusLost(TextEditor& textEdit)
{
	ignoreUnused(textEdit);

	if (m_parentComponent)
	{
		m_parentComponent->TriggerParentConfigDump();
		m_parentComponent->RefreshEngine();
	}
}

/**
 * Helper method to do the toggling of config dialog on respective buttonClicked
 *
 * @param button	The button object that has been clicked
 */
void ProtocolComponent::ToggleOpenCloseProtocolConfig(Button* button)
{
	jassert(button);
	if (!m_parentComponent || !button)
		return;

	ProcessingEngineConfig* config = m_parentComponent->GetConfig();

	// if the config dialog exists, this is a uncheck (close) click,
	// which means we have to process edited data
	if (m_ProtocolConfigDialog)
	{
		if (config)
		{
			m_ProtocolConfigDialog->DumpConfig(*config);
			config->WriteConfiguration();
			
			m_parentComponent->RefreshEngine();
		}

		button->setColour(TextButton::buttonColourId, Colours::dimgrey);
		button->setColour(Label::textColourId, Colours::white);

		m_ProtocolConfigDialog.reset();
	}
	// otherwise we have to create the dialog and show it
	else
	{
		if (config)
		{
			ProtocolType protocolType = config->GetProtocolData(m_NodeId, m_ProtocolId).Type;

			String dialogTitle = ProcessingEngineConfig::ProtocolTypeToString(protocolType) + " protocol configuration (Node Id" + String(m_NodeId) + ", Protocol Id" + String(m_ProtocolId) + ")";

			m_ProtocolConfigDialog = std::make_unique<ProtocolConfigWindow>(dialogTitle, Colours::dimgrey, false, m_NodeId, m_ProtocolId, protocolType);
			m_ProtocolConfigDialog->AddListener(this);
			m_ProtocolConfigDialog->setResizable(true, true);
			m_ProtocolConfigDialog->setUsingNativeTitleBar(true);
			m_ProtocolConfigDialog->setVisible(true);
			m_ProtocolConfigDialog->SetConfig(*config);
#if defined JUCE_IOS ||  defined JUCE_ANDROID
            m_ProtocolConfigDialog->setFullScreen(true);
#else
			const std::pair<int, int> size = m_ProtocolConfigDialog->GetSuggestedSize();
			m_ProtocolConfigDialog->setResizeLimits(size.first, size.second, size.first, size.second);
			m_ProtocolConfigDialog->setBounds(Rectangle<int>(getScreenBounds().getX(), getScreenBounds().getY(), size.first, size.second));
#endif
			button->setColour(TextButton::buttonColourId, Colours::lightblue);
			button->setColour(Label::textColourId, Colours::dimgrey);
		}
	}
}
