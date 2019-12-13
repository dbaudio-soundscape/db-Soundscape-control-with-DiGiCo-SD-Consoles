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

#include "ProtocolProcessor_Abstract.h"

#include "../ProcessingEngineNode.h"

// **************************************************************************************
//    class ProtocolProcessor_Abstract
// **************************************************************************************
/**
 * @fn bool ProtocolProcessor_Abstract::Start()
 * Pure virtual function to start the derived processor object
 */

/**
 * @fn bool ProtocolProcessor_Abstract::Stop()
 * Pure virtual function to stop the derived processor object
 */

/**
 * @fn void ProtocolProcessor_Abstract::SetRemoteObjectsActive(const Array<RemoteObject>& Objs)
 * @param Objs	The objects to set for active handling
 * Pure virtual function to set a set of remote object to be activly handled by derived processor object
 */

/**
 * @fn bool ProtocolProcessor_Abstract::SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * Pure virtual function to trigger sending a message by derived processor object
 */

/**
 * Constructor of abstract class ProtocolProcessor_Abstract.
 */
ProtocolProcessor_Abstract::ProtocolProcessor_Abstract()
{
	m_type = ProtocolType::PT_Invalid;
	m_IsRunning = false;
	m_messageListener = nullptr;
}

/**
 * Destructor
 */
ProtocolProcessor_Abstract::~ProtocolProcessor_Abstract()
{

}

/**
 * Sets the message listener object to be used for callback on message received.
 *
 * @param messageListener	The listener object
 */
void ProtocolProcessor_Abstract::AddListener(Listener *messageListener)
{
	m_messageListener = messageListener;
}

/**
 * Sets the configuration data for the protocol processor object.
 *
 * @param protocolData	The configuration data struct with config data
 * @param activeObjs	The objects to use as 'active' for this protocol
 * @param NId			The node id of the parent node this protocol processing object is child of (needed to access data from config)
 * @param PId			The protocol id of this protocol processing object (needed to access data from config)
 */
void ProtocolProcessor_Abstract::SetProtocolConfigurationData(const ProcessingEngineConfig::ProtocolData& protocolData, const Array<RemoteObject>& activeObjs, NodeId NId, ProtocolId PId)
{
	m_parentNodeId = NId;
	m_protocolProcessorId = PId;

	m_ipAddress = protocolData.IpAddress;
	m_clientPort = protocolData.ClientPort;
	m_hostPort = protocolData.HostPort;

	if (protocolData.UsesActiveRemoteObjects)
		SetRemoteObjectsActive(activeObjs);
}

/**
 * Getter for the type of this protocol processing object
 *
 * @return The type of this protocol processing object
 */
ProtocolType ProtocolProcessor_Abstract::GetType()
{
	return m_type;
}

/**
 * Getter for the id of this protocol processing object
 *
 * @return The id of this protocol processing object
 */
ProtocolId ProtocolProcessor_Abstract::GetId()
{
	return m_protocolProcessorId;
}