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

#include "LoggingTarget_Interface.h"
#include "ProcessingEngine.h"

// Fwd. Declarations
class MainRemoteProtocolBridgeComponent;
class LoggingWindow;


/**
 * Class PlotComponent visualizes message receive rate over time as a 2D plot.
 */
class PlotComponent : public Component,
	private Timer
{	
	enum PlotConstants
	{
		hRange				= 20000,	// 20s on horizontal axis	
		hStepping			= 200,		// 200ms resolution
		hUserVisuStepping	= 1000,		// User is presented with plot legend msg/s to have something more legible than 200ms
		vRange				= 2			// 10 msg/s default on vertical axis (2 msg per 200ms interval)
	};

public:
	PlotComponent();
	~PlotComponent();

	//==============================================================================
	void IncreaseCount(NodeId NId, ProtocolId PId);

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;

private:
	void timerCallback() override;

private:
	int	m_hRange;		/**< Horizontal max plot value (value range) in ms. We use the range from left (0) to right (m_hRange) to plot data. */
	int	m_hStepping;	/**< Horizontal step with in ms. */
	int	m_vRange;		/**< Vertical max plot value (value range). We use the range from bottom (0) to top (m_vRange) where m_vRange 
						*	is dynamically adjusted regarding incoming data to plot. */

	std::map<ProtocolId, int>	m_currentMsgPerProtocol;	/**< Map to help counting messages per protocol in current interval. This is processed every timer callback to update plot data. */

	std::map<int, std::vector<float>>	m_plotData;	/**< Data for plotting. Primitive vector of floats that represents the msg count per hor. step width. */
	std::map<int, Colour> m_protocolPlotColours;	/** Individual colour for each protocol plot. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlotComponent)
};

/**
 * Class GlobalConfigComponent is a container used to hold the GUI controls for modifying the app configuration.
 */
class LoggingComponent : public Component,
	public TextEditor::Listener,
	public Button::Listener,
	public ComboBox::Listener,
	public LoggingTarget_Interface,
	private Timer
{
	enum LoggingMode
	{
		LM_Text = 1,	/**< Use a text window to print message data. */
		LM_Graph,		/**< Use a graph to visualize message count/performance. */
		LM_INVALID		/**< Invalid logging mode. */
	};

public:
	LoggingComponent();
	~LoggingComponent();


	//==============================================================================
	void AddLogData(NodeId NId, ProtocolId SenderPId, ProtocolType SenderType, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;
	void SetLoggingMode(LoggingMode mode);

	//==============================================================================
	void AddListener(LoggingWindow* listener);

private:
	//==============================================================================
	void AddMessage(String messageString);
	void AddMessages(const Array<String>& messageStrings);

	//==============================================================================
	static String		LogModeToString(LoggingMode lm);
	static LoggingMode	LogModeFromString(String mode);

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;

	void buttonClicked(Button* button) override;
	void comboBoxChanged(ComboBox* comboBox) override;

	void timerCallback() override;

private:
	LoggingWindow*							m_parentListener;	/**< Parent that needs to be notified when this window self-destroys. */

	std::unique_ptr<CodeEditorComponent>	m_textBox;			/**< The actual component to show log text within window. */
	CodeDocument							m_doc;				/**< Document object used by codeeditorcomponent for content. */

	std::unique_ptr<PlotComponent>			m_plotBox;			/**< The actual component to show logging graph plot. */

	LoggingMode								m_mode;				/**< The current logging UI mode to use. */

	std::unique_ptr<ComboBox>				m_LogModeDrop;		/**< Dropdown for logging mode selection. */
	std::unique_ptr<TextButton>				m_closeButton;		/**< Button to close the window - identical to Windows titlebar close functionality. */

	std::vector<String>						m_loggingQueue;		/**< List of message strings to be printed on next flush timer callback. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoggingComponent)
};

/**
 * Class LoggingWindow provides a window for showing textbased logging entries
 */
class LoggingWindow : public DialogWindow,
	public LoggingTarget_Interface
{
public:
	//==============================================================================
	LoggingWindow(const String& name,
		Colour backgroundColour,
		bool escapeKeyTriggersCloseButton,
		bool addToDesktop = true);
	~LoggingWindow();

	//==============================================================================
	void AddLogData(NodeId NId, ProtocolId SenderPId, ProtocolType SenderType, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

	//==============================================================================
	void AddListener(MainRemoteProtocolBridgeComponent* listener);

	void closeButtonPressed() override;

private:
	std::unique_ptr<LoggingComponent>	m_loggingComponent;	/**< Actual logging content component to reside in window. */
	MainRemoteProtocolBridgeComponent*	m_parentListener;	/**< Parent that needs to be notified when this window self-destroys. */

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoggingWindow)
};
