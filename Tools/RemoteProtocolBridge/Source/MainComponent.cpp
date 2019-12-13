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

#include "MainComponent.h"

#include "Common.h"
#include "NodeComponent.h"
#include "LoggingWindow.h"
#include "ConfigComponents/GlobalConfigComponents.h"
#include "ProcessingEngineConfig.h"


// **************************************************************************************
//    class MainComponent
// **************************************************************************************
/**
 * Constructor
 */
MainComponent::MainComponent()
{
    m_config.InitConfiguration();
    
	m_ConfigDialog = 0;
	m_LoggingDialog = 0;

	/******************************************************/
	m_AddNodeButton = std::make_unique<ImageButton>();
	m_AddNodeButton->addListener(this);
	addAndMakeVisible(m_AddNodeButton.get());
	Image AddNormalImage = ImageCache::getFromMemory(BinaryData::AddNormalImage_png, BinaryData::AddNormalImage_pngSize);
	Image AddOverImage = ImageCache::getFromMemory(BinaryData::AddOverImage_png, BinaryData::AddOverImage_pngSize);
	Image AddDownImage = ImageCache::getFromMemory(BinaryData::AddDownImage_png, BinaryData::AddDownImage_pngSize);
	m_AddNodeButton->setImages(false, true, true, 
		AddNormalImage, 1.0, Colours::transparentBlack,
		AddOverImage, 1.0, Colours::transparentBlack,
		AddDownImage, 1.0, Colours::transparentBlack);

	m_RemoveNodeButton = std::make_unique<ImageButton>();
	m_RemoveNodeButton->addListener(this);
	addAndMakeVisible(m_RemoveNodeButton.get());
	Image RemoveNormalImage = ImageCache::getFromMemory(BinaryData::RemoveNormalImage_png, BinaryData::RemoveNormalImage_pngSize);
	Image RemoveOverImage = ImageCache::getFromMemory(BinaryData::RemoveOverImage_png, BinaryData::RemoveOverImage_pngSize);
	Image RemoveDownImage = ImageCache::getFromMemory(BinaryData::RemoveDownImage_png, BinaryData::RemoveDownImage_pngSize);
	m_RemoveNodeButton->setImages(false, true, true,
		RemoveNormalImage, 1.0, Colours::transparentBlack,
		RemoveOverImage, 1.0, Colours::transparentBlack,
		RemoveDownImage, 1.0, Colours::transparentBlack);

	/******************************************************/
	m_TriggerOpenConfigButton = std::make_unique<TextButton>();
	m_TriggerOpenConfigButton->addListener(this);
	addAndMakeVisible(m_TriggerOpenConfigButton.get());
	m_TriggerOpenConfigButton->setButtonText("Global Configuration");
	m_TriggerOpenConfigButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
	m_TriggerOpenConfigButton->setColour(Label::textColourId, Colours::white);

	m_TriggerOpenLoggingButton = std::make_unique<TextButton>();
	m_TriggerOpenLoggingButton->addListener(this);
	addAndMakeVisible(m_TriggerOpenLoggingButton.get());
	m_TriggerOpenLoggingButton->setButtonText("Show Traffic Logging");
	m_TriggerOpenLoggingButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
	m_TriggerOpenLoggingButton->setColour(Label::textColourId, Colours::white);

	m_EngineStartStopButton = std::make_unique<TextButton>();
	m_EngineStartStopButton->addListener(this);
	addAndMakeVisible(m_EngineStartStopButton.get());
	m_EngineStartStopButton->setButtonText("Start Engine");
	m_EngineStartStopButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
	m_EngineStartStopButton->setColour(Label::textColourId, Colours::white);

    RefreshUIfromConfig();

	if (m_config.IsEngineStartOnAppStart())
	{
		// Get data from ui together to start the engine correctly.
		DumpUItoConfig();

		m_engine.SetConfig(m_config);

		if (!m_engine.IsRunning() && m_engine.Start())
		{
			m_EngineStartStopButton->setColour(TextButton::buttonColourId, Colours::lightgreen);
			m_EngineStartStopButton->setColour(Label::textColourId, Colours::dimgrey);
			m_EngineStartStopButton->setButtonText("Stop Engine");
		}
	}
}

/**
 * Destructor
 */
MainComponent::~MainComponent()
{
	if (m_engine.IsRunning())
		m_engine.Stop();
    
    m_config.Clear();
}

/**
 * Method to gather data from ui elements to update the app config object with fresh data.
 * This approach is not a very elegant one and should be done more dynamically (esp. since design of the app
 * is made in a way to support multiple dynamic nodes, ...), but for lack of time and resources this is what the
 * first shot looks like.
 * A fix node is created with a hardcoded id, as is done for both protocols of this node.
 */
void MainComponent::DumpUItoConfig()
{
    Array<NodeId> NIds = m_config.GetNodeIds();
    for (int i = 0; i < NIds.size(); ++i)
    {
        NodeId NId = NIds[i];
        if (m_NodeBoxes.count(NId) && m_NodeBoxes.at(NId))
			m_NodeBoxes.at(NId)->DumpUItoConfig(m_config);
    }

	m_config.WriteConfiguration();
}

/**
 * Method to set up ui elements according to configuration contents.
 */
void MainComponent::RefreshUIfromConfig()
{
    Array<NodeId> NIds = m_config.GetNodeIds();

	// go through current ui node boxes to find out which ones need to be removed/destroyed
	Array<NodeId> NIdsToRemove;
	for (std::map<NodeId, std::unique_ptr<NodeComponent>>::iterator nodeIter = m_NodeBoxes.begin(); nodeIter != m_NodeBoxes.end(); ++nodeIter)
		if (!NIds.contains(nodeIter->first))
			NIdsToRemove.add(nodeIter->first);

	for(int i=0; i<NIdsToRemove.size(); ++i)
	{
		if (m_NodeBoxes.count(NIdsToRemove[i]) && m_NodeBoxes.at(NIdsToRemove[i]))
		{
			removeChildComponent(m_NodeBoxes.at(NIdsToRemove[i]).get());
			m_NodeBoxes.erase(NIdsToRemove[i]);
		}
	}

	int requiredNodeAreaHeight = 0;

	// now go through all node ids currently in config and create those nodes
	// that do not already exist or simply update those that are present
    for (int i = 0; i < NIds.size(); ++i)
    {
		NodeId NId = NIds[i];

		if (!m_NodeBoxes.count(NId))
		{
			NodeComponent* Node = new NodeComponent(NId);
			Node->AddListener(this);
			requiredNodeAreaHeight += Node->RefreshUIfromConfig(m_config);
			Node->setText("Protocol Bridging Node Id" + String(NId));
			addAndMakeVisible(Node);

			m_NodeBoxes[NId] = std::unique_ptr<NodeComponent>(Node);
		}
		else
		{
			requiredNodeAreaHeight += m_NodeBoxes[NId]->RefreshUIfromConfig(m_config);
		}
    }

	if (m_config.IsTrafficLoggingAllowed())
		addAndMakeVisible(m_TriggerOpenLoggingButton.get());
	else
		removeChildComponent(m_TriggerOpenLoggingButton.get());

#if defined JUCE_IOS ||  defined JUCE_ANDROID
    if(getScreenBounds().getWidth()<1 || getScreenBounds().getHeight()<1)
        setSize(1,1);
    else
        setSize(getScreenBounds().getWidth(), getScreenBounds().getHeight());
#else
	// For +- and logging/config/start buttons, add some additional height
	int requiredGlobalControlsHeight = 2 * (UIS_ElmSize + UIS_Margin_m);

	setSize(UIS_MainComponentWidth, requiredNodeAreaHeight + requiredGlobalControlsHeight);
#endif
}

/**
 * Method to allow access to internal config.
 *
 * @return	Reference to the internal config object
 */
ProcessingEngineConfig* MainComponent::GetConfig()
{
	return &m_config;
}

/**
 * Method to allow access to internal engine.
 *
 * @return	Reference to the internal engine object
 */
ProcessingEngine* MainComponent::GetEngine()
{
	return &m_engine;
}

/**
 * Overloaded paint method that fills background with solid color
 *
 * @param g	Graphics painting object to use for filling background
 */
void MainComponent::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
}

/**
 * Overloaded method to resize contents
 */
void MainComponent::resized()
{
	Component::resized();

	int windowWidth = getWidth();
	int windowHeight = getHeight();

	/*Config, TrafficLogging, Start/Stop Buttons*/
	int yPositionConfTrafButtons = windowHeight - UIS_ElmSize - UIS_Margin_m;
	if (m_TriggerOpenConfigButton)
		m_TriggerOpenConfigButton->setBounds(UIS_Margin_m, yPositionConfTrafButtons, UIS_OpenConfigWidth, UIS_ElmSize);
	if (m_TriggerOpenLoggingButton)
		m_TriggerOpenLoggingButton->setBounds(windowWidth - 160, yPositionConfTrafButtons, UIS_ButtonWidth, UIS_ElmSize);
	if (m_EngineStartStopButton)
		m_EngineStartStopButton->setBounds(windowWidth - 80, yPositionConfTrafButtons, UIS_ButtonWidth, UIS_ElmSize);

	/*Add/Remove Buttons*/
	int yPositionAddRemButts = yPositionConfTrafButtons - UIS_ElmSize;
	if (m_AddNodeButton)
		m_AddNodeButton->setBounds(windowWidth - 25, yPositionAddRemButts, UIS_ElmSize - UIS_Margin_s, UIS_ElmSize - UIS_Margin_s);
	if (m_RemoveNodeButton)
		m_RemoveNodeButton->setBounds(windowWidth - 45, yPositionAddRemButts, UIS_ElmSize - UIS_Margin_s, UIS_ElmSize - UIS_Margin_s);

	/*Dynamically sized nodes*/
	int nodeAreaWidth = windowWidth - 2 * UIS_Margin_s;
	int nodeAreaHeight = yPositionAddRemButts - UIS_Margin_m;
    Array<NodeId> NIds = m_config.GetNodeIds();
	int nodeCount = NIds.size();
	int nodeHeight = nodeCount > 0 ? nodeAreaHeight / nodeCount : 0;
    for (int i = 0; i < nodeCount; ++i)
    {
        NodeId NId = NIds[i];
		if (m_NodeBoxes.count(NId) && m_NodeBoxes.at(NId))
			m_NodeBoxes.at(NId)->setBounds(UIS_Margin_s, UIS_Margin_s + (i*nodeHeight), nodeAreaWidth, nodeHeight);
    }

}

/**
 * Overloaded method called by button objects on click events.
 * All internal button objects are registered to trigger this by calling
 * their ::addListener method with this object as argument
 *
 * @param button	The button object that has been clicked
 */
void MainComponent::buttonClicked(Button* button)
{
	if (button == m_AddNodeButton.get())
	{
		bool EngineIsRunning = m_engine.IsRunning();
		if (EngineIsRunning)
			m_engine.Stop();

		m_config.AddDefaultNode();
		m_config.WriteConfiguration();
		m_engine.SetConfig(m_config);

		RefreshUIfromConfig();

		if (EngineIsRunning)
			m_engine.Start();
	}
	else if (button == m_RemoveNodeButton.get())
	{
		if (m_config.GetNodeIds().size() > 0)
		{
			bool EngineIsRunning = m_engine.IsRunning();
			if (EngineIsRunning)
				m_engine.Stop();

			m_config.RemoveNode(m_config.GetNodeIds().getLast());
			m_config.WriteConfiguration();
			m_engine.SetConfig(m_config);

			RefreshUIfromConfig();

			if (EngineIsRunning)
				m_engine.Start();
		}
	}
	else if (button == m_TriggerOpenConfigButton.get())
	{
		// if the config dialog exists, this is a uncheck (close) click,
		// which means we have to process edited data
		if (m_ConfigDialog != 0)
		{
			bool EngineIsRunning = m_engine.IsRunning();
			if (EngineIsRunning)
				m_engine.Stop();

			m_ConfigDialog->DumpConfig(m_config);
			m_config.WriteConfiguration();
			m_engine.SetConfig(m_config);

			RefreshUIfromConfig();

			if (EngineIsRunning)
				m_engine.Start();

			button->setColour(TextButton::buttonColourId, Colours::dimgrey);
			button->setColour(Label::textColourId, Colours::white);

			m_ConfigDialog.reset();
		}
		// otherwise we have to create the dialog and show it
		else
		{
			m_ConfigDialog = std::make_unique<GlobalConfigWindow>("General configuration", Colours::dimgrey, false);
			m_ConfigDialog->AddListener(this);
			m_ConfigDialog->setResizable(true, true);
			m_ConfigDialog->setUsingNativeTitleBar(true);
			m_ConfigDialog->setVisible(true);
#if defined JUCE_IOS ||  defined JUCE_ANDROID
            m_ConfigDialog->setFullScreen(true);
#else
            std::pair<int, int> size = m_ConfigDialog->GetSuggestedSize();
            m_ConfigDialog->setResizeLimits(size.first, size.second, size.first, size.second);
			m_ConfigDialog->setBounds(Rectangle<int>(getScreenBounds().getX() + getWidth(), getScreenBounds().getY(), size.first, size.second));
#endif
			m_ConfigDialog->SetConfig(m_config);

			button->setColour(TextButton::buttonColourId, Colours::lightblue);
			button->setColour(Label::textColourId, Colours::dimgrey);
		}
	}
	else if (button == m_TriggerOpenLoggingButton.get())
	{
		if (m_engine.IsLoggingEnabled())
		{
			m_engine.SetLoggingEnabled(false);
			m_engine.SetLoggingTarget(0);
			button->setColour(TextButton::buttonColourId, Colours::dimgrey);
			button->setColour(Label::textColourId, Colours::white);

			m_LoggingDialog.reset();
		}
		else
		{
			m_LoggingDialog = std::make_unique<LoggingWindow>("Protocol Traffic Logging", Colours::dimgrey, false);
			m_LoggingDialog->AddListener(this);
			m_LoggingDialog->setResizeLimits(480, 320, 1920, 1080);
			m_LoggingDialog->setResizable(true, true);
			m_LoggingDialog->setUsingNativeTitleBar(true);
			m_LoggingDialog->setVisible(true);
#if defined JUCE_IOS ||  defined JUCE_ANDROID
			m_LoggingDialog->setFullScreen(true);
#else
			m_LoggingDialog->setBounds(Rectangle<int>(getScreenBounds().getX() + getWidth(), getScreenBounds().getY(), 800, 500));
#endif

			m_engine.SetLoggingEnabled(true);
			m_engine.SetLoggingTarget(m_LoggingDialog.get());
			button->setColour(TextButton::buttonColourId, Colours::orange);
			button->setColour(Label::textColourId, Colours::dimgrey);
		}
	}
	else if (button == m_EngineStartStopButton.get())
	{
		if (m_engine.IsRunning())
		{
			m_engine.Stop();
			button->setColour(TextButton::buttonColourId, Colours::dimgrey);
			button->setColour(Label::textColourId, Colours::white);
			//button->setButtonText("Start Engine");
		}
		else
		{
			// Get data from ui together to start the engine correctly.
			DumpUItoConfig();

			m_engine.SetConfig(m_config);

			if (m_engine.Start())
			{
				button->setColour(TextButton::buttonColourId, Colours::lightgreen);
				button->setColour(Label::textColourId, Colours::dimgrey);
				//button->setButtonText("Stop Engine");
			}
		}
	}
}

/**
 * Method to be called by child windows when closed, to enshure
 * button states, logging activity are resetted and the internal
 * window object is invalidated to avoid accessviolation
 *
 * @param childWindow	The DialogWindow object that has been triggered to close
 */
void MainComponent::childWindowCloseTriggered(DialogWindow* childWindow)
{
	if (childWindow == m_ConfigDialog.get())
	{
		if (m_ConfigDialog != 0)
		{
			bool EngineIsRunning = m_engine.IsRunning();
			if (EngineIsRunning)
				m_engine.Stop();

			m_ConfigDialog->DumpConfig(m_config);
			m_config.WriteConfiguration();
			m_engine.SetConfig(m_config);

			RefreshUIfromConfig();

			if (EngineIsRunning)
				m_engine.Start();

			m_TriggerOpenConfigButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
			m_TriggerOpenConfigButton->setColour(Label::textColourId, Colours::white);
		}

		m_ConfigDialog.reset();
	}
	else if (childWindow == m_LoggingDialog.get())
	{
		if (m_engine.IsLoggingEnabled())
		{
			m_engine.SetLoggingEnabled(false);
			m_engine.SetLoggingTarget(0);
			m_TriggerOpenLoggingButton->setColour(TextButton::buttonColourId, Colours::dimgrey);
			m_TriggerOpenLoggingButton->setColour(Label::textColourId, Colours::white);
		}

		m_LoggingDialog.reset();
	}
}
