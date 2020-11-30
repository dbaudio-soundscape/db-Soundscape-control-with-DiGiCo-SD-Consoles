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
class NodeComponent;
class ProtocolComponent;
class ProtocolConfigWindow;


/**
 * Class ProtocolGroupComponent is a groupbox component to be used to hold
 * components dedicated to represent/modify configuration of protocols of a
 * common role in a node.
 */
class ProtocolGroupComponent   :	public GroupComponent,
									public Button::Listener
{
public:
    //==============================================================================
    ProtocolGroupComponent();
    ~ProtocolGroupComponent();

    //==============================================================================
    void resized() override;

	//==============================================================================
	void DumpUItoConfig(ProcessingEngineConfig& config);
	int RefreshUIfromConfig(const NodeId& parentNodeId, const Array<ProtocolId> protocolIds, const ProcessingEngineConfig& config);
	void TriggerParentConfigDump();
	void TriggerParentConfigRefresh();
	
	//==============================================================================
	ProcessingEngineConfig* GetConfig();
	ProcessingEngine* GetEngine();
	NodeId GetNodeId();
	const Array<ProtocolId>& GetProtocolIds();
    
    //==============================================================================
    void AddListener(NodeComponent* listener);

private:
    //==============================================================================
	NodeId														m_NodeId;				/**< Id of the node this component manages configuration for. */
	Array<ProtocolId>											m_ProtocolIds;			/**< The protocol id of this protocol config component. */
    
	NodeComponent*												m_parentComponent;		/**< The parent of this config component for callback delivery. */

	std::unique_ptr<ImageButton>								m_AddProtocolButton;	/**< Button to trigger adding another protocol to this group of protocols. */
	std::unique_ptr<ImageButton>								m_RemoveProtocolButton;	/**< Button to trigger removing the last protocol of this group of protocols. */

	std::map<ProtocolId, std::unique_ptr<ProtocolComponent>>	m_ProtocolComponents;	/**< Map of protocols in this group of protocols. */

	void buttonClicked(Button* button) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProtocolGroupComponent)
};


//==============================================================================
// Class ProtocolComponent
//==============================================================================
/**
 * Class constructor.
 */
class ProtocolComponent : public Component,
	public TextEditor::Listener,
	public ComboBox::Listener,
	public Button::Listener
{
public:
	//==============================================================================
	ProtocolComponent(const NodeId& NId, const ProtocolId& PId);
	~ProtocolComponent();

	//==============================================================================
	void resized() override;

	//==============================================================================
	void childWindowCloseTriggered(DialogWindow* childWindow);

	//==============================================================================
	void DumpUItoConfigData(ProcessingEngineConfig::ProtocolData& protocolData);
	int RefreshUIfromConfigData(const ProcessingEngineConfig::ProtocolData& protocolData);
	void ToggleOpenCloseProtocolConfig(Button* button);
	ProtocolId GetProtocolId();

	//==============================================================================
	void AddListener(ProtocolGroupComponent* listener);

private:

	NodeId									m_NodeId;					/**< The parent node id of this protocol config component. */
	ProtocolId								m_ProtocolId;				/**< The protocol id of this protocol config component. */

	ProtocolGroupComponent*					m_parentComponent;			/**< The parent of this config component for callback delivery. */

	std::unique_ptr<Label>					m_ProtocolLabel;			/**< Label for a descriptive string for this protocol component. */
	std::unique_ptr<ComboBox>				m_ProtocolDrop;				/**< Dropdown for protocol type selection for this protocol config component. */
	std::unique_ptr<TextEditor>				m_IpEdit;					/**< Edit for IP address to use for this protocol. */

	std::unique_ptr<TextButton>				m_ProtocolConfigEditButton;	/**< Button to invoke extended protocol configuration editing dialog. */
	std::unique_ptr<ProtocolConfigWindow>	m_ProtocolConfigDialog;		/**< Extended protocol configuration dialog, created on demand and required as member to be able to trigger closing. */


	void buttonClicked(Button* button) override;
	void comboBoxChanged(ComboBox* comboBox) override;
	void textEditorTextChanged(TextEditor& textEdit) override;
	void textEditorReturnKeyPressed(TextEditor& textEdit) override;
	void textEditorEscapeKeyPressed(TextEditor& textEdit) override;
	void textEditorFocusLost(TextEditor& textEdit) override;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProtocolComponent)
};