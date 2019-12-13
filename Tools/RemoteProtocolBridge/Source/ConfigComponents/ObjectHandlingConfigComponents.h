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
class NodeComponent;
class ObjectHandlingConfigWindow;

/**
 * Class ObjectHandlingConfigComponent_Abstract is a container used to hold the GUI controls for modifying the node object handling configuration.
 */
class ObjectHandlingConfigComponent_Abstract : public Component,
	public Button::Listener
{
public:
	ObjectHandlingConfigComponent_Abstract(ObjectHandlingMode mode = OHM_Invalid);
	~ObjectHandlingConfigComponent_Abstract();

	//==============================================================================
	virtual ProcessingEngineConfig::ObjectHandlingData DumpObjectHandlingData() = 0;
	virtual void FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData) = 0;

	//==============================================================================
	virtual const std::pair<int, int> GetSuggestedSize() = 0;

	//==============================================================================
	virtual void AddListener(ObjectHandlingConfigWindow* listener);

private:
	virtual void paint(Graphics&) override;
	virtual void resized() override = 0;

	void buttonClicked(Button* button) override;

protected:
	std::unique_ptr<Label>			m_Headline;				/**< Headlining Label for complete object list section. */
	std::unique_ptr<TextButton>		m_applyConfigButton;	/**< Button to apply edited values to configuration and leave. */
	ObjectHandlingConfigWindow*	m_parentListener;		/**< Parent that needs to be notified when this window self-destroys. */
	ObjectHandlingMode				m_mode;					/**< The mode of this OH object. */

};

/**
 * Class OHNoConfigComponent is a container used to hold the GUI
 * for Object Handling mode modules that do not require configuration.
 */
class OHNoConfigComponent : public ObjectHandlingConfigComponent_Abstract,
	public TextEditor::Listener
{
public:
	OHNoConfigComponent(ObjectHandlingMode mode);
	~OHNoConfigComponent();

	//==============================================================================
	ProcessingEngineConfig::ObjectHandlingData DumpObjectHandlingData() override;
	void FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData) override;

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize() override;

private:
	void resized() override;

};

/**
 * Class OHMultiplexAtoBConfigComponent is a container used to hold the GUI
 * specifically used to configure configuration of mux n A to m B protocol channel counts.
 */
class OHMultiplexAtoBConfigComponent : public ObjectHandlingConfigComponent_Abstract,
	public TextEditor::Listener
{
public:
	OHMultiplexAtoBConfigComponent(ObjectHandlingMode mode);
	~OHMultiplexAtoBConfigComponent();

	//==============================================================================
	ProcessingEngineConfig::ObjectHandlingData DumpObjectHandlingData() override;
	void FillObjectHandlingData(const ProcessingEngineConfig::ObjectHandlingData& ohData) override;

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize() override;

private:
	virtual void resized() override;

	virtual void textEditorFocusLost(TextEditor &) override;
	virtual void textEditorReturnKeyPressed(TextEditor &) override;

	std::unique_ptr<Label>		m_CountALabel;	/**< Headlining Label for enable checks. */
	std::unique_ptr<TextEditor>	m_CountAEdit;	/**< Headlining Label for channel range edits. */
	std::unique_ptr<Label>		m_CountBLabel;	/**< Headlining Label for mapping checks. */
	std::unique_ptr<TextEditor>	m_CountBEdit;	/**< Headlining Label for mapping1 checks. */

};

/**
 * Class ObjectHandlingConfigWindow provides a window that embedds an ObjectHandlingConfigComponent_Abstract
 */
class ObjectHandlingConfigWindow : public DialogWindow
{
public:
	//==============================================================================
	ObjectHandlingConfigWindow(const String &name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, NodeId NId, ObjectHandlingMode mode,
							   bool addToDesktop = true);
	~ObjectHandlingConfigWindow();

	//==============================================================================
	bool DumpConfig(ProcessingEngineConfig& config);
	void SetConfig(const ProcessingEngineConfig& config);
	void OnEditingFinished();

	//==============================================================================
	const std::pair<int, int> GetSuggestedSize();

	//==============================================================================
	void AddListener(NodeComponent* listener);

private:
	void closeButtonPressed() override;

	std::unique_ptr<ObjectHandlingConfigComponent_Abstract>	m_configComponent;	/**< Actual config content component to reside in window. */
	NodeComponent*									m_parentListener;	/**< Parent that needs to be notified when this window self-destroys. */
	NodeId											m_NId;				/**< ID of the node this config dialog refers to. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectHandlingConfigWindow)
};