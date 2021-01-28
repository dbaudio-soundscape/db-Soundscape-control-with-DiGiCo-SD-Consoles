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

#include "LoggingWindow.h"

#include "LoggingTarget_Interface.h"
#include "MainRemoteProtocolBridgeComponent.h"
#include "RemoteProtocolBridgeCommon.h"

#include "ProtocolProcessor/OSCProtocolProcessor/OSCProtocolProcessor.h"
#include "ProtocolProcessor/OCAProtocolProcessor/OCAProtocolProcessor.h"


constexpr int NODE = INT_MAX;

//==============================================================================
// Class PlotComponent
//==============================================================================
/**
 * Class constructor.
 */
PlotComponent::PlotComponent()
{
	m_hRange = hRange;
	m_hStepping = hStepping;
	m_vRange = vRange;

	m_plotData[NODE].resize(m_hRange/m_hStepping);

	/*fill plotdata with default zero*/
	for (int i = 0; i < (m_hRange / m_hStepping); ++i)
		m_plotData[NODE].at(i) = 0;

	startTimer(m_hStepping);
}

/**
 * Destructor
 */
PlotComponent::~PlotComponent()
{

}

/**
 * Method to increase the received message counter per current interval for given Node and Protocol.
 * Currently we simply sum up all protocol traffic per node.
 *
 * @param NId	The node id the count shall be increased for
 * @param PId	The node protocol id the count shall be increased for
 */
void PlotComponent::IncreaseCount(NodeId NId, ProtocolId PId)
{
	ignoreUnused(NId);
	m_currentMsgPerProtocol[PId]++;

	if (!m_protocolPlotColours.count(int(PId)))
	{
		float r = float(rand()) / float(RAND_MAX);
		float g = float(rand()) / float(RAND_MAX);
		float b = float(rand()) / float(RAND_MAX);
		float a = 170.0f;
		m_protocolPlotColours[int(PId)] = Colour::fromFloatRGBA(r, g, b, a);
	}
}

/**
 * Reimplemented from Timer - called every timeout timer
 * We do the processing of count of messages per node during last interval into our plot data for next paint here.
 */
void PlotComponent::timerCallback()
{
	// accumulate all protocol msgs as well as handle individual protocol msg counts
	int msgCount = 0;
	for (const std::pair<const ProtocolId, int> &msgCountPerProtocol : m_currentMsgPerProtocol)
	{
		if (m_plotData[int(msgCountPerProtocol.first)].size() != m_plotData[NODE].size())
			m_plotData[int(msgCountPerProtocol.first)].resize(m_plotData[NODE].size());

		std::vector<float> shiftedVector(m_plotData[int(msgCountPerProtocol.first)].begin() + 1, m_plotData[int(msgCountPerProtocol.first)].end());
		m_plotData[int(msgCountPerProtocol.first)].swap(shiftedVector);
		m_plotData[int(msgCountPerProtocol.first)].push_back(float(msgCountPerProtocol.second));

		msgCount += msgCountPerProtocol.second;

		m_currentMsgPerProtocol[msgCountPerProtocol.first] = 0;
	}

	std::vector<float> shiftedVector(m_plotData[NODE].begin() + 1, m_plotData[NODE].end());
	m_plotData[NODE].swap(shiftedVector);
	m_plotData[NODE].push_back(float(msgCount));

	// Adjust our vertical plotting range to have better visu when large peaks would get out of scope
	m_vRange = int(round(std::max(float(vRange), *std::max_element(m_plotData[NODE].begin(), m_plotData[NODE].end()))));

	repaint();
}

/**
 * Reimplemented from Component. We do the actual plot drawing here, based on our
 * cyclically processed plotdata.
 *
 * @param g	The graphics object to use for painting
 */
void PlotComponent::paint(Graphics &g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(CodeEditorComponent::ColourIds::backgroundColourId).overlaidWith(getLookAndFeel().findColour(CodeEditorComponent::ColourIds::lineNumberBackgroundId)));

	int width  = getWidth();
	int height = getHeight();

	// fill margin values to floats to enhance code readability
	float ms  = float(UIS_Margin_s);
	float mm  = float(UIS_Margin_m);
	float ml  = float(UIS_Margin_l);
	float mxl = float(UIS_Margin_xl);

	float plotWidth	 = width - 2 * mxl;
	float plotHeight = height - 2 * mxl;

	Rectangle<float> plotArea(mxl, mxl, plotWidth, plotHeight);

	g.setColour(getLookAndFeel().findColour(CodeEditorComponent::ColourIds::backgroundColourId));
	g.fillRect(plotArea);

	if(!m_plotData[NODE].empty())
	{
		float plotOrigX		  = mxl;
		float plotOrigY		  = float(mxl + plotHeight);
		float plotStepWidthPx = float(plotWidth) / float((m_plotData[NODE].size() > 0 ? m_plotData[NODE].size() : 1) - 1);

		g.setColour(getLookAndFeel().findColour(CodeEditorComponent::ColourIds::defaultTextColourId));
		g.drawLine(Line<float>(plotOrigX, plotOrigY, plotOrigX, plotOrigY - plotHeight));
		g.drawLine(Line<float>(plotOrigX, plotOrigY, plotOrigX + plotWidth, plotOrigY));

		float vUserRange = float(m_vRange) * (float(hUserVisuStepping) / float(m_hStepping));
		g.drawText("msg/s", Rectangle<float>(ms, mm, 3 * ml, ml), Justification::topLeft, true);
		g.drawText(String(vUserRange), Rectangle<float>(ms * 0.5f, ms + mxl, mxl - ms * 0.5f, mm), Justification::centred, true);
		g.drawText(String(vUserRange * 0.5f), Rectangle<float>(ms * 0.5f, ms + plotOrigY - (plotHeight * 0.5f), mxl - ms * 0.5f, mm),
				   Justification::centred, true);
		g.drawText(String(0), Rectangle<float>(ms * 0.5f, ms + plotOrigY, mxl - ms * 0.5f, mm), Justification::centred, true);
		g.drawLine(Line<float>(plotOrigX - mm, plotOrigY - plotHeight, plotOrigX, plotOrigY - plotHeight));
		g.drawLine(Line<float>(plotOrigX - ms, plotOrigY - (plotHeight * 0.75f), plotOrigX, plotOrigY - (plotHeight * 0.75f)));
		g.drawLine(Line<float>(plotOrigX - mm, plotOrigY - (plotHeight * 0.5f), plotOrigX, plotOrigY - (plotHeight * 0.5f)));
		g.drawLine(Line<float>(plotOrigX - ms, plotOrigY - (plotHeight * 0.25f), plotOrigX, plotOrigY - (plotHeight * 0.25f)));
		g.drawLine(Line<float>(plotOrigX - mm, plotOrigY, plotOrigX, plotOrigY));

		int hTime = int(float(m_plotData[NODE].size()) * float(m_hStepping) * 0.001);
		g.drawText(String(hTime), Rectangle<float>(ms + plotOrigX, mm + plotOrigY, mxl, mm), Justification::bottomLeft, true);
		g.drawText(String(hTime * 0.5f), Rectangle<float>(ms + plotOrigX + (plotWidth * 0.5f), mm + plotOrigY, mxl, mm), Justification::bottomLeft, true);
		g.drawText(String(0), Rectangle<float>(ms + plotOrigX + plotWidth, mm + plotOrigY, mxl, mm), Justification::bottomLeft, true);
		g.drawText("time (s)", Rectangle<float>(mxl + plotWidth - 2 * mxl, plotOrigY, 2 * ml, ml), Justification::bottomRight, true);
		g.drawLine(Line<float>(plotOrigX, plotOrigY, plotOrigX, plotOrigY + mm));
		g.drawLine(Line<float>(plotOrigX + (plotWidth * 0.25f), plotOrigY, plotOrigX + (plotWidth * 0.25f), plotOrigY + ms));
		g.drawLine(Line<float>(plotOrigX + (plotWidth * 0.5f), plotOrigY, plotOrigX + (plotWidth * 0.5f), plotOrigY + mm));
		g.drawLine(Line<float>(plotOrigX + (plotWidth * 0.75f), plotOrigY, plotOrigX + (plotWidth * 0.75f), plotOrigY + ms));
		g.drawLine(Line<float>(plotOrigX + plotWidth, plotOrigY, plotOrigX + plotWidth, plotOrigY + mm));

		float legendPosX = plotOrigX + mxl;
		g.drawText("Total", Rectangle<float>(legendPosX, mm, 2 * ml, mm), Justification::centred, true);
		legendPosX += 2 * ml;
		g.drawLine(Line<float>(legendPosX, mm + ms, legendPosX + ml, mm + ms));
		legendPosX += 3 * ml;
		for (const std::pair<const int, Colour> &protoCol : m_protocolPlotColours)
		{
			g.setColour(protoCol.second);
			g.drawText("PId"+String(protoCol.first), Rectangle<float>(legendPosX, mm, 2 * ml, mm), Justification::centred, true);
			legendPosX += 2 * ml;
			g.drawLine(Line<float>(legendPosX, mm + ms, legendPosX + ml, mm + ms));
			legendPosX += 3 * ml;
		}

		float newPointX = 0;
		float newPointY = 0;
		float vFactor	= float(plotHeight) / float(m_vRange > 0 ? m_vRange : 1);

		Path path;	
		for (const std::pair<int, std::vector<float>> pd : m_plotData)
		{
			if (pd.first == NODE)
				// Graph curve colour for accumulated data	
				g.setColour(getLookAndFeel().findColour(CodeEditorComponent::ColourIds::defaultTextColourId));
			else
				//Graph curve colour for individual protocols
				g.setColour(m_protocolPlotColours.at(pd.first));

			path.startNewSubPath(Point<float>(plotOrigX, plotOrigY - (m_plotData[pd.first].front()) * vFactor));
			for (int i = 1; i < m_plotData[pd.first].size(); ++i)
			{
				newPointX = plotOrigX + float(i) * plotStepWidthPx;
				newPointY = plotOrigY - (m_plotData[pd.first].at(i) * vFactor);

				path.lineTo(Point<float>(newPointX, newPointY));
			}
			g.strokePath(path, PathStrokeType(2));
			path.closeSubPath();
			path.clear();
		}
	}
}

/**
 * Reimplemented from Component.
 * Currently this only forwards to base implementation.
 */
void PlotComponent::resized()
{
	Component::resized();
}


//==============================================================================
// Class LoggingComponent
//==============================================================================
/**
 * Class constructor.
 */
LoggingComponent::LoggingComponent()
{
	m_parentListener = 0;

	m_textBox = std::make_unique<CodeEditorComponent>(m_doc, nullptr);
	addChildComponent(m_textBox.get());

	m_plotBox = std::make_unique<PlotComponent>();
	addChildComponent(m_plotBox.get());

	m_LogModeDrop = std::make_unique<ComboBox>();
	m_LogModeDrop->addListener(this);
	addAndMakeVisible(m_LogModeDrop.get());
	m_LogModeDrop->addItem(LogModeToString(LM_Text), LM_Text);
	m_LogModeDrop->addItem(LogModeToString(LM_Graph), LM_Graph);
	m_LogModeDrop->setColour(Label::textColourId, Colours::white);
	m_LogModeDrop->setJustificationType(Justification::right);
	SetLoggingMode(LM_Graph);

	m_closeButton = std::make_unique<TextButton>("Close");
	addAndMakeVisible(m_closeButton.get());
	m_closeButton->addListener(this);

	startTimer(ET_LoggingFlushRate);
}

/**
 * Destructor
 */
LoggingComponent::~LoggingComponent()
{
	removeChildComponent(m_textBox.get());
	m_textBox.reset();
}

/**
 * Sets a logging message to logging queue
 *
 * @param messageString	The message text string to add to logging queue
 */
void LoggingComponent::AddMessage(String messageString)
{
	if (m_mode==LM_Text)
	{
		m_loggingQueue.push_back(messageString);
	}
}

/**
 * Sets multiple logging messages to logging queue
 *
 * @param messageStrings	The message text strings to add to logging queue
 */
void LoggingComponent::AddMessages(const Array<String>& messageStrings)
{
	if (m_mode == LM_Text)
	{
		for (const String& message : messageStrings)
		{
			m_loggingQueue.push_back(message);
		}
	}
}

/**
 * Reimplemented from Timer - called every timeout timer
 * 
 * Iterates over logging queue and adds all queued messages
 * to end of text area and terminates with a newline
 */
void LoggingComponent::timerCallback()
{
	if (m_textBox && m_mode == LM_Text)
	{
		m_textBox->moveCaretToEnd(false);
		for (const String& message : m_loggingQueue)
		{
			m_textBox->insertTextAtCaret(message);
			m_textBox->insertTextAtCaret("\n");
		}
	}

	m_loggingQueue.clear();
}

/**
 * Overloaded method to add logging entry data to componentn
 *
 * @param NId			The node id the logging data comes from
 * @param SenderPId		The protocol id of the protocol the data was received at
 * @param SenderType	The protocol type of the protocol that received the data
 * @param Id			The message id of the data
 * @param msgData		The actual data that is to be logged
 */
void LoggingComponent::AddLogData(NodeId NId, ProtocolId SenderPId, ProtocolType SenderType, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	if (m_mode == LM_Text)
	{
		String objectString;
		switch (SenderType)
		{
		case PT_OSCProtocol:
			objectString += OSCProtocolProcessor::GetRemoteObjectString(Id) +
				String::formatted(" | ch%d rec%d", msgData.addrVal.first, msgData.addrVal.second);
			break;
		case PT_OCAProtocol:
			objectString += OCAProtocolProcessor::GetRemoteObjectString(Id) +
				String::formatted(" | ch%d rec%d", msgData.addrVal.first, msgData.addrVal.second);
			break;
		default:
			break;
		}

		if (msgData.payload != 0)
		{
			objectString += " |";

			if (msgData.valType == ROVT_FLOAT)
			{
				float fvalue;
				for (int i = 0; i < msgData.valCount; ++i)
				{
					fvalue = ((float*)msgData.payload)[i];
					objectString += String::formatted(" %f", fvalue);
				}
			}
			else if (msgData.valType == ROVT_INT)
			{
				int ivalue;
				for (int i = 0; i < msgData.valCount; ++i)
				{
					ivalue = ((int*)msgData.payload)[i];
					objectString += String::formatted(" %d", ivalue);
				}
			}
		}

		String SenderName = ProcessingEngineConfig::ProtocolTypeToString(SenderType);
		String logString;
		logString << "Node" << (int)NId << "[In:" << SenderName << ":PId" << (int)SenderPId << "]: " << objectString;

		AddMessage(logString);
	}
	else if (m_mode == LM_Graph)
	{
		if (m_plotBox)
			m_plotBox->IncreaseCount(NId, SenderPId);
	}
}

/**
 * Method to toggle the logging mode defined in 'LoggingMode' enum
 *
 * @param mode	The new logging mode to activate
 */
void LoggingComponent::SetLoggingMode(LoggingMode mode)
{
	if (m_mode != mode)
	{
		m_mode = mode;
		m_LogModeDrop->setSelectedId(mode);

		switch (m_mode)
		{
		case LM_Text:
			if (m_textBox)
				m_textBox->setVisible(true);
			if (m_plotBox)
				m_plotBox->setVisible(false);
			break;
		case LM_Graph:
			if (m_textBox)
				m_textBox->setVisible(false);
			if (m_plotBox)
				m_plotBox->setVisible(true);
			break;
		case LM_INVALID:
		default:
			if (m_textBox)
				m_textBox->setVisible(false);
			if (m_plotBox)
				m_plotBox->setVisible(false);
			break;
		}
	}
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void LoggingComponent::AddListener(LoggingWindow* listener)
{
	m_parentListener = listener;
}

/**
 * Reimplemented from Component.
 * We fill the background with a solid colour here.
 *
 * @param g	The graphics object to use for painting
 */
void LoggingComponent::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

/**
 * Reimplemented from Component.
 * We resize our child components here appropriately.
 */
void LoggingComponent::resized()
{
	Component::resized();

	int windowWidth = getWidth();
	int windowHeight = getHeight();

	/*Mode dropdown*/
	int xPositionModeDrop = UIS_Margin_m;
	int yPositionModeDrop = windowHeight - UIS_ElmSize - UIS_Margin_m;
	m_LogModeDrop->setBounds(xPositionModeDrop, yPositionModeDrop, UIS_OpenConfigWidth, UIS_ElmSize);

	/*Close Button*/
	int xPositionCloseButton = windowWidth - UIS_Margin_m - UIS_OpenConfigWidth;
	int yPositionCloseButton = yPositionModeDrop;
	m_closeButton->setBounds(xPositionCloseButton, yPositionCloseButton, UIS_OpenConfigWidth, UIS_ElmSize);

	/*Logging Component*/
	int loggingComponentHeight = yPositionCloseButton - UIS_Margin_m;
	m_textBox->setBounds(Rectangle<int>(0, 0, windowWidth, loggingComponentHeight));
	m_plotBox->setBounds(Rectangle<int>(0, 0, windowWidth, loggingComponentHeight));
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void LoggingComponent::buttonClicked(Button* button)
{
	if (button == m_closeButton.get() && m_parentListener)
	{
		m_parentListener->closeButtonPressed();
	}
}

/**
 * Overloaded method called by ComboBox objects on change events.
 *
 * @param comboBox	The comboBox object that has been changed
 */
void LoggingComponent::comboBoxChanged(ComboBox* comboBox)
{
	if (m_LogModeDrop && (m_LogModeDrop.get() == comboBox))
	{
		SetLoggingMode((LoggingMode)m_LogModeDrop->getSelectedId());
	}
}

/*
 * Helper method to get a name string for a given mode
 *
 * @param lm	The mode to get the string for
 */
String LoggingComponent::LogModeToString(LoggingMode lm)
{
	switch (lm)
	{
	case LM_Text:
		return "Message Log";
	case LM_Graph:
		return "Traffic Graph";
	case LM_INVALID:
		return "Invalid";
	default:
		return "";
	}
}

/*
 * Helper method to get the mode for a given mode name string
 *
 * @param mode	The name to get the corresponding mode for
 */
LoggingComponent::LoggingMode LoggingComponent::LogModeFromString(String mode)
{
	if (mode == LogModeToString(LM_Text))
		return LM_Text;
	if (mode == LogModeToString(LM_Graph))
		return LM_Graph;

	return LM_INVALID;
}


// **************************************************************************************
//    class LoggingWindow
// **************************************************************************************
/**
 * Constructor for the LoggingWindow class. Internal contents are created here.
 *
 * @param name							The name of the window
 * @param backgroundColour				The background color of the window
 * @param escapeKeyTriggersCloseButton	Flag value to set behaviour of hitting escape key on window
 * @param addToDesktop					Flag value to define if window is to be standalone or embedded in other content
 */
LoggingWindow::LoggingWindow(const String& name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, bool addToDesktop)
	: DialogWindow(name, backgroundColour, escapeKeyTriggersCloseButton, addToDesktop)
{
	m_parentListener = 0;

	m_loggingComponent = std::make_unique<LoggingComponent>();

	// Component resizes automatically anyway, but need size > 0;
	m_loggingComponent->setBounds(Rectangle<int>(1, 1));
	m_loggingComponent->AddListener(this);

	setContentOwned(m_loggingComponent.get(), true);
    
#if JUCE_IOS || JUCE_ANDROID
    setFullScreen(true);
#endif
}

/**
 * Destructor
 */
LoggingWindow::~LoggingWindow()
{
}

/**
 * Overloaded method that is called when window close button is pressed.
 * We enshure the window self-destroys here, but first notify the parent of it.
 */
void LoggingWindow::closeButtonPressed()
{
	m_parentListener->childWindowCloseTriggered(this);
}

/**
 * 
 */
void LoggingWindow::AddLogData(NodeId NId, ProtocolId SenderPId, ProtocolType SenderType, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	if (m_loggingComponent)
	{
		m_loggingComponent->AddLogData(NId, SenderPId, SenderType, Id, msgData);
	}
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void LoggingWindow::AddListener(MainRemoteProtocolBridgeComponent* listener)
{
	m_parentListener = listener;
}
