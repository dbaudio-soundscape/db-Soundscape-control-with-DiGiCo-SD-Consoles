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

#include "../JuceLibraryCode/JuceHeader.h"

#include "../Common.h"
#include "../ProcessingEngine.h"

// Fwd. Declarations
class ProtocolComponent;
class ProtocolConfigWindow;

/**
 * Class ProtocolConfigComponent_Abstract is a container used to hold the GUI controls for modifying the app configuration.
 */
class ProtocolConfigComponent_Abstract : public Component,
	public Button::Listener
{
public:
	ProtocolConfigComponent_Abstract();
	~ProtocolConfigComponent_Abstract();

	//==============================================================================
	virtual bool				DumpActiveHandlingUsed() = 0;
	virtual Array<RemoteObject> DumpActiveRemoteObjects() = 0;
	virtual std::pair<int, int> DumpProtocolPorts();
	virtual void				SetActiveHandlingUsed(bool active);
	virtual void				FillActiveRemoteObjects(const Array<RemoteObject>& Objs) = 0;
	virtual void				FillProtocolPorts(const std::pair<int, int>& ports);

	//==============================================================================
	virtual const std::pair<int, int> GetSuggestedSize() = 0;

	//==============================================================================
	virtual void AddListener(ProtocolConfigWindow* listener);

private:
	void paint(Graphics&) override;
	void resized() override = 0;

	void buttonClicked(Button* button) override;

protected:
	std::unique_ptr<Label>		m_HostPortLabel;		/**< Label as description of host port edit. */
	std::unique_ptr<TextEditor>	m_HostPortEdit;			/**< Edit for editing of host port. */
	std::unique_ptr<Label>		m_ClientPortLabel;		/**< Label as description of client port edit. */
	std::unique_ptr<TextEditor> m_ClientPortEdit;		/**< Edit for editing of client port. */

	std::unique_ptr<Label>		m_Headline;				/**< Headlining Label for complete object list section. */
	std::unique_ptr<TextButton> m_applyConfigButton;	/**< Button to apply edited values to configuration and leave. */
	ProtocolConfigWindow*		m_parentListener;		/**< Parent that needs to be notified when this window self-destroys. */

};

/**
 * Class BasicProtocolConfigComponent is a container used to hold the GUI controls
 * for modifying the protocol configuration in a very basic way.
 */
class BasicProtocolConfigComponent : public ProtocolConfigComponent_Abstract,
	public TextEditor::Listener
{
public:
	BasicProtocolConfigComponent();
	~BasicProtocolConfigComponent();

	//==============================================================================
	bool				DumpActiveHandlingUsed() override;
	Array<RemoteObject> DumpActiveRemoteObjects() override;
	void				SetActiveHandlingUsed(bool active) override;
	void				FillActiveRemoteObjects(const Array<RemoteObject>& Objs) override;

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize() override;

	//==============================================================================
	void AddListener(ProtocolConfigWindow* listener) override;

private:
	virtual void resized() override;

	void textEditorFocusLost(TextEditor &) override;
	void textEditorReturnKeyPressed(TextEditor &) override;

	void buttonClicked(Button* button) override;

	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjEnableChecks;		/**< Enable checkboxes for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<Label>>			m_RemObjNameLabels;			/**< Name labels for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<TextEditor>>		m_RemObjActiveChannelEdits;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<TextEditor>>		m_RemObjActiveRecordEdits;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */

	std::unique_ptr<Label>			m_EnableHeadlineLabel;		/**< Headlining Label for enable checks. */
	std::unique_ptr<Label>			m_ChannelHeadlineLabel;		/**< Headlining Label for channel range edits. */
	std::unique_ptr<Label>			m_RecordHeadlineLabel;		/**< Headlining Label for record range edits. */

	std::unique_ptr<ToggleButton>	m_UseActiveHandlingCheck;	/**< Checkbox to toggle active remote object handling setting. */
	std::unique_ptr<Label>			m_UseActiveHandlingLabel;	/**< Descriptive label for active remote object handling checkbox. */

};

/**
 * Class OSCProtocolConfigComponent is a container used to hold the GUI controls
 * specifically used to configure d&b OSC protocol configuration.
 */
class OSCProtocolConfigComponent : public ProtocolConfigComponent_Abstract,
	public TextEditor::Listener
{
public:
	OSCProtocolConfigComponent();
	~OSCProtocolConfigComponent();

	//==============================================================================
	bool				DumpActiveHandlingUsed() override;
	Array<RemoteObject> DumpActiveRemoteObjects() override;
	void				FillActiveRemoteObjects(const Array<RemoteObject>& Objs) override;

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize() override;

	//==============================================================================
	void AddListener(ProtocolConfigWindow* listener) override;

private:
	virtual void resized() override;

	virtual void textEditorFocusLost(TextEditor &) override;
	virtual void textEditorReturnKeyPressed(TextEditor &) override;

	void buttonClicked(Button* button) override;

	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjEnableChecks;		/**< Enable checkboxes for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<Label>>			m_RemObjNameLabels;			/**< Name labels for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<TextEditor>>		m_RemObjActiveChannelEdits;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjMappingArea1Checks;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjMappingArea2Checks;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjMappingArea3Checks;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */
	std::map<int, std::unique_ptr<ToggleButton>>	m_RemObjMappingArea4Checks;	/**< Channel Range editing fields for all remote object to be configured/listed on ui. */

	std::unique_ptr<Label>		m_EnableHeadlineLabel;		/**< Headlining Label for enable checks. */
	std::unique_ptr<Label>		m_ChannelHeadlineLabel;		/**< Headlining Label for channel range edits. */
	std::unique_ptr<Label>		m_MappingsHeadlineLabel;	/**< Headlining Label for mapping checks. */
	std::unique_ptr<Label>		m_Mapping1HeadlineLabel;	/**< Headlining Label for mapping1 checks. */
	std::unique_ptr<Label>		m_Mapping2HeadlineLabel;	/**< Headlining Label for mapping2 checks. */
	std::unique_ptr<Label>		m_Mapping3HeadlineLabel;	/**< Headlining Label for mapping3 checks. */
	std::unique_ptr<Label>		m_Mapping4HeadlineLabel;	/**< Headlining Label for mapping4 checks. */

};

/**
 * Class ProtocolConfigWindow provides a window that embedds a ProtocolConfigComponent_Abstract
 */
class ProtocolConfigWindow : public DialogWindow
{
public:
	//==============================================================================
	ProtocolConfigWindow(const String &name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, NodeId NId, ProtocolId PId,
						 ProtocolType Type, bool addToDesktop = true);
	~ProtocolConfigWindow();

	//==============================================================================
	bool DumpConfig(ProcessingEngineConfig& config);
	void SetConfig(const ProcessingEngineConfig& config);
	void OnEditingFinished();

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize();

	//==============================================================================
	void AddListener(ProtocolComponent* listener);

private:
	void closeButtonPressed() override;

	std::unique_ptr<ProtocolConfigComponent_Abstract>	m_configComponent;	/**< Actual config content component to reside in window. */
	ProtocolComponent*		m_parentListener;	/**< Parent that needs to be notified when this window self-destroys. */
	NodeId					m_NId;				/**< ID of the node this config dialog refers to. */
	ProtocolId				m_PId;				/**< ID of the nodes' protocl this config dialog refers to. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProtocolConfigWindow)
};
