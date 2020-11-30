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

#include "OSCProtocolProcessor.h"

#include "../../ProcessingEngineConfig.h"
//#include "../../ProcessingEngineNode.h"


// **************************************************************************************
//    class OSCProtocolProcessor
// **************************************************************************************
/**
 * Derived OSC remote protocol processing class
 */
OSCProtocolProcessor::OSCProtocolProcessor(int listenerPortNumber)
	: ProtocolProcessor_Abstract(), m_oscReceiver(listenerPortNumber)
{
	m_type = ProtocolType::PT_OSCProtocol;
	m_oscMsgRate = ET_DefaultPollingRate;

	// OSCProtocolProcessor derives from OSCReceiver::Listener
	m_oscReceiver.addListener(this);
}

/**
 * Destructor
 */
OSCProtocolProcessor::~OSCProtocolProcessor()
{
	Stop();
}

/**
 * Overloaded method to start the protocol processing object.
 * Usually called after configuration has been set.
 */
bool OSCProtocolProcessor::Start()
{
	bool successS = false;
	bool successR = false;

	// Connect both sender and receiver  
	successS = m_oscSender.connect(m_ipAddress, m_clientPort);
	jassert(successS);

	successR = m_oscReceiver.connect();
	jassert(successR);

	m_IsRunning = (successS && successR);

	return m_IsRunning;
}

/**
 * Overloaded method to stop to protocol processing object.
 */
bool OSCProtocolProcessor::Stop()
{
	m_IsRunning = false;

	bool successS = false;
	bool successR = false;

	// Connect both sender and receiver  
	successS = m_oscSender.disconnect();
	jassert(successS);

	successR = m_oscReceiver.disconnect();
	jassert(successR);

	return (successS && successR);
}

/**
 * Reimplemented setter for protocol config data.
 * This calls the base implementation and in addition
 * takes care of setting polling interval.
 *
 * @param protocolData	The configuration data struct with config data
 * @param activeObjs	The objects to use as 'active' for this protocol
 * @param NId			The node id of the parent node this protocol processing object is child of (needed to access data from config)
 * @param PId			The protocol id of this protocol processing object (needed to access data from config)
 */
void OSCProtocolProcessor::SetProtocolConfigurationData(const ProcessingEngineConfig::ProtocolData& protocolData, const Array<RemoteObject>& activeObjs, NodeId NId, ProtocolId PId)
{
	m_oscMsgRate = protocolData.PollingInterval;

	ProtocolProcessor_Abstract::SetProtocolConfigurationData(protocolData, activeObjs, NId, PId);
}

/**
 * Setter for remote object to specifically activate.
 * For OSC processing this is used to activate internal polling
 * of the object values.
 * In case an empty list of objects is passed, polling is stopped and
 * the internal list is cleared.
 *
 * @param Objs	The list of RemoteObjects that shall be activated
 */
void OSCProtocolProcessor::SetRemoteObjectsActive(const Array<RemoteObject>& Objs)
{
	// Start timer callback if objects are to be polled
	if (Objs.size() > 0)
	{
		m_activeRemoteObjects = Objs;

		startTimer(m_oscMsgRate);
	}
	else
	{
		m_activeRemoteObjects.clear();

		stopTimer();
	}
}

/**
 * Method to trigger sending of a message
 *
 * @param Id		The id of the object to send a message for
 * @param msgData	The message payload and metadata
 */
bool OSCProtocolProcessor::SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	if (!m_IsRunning)
		return false;

	bool sendSuccess = false;

	String addressString = GetRemoteObjectString(Id);

	if (msgData.addrVal.second != INVALID_ADDRESS_VALUE)
		addressString += String::formatted("/%d", msgData.addrVal.second);

	if (msgData.addrVal.first != INVALID_ADDRESS_VALUE)
		addressString += String::formatted("/%d", msgData.addrVal.first);

	uint16 valSize;
	switch (msgData.valType)
	{
	case ROVT_INT:
		valSize = sizeof(int);
		break;
	case ROVT_FLOAT:
		valSize = sizeof(float);
		break;
	case ROVT_STRING:
		jassertfalse; // String not (yet?) supported
		valSize = 0;
		break;
	case ROVT_NONE:
	default:
		valSize = 0;
		break;
	}

	jassert((msgData.valCount*valSize) == msgData.payloadSize);

	switch (msgData.valType)
	{
	case ROVT_INT:
		{
		jassert(msgData.valCount < 4); // max known d&b OSC msg val cnt would be positioning xyz
		int multivalues[3];

		for (int i = 0; i < msgData.valCount; ++i)
			multivalues[i] = ((int*)msgData.payload)[i];

		if (msgData.valCount == 1)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0]));
		else if (msgData.valCount == 2)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0], multivalues[1]));
		else if (msgData.valCount == 3)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0], multivalues[1], multivalues[2]));
		else
			sendSuccess = m_oscSender.send(OSCMessage(addressString));
		}
		break;
	case ROVT_FLOAT:
		{
		jassert(msgData.valCount < 4); // max known d&b OSC msg val cnt would be positioning xyz
		float multivalues[3];

		for (int i = 0; i < msgData.valCount; ++i)
			multivalues[i] = ((float*)msgData.payload)[i];

		if (msgData.valCount == 1)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0]));
		else if (msgData.valCount == 2)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0], multivalues[1]));
		else if (msgData.valCount == 3)
			sendSuccess = m_oscSender.send(OSCMessage(addressString, multivalues[0], multivalues[1], multivalues[2]));
		else
			sendSuccess = m_oscSender.send(OSCMessage(addressString));
		}
		break;
	case ROVT_NONE:
		sendSuccess = m_oscSender.send(OSCMessage(addressString));
		break;
	case ROVT_STRING:
	default:
		break;
	}

	return sendSuccess;
}

/**
* Called when the OSCReceiver receives a new OSC bundle.
* The bundle is processed and all contained individual messages passed on
* to oscMessageReceived for further handling.
*
* @param bundle				The received OSC bundle.
* @param senderIPAddress	The ip the bundle originates from.
* @param senderPort			The port this bundle was received on.
*/
void OSCProtocolProcessor::oscBundleReceived(const OSCBundle &bundle, const String& senderIPAddress, const int& senderPort)
{
	if (senderIPAddress != m_ipAddress)
	{
#ifdef DEBUG
		DBG("NId"+String(m_parentNodeId) 
			+ " PId"+String(m_protocolProcessorId) + ": ignore unexpected OSC bundle from " 
			+ senderIPAddress + " (" + m_ipAddress + " expected)");
#endif
		return;
	}

	for (int i = 0; i < bundle.size(); ++i)
	{
		if (bundle[i].isBundle())
			oscBundleReceived(bundle[i].getBundle(), senderIPAddress, senderPort);
		else if (bundle[i].isMessage())
			oscMessageReceived(bundle[i].getMessage(), senderIPAddress, senderPort);
	}
}

/**
 * Called when the OSCReceiver receives a new OSC message and parses its contents to
 * pass the received data to parent node for further handling
 *
 * @param message			The received OSC message.
* @param senderIPAddress	The ip the message originates from.
* @param senderPort			The port this message was received on.
 */
void OSCProtocolProcessor::oscMessageReceived(const OSCMessage &message, const String& senderIPAddress, const int& senderPort)
{
	ignoreUnused(senderPort);
	if (senderIPAddress != m_ipAddress)
	{
#ifdef DEBUG
		DBG("NId" + String(m_parentNodeId)
			+ " PId" + String(m_protocolProcessorId) + ": ignore unexpected OSC message from " 
			+ senderIPAddress + " (" + m_ipAddress + " expected)");
#endif
		return;
	}

	int messageSize = message.size();
	bool isContentMessage = (messageSize > 0); // the value count is reported by JUCE as OSCMessage::size

	RemoteObjectMessageData newMsgData;
	newMsgData.addrVal.first = INVALID_ADDRESS_VALUE;
	newMsgData.addrVal.second = INVALID_ADDRESS_VALUE;
	newMsgData.valType = ROVT_NONE;
	newMsgData.valCount = 0;
	newMsgData.payload = 0;
	newMsgData.payloadSize = 0;

	String addressString = message.getAddressPattern().toString();
	// Check if the incoming message is a response to a sent "ping" heartbeat.
	if (addressString.startsWith(GetRemoteObjectString(ROI_HeartbeatPong)) && m_messageListener)
		m_messageListener->OnProtocolMessageReceived(this, ROI_HeartbeatPong, newMsgData);
	// Check if the incoming message is a response to a sent "pong" heartbeat.
	else if (addressString.startsWith(GetRemoteObjectString(ROI_HeartbeatPing)) && m_messageListener)
		m_messageListener->OnProtocolMessageReceived(this, ROI_HeartbeatPing, newMsgData);
	// Check if the incoming message contains parameters.
	else if (messageSize > 0)
	{
		// Parse the Source ID
		int sourceId = (addressString.fromLastOccurrenceOf("/", false, true)).getIntValue();
		jassert(sourceId > 0);
		if (sourceId > 0)
		{
			RemoteObjectIdentifier newObjectId;

			newMsgData.addrVal.first = int16(sourceId);
			newMsgData.valType = ROVT_FLOAT;
		
			float newFloatValue;
			float newDualFloatValue[2];
			int newIntValue;

			// Determine which parameter was changed depending on the incoming message's address pattern.
			if (addressString.startsWith(GetRemoteObjectString(ROI_SoundObject_Position_XY)))
			{
				// Parse the Mapping ID
				addressString = addressString.upToLastOccurrenceOf("/", false, true);
				newMsgData.addrVal.second = int16((addressString.fromLastOccurrenceOf("/", false, true)).getIntValue());
				jassert(newMsgData.addrVal.second > 0);

				newObjectId = ROI_SoundObject_Position_XY;

				if (isContentMessage)
				{
					newDualFloatValue[0] = message[0].getFloat32();
					newDualFloatValue[1] = message[1].getFloat32();

					newMsgData.valCount = 2;
					newMsgData.payload = &newDualFloatValue;
					newMsgData.payloadSize = 2 * sizeof(float);
				}
			}
			else if (addressString.startsWith(GetRemoteObjectString(ROI_SoundObject_Position_X)))
			{
				// Parse the Mapping ID
				addressString = addressString.upToLastOccurrenceOf("/", false, true);
				newMsgData.addrVal.second = int16((addressString.fromLastOccurrenceOf("/", false, true)).getIntValue());
				jassert(newMsgData.addrVal.second > 0);

				newObjectId = ROI_SoundObject_Position_X;
				
				if (isContentMessage)
				{
					newFloatValue = message[0].getFloat32();

					newMsgData.valCount = 1;
					newMsgData.payload = &newFloatValue;
					newMsgData.payloadSize = sizeof(float);
				}
			}
			else if (addressString.startsWith(GetRemoteObjectString(ROI_SoundObject_Position_Y)))
			{
				// Parse the Mapping ID
				addressString = addressString.upToLastOccurrenceOf("/", false, true);
				newMsgData.addrVal.second = int16((addressString.fromLastOccurrenceOf("/", false, true)).getIntValue());
				jassert(newMsgData.addrVal.second > 0);

				newObjectId = ROI_SoundObject_Position_Y;

				if (isContentMessage)
				{
					newFloatValue = message[0].getFloat32();

					newMsgData.valCount = 1;
					newMsgData.payload = &newFloatValue;
					newMsgData.payloadSize = sizeof(float);
				}
			}
			else if (addressString.startsWith(GetRemoteObjectString(ROI_SoundObject_Spread)))
			{
				newObjectId = ROI_SoundObject_Spread;

				if (isContentMessage)
				{
					newFloatValue = message[0].getFloat32();

					newMsgData.valCount = 1;
					newMsgData.payload = &newFloatValue;
					newMsgData.payloadSize = sizeof(float);
				}
			}
			else if (addressString.startsWith(GetRemoteObjectString(ROI_SoundObject_DelayMode)))
			{
				newObjectId = ROI_SoundObject_DelayMode;

				if (isContentMessage)
				{
					// delaymode should be an int, but since some OSC appliances can only process floats,
					// we need to be prepared to optionally accept float as well
					if (message[0].isInt32())
						newIntValue = message[0].getInt32();
					else if (message[0].isFloat32())
						newIntValue = (int)round(message[0].getFloat32());

					newMsgData.valType = ROVT_INT;
					newMsgData.valCount = 1;
					newMsgData.payload = &newIntValue;
					newMsgData.payloadSize = sizeof(int);
				}
			}
			else if (addressString.startsWith(GetRemoteObjectString(ROI_ReverbSendGain)))
			{
				newObjectId = ROI_ReverbSendGain;

				if (isContentMessage)
				{
					newFloatValue = message[0].getFloat32();

					newMsgData.valCount = 1;
					newMsgData.payload = &newFloatValue;
					newMsgData.payloadSize = sizeof(float);
				}
			}
			else
			{
				newObjectId = ROI_Invalid;
			}

			// provide the received message to parent node
			if (m_messageListener)
				m_messageListener->OnProtocolMessageReceived(this, newObjectId, newMsgData);
		}
	}
}

/**
 * Private method to get OSC object specific ObjectName string
 *
 * @param id	The object id to get the OSC specific object name
 * @return		The OSC specific object name
 */
String OSCProtocolProcessor::GetRemoteObjectString(RemoteObjectIdentifier id)
{
	switch (id)
	{
	case ROI_HeartbeatPong:
		return "/pong";
	case ROI_HeartbeatPing:
		return "/ping";
	case ROI_SoundObject_Position_X:
		return "/dbaudio1/coordinatemapping/source_position_x";
	case ROI_SoundObject_Position_Y:
		return "/dbaudio1/coordinatemapping/source_position_y";
	case ROI_SoundObject_Position_XY:
		return "/dbaudio1/coordinatemapping/source_position_xy";
	case ROI_SoundObject_Spread:
		return "/dbaudio1/positioning/source_spread";
	case ROI_SoundObject_DelayMode:
		return "/dbaudio1/positioning/source_delaymode";
	case ROI_ReverbSendGain:
		return "/dbaudio1/matrixinput/reverbsendgain";
	default:
		return "";
	}
}

/**
 * Timer callback function, which will be called at regular intervals to
 * send out OSC poll messages.
 */
void OSCProtocolProcessor::timerCallback()
{
	int objectCount = m_activeRemoteObjects.size();
	for (int i = 0; i < objectCount; i++)
	{
		RemoteObjectMessageData msgData;
		msgData.addrVal = m_activeRemoteObjects[i].Addr;
		msgData.valCount = 0;
		msgData.valType = ROVT_NONE;
		msgData.payload = 0;
		msgData.payloadSize = 0;
		
		SendMessage(m_activeRemoteObjects[i].Id, msgData);
	}
}