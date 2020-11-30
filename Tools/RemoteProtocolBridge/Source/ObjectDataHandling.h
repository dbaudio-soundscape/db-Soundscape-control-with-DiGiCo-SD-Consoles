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

#include "RemoteProtocolBridgeCommon.h"

#include <JuceHeader.h>

// Fwd. declarations
class ProcessingEngineNode;
class ProcessingEngineConfig;

/**
 * Class ObjectDataHandling_Abstract is an abstract interfacing base class for .
 */
class ObjectDataHandling_Abstract
{
public:
	ObjectDataHandling_Abstract(ProcessingEngineNode* parentNode);
	virtual ~ObjectDataHandling_Abstract();

	virtual void SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId);
	ObjectHandlingMode GetMode();

	void AddProtocolAId(ProtocolId PAId);
	void AddProtocolBId(ProtocolId PBId);
	void ClearProtocolIds();

	virtual bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) = 0;

protected:
	ProcessingEngineNode*	m_parentNode;			/**< The parent node object. Needed for e.g. triggering receive notifications. */
	ObjectHandlingMode		m_mode;					/**< Mode identifier enabling resolving derived instance type. */
	NodeId					m_parentNodeId;			/**< The id of the objects' parent node. */
	Array<ProtocolId>		m_protocolAIds;			/**< Id list of protocols of type A that is active for the node and this handling module therefor. */
	Array<ProtocolId>		m_protocolBIds;			/**< Id list of protocols of type B that is active for the node and this handling module therefor. */

};


/**
 * Class BypassHandling is a class for dummy bypassing message data without modifications.
 */
class BypassHandling : public ObjectDataHandling_Abstract
{
public:
	BypassHandling(ProcessingEngineNode* parentNode);
	~BypassHandling();

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

protected:

};


/**
 * Class Remap_A_X_Y_to_B_XY_Handling is a class for hardcoded remapping fo message data
 * of separate received x and y position data from protocol a to a combined xy data message
 * forwarded to protocol b. Combined xy data received from protocol b on the other hand is
 * split in two messages and sent out over protocol a. Other data is simply bypassed.
 */
class Remap_A_X_Y_to_B_XY_Handling : public ObjectDataHandling_Abstract
{
	// helper type to be used in hashmap for three position related floats
    struct xyzVals
	{
		float x;	//< x pos component. */
		float y;	//< y pos component. */
		float z;	//< z pos component. */
	};

public:
	Remap_A_X_Y_to_B_XY_Handling(ProcessingEngineNode* parentNode);
	~Remap_A_X_Y_to_B_XY_Handling();

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

protected:
	HashMap<int32, xyzVals> m_currentPosValue;	/**< Hash to hold current x y values for all currently used objects (identified by merge of obj. addressing to a single uint32 used as key). */

};


/**
 * Class Mux_nA_to_mB is a class for multiplexing modulo n channels of protocols typeA 
 * to modulo m channels of protocols typeB.
 * This is especially introduced as solution for #35559 - Mapping of 128 channels of a DiGiCo
 * mixing desk to multiple DS100, each capable of processing 64 channels
 */
class Mux_nA_to_mB : public ObjectDataHandling_Abstract
{
public:
	Mux_nA_to_mB(ProcessingEngineNode* parentNode);
	~Mux_nA_to_mB();

	void SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId) override;

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

private:
	int m_protoChCntA;	/**< Channel count configuration value that is to be expected per protocol type A. */
	int m_protoChCntB;	/**< Channel count configuration value that is to be expected per protocol type B. */

};


/**
 * Class Forward_only_valueChanges is a class for filtering received value data to only forward changed values.
 */
class Forward_only_valueChanges : public ObjectDataHandling_Abstract
{
public:
	Forward_only_valueChanges(ProcessingEngineNode* parentNode);
	~Forward_only_valueChanges();

	void SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId) override;

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

private:
	bool IsChangedDataValue(const RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData);
	void SetCurrentDataValue(const RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData);
	
	std::map<RemoteObjectIdentifier, std::map<RemoteObjectAddressing, RemoteObjectMessageData>>	m_currentValues;	/**< Hash of current value data to use to compare to incoming data regarding value changes. */
	double m_precision;																								/**< Value precision to use for processing. */
};

/**
 * Class Forward_A_to_B_only is a class for filtering received value data from RoleB protocols to not be forwarded
 */
class Forward_A_to_B_only : public ObjectDataHandling_Abstract
{
public:
	Forward_A_to_B_only(ProcessingEngineNode* parentNode);
	~Forward_A_to_B_only();

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

protected:

};

/**
 * Class Reverse_B_to_A_only is a class for filtering received value data from RoleA protocols to not be forwarded
 */
class Reverse_B_to_A_only : public ObjectDataHandling_Abstract
{
public:
	Reverse_B_to_A_only(ProcessingEngineNode* parentNode);
	~Reverse_B_to_A_only();

	bool OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData) override;

protected:

};