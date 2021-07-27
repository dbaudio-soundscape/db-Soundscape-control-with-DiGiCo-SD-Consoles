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

/**
 * Class ProcessingEngineConfig is class for managing application runtime configuration.
 * It is used to be passed to different object wihtin application, that can then access it to
 * get specific config infos through its interface methods
 */
class ProcessingEngineConfig
{

public:
	/**
	 * Type to combine generic protocol configuration values
	 */
    struct ProtocolData
	{
		ProtocolId			Id;							/**< The protocol id of a protocol instance. */
		ProtocolType		Type;						/**< The protocol type of a protocol instance. */
		String				IpAddress;					/**< The ip address for a protocol instance. */
		int					ClientPort;					/**< The tcp/udp port to use as client. */
		int					HostPort;					/**< The tcp/udp port to use as host. */
		bool				UsesActiveRemoteObjects;    /**< Flag specifying if this protocol is supposed to activly handle specified remote objects. */
		Array<RemoteObject>	RemoteObjects;				/**< The remote objects actively used by a protocol instance. */
		int					PollingInterval;			/**< The polling interval in ms. */
	};

	/**
	 * Type to define configuration for how object in a node shall be handled
	 */
	struct ObjectHandlingData
	{
		ObjectHandlingMode	Mode;						/**< The mode the node should operate in to handl msg data (defines what internal handling object is created). */
		int					ACnt;						/**< Channel count configuration value that is to be expected per protocol type A for object handling module. */
		int					BCnt;						/**< Channel count configuration value that is to be expected per protocol type B for object handling module. */
		double				Prec;						/**< Data precision value to be used for evaluation of valu changes of incoming data. */
	};

	/**
	 * Type to combine generic bridging node configuration values
	 */
    struct NodeData
	{
		NodeId				Id;							/**< The id of a processing node. */
		ObjectHandlingData	ObjectHandling;				/**< The mode the node should operate in to handl msg data (defines what internal handling object is created). */
		Array<ProtocolId>	RoleAProtocols;				/**< The role A protocol ids per node. */
		Array<ProtocolId>	RoleBProtocols;				/**< The role B protocol ids per node. */
	};

public:
	ProcessingEngineConfig();
	~ProcessingEngineConfig();
	ProcessingEngineConfig(const ProcessingEngineConfig&r);

	ProcessingEngineConfig& operator=(const ProcessingEngineConfig& r);

	NodeData			GetNodeData(NodeId NId) const;
	Array<NodeId>		GetNodeIds() const;
	ObjectHandlingData	GetObjectHandlingData(NodeId NId) const;
	bool				SetObjectHandlingData(NodeId NId, const ObjectHandlingData& ohData);
	int					GetPollingInterval(NodeId NId, ProtocolId PId) const;
	bool				SetPollingInterval(NodeId NId, ProtocolId PId, int interval);
	ProtocolData		GetProtocolData(NodeId NId, ProtocolId PId) const;
	bool				SetProtocolData(NodeId NId, ProtocolId PId, const ProtocolData& data);
	Array<ProtocolId>	GetProtocolAIds(NodeId NId) const;
	Array<ProtocolId>	GetProtocolBIds(NodeId NId) const;
	Array<RemoteObject>	GetRemoteObjectsToActivate(NodeId NId, ProtocolId PId) const;
	bool				SetRemoteObjectsToActivate(NodeId NId, ProtocolId PId, const Array<RemoteObject>& Objs);
	bool				GetUseActiveHandling(NodeId NId, ProtocolId PId) const;
	bool				SetUseActiveHandling(NodeId NId, ProtocolId PId, bool enable);
	bool				SetProtocolPorts(NodeId NId, ProtocolId PId, const std::pair<int, int>& ports);

	bool				IsTrafficLoggingAllowed() const;
	void				SetTrafficLoggingAllowed(bool allowed = true);
	bool				IsEngineStartOnAppStart() const;
	void				SetEngineStartOnAppStart(bool start = true);
    
    bool				InitConfiguration();
	bool				ReadConfiguration();
	bool				ReadActiveObjects(XmlElement* ActiveObjectsElement, Array<RemoteObject>& RemoteObjects);
	bool				ReadPollingInterval(XmlElement* ActiveObjectsElement, int& PollingInterval);
	bool				WriteConfiguration();
	bool				WriteActiveObjects(XmlElement* ActiveObjectsElement, Array<RemoteObject> const& RemoteObjects);

	void				SetNode(NodeId NId, NodeData& node);
	void				AddDefaultNode();
	void				RemoveNode(NodeId NId);

	void				AddDefaultProtocolA(NodeId NId);
	void				AddDefaultProtocolB(NodeId NId);
	void				RemoveProtocol(NodeId NId, ProtocolId PId);

	void				Clear();

	static String				ProtocolTypeToString(ProtocolType pt);
	static ProtocolType			ProtocolTypeFromString(String type);
	static String				ObjectHandlingModeToString(ObjectHandlingMode ohm);
	static ObjectHandlingMode	ObjectHandlingModeFromString(String mode);

	static String GetObjectDescription(RemoteObjectIdentifier Id);
	static bool IsKeepaliveObject(RemoteObjectIdentifier Id);


private:
    int GetNextUniqueId();
	int ValidateUniqueId(int uniqueId);

	Array<NodeId>						m_nodeIds;				/**< Array with ids of all nodes for this configuration. */
	HashMap<NodeId, NodeData>			m_nodeData;				/**< Map combining node ids with actual node configurations. */
	HashMap<ProtocolId, ProtocolData>	m_protocolData;			/**< The protocols of this node. */
	
	bool								m_TrafficLoggingAllowed;/**< Flag defining if the TrafficLogging togglebutton should be available. */
	bool								m_EngineStartOnAppStart;/**< Flag defining if the engine should be automatically started on app start. */

	String								m_configFilePath;		/**< The path string where the config file should be read from / written to. */

};
