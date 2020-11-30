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
#include "ConfigComponents/ObjectHandlingConfigComponents.h"

// Fwd. Declarations
class MainRemoteProtocolBridgeComponent;
class ProcessingEngine;
class ProcessingEngineConfig;
class ProtocolGroupComponent;


/**
 * Class ProtocolGroupComponent is a groupbox component to be used to hold
 * components dedicated to represent/modify configuration of nodes.
 */
class NodeComponent   : public GroupComponent,
						public TextEditor::Listener,
						public ComboBox::Listener,
						public Button::Listener
{
public:
    //==============================================================================
    NodeComponent(NodeId NId);
    ~NodeComponent();

    //==============================================================================
    void resized() override;

	//==============================================================================
	void childWindowCloseTriggered(DialogWindow* childWindow);

	//==============================================================================
	void DumpUItoConfig(ProcessingEngineConfig& config);
    int RefreshUIfromConfig(const ProcessingEngineConfig& config);
	void ToggleOpenCloseObjectHandlingConfig(Button* button);
	void TriggerParentConfigDump();
	void TriggerParentConfigRefresh();

	//==============================================================================
	bool AddDefaultProtocol(const ProtocolGroupComponent* targetPGC);
	bool RemoveProtocol(const NodeId& NId, const ProtocolId& PId);

	//==============================================================================
	ProcessingEngineConfig* GetConfig();
	ProcessingEngine* GetEngine();
	NodeId GetNodeId();
    
    //==============================================================================
    void AddListener(MainRemoteProtocolBridgeComponent* listener);

private:
    //==============================================================================
	bool AddDefaultProtocolA();
	bool AddDefaultProtocolB();

    NodeId											m_NodeId;				/**< Id of the node this component manages configuration for. */
    																		
	MainRemoteProtocolBridgeComponent*				m_parentComponent;		/**< The parent component that needs to be triggered regarding callbacks. */
																			
	std::unique_ptr<ProtocolGroupComponent>			m_protocolsAComponent;	/**< Group component holding the protocol components of type A for this node component. */
	std::unique_ptr<ProtocolGroupComponent>			m_protocolsBComponent;	/**< Group component holding the protocol components of type B for this node component. */
																			
	std::unique_ptr<Label>							m_NodeModeLabel;		/**< Descriptive label for the node operation mode selection. */
	std::unique_ptr<ComboBox>						m_NodeModeDrop;			/**< Dropdown for selection of the node operation mode of this node. */
																			
	std::unique_ptr<TextButton>						m_OHMConfigEditButton;	/**< Button to invoke extended object handling mode configuration dialog. */
	std::unique_ptr<ObjectHandlingConfigWindow>		m_OHMConfigDialog;		/**< Member to hold instance of object handling mode config dialog that is created on demand. */

	void buttonClicked(Button* button) override;
	void comboBoxChanged(ComboBox* comboBox) override;
	void textEditorTextChanged(TextEditor& textEdit) override;
	void textEditorReturnKeyPressed(TextEditor& textEdit) override;
	void textEditorEscapeKeyPressed(TextEditor& textEdit) override;
	void textEditorFocusLost(TextEditor& textEdit) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeComponent)
};
