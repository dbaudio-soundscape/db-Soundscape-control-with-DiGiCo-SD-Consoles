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

#include "ProtocolConfigComponents.h"

#include "../ProtocolComponent.h"
#include "../Common.h"

//==============================================================================
// Class ProtocolConfigComponent_Abstract
//==============================================================================
/**
 * @fn bool ProtocolConfigComponent_Abstract::DumpActiveHandlingUsed()
 * @return True on success, false on failure.
 * Pure virtual function to be implemented to return if protocol specific config elements are set to active handling or not.
 */

/**
 * @fn Array<RemoteObject> ProtocolConfigComponent_Abstract::DumpActiveRemoteObjects()
 * @return True on success, false on failure.
 * Pure virtual function to be implemented by derived config components to dump ui contents regarding remote object active setting.
 */

/**
 * @fn void ProtocolConfigComponent_Abstract::FillActiveRemoteObjects(const Array<RemoteObject>& Objs)
 * @return True on success, false on failure.
 * Pure virtual function to be implemented by derived config components to fill ui contents regarding remote object active setting.
 */

/**
 * @fn const std::pair<int, int> ProtocolConfigComponent_Abstract::GetSuggestedSize()
 * @return True on success, false on failure.
 * Pure virtual function to be implemented by protocol config component objects to suggest a favoured size.
 */

/**
 * Class constructor.
 */
ProtocolConfigComponent_Abstract::ProtocolConfigComponent_Abstract()
{
	m_parentListener	= 0;

	m_Headline = std::make_unique<Label>();
	m_Headline->setText("Generic Protocol Configuration:", dontSendNotification);
	addAndMakeVisible(m_Headline.get());

	m_HostPortLabel = std::make_unique<Label>();
	addAndMakeVisible(m_HostPortLabel.get());
	m_HostPortLabel->setText("Listening port", dontSendNotification);
	m_HostPortEdit = std::make_unique<TextEditor>();
	addAndMakeVisible(m_HostPortEdit.get());
	m_ClientPortLabel = std::make_unique<Label>();
	addAndMakeVisible(m_ClientPortLabel.get());
	m_ClientPortLabel->setText("Remote port", dontSendNotification);
	m_ClientPortEdit = std::make_unique<TextEditor>();
	addAndMakeVisible(m_ClientPortEdit.get());

	m_applyConfigButton = std::make_unique<TextButton>("Ok");
	addAndMakeVisible(m_applyConfigButton.get());
	m_applyConfigButton->addListener(this);
}

/**
 * Class destructor.
 */
ProtocolConfigComponent_Abstract::~ProtocolConfigComponent_Abstract()
{
}

/**
 * Reimplemented paint method that fills background with solid color
 *
 * @param g	Graphics painting object to use for filling background
 */
void ProtocolConfigComponent_Abstract::paint(Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void ProtocolConfigComponent_Abstract::buttonClicked(Button* button)
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
void ProtocolConfigComponent_Abstract::AddListener(ProtocolConfigWindow* listener)
{
	m_parentListener = listener;
}

/**
 * Method to trigger dumping contents of configcomponent member
 * to pair of ports to return to the app to initialize from
 *
 * @return	The pair of ports (host/client) set in UI to be dumped into config.
 */
std::pair<int, int> ProtocolConfigComponent_Abstract::DumpProtocolPorts()
{
	int clientPort = 0;
	int hostPort = 0;

	StringArray portStrings;
	portStrings.addTokens(m_HostPortEdit->getText(), ";, ", "");
	if (portStrings.size() == 1)
	{
		hostPort = portStrings[0].getIntValue();
	}
	portStrings.clear();
	portStrings.addTokens(m_ClientPortEdit->getText(), ";, ", "");
	if (portStrings.size() == 1)
	{
		clientPort = portStrings[0].getIntValue();
	}

	return std::pair<int, int>(clientPort, hostPort);
}

/**
 * Method to trigger filling contents of
 * configcomponent member with protocol ports
 *
 * @param ports		The client+host port from config.
 */
void ProtocolConfigComponent_Abstract::FillProtocolPorts(const std::pair<int, int>& ports)
{
	if (m_HostPortEdit)
		m_HostPortEdit->setText(String(ports.second));
	if (m_ClientPortEdit)
		m_ClientPortEdit->setText(String(ports.first));

	return;
}

/**
 * Setter of state of button for if active object handling shall be used
 *
 * @param active	True if active object handling shall be activated.
 */
void ProtocolConfigComponent_Abstract::SetActiveHandlingUsed(bool active)
{
	ignoreUnused(active);
}


//==============================================================================
// Class BasicProtocolConfigComponent
//==============================================================================
/**
 * Class constructor.
 */
BasicProtocolConfigComponent::BasicProtocolConfigComponent()
{
	m_UseActiveHandlingCheck = std::make_unique<ToggleButton>();
	addAndMakeVisible(m_UseActiveHandlingCheck.get());

	m_UseActiveHandlingLabel = std::make_unique<Label>();
	addAndMakeVisible(m_UseActiveHandlingLabel.get());
	m_UseActiveHandlingLabel->setText("Enable active object handling", dontSendNotification);

	m_Headline->setText("Objects to activly handle (OSC polling, OCA subscriptions)", dontSendNotification);

	m_EnableHeadlineLabel = std::make_unique<Label>();
	m_EnableHeadlineLabel->setText("active", dontSendNotification);
	addAndMakeVisible(m_EnableHeadlineLabel.get());
	m_ChannelHeadlineLabel = std::make_unique<Label>();
	m_ChannelHeadlineLabel->setText("channel", dontSendNotification);
	addAndMakeVisible(m_ChannelHeadlineLabel.get());
	m_RecordHeadlineLabel = std::make_unique<Label>();
	m_RecordHeadlineLabel->setText("record", dontSendNotification);
	addAndMakeVisible(m_RecordHeadlineLabel.get());

	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		m_RemObjNameLabels[i] = std::make_unique<Label>();
		m_RemObjNameLabels.at(i)->setText(ProcessingEngineConfig::GetObjectDescription((RemoteObjectIdentifier)i), dontSendNotification);
		addAndMakeVisible(m_RemObjNameLabels.at(i).get());

		m_RemObjEnableChecks[i] = std::make_unique<ToggleButton>();
		addAndMakeVisible(m_RemObjEnableChecks.at(i).get());
		
		m_RemObjActiveChannelEdits[i] = std::make_unique<TextEditor>();
		addAndMakeVisible(m_RemObjActiveChannelEdits.at(i).get());
		
		m_RemObjActiveRecordEdits[i] = std::make_unique<TextEditor>();
		addAndMakeVisible(m_RemObjActiveRecordEdits.at(i).get());
	}

}

/**
 * Class destructor.
 */
BasicProtocolConfigComponent::~BasicProtocolConfigComponent()
{
	m_RemObjNameLabels.clear();
	m_RemObjEnableChecks.clear();
	m_RemObjActiveChannelEdits.clear();
	m_RemObjActiveRecordEdits.clear();
}

/**
 * Reimplemented to resize and re-postion controls on the overview window.
 */
void BasicProtocolConfigComponent::resized()
{
	double usableWidth = double(getWidth()) - 2 * UIS_Margin_s;
	int remObjNameWidth = (int)(usableWidth*0.5);
	int remObjEnableWidth = (int)(usableWidth*0.1);
	int remObjChRngeWidth = (int)(usableWidth*0.2);
	int remObjRecRngeWidth = (int)(usableWidth*0.2);

	int yOffset = UIS_Margin_s;
	m_HostPortLabel->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
	m_HostPortEdit->setBounds(Rectangle<int>(2 * UIS_Margin_s + remObjNameWidth, yOffset, remObjEnableWidth + remObjChRngeWidth - UIS_Margin_m, UIS_ElmSize));

	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_ClientPortLabel->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
	m_ClientPortEdit->setBounds(Rectangle<int>(2 * UIS_Margin_s + remObjNameWidth, yOffset, remObjEnableWidth + remObjChRngeWidth - UIS_Margin_m, UIS_ElmSize));

	// active handling checkbox
	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_UseActiveHandlingLabel->setBounds(Rectangle<int>(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize)));
	m_UseActiveHandlingCheck->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s, yOffset, remObjEnableWidth, UIS_ElmSize));

	// table headline labels
	yOffset += UIS_Margin_s + UIS_ElmSize + UIS_Margin_s;
	m_EnableHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s, yOffset, remObjEnableWidth, UIS_ElmSize));
	m_ChannelHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth, yOffset, remObjChRngeWidth, UIS_ElmSize));
	m_RecordHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth, yOffset, remObjRecRngeWidth, UIS_ElmSize));

	// table items
	yOffset += UIS_Margin_s;
	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		yOffset += UIS_Margin_s + UIS_ElmSize;
		if (m_RemObjNameLabels.count(i) && m_RemObjNameLabels.at(i))
			m_RemObjNameLabels.at(i)->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(i))
			m_RemObjEnableChecks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s, yOffset, remObjEnableWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjActiveChannelEdits.count(i) && m_RemObjActiveChannelEdits.at(i))
			m_RemObjActiveChannelEdits.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth, yOffset, remObjChRngeWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjActiveRecordEdits.count(i) && m_RemObjActiveRecordEdits.at(i))
			m_RemObjActiveRecordEdits.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth, yOffset, remObjRecRngeWidth - UIS_Margin_s, UIS_ElmSize));
	}

	// ok button
	yOffset += UIS_Margin_s + UIS_ElmSize + UIS_Margin_s;
	m_applyConfigButton->setBounds(Rectangle<int>((int)usableWidth - UIS_ButtonWidth, yOffset, UIS_ButtonWidth, UIS_ElmSize));
}

/**
 * Callback function for changes to our textEditors.
 * @param textEditor	The TextEditor object whose content has just changed.
 */
void BasicProtocolConfigComponent::textEditorFocusLost(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for Enter key presses on textEditors.
 * @param textEditor	The TextEditor object whose where enter key was pressed.
 */
void BasicProtocolConfigComponent::textEditorReturnKeyPressed(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void BasicProtocolConfigComponent::buttonClicked(Button* button)
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
void BasicProtocolConfigComponent::AddListener(ProtocolConfigWindow* listener)
{
	m_parentListener = listener;
}

/**
 * Method to trigger dumping contents of configcomponent member
 * to list of objects to return to the app to initialize from
 *
 * @return	The list of objects to actively handle when running the engine.
 */
Array<RemoteObject> BasicProtocolConfigComponent::DumpActiveRemoteObjects()
{
	Array<RemoteObject> activeObjects;

	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		RemoteObject obj;

		//ToggleButton* c = m_RemObjEnableChecks[i];
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(i) && m_RemObjEnableChecks.at(i)->getToggleState())
		{
			obj.Id = (RemoteObjectIdentifier)i;

			// Since we expect the user to enter something like "1,4,6,8-12" to select
			// channels 1 4 6 8 9 10 11 12, we need to do some parsing. First we split
			// our input string for channels and records based on separators ',' or ';'
			// and second we inspect the results if they are singles or ranges.
			// The result is supposted to be two array with all channels and records included
			// as separate entries.
			Array<int> channels;
			Array<int> records;
			if (m_RemObjActiveChannelEdits.count(i) && m_RemObjActiveChannelEdits.at(i))
			{
				StringArray channelNumberSections;
				String chSelToSplit = m_RemObjActiveChannelEdits.at(i)->getText();
				channelNumberSections.addTokens(chSelToSplit, ",; ", "");
				for (int j = 0; j < channelNumberSections.size(); ++j)
				{
					StringArray channelNumbers;
					channelNumbers.addTokens(channelNumberSections[j], "-", "");
					if (channelNumbers.size() == 1)
					{
						channels.add(channelNumbers[0].getIntValue());
					}
					else if (channelNumbers.size() == 2)
					{
						int startVal = channelNumbers[0].getIntValue();
						int stopVal = channelNumbers[1].getIntValue();
						for (int k = startVal; k <= stopVal; ++k)
							channels.add(k);
					}
				}
			}
			if (m_RemObjActiveRecordEdits.count(i) && m_RemObjActiveRecordEdits.at(i))
			{
				StringArray recordNumberSections;
				String recSelToSplit = m_RemObjActiveRecordEdits.at(i)->getText();
				recordNumberSections.addTokens(recSelToSplit, ",; ", "");
				for (int j = 0; j < recordNumberSections.size(); ++j)
				{
					StringArray recordNumbers;
					recordNumbers.addTokens(recordNumberSections[j], "-", "");
					if (recordNumbers.size() == 1)
					{
						records.add(recordNumbers[0].getIntValue());
					}
					else if (recordNumbers.size() == 2)
					{
						int startVal = recordNumbers[0].getIntValue();
						int stopVal = recordNumbers[1].getIntValue();
						for (int k = startVal; k <= stopVal; ++k)
							records.add(k);
					}
				}
			}

			// now that we have all channels and records, recursively iterate through both arrays
			// to add an entry to our active objects list for every resulting ch/rec combi object
			for (int j = 0; j < channels.size(); ++j)
			{
				if (records.size() > 0)
				{
					for (int k = 0; k < records.size(); ++k)
					{
						obj.Addr.first = int16(channels[j]);
						obj.Addr.second = int16(records[k]);
						activeObjects.add(obj);
					}
				}
				else
				{
					obj.Addr.first = int16(channels[j]);
					obj.Addr.second = -1;
					activeObjects.add(obj);
				}
			}
		}
	}

	return activeObjects;
}

/**
 * Method to trigger filling contents of
 * configcomponent member with list of objects
 *
 * @param Objs	The list of objects to set as default.
 */
void BasicProtocolConfigComponent::FillActiveRemoteObjects(const Array<RemoteObject>& Objs)
{
	Array<int> activeObjects;
	HashMap<int, Array<int>> channelsPerObj;
	HashMap<int, Array<int>> recordsPerObj;

	for (int i = 0; i < Objs.size(); ++i)
	{
		Array<int> selChs = channelsPerObj[Objs[i].Id];
		if (!selChs.contains(Objs[i].Addr.first))
		{
			selChs.add(Objs[i].Addr.first);
			channelsPerObj.set(Objs[i].Id, selChs);
		}

		Array<int> selRecs = recordsPerObj[Objs[i].Id];
		if (!selRecs.contains(Objs[i].Addr.second))
		{
			selRecs.add(Objs[i].Addr.second);
			recordsPerObj.set(Objs[i].Id, selRecs);
		}

		if (!activeObjects.contains(Objs[i].Id))
			activeObjects.add(Objs[i].Id);
	}

	for (int i = 0; i < activeObjects.size(); ++i)
	{
		int Id = activeObjects[i];
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(Id) && !m_RemObjEnableChecks.at(Id)->getToggleState())
			m_RemObjEnableChecks.at(Id)->setToggleState(true, dontSendNotification);

		if (m_RemObjActiveChannelEdits.count(i) && m_RemObjActiveChannelEdits.at(Id))
		{
			String selChanTxt;
			for (int j = 0; j < channelsPerObj[Id].size(); ++j)
			{
				if (channelsPerObj[Id][j] > 0)
				{
					if (!selChanTxt.isEmpty())
						selChanTxt << ", ";
					selChanTxt << channelsPerObj[Id][j];
				}
			}

			m_RemObjActiveChannelEdits.at(Id)->setText(selChanTxt);
		}

		if (m_RemObjActiveRecordEdits.count(i) && m_RemObjActiveRecordEdits.at(Id))
		{
			String selRecTxt;
			for (int j = 0; j < recordsPerObj[Id].size(); ++j)
			{
				if (recordsPerObj[Id][j] > 0)
				{
					if (!selRecTxt.isEmpty())
						selRecTxt << ", ";
					selRecTxt << recordsPerObj[Id][j];
				}
			}

			m_RemObjActiveRecordEdits.at(Id)->setText(selRecTxt);
		}
	}
}

/**
 * Method to trigger dumping of state of button for if active object handling shall be used
 *
 * @return	True if active object handling shall be used.
 */
bool BasicProtocolConfigComponent::DumpActiveHandlingUsed()
{
	if (m_UseActiveHandlingCheck)
		return m_UseActiveHandlingCheck->getToggleState();
	else
		return false;
}

/**
 * Setter of state of button for if active object handling shall be used
 *
 * @param active	True if active object handling shall be activated.
 */
void BasicProtocolConfigComponent::SetActiveHandlingUsed(bool active)
{
	if (m_UseActiveHandlingCheck)
		m_UseActiveHandlingCheck->setToggleState(active, dontSendNotification);
}

/**
 * Method to get the components' suggested size. This will be deprecated as soon as
 * the primitive UI is refactored and uses dynamic / proper layouting
 *
 * @return	The pair of int representing the suggested size for this component
 */
const std::pair<int, int> BasicProtocolConfigComponent::GetSuggestedSize()
{
	int width = UIS_BasicConfigWidth;
	int height = UIS_Margin_s +
		UIS_Margin_s + UIS_ElmSize +
		UIS_Margin_s + UIS_ElmSize +
		UIS_Margin_s + UIS_ElmSize + UIS_Margin_s +
		UIS_Margin_s +
		((ROI_UserMAX - ROI_Invalid)*(UIS_Margin_s + UIS_ElmSize + UIS_Margin_s)) +
		UIS_Margin_s + UIS_ElmSize + UIS_Margin_s +
		UIS_Margin_s;

	return std::pair<int, int>(width, height);
}


//==============================================================================
// Class OSCProtocolConfigComponent
//==============================================================================
/**
 * Class constructor.
 */
OSCProtocolConfigComponent::OSCProtocolConfigComponent()
{
	m_Headline->setText("Objects enabled for polling:", dontSendNotification);

	m_EnableHeadlineLabel = std::make_unique<Label>();
	m_EnableHeadlineLabel->setText("enable", dontSendNotification);
	addAndMakeVisible(m_EnableHeadlineLabel.get());

	m_ChannelHeadlineLabel = std::make_unique<Label>();
	m_ChannelHeadlineLabel->setText("Object Nr.", dontSendNotification);
	addAndMakeVisible(m_ChannelHeadlineLabel.get());

	m_MappingsHeadlineLabel = std::make_unique<Label>();
	m_MappingsHeadlineLabel->setText("Mapping", dontSendNotification);
	addAndMakeVisible(m_MappingsHeadlineLabel.get());
	m_Mapping1HeadlineLabel = std::make_unique<Label>();
	m_Mapping1HeadlineLabel->setText("1", dontSendNotification);
	addAndMakeVisible(m_Mapping1HeadlineLabel.get());
	m_Mapping2HeadlineLabel = std::make_unique<Label>();
	m_Mapping2HeadlineLabel->setText("2", dontSendNotification);
	addAndMakeVisible(m_Mapping2HeadlineLabel.get());
	m_Mapping3HeadlineLabel = std::make_unique<Label>();
	m_Mapping3HeadlineLabel->setText("3", dontSendNotification);
	addAndMakeVisible(m_Mapping3HeadlineLabel.get());
	m_Mapping4HeadlineLabel = std::make_unique<Label>();
	m_Mapping4HeadlineLabel->setText("4", dontSendNotification);
	addAndMakeVisible(m_Mapping4HeadlineLabel.get());

	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		m_RemObjNameLabels[i] = std::make_unique<Label>();
		m_RemObjNameLabels.at(i)->setText(ProcessingEngineConfig::GetObjectDescription((RemoteObjectIdentifier)i), dontSendNotification);
		addAndMakeVisible(m_RemObjNameLabels.at(i).get());

		m_RemObjEnableChecks[i] = std::make_unique<ToggleButton>();
		addAndMakeVisible(m_RemObjEnableChecks.at(i).get());

		m_RemObjActiveChannelEdits[i] = std::make_unique<TextEditor>();
		addAndMakeVisible(m_RemObjActiveChannelEdits.at(i).get());

		// the mapping checks are only wanted for positioning objects
		if (i >= ROI_SoundObject_Position_X && i <= ROI_SoundObject_Position_XY)
		{
			m_RemObjMappingArea1Checks[i] = std::make_unique<ToggleButton>();
			addAndMakeVisible(m_RemObjMappingArea1Checks.at(i).get());

			m_RemObjMappingArea2Checks[i] = std::make_unique<ToggleButton>();
			addAndMakeVisible(m_RemObjMappingArea2Checks.at(i).get());

			m_RemObjMappingArea3Checks[i] = std::make_unique<ToggleButton>();
			addAndMakeVisible(m_RemObjMappingArea3Checks.at(i).get());

			m_RemObjMappingArea4Checks[i] = std::make_unique<ToggleButton>();
			addAndMakeVisible(m_RemObjMappingArea4Checks.at(i).get());
		}
	}

}

/**
 * Class destructor.
 */
OSCProtocolConfigComponent::~OSCProtocolConfigComponent()
{
	m_RemObjNameLabels.clear();
	m_RemObjEnableChecks.clear();
	m_RemObjActiveChannelEdits.clear();
	m_RemObjMappingArea1Checks.clear();
	m_RemObjMappingArea2Checks.clear();
	m_RemObjMappingArea3Checks.clear();
	m_RemObjMappingArea4Checks.clear();
}

/**
 * Reimplemented to resize and re-postion controls on the overview window.
 */
void OSCProtocolConfigComponent::resized()
{
	double usableWidth = double(getWidth()) - 2 * UIS_Margin_s;
	int remObjNameWidth = (int)(usableWidth*0.45);
	int remObjEnableWidth = (int)(usableWidth*0.1);
	int remObjChRngeWidth = (int)(usableWidth*0.2);
	int remObjRecRngeWidth = (int)(usableWidth*0.2);

	// port edits with labels
	int yOffset = UIS_Margin_s;
	m_HostPortLabel->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
	m_HostPortEdit->setBounds(Rectangle<int>(2*UIS_Margin_s + remObjNameWidth, yOffset, remObjEnableWidth + remObjChRngeWidth - UIS_Margin_m, UIS_ElmSize));

	yOffset += UIS_Margin_s + UIS_ElmSize;
	m_ClientPortLabel->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
	m_ClientPortEdit->setBounds(Rectangle<int>(2*UIS_Margin_s + remObjNameWidth, yOffset, remObjEnableWidth + remObjChRngeWidth - UIS_Margin_m, UIS_ElmSize));
	
	// active objects headline
	yOffset += 2*UIS_Margin_m + UIS_ElmSize;
	m_Headline->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, (int)usableWidth, UIS_ElmSize));

	// table headline labels
	//yOffset += UIS_ElmSize + UIS_Margin_s;
	m_MappingsHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth, yOffset, remObjRecRngeWidth, UIS_ElmSize));

	yOffset += UIS_ElmSize;
	m_EnableHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s, yOffset, remObjEnableWidth, UIS_ElmSize));
	m_ChannelHeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth, yOffset, remObjChRngeWidth, UIS_ElmSize));

	m_Mapping1HeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth, yOffset, UIS_ElmSize, UIS_ElmSize));
	m_Mapping2HeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 1 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize, UIS_ElmSize));
	m_Mapping3HeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 2 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize, UIS_ElmSize));
	m_Mapping4HeadlineLabel->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 3 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize, UIS_ElmSize));

	// table items
	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		yOffset += UIS_Margin_s + UIS_ElmSize;
		if (m_RemObjNameLabels.count(i) && m_RemObjNameLabels.at(i))
			m_RemObjNameLabels.at(i)->setBounds(Rectangle<int>(UIS_Margin_s, yOffset, remObjNameWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(i))
			m_RemObjEnableChecks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s, yOffset, remObjEnableWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjActiveChannelEdits.count(i) && m_RemObjActiveChannelEdits.at(i))
			m_RemObjActiveChannelEdits.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth, yOffset, remObjChRngeWidth - UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjMappingArea1Checks.count(i) && m_RemObjMappingArea1Checks.at(i))
			m_RemObjMappingArea1Checks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth, yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjMappingArea2Checks.count(i) && m_RemObjMappingArea2Checks.at(i))
			m_RemObjMappingArea2Checks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 1 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjMappingArea3Checks.count(i) && m_RemObjMappingArea3Checks.at(i))
			m_RemObjMappingArea3Checks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 2 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));
		if (m_RemObjMappingArea4Checks.count(i) && m_RemObjMappingArea4Checks.at(i))
			m_RemObjMappingArea4Checks.at(i)->setBounds(Rectangle<int>(remObjNameWidth + UIS_Margin_s + remObjEnableWidth + remObjChRngeWidth + 3 * (UIS_ElmSize + UIS_Margin_s), yOffset, UIS_ElmSize + UIS_Margin_s, UIS_ElmSize));
	}

	// ok button
	yOffset += UIS_Margin_s + UIS_ElmSize + UIS_Margin_s;
	m_applyConfigButton->setBounds(Rectangle<int>((int)usableWidth - UIS_ButtonWidth, yOffset, UIS_ButtonWidth, UIS_ElmSize));
}

/**
 * Callback function for changes to our textEditors.
 * @param textEditor	The TextEditor object whose content has just changed.
 */
void OSCProtocolConfigComponent::textEditorFocusLost(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for Enter key presses on textEditors.
 * @param textEditor	The TextEditor object whose where enter key was pressed.
 */
void OSCProtocolConfigComponent::textEditorReturnKeyPressed(TextEditor& textEditor)
{
	ignoreUnused(textEditor);
}

/**
 * Callback function for button clicks on buttons.
 * @param button	The button object that was pressed.
 */
void OSCProtocolConfigComponent::buttonClicked(Button* button)
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
void OSCProtocolConfigComponent::AddListener(ProtocolConfigWindow* listener)
{
	m_parentListener = listener;
}

/**
 * Method to trigger dumping contents of configcomponent member
 * to list of objects to return to the app to initialize from
 *
 * @return	The list of objects to actively handle when running the engine.
 */
Array<RemoteObject> OSCProtocolConfigComponent::DumpActiveRemoteObjects()
{
	Array<RemoteObject> activeObjects;

	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		RemoteObject obj;

		//ToggleButton* c = m_RemObjEnableChecks.at(i).get();
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(i) && m_RemObjEnableChecks.at(i)->getToggleState())
		{
			obj.Id = (RemoteObjectIdentifier)i;

			// Since we expect the user to enter something like "1,4,6,8-12" to select
			// channels 1 4 6 8 9 10 11 12, we need to do some parsing. First we split
			// our input string for channels and records based on separators ',' or ';'
			// and second we inspect the results if they are singles or ranges.
			// The result is supposted to be two array with all channels and records included
			// as separate entries.
			Array<int> channels;
			Array<int> records;
			if (m_RemObjActiveChannelEdits.count(i) && m_RemObjActiveChannelEdits.at(i))
			{
				StringArray channelNumberSections;
				String chSelToSplit = m_RemObjActiveChannelEdits.at(i)->getText();
				channelNumberSections.addTokens(chSelToSplit, ",; ", "");
				for (int j = 0; j < channelNumberSections.size(); ++j)
				{
					StringArray channelNumbers;
					channelNumbers.addTokens(channelNumberSections[j], "-", "");
					if (channelNumbers.size() == 1)
					{
						channels.add(channelNumbers[0].getIntValue());
					}
					else if (channelNumbers.size() == 2)
					{
						int startVal = channelNumbers[0].getIntValue();
						int stopVal = channelNumbers[1].getIntValue();
						for (int k = startVal; k <= stopVal; ++k)
							channels.add(k);
					}
				}
			}
			if (m_RemObjMappingArea1Checks.count(i) && m_RemObjMappingArea1Checks.at(i) && m_RemObjMappingArea1Checks.at(i)->getToggleState())
				records.add(1);
			if (m_RemObjMappingArea2Checks.count(i) && m_RemObjMappingArea2Checks.at(i) && m_RemObjMappingArea2Checks.at(i)->getToggleState())
				records.add(2);
			if (m_RemObjMappingArea3Checks.count(i) && m_RemObjMappingArea3Checks.at(i) && m_RemObjMappingArea3Checks.at(i)->getToggleState())
				records.add(3);
			if (m_RemObjMappingArea4Checks.count(i) && m_RemObjMappingArea4Checks.at(i) && m_RemObjMappingArea4Checks.at(i)->getToggleState())
				records.add(4);

			// now that we have all channels and records, recursively iterate through both arrays
			// to add an entry to our active objects list for every resulting ch/rec combi object
			for (int j = 0; j < channels.size(); ++j)
			{
				if (records.size() > 0)
				{
					for (int k = 0; k < records.size(); ++k)
					{
						obj.Addr.first = int16(channels[j]);
						obj.Addr.second = int16(records[k]);
						activeObjects.add(obj);
					}
				}
				else
				{
					obj.Addr.first = int16(channels[j]);
					obj.Addr.second = -1;
					activeObjects.add(obj);
				}
			}
		}
	}

	return activeObjects;
}

/**
 * Method to trigger filling contents of
 * configcomponent member with list of objects
 *
 * @param Objs	The list of objects to set as default.
 */
void OSCProtocolConfigComponent::FillActiveRemoteObjects(const Array<RemoteObject>& Objs)
{
	Array<int> activeObjects;
	HashMap<int, Array<int>> channelsPerObj;
	HashMap<int, Array<int>> recordsPerObj;

	for (int i = 0; i < Objs.size(); ++i)
	{
		Array<int> selChs = channelsPerObj[Objs[i].Id];
		if (!selChs.contains(Objs[i].Addr.first))
		{
			selChs.add(Objs[i].Addr.first);
			channelsPerObj.set(Objs[i].Id, selChs);
		}

		Array<int> selRecs = recordsPerObj[Objs[i].Id];
		if (!selRecs.contains(Objs[i].Addr.second))
		{
			selRecs.add(Objs[i].Addr.second);
			recordsPerObj.set(Objs[i].Id, selRecs);
		}

		if (!activeObjects.contains(Objs[i].Id))
			activeObjects.add(Objs[i].Id);
	}

	for (int i = 0; i < activeObjects.size(); ++i)
	{
		int Id = activeObjects[i];
		if (m_RemObjEnableChecks.count(Id) && m_RemObjEnableChecks.at(Id) && !m_RemObjEnableChecks.at(Id)->getToggleState())
			m_RemObjEnableChecks.at(Id)->setToggleState(true, dontSendNotification);

		if (m_RemObjActiveChannelEdits.count(Id) && m_RemObjActiveChannelEdits.at(Id))
		{
			String selChanTxt;
			for (int j = 0; j < channelsPerObj[Id].size(); ++j)
			{
				if (channelsPerObj[Id][j] > 0)
				{
					if (!selChanTxt.isEmpty())
						selChanTxt << ", ";
					selChanTxt << channelsPerObj[Id][j];
				}
			}

			m_RemObjActiveChannelEdits.at(Id)->setText(selChanTxt);
		}

		if (m_RemObjMappingArea1Checks.count(Id) && m_RemObjMappingArea1Checks.at(Id))
			m_RemObjMappingArea1Checks.at(Id)->setToggleState(recordsPerObj[Id].contains(1), dontSendNotification);
		if (m_RemObjMappingArea2Checks.count(Id) && m_RemObjMappingArea2Checks.at(Id))
			m_RemObjMappingArea2Checks.at(Id)->setToggleState(recordsPerObj[Id].contains(2), dontSendNotification);
		if (m_RemObjMappingArea3Checks.count(Id) && m_RemObjMappingArea3Checks.at(Id))
			m_RemObjMappingArea3Checks.at(Id)->setToggleState(recordsPerObj[Id].contains(3), dontSendNotification);
		if (m_RemObjMappingArea4Checks.count(Id) && m_RemObjMappingArea4Checks.at(Id))
			m_RemObjMappingArea4Checks.at(Id)->setToggleState(recordsPerObj[Id].contains(4), dontSendNotification);
	}
}

/**
 * Method to trigger dumping of state of button for if active object handling shall be used
 *
 * @return	True if active object handling shall be used.
 */
bool OSCProtocolConfigComponent::DumpActiveHandlingUsed()
{
	for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
	{
		if (m_RemObjEnableChecks.count(i) && m_RemObjEnableChecks.at(i) && m_RemObjEnableChecks.at(i)->getToggleState())
			return true;
	}
	
	return false;
}

/**
 * Method to get the components' suggested size. This will be deprecated as soon as
 * the primitive UI is refactored and uses dynamic / proper layouting
 *
 * @return	The pair of int representing the suggested size for this component
 */
const std::pair<int, int> OSCProtocolConfigComponent::GetSuggestedSize()
{
	int width	=	UIS_OSCConfigWidth;
	int height	=	UIS_Margin_s + 
					UIS_Margin_s + UIS_ElmSize + 
					2 * UIS_Margin_m + UIS_ElmSize + 
					UIS_ElmSize + 
					((ROI_UserMAX - ROI_Invalid)*(UIS_Margin_s + UIS_ElmSize + UIS_Margin_s)) + 
					UIS_Margin_s + UIS_ElmSize + UIS_Margin_s +
					UIS_Margin_s;

	return std::pair<int, int>(width, height);
}


// **************************************************************************************
//    class ProtocolConfigWindow
// **************************************************************************************
/**
 * Constructor for the ProtocolConfigWindow class. Internal contents are created here.
 *
 * @param name							The name of the window
 * @param backgroundColour				The background color of the window
 * @param escapeKeyTriggersCloseButton	Flag value to set behaviour of hitting escape key on window
 * @param NId							Node id of the node the protocol this object is used to configure belongs to.
 * @param PId							Protocol id of the protocol this object is used to configure.
 * @param type							Type of the protocol this object is used to configure.
 * @param addToDesktop					Flag value to define if window is to be standalone or embedded in other content
 */
ProtocolConfigWindow::ProtocolConfigWindow(const String& name, Colour backgroundColour, bool escapeKeyTriggersCloseButton, NodeId NId, ProtocolId PId, ProtocolType type, bool addToDesktop)
	: DialogWindow(name, backgroundColour, escapeKeyTriggersCloseButton, addToDesktop)
{
	m_parentListener = 0;

	m_NId = NId;
	m_PId = PId;

	switch (type)
	{
	case ProtocolType::PT_OSCProtocol:
		m_configComponent = std::make_unique<OSCProtocolConfigComponent>();
		break;
	case ProtocolType::PT_OCAProtocol:
		// intentionally no break to run into default
	case ProtocolType::PT_DummyMidiProtocol:
		// intentionally no break to run into default
	case ProtocolType::PT_Invalid:
		// intentionally no break to run into default
	default:
		m_configComponent = std::make_unique<BasicProtocolConfigComponent>();
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
ProtocolConfigWindow::~ProtocolConfigWindow()
{

}

/**
 * Overloaded method that is called when window close button is pressed.
 * We enshure the window self-destroys here, but first notify the parent of it.
 */
void ProtocolConfigWindow::closeButtonPressed()
{
	if (m_parentListener)
		m_parentListener->childWindowCloseTriggered(this);
}

/**
 * Method to add parent object as 'listener'.
 * This is done in a way JUCE uses to connect child-parent relations for handling 'signal' calls
 */
void ProtocolConfigWindow::AddListener(ProtocolComponent* listener)
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
bool ProtocolConfigWindow::DumpConfig(ProcessingEngineConfig& config)
{
	config.SetProtocolPorts(m_NId, m_PId, m_configComponent->DumpProtocolPorts());
	config.SetUseActiveHandling(m_NId, m_PId, m_configComponent->DumpActiveHandlingUsed());
	config.SetRemoteObjectsToActivate(m_NId, m_PId, m_configComponent->DumpActiveRemoteObjects());

	return true;
}

/**
 * Proxy setter method to trigger filling contents of
 * configcomponent member with configuration contents
 *
 * @param config	The global configuration object.
 */
void ProtocolConfigWindow::SetConfig(const ProcessingEngineConfig& config)
{
	std::pair<int, int> ports(config.GetProtocolData(m_NId, m_PId).ClientPort, config.GetProtocolData(m_NId, m_PId).HostPort);
	m_configComponent->FillProtocolPorts(ports);
	m_configComponent->SetActiveHandlingUsed(config.GetUseActiveHandling(m_NId, m_PId));
	m_configComponent->FillActiveRemoteObjects(config.GetRemoteObjectsToActivate(m_NId, m_PId));
}

/**
 * Method to be called by child component to trigger closing down and applying
 * edited contents.
 */
void ProtocolConfigWindow::OnEditingFinished()
{
	closeButtonPressed();
}

/**
 * Proxy method to get the windows' components' suggested size.
 *
 * @return	The pair of int representing the suggested size for the window
 */
const std::pair<int, int> ProtocolConfigWindow::GetSuggestedSize()
{
	return m_configComponent->GetSuggestedSize();
}
