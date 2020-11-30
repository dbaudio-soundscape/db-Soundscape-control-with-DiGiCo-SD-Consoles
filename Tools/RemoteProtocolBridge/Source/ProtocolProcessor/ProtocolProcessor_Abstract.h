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

#include "../RemoteProtocolBridgeCommon.h"
#include "../ProcessingEngineConfig.h"
#include "../JuceLibraryCode/JuceHeader.h"


/**
 * Class ProtocolProcessor_Abstract is an abstract interfacing base class for protocol interaction.
 * It provides a gerenic interface to start, stop, initialize and interact with the protocol it
 * implements in a derived object. Its parent node object provides a handler method to processed
 * received protocol message data.
 */
class ProtocolProcessor_Abstract
{
public:
	/**
	 * Abstract embedded interface class for message data handling
	 */
	class Listener
	{
	public:
		Listener() {};
		virtual ~Listener() {};

		/**
		 * Method to be overloaded by ancestors to act as an interface
		 * for handling of received message data
		 */
		virtual void OnProtocolMessageReceived(ProtocolProcessor_Abstract* receiver, RemoteObjectIdentifier id, RemoteObjectMessageData& msgData) = 0;
	};

public:
	ProtocolProcessor_Abstract();
	virtual ~ProtocolProcessor_Abstract();

	void AddListener(Listener *messageReceiver);
	ProtocolType GetType();
	ProtocolId GetId();
	virtual void SetProtocolConfigurationData(const ProcessingEngineConfig::ProtocolData& protocolData, const Array<RemoteObject>& activeObjs, NodeId NId, ProtocolId PId);

	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	virtual void SetRemoteObjectsActive(const Array<RemoteObject>& Objs) = 0;
	virtual bool SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) = 0;

protected:
	Listener				*m_messageListener;		/**< The parent node object. Needed for e.g. triggering receive notifications. */
	ProtocolType			m_type;					/**< Processor type regarding the protocol being handled */
	NodeId					m_parentNodeId;			/**< The id of the objects' parent node. */
	ProtocolId				m_protocolProcessorId;	/**< The id of the processor object itself. */

	String					m_ipAddress;			/**< IP Address where messages will be sent to / received from. */
	int						m_clientPort;			/**< TCP/UDP port where messages will be received from. */
	int						m_hostPort;				/**< TCP/UDP port where messages will be sent to. */

	bool					m_IsRunning;			/**< Bool indication if the processor is successfully running. */

};
