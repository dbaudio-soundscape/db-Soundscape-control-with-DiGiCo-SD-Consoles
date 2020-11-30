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

#include "ProcessingEngineConfig.h"
#include "ProcessingEngineNode.h"
#include "RemoteProtocolBridgeCommon.h"

#include <JuceHeader.h>

// Fwd. Declarations
class LoggingTarget_Interface;


/**
 * Class ProcessingEngine is the managing core of the application that is setup
 * using config class and holds multiple node instances that represent single protocol bridges.
 * The engine is responsible for instanciating node objects and handle their configuration, logging output, running state, etc.
 */
class ProcessingEngine : public ProcessingEngineNode::NodeListener
{
public:
	ProcessingEngine();
	~ProcessingEngine();

	// ============================================================
	bool IsLoggingEnabled();
	bool IsRunning();
	void SetConfig(ProcessingEngineConfig &config);
	void SetLoggingEnabled(bool enable);
	void SetLoggingTarget(LoggingTarget_Interface* logTarget);
	bool Start();
	void Stop();

	// ============================================================
	void HandleNodeData(NodeId nodeId, ProtocolId senderProtocolId, ProtocolType senderProtocolType, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

private:
	// ============================================================
	ProcessingEngineConfig											m_configuration;	/**< Internal configuration object to hold runtime config and to be passed around for anyone to extract desired config info from. */
	std::map<unsigned int, std::unique_ptr<ProcessingEngineNode>>	m_ProcessingNodes;	/**< Hash table to hold all node objects currently active as define by config. */
	bool															m_IsRunning;		/**< Running state flag. */
	bool															m_LoggingEnabled;	/**< Logging state flag. */
	LoggingTarget_Interface*										m_logTarget;		/**< Pointer to the object that shall receive logging data from the engine. */
	Array<String>													m_loggingQueue;		/**< Array queue with messages to be logged. */

};
