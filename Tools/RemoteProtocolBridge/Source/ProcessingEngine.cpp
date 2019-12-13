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

#include "ProcessingEngine.h"

#include "LoggingTarget_Interface.h"

#include "ProtocolProcessor/OSCProtocolProcessor/OSCProtocolProcessor.h"
#include "ProtocolProcessor/OCAProtocolProcessor/OCAProtocolProcessor.h"

// **************************************************************************************
//    class ProcessingEngine
// **************************************************************************************
/**
 * Constructor of central processing engine
 */
ProcessingEngine::ProcessingEngine()
{
	m_IsRunning = false;
	m_LoggingEnabled = false;
	m_logTarget = 0;
}

/**
 * Destructor of central processing engine
 */
ProcessingEngine::~ProcessingEngine()
{
}

/**
 * Initialization+Startup method for the engine.
 * It is responsible to create, configure and start the child nodes
 * and sets the interenal running flag to true if startup was successful.
 *
 * @return	True if startup was successful, otherwise false
 */
bool ProcessingEngine::Start()
{
	bool startSuccess = true;

	Array<unsigned int> NodeIds = m_configuration.GetNodeIds();
	for (int i = 0; i < NodeIds.size(); ++i)
	{
		ProcessingEngineNode* node = new ProcessingEngineNode(this);
		node->SetNodeConfiguration(m_configuration, NodeIds[i]);
		startSuccess = startSuccess && node->Start();

		m_ProcessingNodes[NodeIds[i]] = std::unique_ptr<ProcessingEngineNode>(node);
	}

	if (startSuccess)
		m_IsRunning = true;

	return startSuccess;
}

/**
 * Shuts down the engine and clears the running flag.
 * This includes shutting down the child nodes as well.
 */
void ProcessingEngine::Stop()
{
	m_ProcessingNodes.clear();

	m_IsRunning = false;
}

/**
 * Getter for the is running flag.
 *
 * @return	True if the engine is in running state, otherwise false
 */
bool ProcessingEngine::IsRunning()
{
	return m_IsRunning;
}

/**
 * Setter for the configuration object that holds app config data
 *
 * @param config		The configuration object to set
 */
void ProcessingEngine::SetConfig(ProcessingEngineConfig &config)
{
	m_configuration = config;
}

/**
 * Method to enqueue logging data regarding message traffic in the nodes.
 * This method is to be used by child nodes to output their message traffic to log.
 *
 * @param node		The node the logging data originates from
 * @param sender	The protocol processor that has received the message
 * @param Id		The remote object id the message refers to
 * @param msgData	The remote object message data of the object that is currently sent by a node and therefor forwarded here to be logged
 */
void ProcessingEngine::LogInput(ProcessingEngineNode* node, ProtocolProcessor_Abstract* sender, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	if (!IsLoggingEnabled())
		return;

	if (!node || !sender)
		return;

	if (m_logTarget)
	{
		m_logTarget->AddLogData(node->GetId(), sender->GetId(), sender->GetType(), Id, msgData);
	}
}

/**
 * Setter for internal logging enabled flag.
 *
 * @param enable	The state (en-/disable) to set the internal flag to
 */
void ProcessingEngine::SetLoggingEnabled(bool enable)
{
	m_LoggingEnabled = enable;
}

/**
 * Getter for internal logging enabled flag
 *
 * @return	True if enabled, false if not
 */
bool ProcessingEngine::IsLoggingEnabled()
{
	return m_LoggingEnabled;
}

/**
* Setter for logging target object to be used to push messages to
*
* @param logTarget	The target object for logging data
*/
void ProcessingEngine::SetLoggingTarget(LoggingTarget_Interface* logTarget)
{
	m_logTarget = logTarget;
}
