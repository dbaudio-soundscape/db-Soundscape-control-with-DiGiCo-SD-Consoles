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

#include "ProcessingEngineConfig.h"

// **************************************************************************************
//    class ProcessingEngineConfig
// **************************************************************************************
/**
 * Constructs an object
 * and calls the InitConfiguration method
 */
ProcessingEngineConfig::ProcessingEngineConfig()
{
	m_TrafficLoggingAllowed = true;
	m_EngineStartOnAppStart = false;

	m_configFilePath = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory).getFullPathName() + "/RemoteProtocolBridge/";
	File(m_configFilePath).createDirectory();
}

/**
 * Destructor for the object
 */
ProcessingEngineConfig::~ProcessingEngineConfig()
{
}

/**
 * Copy constructor for the object
 */
ProcessingEngineConfig::ProcessingEngineConfig(const ProcessingEngineConfig& source)
{
	*this = source;
}

/**
 * Assignment operator for the object
 * Takes care of copying all internal data
 *
 * @param r	The object to copy data from
 * @return	The object copy
 */
ProcessingEngineConfig& ProcessingEngineConfig::operator=(const ProcessingEngineConfig& r)
{
	m_nodeIds = r.m_nodeIds;

	for (HashMap<NodeId, NodeData>::Iterator NodeIter = r.m_nodeData.begin(); NodeIter != r.m_nodeData.end(); ++NodeIter)
		m_nodeData.set(NodeIter.getKey(), NodeIter.getValue());

	for (HashMap<ProtocolId, ProtocolData>::Iterator ProtocolIter = r.m_protocolData.begin(); ProtocolIter != r.m_protocolData.end(); ++ProtocolIter)
		m_protocolData.set(ProtocolIter.getKey(), ProtocolIter.getValue());

	return *this;
}

/**
 * Getter for the object handling data for a given node off of the current configuration
 *
 * @param NId	The node id to use to get objectdata handling mode for
 * @return		The configured object handling data for the requested node
 */
ProcessingEngineConfig::ObjectHandlingData	ProcessingEngineConfig::GetObjectHandlingData(NodeId NId) const
{
	return GetNodeData(NId).ObjectHandling;
}

/**
 * Getter for the configuration data of the object handling module of a given node
 *
 * @param NId		The node id to get protocol config data for
 * @param ohData	The object handling data to set to config
 * @return	True on success
 */
bool ProcessingEngineConfig::SetObjectHandlingData(NodeId NId, const ProcessingEngineConfig::ObjectHandlingData& ohData)
{
	if (!m_nodeIds.contains(NId))
		return false;

	NodeData nd = m_nodeData[NId];

	nd.ObjectHandling = ohData;
	m_nodeData.set(NId, nd);

	return true;
}

/**
 * Getter for the list of node ids of current configuration
 *
 * @return	The list of node ids of current configuration
 */
Array<NodeId> ProcessingEngineConfig::GetNodeIds() const
{
	return m_nodeIds;
}

/**
 * Getter for the node configuration for a given node off of the current configuration
 *
 * @param NId	The node id to use to get node configuration data for
 * @return		The configuration data for the requested node
 */
ProcessingEngineConfig::NodeData ProcessingEngineConfig::GetNodeData(NodeId NId) const
{
	NodeData node;

	node.Id = 0;
	node.ObjectHandling.Mode = OHM_Invalid;
	node.ObjectHandling.ACnt = 0;
	node.ObjectHandling.BCnt = 0;

	if (m_nodeData.contains(NId))
		node = m_nodeData[NId];

	return node;
}

/**
 * Getter for the list of remote objects to activly handle for a given node
 *
 * @param NId	The node id to use to get objectdata for
 * @param PId	The protocol id to use to get objectdata for
 * @return		The list of objects to activly handle for the given node
 */
Array<RemoteObject> ProcessingEngineConfig::GetRemoteObjectsToActivate(NodeId NId, ProtocolId PId) const
{
	return GetProtocolData(NId, PId).RemoteObjects;
}

/**
 * Setter for the list of remote objects to activly handle for a given node
 *
 * @param NId	The node id to use to get objectdata for
 * @param PId	The protocol id to use to get objectdata for
 * @param Objs	The list of objects to activly handle for the given node
 * @return		True on success, false if given NId/PId are not valid
 */
bool ProcessingEngineConfig::SetRemoteObjectsToActivate(NodeId NId, ProtocolId PId, const Array<RemoteObject>& Objs)
{
	if (m_nodeData.contains(NId) && m_protocolData.contains(PId))
	{
		ProtocolData protocol = m_protocolData[PId];

		protocol.RemoteObjects = Objs;
		m_protocolData.set(PId, protocol);

		return true;
	}

	return false;
}

/**
 * Getter for the list of remote objects to activly handle for a given node
 *
 * @param NId	The node id to use to get objectdata for
 * @param PId	The protocol id to use to get objectdata for
 * @return		The list of objects to activly handle for the given node
 */
bool ProcessingEngineConfig::GetUseActiveHandling(NodeId NId, ProtocolId PId) const
{
	return GetProtocolData(NId, PId).UsesActiveRemoteObjects;
}

/**
 * Setter for the list of remote objects to activly handle for a given node
 *
 * @param NId		The node id to use to get objectdata for
 * @param PId		The protocol id to use to get objectdata for
 * @param enable	The enable flag for using active handling for the given protocol
 * @return			True on success, false if given NId/PId are not valid
 */
bool ProcessingEngineConfig::SetUseActiveHandling(NodeId NId, ProtocolId PId, bool enable)
{
	if (m_nodeData.contains(NId) && m_protocolData.contains(PId))
	{
		ProtocolData protocol = m_protocolData[PId];

		protocol.UsesActiveRemoteObjects = enable;
		m_protocolData.set(PId, protocol);

		return true;
	}

	return false;
}

/**
 * Getter for the polling interval in ms for a given nodes protocol
 *
 * @param NId	The node id to use to get objectdata for
 * @param PId	The protocol id to use to get objectdata for
 * @return		The polling interval in ms
 */
int ProcessingEngineConfig::GetPollingInterval(NodeId NId, ProtocolId PId) const
{
	return GetProtocolData(NId, PId).PollingInterval;
}

/**
 * Setter for the polling interval in ms for a given nodes protocol
 *
 * @param NId		The node id to use to get objectdata for
 * @param PId		The protocol id to use to get objectdata for
 * @param interval	The interval to set for the given protocol
 * @return			True on success, false if given NId/PId are not valid
 */
bool ProcessingEngineConfig::SetPollingInterval(NodeId NId, ProtocolId PId, int interval)
{
	if (m_nodeData.contains(NId) && m_protocolData.contains(PId))
	{
		ProtocolData protocol = m_protocolData[PId];

		protocol.PollingInterval = interval;
		m_protocolData.set(PId, protocol);

		return true;
	}

	return false;
}

/**
 * Setter for the protocol ports for a given node/protocol
 *
 * @param NId		The node id to use to set protocol ports for
 * @param PId		The protocol id to use to set protocol ports for
 * @param ports		The pair of client/host port for the protocol/node
 * @return			True on success, false if given NId/PId are not valid
 */
bool ProcessingEngineConfig::SetProtocolPorts(NodeId NId, ProtocolId PId, const std::pair<int, int>& ports)
{
	if (m_nodeData.contains(NId) && m_protocolData.contains(PId))
	{
		ProtocolData protocol = m_protocolData[PId];

		protocol.ClientPort = ports.first;
		protocol.HostPort = ports.second;
		m_protocolData.set(PId, protocol);

		return true;
	}

	return false;
}

/**
 * Getter for the bool memeber defining if trafficlogging button should be available to user
 *
 * @return	True if the togglebutton should be available to the user on ui
 */
bool ProcessingEngineConfig::IsTrafficLoggingAllowed() const
{
	return m_TrafficLoggingAllowed;
}

/**
 * Setter for the bool memeber defining if trafficlogging button should be available to user
 *
 * @param allowed	True if button should be available to the user
 */
void ProcessingEngineConfig::SetTrafficLoggingAllowed(bool allowed)
{
	m_TrafficLoggingAllowed = allowed;
}

/**
 * Getter for the bool memeber defining if the engine should be automatically started on app startup
 *
 * @return	True if the engine should be automatically started
 */
bool ProcessingEngineConfig::IsEngineStartOnAppStart() const
{
	return m_EngineStartOnAppStart;
}

/**
 * Setter for the bool memeber defining if the engine should be automatically started on app startup
 *
 * @param start	True if the engine should be automatically started
 */
void ProcessingEngineConfig::SetEngineStartOnAppStart(bool start)
{
	m_EngineStartOnAppStart = start;
}

/**
 * Getter for the typeA protocol ids used in a given node
 *
 * @param NId	The node to get the used typeA protocol ids for
 * @return		The list of protocol ids for the given node
 */
Array<ProtocolId> ProcessingEngineConfig::GetProtocolAIds(NodeId NId) const
{
	return m_nodeData[NId].RoleAProtocols;
}

/**
 * Getter for the typeB protocol ids used in a given node
 *
 * @param NId	The node to get the used typeB protocol ids for
 * @return		The list of protocol ids for the given node
 */
Array<ProtocolId> ProcessingEngineConfig::GetProtocolBIds(NodeId NId) const
{
	return m_nodeData[NId].RoleBProtocols;
}

/**
 * Getter for the configuration data of a protocol of a given node
 * 
 * @param NId	The node id to get protocol config data for
 * @param PId	The protocol id to get protocol config data for
 * @return		The protocol config data as requested
 */
ProcessingEngineConfig::ProtocolData ProcessingEngineConfig::GetProtocolData(NodeId NId, ProtocolId PId) const
{
	if (m_nodeData.contains(NId))
	{
		if (m_protocolData.contains(PId))
		{
			return m_protocolData[PId];
		}
	}
	
	return ProtocolData();
}

/**
 * Getter for the configuration data of a protocol of a given node
 *
 * @param NId	The node id to get protocol config data for
 * @param PId	The protocol id to get protocol config data for
 * @param data	The protocol data to set to config
 * @return		True on success
 */
bool ProcessingEngineConfig::SetProtocolData(NodeId NId, ProtocolId PId, const ProcessingEngineConfig::ProtocolData& data)
{
	if (!m_nodeIds.contains(NId) || (!m_nodeData[NId].RoleAProtocols.contains(PId) && !m_nodeData[NId].RoleBProtocols.contains(PId)))
		return false;

	m_protocolData.set(PId, data);

	return true;
}

/**
 * Initializes the configuration object
 *
 * @return	True on success, false if failed
 */
bool ProcessingEngineConfig::InitConfiguration()
{
	if (XmlDocument::parse(File(m_configFilePath + CONFIGURATION_FILE)))
	{
		return ReadConfiguration();
	}
	else
	{
		// Add a default node               
		AddDefaultNode();

		return WriteConfiguration();
	}
}

/**
* Reads the configuration data from xml file into object
*
* @return	True on success, false if failed
*/
bool ProcessingEngineConfig::ReadConfiguration()
{
    if (std::unique_ptr<XmlElement> elm = std::unique_ptr<XmlElement>(XmlDocument::parse(File(m_configFilePath + CONFIGURATION_FILE))))
	{
		XmlElement* rootChild = elm->getFirstChildElement();
		while (rootChild !=nullptr)
		{
			if (rootChild->getTagName() == "Node")
			{
				NodeData node;
				node.Id = NodeId(ValidateUniqueId(rootChild->getAttributeValue(0).getIntValue()));

				XmlElement* nodeChild = rootChild->getFirstChildElement();
				while (nodeChild != nullptr)
				{
					if (nodeChild->getTagName() == "ObjectHandling")
					{
						node.ObjectHandling.Mode = ObjectHandlingModeFromString(nodeChild->getAttributeValue(0));
						node.ObjectHandling.Prec = nodeChild->getAttributeValue(1).getDoubleValue();

						XmlElement* nodeDataChild = nodeChild->getFirstChildElement();
						while (nodeDataChild != nullptr)
						{
							if (nodeDataChild->getTagName() == "ProtocolAChCnt")
								node.ObjectHandling.ACnt = nodeDataChild->getAttributeValue(0).getIntValue();
							if (nodeDataChild->getTagName() == "ProtocolBChCnt")
								node.ObjectHandling.BCnt = nodeDataChild->getAttributeValue(0).getIntValue();

							nodeDataChild = nodeDataChild->getNextElement();
						}
					}
					else if (nodeChild->getTagName() == "ProtocolA")
					{
						ProtocolData protocol;
						protocol.Id = ProtocolId(ValidateUniqueId(nodeChild->getAttributeValue(0).getIntValue()));
						protocol.Type = ProtocolTypeFromString(nodeChild->getAttributeValue(1));
						protocol.PollingInterval = ET_DefaultPollingRate;
						protocol.UsesActiveRemoteObjects = nodeChild->getAttributeValue(2).getIntValue()>0;

						XmlElement* nodeDataChild = nodeChild->getFirstChildElement();
						while (nodeDataChild != nullptr)
						{
							if (nodeDataChild->getTagName() == "IpAddress")
								protocol.IpAddress = nodeDataChild->getAttributeValue(0);
							else if (nodeDataChild->getTagName() == "ClientPort")
								protocol.ClientPort = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "HostPort")
								protocol.HostPort = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "PollingInterval")
								protocol.PollingInterval = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "ActiveObjects")
								ReadActiveObjects(nodeDataChild->getFirstChildElement(), protocol.RemoteObjects);

							nodeDataChild = nodeDataChild->getNextElement();
						}

						if (m_protocolData.contains(protocol.Id) || node.RoleAProtocols.contains(protocol.Id))
						{
#ifdef DEBUG
							DBG("Double Protocol typeA Id found, cannot add this to configuration");
#endif
						}
						else
						{
							m_protocolData.set(protocol.Id, protocol);
							node.RoleAProtocols.add(protocol.Id);
						}
					}
					else if (nodeChild->getTagName() == "ProtocolB")
					{
						ProtocolData protocol;
						protocol.Id = ProtocolId(ValidateUniqueId(nodeChild->getAttributeValue(0).getIntValue()));
						protocol.Type = ProtocolTypeFromString(nodeChild->getAttributeValue(1));
						protocol.PollingInterval = ET_DefaultPollingRate;
						protocol.UsesActiveRemoteObjects = nodeChild->getAttributeValue(2).getIntValue()>0;

						XmlElement* nodeDataChild = nodeChild->getFirstChildElement();
						while (nodeDataChild != nullptr)
						{
							if (nodeDataChild->getTagName() == "IpAddress")
								protocol.IpAddress = nodeDataChild->getAttributeValue(0);
							else if (nodeDataChild->getTagName() == "ClientPort")
								protocol.ClientPort = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "HostPort")
								protocol.HostPort = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "PollingInterval")
								protocol.PollingInterval = nodeDataChild->getAttributeValue(0).getIntValue();
							else if (nodeDataChild->getTagName() == "ActiveObjects")
								ReadActiveObjects(nodeDataChild->getFirstChildElement(), protocol.RemoteObjects);

							nodeDataChild = nodeDataChild->getNextElement();
						}

						if (m_protocolData.contains(protocol.Id) || node.RoleBProtocols.contains(protocol.Id))
						{
#ifdef DEBUG
							DBG("Double Protocol typeB Id found, cannot add this to configuration");
#endif
						}
						else
						{
							m_protocolData.set(protocol.Id, protocol);
							node.RoleBProtocols.add(protocol.Id);
						}
					}

					nodeChild = nodeChild->getNextElement();
				}

				SetNode(node.Id, node);
			}
			else if (rootChild->getTagName() == "GlobalConfig")
			{
				XmlElement* globalConfigChild = rootChild->getFirstChildElement();
				while (globalConfigChild != nullptr)
				{
					if (globalConfigChild->getTagName() == "TrafficLogging")
					{
						m_TrafficLoggingAllowed = globalConfigChild->getAttributeValue(0).getIntValue() > 0;
					}
					else if (globalConfigChild->getTagName() == "Engine")
					{
						m_EngineStartOnAppStart = globalConfigChild->getAttributeValue(0).getIntValue() > 0;
					}

					globalConfigChild = globalConfigChild->getNextElement();
				}
			}

			rootChild = rootChild->getNextElement();
		}

		return true;
	}

	return false;
}

/**
 * Method to read the node configuration part regarding active objects per protocol
 *
 * @param ActiveObjectsElement	The xml element for the nodes' protocols' active objects in the DOM
 * @param RemoteObjects			The remote objects list to fill according config contents
 * @return	True if remote objects were inserted, false if empty list is returned
 */
bool ProcessingEngineConfig::ReadActiveObjects(XmlElement* ActiveObjectsElement, Array<RemoteObject>& RemoteObjects)
{
	RemoteObjects.clear();

	if (!ActiveObjectsElement)
		return false;

	XmlElement* objectChild = ActiveObjectsElement;
	RemoteObject obj;
	
	while (objectChild != nullptr)
	{
		Array<int> channels;
		String chStrToSplit = objectChild->getAttributeValue(0);
		StringArray chNumbers;
		chNumbers.addTokens(chStrToSplit, ", ", "");
		for (int j = 0; j < chNumbers.size(); ++j)
		{
			int chNum = chNumbers[j].getIntValue();
			if (chNum > 0)
				channels.add(chNum);
		}
	
		Array<int> records;
		String recStrToSplit = objectChild->getAttributeValue(1);
		StringArray recNumbers;
		recNumbers.addTokens(recStrToSplit, ", ", "");
		for (int j = 0; j < recNumbers.size(); ++j)
		{
			int recNum = recNumbers[j].getIntValue();
			if (recNum > 0)
				records.add(recNum);
		}
	
		for (int i = ROI_Invalid + 1; i < ROI_UserMAX; ++i)
		{
			RemoteObjectIdentifier ROId = (RemoteObjectIdentifier)i;
			if (objectChild->getTagName() == GetObjectDescription(ROId).removeCharacters(" "))
			{
				obj.Id = ROId;
	
				// now that we have all channels and records, recursively iterate through both arrays
				// to add an entry to our active objects list for every resulting ch/rec combi object
				for (int j = 0; j < channels.size(); ++j)
				{
					if (records.size() > 0)
					{
						for (int k = 0; k < records.size(); ++k)
						{
							obj.Addr.first = (int16)channels[j];
							obj.Addr.second = (int16)records[k];
							RemoteObjects.add(obj);
						}
					}
					else
					{
						obj.Addr.first = (int16)channels[j];
						obj.Addr.second = -1;
						RemoteObjects.add(obj);
					}
				}
			}
		}
	
		objectChild = objectChild->getNextElement();
	}

	return !RemoteObjects.isEmpty();
}

/**
 * Method to read the node configuration part regarding polling interval per protocol.
 * Includes fixup to default if not found in xml.
 *
 * @param PollingIntervalElement	The xml element for the nodes' protocols' polling interval in the DOM
 * @param PollingInterval			The polling interval var to fill according config contents
 * @return	True if value was read from xml, false if default was used.
 */
bool ProcessingEngineConfig::ReadPollingInterval(XmlElement* PollingIntervalElement, int& PollingInterval)
{
	XmlElement* objectChild = PollingIntervalElement;
	PollingInterval = ET_DefaultPollingRate;

	if (objectChild != nullptr && objectChild->getAttributeName(0) == "Interval")
	{
		PollingInterval = objectChild->getAttributeValue(0).getIntValue();
		return true;
	}
	else
		return false;
}

/**
 * Writes the configuration data from object into xml file
 *
 * @return	True on success, false if failed
 */
bool ProcessingEngineConfig::WriteConfiguration()
{
	std::unique_ptr<XmlElement> XmlConfig = std::make_unique<XmlElement>("RemoteProtocolBridgeConfig");

	for (int i = 0; i < m_nodeIds.size(); ++i)
	{
		if (XmlElement* NodeElement = XmlConfig->createNewChildElement("Node"))
		{
			NodeElement->setAttribute("Id", (int)m_nodeIds[i]);

			if (m_nodeData.contains(m_nodeIds[i]))
			{
				if (m_nodeData[m_nodeIds[i]].ObjectHandling.Mode != OHM_Invalid)
				{
					if (XmlElement* ObjectHandlingElement = NodeElement->createNewChildElement("ObjectHandling"))
					{
						ObjectHandlingElement->setAttribute("Mode", ObjectHandlingModeToString(m_nodeData[m_nodeIds[i]].ObjectHandling.Mode));
						ObjectHandlingElement->setAttribute("DataPrecision", m_nodeData[m_nodeIds[i]].ObjectHandling.Prec);
						if (XmlElement* ProtocolAChCntElement = ObjectHandlingElement->createNewChildElement("ProtocolAChCnt"))
							ProtocolAChCntElement->setAttribute("Count", m_nodeData[m_nodeIds[i]].ObjectHandling.ACnt);
						if (XmlElement* ProtocolBChCntElement = ObjectHandlingElement->createNewChildElement("ProtocolBChCnt"))
							ProtocolBChCntElement->setAttribute("Count", m_nodeData[m_nodeIds[i]].ObjectHandling.BCnt);
					}
				}

				for (int j = 0; j < m_nodeData[m_nodeIds[i]].RoleAProtocols.size(); ++j)
				{
					if (XmlElement* ProtocolAElement = NodeElement->createNewChildElement("ProtocolA"))
					{
						ProtocolId PAId = m_nodeData[m_nodeIds[i]].RoleAProtocols[j];

						ProtocolAElement->setAttribute("Id", (int)m_protocolData[PAId].Id);
						ProtocolAElement->setAttribute("Type", ProtocolTypeToString(m_protocolData[PAId].Type));
						ProtocolAElement->setAttribute("UsesActiveRemoteObjects", (int)m_protocolData[PAId].UsesActiveRemoteObjects);

						if (XmlElement* IpAddrElement = ProtocolAElement->createNewChildElement("IpAddress"))
							IpAddrElement->setAttribute("Address", m_protocolData[PAId].IpAddress);
						if (XmlElement* ClientPortElement = ProtocolAElement->createNewChildElement("ClientPort"))
							ClientPortElement->setAttribute("Port", m_protocolData[PAId].ClientPort);
						if (XmlElement* HostPortElement = ProtocolAElement->createNewChildElement("HostPort"))
							HostPortElement->setAttribute("Port", m_protocolData[PAId].HostPort);
						if (XmlElement* PollingIntervalElement = ProtocolAElement->createNewChildElement("PollingInterval"))
							PollingIntervalElement->setAttribute("Interval", m_protocolData[PAId].PollingInterval);
						if (XmlElement* ActiveObjectsElement = ProtocolAElement->createNewChildElement("ActiveObjects"))
							WriteActiveObjects(ActiveObjectsElement, m_protocolData[PAId].RemoteObjects);
					}
				}

				for (int j = 0; j < m_nodeData[m_nodeIds[i]].RoleBProtocols.size(); ++j)
				{
					if (XmlElement* ProtocolBElement = NodeElement->createNewChildElement("ProtocolB"))
					{
						ProtocolId PBId = m_nodeData[m_nodeIds[i]].RoleBProtocols[j];

						ProtocolBElement->setAttribute("Id", (int)m_protocolData[PBId].Id);
						ProtocolBElement->setAttribute("Type", ProtocolTypeToString(m_protocolData[PBId].Type));
						ProtocolBElement->setAttribute("UsesActiveRemoteObjects", (int)m_protocolData[PBId].UsesActiveRemoteObjects);

						if (XmlElement* IpAddrElement = ProtocolBElement->createNewChildElement("IpAddress"))
							IpAddrElement->setAttribute("Address", m_protocolData[PBId].IpAddress);
						if (XmlElement* ClientPortElement = ProtocolBElement->createNewChildElement("ClientPort"))
							ClientPortElement->setAttribute("Port", m_protocolData[PBId].ClientPort);
						if (XmlElement* HostPortElement = ProtocolBElement->createNewChildElement("HostPort"))
							HostPortElement->setAttribute("Port", m_protocolData[PBId].HostPort);
						if (XmlElement* PollingIntervalElement = ProtocolBElement->createNewChildElement("PollingInterval"))
							PollingIntervalElement->setAttribute("Interval", m_protocolData[PBId].PollingInterval);
						if (XmlElement* ActiveObjectsElement = ProtocolBElement->createNewChildElement("ActiveObjects"))
							WriteActiveObjects(ActiveObjectsElement, m_protocolData[PBId].RemoteObjects);
					}
				}
			}
		}
	}

	if (XmlElement* GlobalConfigElement = XmlConfig->createNewChildElement("GlobalConfig"))
	{
		if (XmlElement* TrafficLoggingElement = GlobalConfigElement->createNewChildElement("TrafficLogging"))
		{
			TrafficLoggingElement->setAttribute("Allowed", m_TrafficLoggingAllowed);
		}
		if (XmlElement* EngineElement = GlobalConfigElement->createNewChildElement("Engine"))
		{
			EngineElement->setAttribute("AutoStart", m_EngineStartOnAppStart);
		}
	}

	bool success = XmlConfig->writeTo(File(m_configFilePath + CONFIGURATION_FILE));

	return success;
}

/**
 * Method to write the node configuration part regarding active objects per protocol
 *
 * @param ActiveObjectsElement	The xml element for the nodes' protocols' active objects in the DOM
 * @param RemoteObjects			The remote objects to set active in config
 * @return	True on success, false on failure
 */
bool ProcessingEngineConfig::WriteActiveObjects(XmlElement* ActiveObjectsElement, Array<RemoteObject> const& RemoteObjects)
{
	if (!ActiveObjectsElement)
		return false;

	int RemoteObjectCount = RemoteObjects.size();

	HashMap<int, Array<int>> channelsPerObj;
	HashMap<int, Array<int>> recordsPerObj;
	for (int j = 0; j < RemoteObjectCount; ++j)
	{
		Array<int> selChs = channelsPerObj[RemoteObjects[j].Id];
		if (!selChs.contains(RemoteObjects[j].Addr.first))
		{
			selChs.add(RemoteObjects[j].Addr.first);
			channelsPerObj.set(RemoteObjects[j].Id, selChs);
		}

		Array<int> selRecs = recordsPerObj[RemoteObjects[j].Id];
		if (!selRecs.contains(RemoteObjects[j].Addr.second))
		{
			selRecs.add(RemoteObjects[j].Addr.second);
			recordsPerObj.set(RemoteObjects[j].Id, selRecs);
		}
	}

	for (int k = ROI_Invalid + 1; k < ROI_UserMAX; ++k)
	{
		if (XmlElement* ObjectElement = ActiveObjectsElement->createNewChildElement(GetObjectDescription((RemoteObjectIdentifier)k).removeCharacters(" ")))
		{
			String selChanTxt;
			for (int j = 0; j < channelsPerObj[k].size(); ++j)
			{
				if (channelsPerObj[k][j] > 0)
				{
					if (!selChanTxt.isEmpty())
						selChanTxt << ", ";
					selChanTxt << channelsPerObj[k][j];
				}
			}
			ObjectElement->setAttribute("channels", selChanTxt);

			String selRecTxt;
			for (int j = 0; j < recordsPerObj[k].size(); ++j)
			{
				if (recordsPerObj[k][j] > 0)
				{
					if (!selRecTxt.isEmpty())
						selRecTxt << ", ";
					selRecTxt << recordsPerObj[k][j];
				}
			}
			ObjectElement->setAttribute("records", selRecTxt);
		}
	}

	return true;
}

/**
 * Method to generate next available unique id.
 * There is no cleanup / recycling of old ids available yet,
 * we only perform a ++ on a static counter
 */
int ProcessingEngineConfig::GetNextUniqueId()
{
    return ++uniqueIdCounter;
}

/**
 * Method to validate new external unique id to not be in conflict with internal id counter.
 * Internal counter is simply increased to not be in conflict with given new id.
 */
int ProcessingEngineConfig::ValidateUniqueId(int uniqueId)
{
	if (uniqueIdCounter < uniqueId)
		uniqueIdCounter = uniqueId;

	return uniqueId;
}

/**
 * Setter for the node configuration data for a given node.
 * Takes care of the internal unique id counter not being within
 * already used range of ids.
 *
 * @param NId	The node id of the node to set configuration data for
 * @param node	Ten node config data
 */
void ProcessingEngineConfig::SetNode(NodeId NId, NodeData& node)
{
	NodeId validNId = ValidateUniqueId(NId);
	if (validNId != node.Id)
		jassertfalse;

	if (m_nodeIds.contains(NId))
	{
		m_nodeData.set(NId, node);
	}
	else
	{
		m_nodeIds.add(NId);
		m_nodeData.set(NId, node);
	}
}

/**
 * Adds a bridging node with default values to configuration object
 */
void ProcessingEngineConfig::AddDefaultNode()
{
	// Active objects preparation
	Array<RemoteObject> remoteObjects;
	RemoteObject objectX, objectY;

	objectX.Id = ROI_SoundObject_Position_X;
	objectY.Id = ROI_SoundObject_Position_Y;
	for (int16 i = 1; i <= 16; ++i)
	{
		RemoteObjectAddressing addr;
		addr.first = i; //channel = source
		addr.second = 1; //record = mapping

		objectX.Addr = addr;
		objectY.Addr = addr;

		remoteObjects.add(objectX);
		remoteObjects.add(objectY);
	}

	// Node Configuration presetting
	NodeData node;

	node.Id = GetNextUniqueId();
	node.ObjectHandling.Mode = OHM_Bypass;
	node.ObjectHandling.ACnt = 0;
	node.ObjectHandling.BCnt = 0;
	node.ObjectHandling.Prec = 0.001;

	ProtocolData ProtocolA;
	ProtocolA.Id = GetNextUniqueId();
	ProtocolA.Type = PT_OSCProtocol;
	ProtocolA.ClientPort = 50010;
	ProtocolA.HostPort = 50011;
	ProtocolA.IpAddress = "10.255.0.100";
	ProtocolA.UsesActiveRemoteObjects = false;
	ProtocolA.PollingInterval = ET_DefaultPollingRate;
	ProtocolA.RemoteObjects = remoteObjects;

	m_protocolData.set(ProtocolA.Id, ProtocolA);
	node.RoleAProtocols.add(ProtocolA.Id);

	ProtocolData ProtocolB;
	ProtocolB.Id = GetNextUniqueId();
	ProtocolB.Type = PT_OSCProtocol;
	ProtocolB.ClientPort = 50012;
	ProtocolB.HostPort = 50013;
	ProtocolB.IpAddress = "127.0.0.1";
	ProtocolB.UsesActiveRemoteObjects = false;
	ProtocolB.PollingInterval = ET_DefaultPollingRate;
	ProtocolB.RemoteObjects = remoteObjects;

	m_protocolData.set(ProtocolB.Id, ProtocolB);
	node.RoleBProtocols.add(ProtocolB.Id);

	m_nodeIds.add(node.Id);
	m_nodeData.set(node.Id, node);
}

/**
 * Adds a bridging node protocol of role B with default values to configuration object
 *
 * @param NId	The node to add the role B protocol to
 */
void ProcessingEngineConfig::AddDefaultProtocolB(NodeId NId)
{
	// Active objects preparation
	Array<RemoteObject> remoteObjects;
	RemoteObject objectX, objectY;
	
	objectX.Id = ROI_SoundObject_Position_X;
	objectY.Id = ROI_SoundObject_Position_Y;
	for (int16 i = 1; i <= 16; ++i)
	{
		RemoteObjectAddressing addr;
		addr.first = i; //channel = source
		addr.second = 1; //record = mapping
	
		objectX.Addr = addr;
		objectY.Addr = addr;
	
		remoteObjects.add(objectX);
		remoteObjects.add(objectY);
	}
	
	// Node Configuration presetting
	NodeData node = GetNodeData(NId);
	
	ProtocolData ProtocolB;
	ProtocolB.Id = GetNextUniqueId();
	ProtocolB.Type = PT_OSCProtocol;
	ProtocolB.ClientPort = 50012;
	ProtocolB.HostPort = 50013;
	ProtocolB.IpAddress = "127.0.0.1";
	ProtocolB.UsesActiveRemoteObjects = false;
	ProtocolB.PollingInterval = ET_DefaultPollingRate;
	ProtocolB.RemoteObjects = remoteObjects;
	
	m_protocolData.set(ProtocolB.Id, ProtocolB);
	node.RoleBProtocols.add(ProtocolB.Id);
	
	m_nodeData.set(node.Id, node);
}

/**
 * Adds a bridging node protocol of role A with default values to configuration object
 *
 * @param NId	The node to add the role A protocol to
 */
void ProcessingEngineConfig::AddDefaultProtocolA(NodeId NId)
{
	// Active objects preparation
	Array<RemoteObject> remoteObjects;
	RemoteObject objectX, objectY;
	
	objectX.Id = ROI_SoundObject_Position_X;
	objectY.Id = ROI_SoundObject_Position_Y;
	for (int16 i = 1; i <= 16; ++i)
	{
		RemoteObjectAddressing addr;
		addr.first = i; //channel = source
		addr.second = 1; //record = mapping
	
		objectX.Addr = addr;
		objectY.Addr = addr;
	
		remoteObjects.add(objectX);
		remoteObjects.add(objectY);
	}
	
	// Node Configuration presetting
	NodeData node = GetNodeData(NId);
	
	ProtocolData ProtocolA;
	ProtocolA.Id = GetNextUniqueId();
	ProtocolA.Type = PT_OSCProtocol;
	ProtocolA.ClientPort = 50010;
	ProtocolA.HostPort = 50011;
	ProtocolA.IpAddress = "10.255.0.100";
	ProtocolA.UsesActiveRemoteObjects = false;
	ProtocolA.PollingInterval = ET_DefaultPollingRate;
	ProtocolA.RemoteObjects = remoteObjects;
	
	m_protocolData.set(ProtocolA.Id, ProtocolA);
	node.RoleAProtocols.add(ProtocolA.Id);
	
	m_nodeData.set(node.Id, node);
}

/**
 * Removes the bridging node with given NodeId from configuration object
 *
 * @param NId	The id of the node to remove
 */
void ProcessingEngineConfig::RemoveNode(NodeId NId)
{
	for (int i = 0; i < m_nodeIds.size(); ++i)
	{
		if (m_nodeIds[i] == NId)
		{
			m_nodeIds.remove(i);
			break;
		}
	}
	m_nodeData.remove(NId);
}

/**
 * Removes the bridging node protocol with given NodeId and ProtocolId from configuration object
 *
 * @param NId	The id of the protocols' parent node
 * @param PId	The id of the protocol to remove
 */
void ProcessingEngineConfig::RemoveProtocol(NodeId NId, ProtocolId PId)
{
	NodeData node = GetNodeData(NId);
	if (node.RoleAProtocols.contains(PId) && node.RoleAProtocols.size() <= 1)
		return;
	if (node.RoleBProtocols.contains(PId) && node.RoleBProtocols.size() <= 1)
		return;

	if (node.RoleAProtocols.contains(PId))
		node.RoleAProtocols.removeAllInstancesOf(PId);
	else if (node.RoleBProtocols.contains(PId))
		node.RoleBProtocols.removeAllInstancesOf(PId);
	else
		jassertfalse;

	m_nodeData.set(node.Id, node);

	if (m_protocolData.contains(PId))
		m_protocolData.remove(PId);
}

/**
 * Clears the configuration object
 */
void ProcessingEngineConfig::Clear()
{
	m_nodeIds.clear();
	m_nodeData.clear();
}

/**
* Helper to resolve ROI to human readable string.
*
* @param Id	The remote object id to be resolved to a string.
*/
String ProcessingEngineConfig::GetObjectDescription(RemoteObjectIdentifier Id)
{
	switch (Id)
	{
	case ROI_HeartbeatPing:
		return "PING";
	case ROI_HeartbeatPong:
		return "PONG";
	case ROI_SoundObject_Position_X:
		return "Sound Object Position X";
	case ROI_SoundObject_Position_Y:
		return "Sound Object Position Y";
	case ROI_SoundObject_Position_XY:
		return "Sound Object Position XY";
	case ROI_SoundObject_Spread:
		return "Sound Object Spread";
	case ROI_SoundObject_DelayMode:
		return "Sound Object Delay Mode";
	case ROI_ReverbSendGain:
		return "En-Space Send Gain";
	default:
		return "-";
	}
}

/**
 * Helper method to check if a given remote object is involved in keepalive transmission and must not be filtered out e.g. based on value change detection.
 * @param objectId    The remote object id to check.
 * @return True if the object is involved in keepalive transmission, false if not.
 */
bool ProcessingEngineConfig::IsKeepaliveObject(RemoteObjectIdentifier objectId)
{
	switch(objectId)
	{
		case ROI_HeartbeatPing:
		case ROI_HeartbeatPong:
			return true;
		default:
			return false;
	}
}

/**
* Convenience function to resolve enum to sth. human readable (e.g. in config file)
*/
String  ProcessingEngineConfig::ProtocolTypeToString(ProtocolType pt)
{
	switch (pt)
	{
	case PT_OCAProtocol:
		return "OCA";
	case PT_OSCProtocol:
		return "OSC";
	case PT_Invalid:
		return "Invalid";
	default:
		return "";
	}
}

/**
* Convenience function to resolve string to enum
*/
ProtocolType  ProcessingEngineConfig::ProtocolTypeFromString(String type)
{
	if (type == "OCA")
		return PT_OCAProtocol;
	if (type == "OSC")
		return PT_OSCProtocol;

	return PT_Invalid;
}

/**
* Convenience function to resolve enum to sth. human readable (e.g. in config file)
*/
String ProcessingEngineConfig::ObjectHandlingModeToString(ObjectHandlingMode ohm)
{
	switch (ohm)
	{
	case OHM_Bypass:
		return "Bypass (A<->B)";
	case OHM_Remap_A_X_Y_to_B_XY:
		return "Reroute single A (x), (y) to combi B (xy)";
	case OHM_Mux_nA_to_mB:
		return "Multiplex multiple n-ch. A to m-ch. B protocols";
	case OHM_Forward_only_valueChanges:
		return "Forward value changes only";
	case OHM_Forward_A_to_B_only:
		return "Forward data only (A->B)";
	case OHM_Reverse_B_to_A_only:
		return "Reverse data only (B->A)";
	case OHM_Mux_nA_to_mB_withValFilter:
		return "Multiplex mult. n-ch. A to m-ch. B (fwd. val. changes only)";
	case OHM_A1active_withValFilter:
		return "A1 forwarding only (val. changes only)";
	case OHM_A2active_withValFilter:
		return "A2 forwarding only (val. changes only)";
	default:
		return "";
	}
}

/**
* Convenience function to resolve string to enum
*/
ObjectHandlingMode ProcessingEngineConfig::ObjectHandlingModeFromString(String mode)
{
	if (mode == ObjectHandlingModeToString(OHM_Bypass))
		return OHM_Bypass;
	if (mode == ObjectHandlingModeToString(OHM_Remap_A_X_Y_to_B_XY))
		return OHM_Remap_A_X_Y_to_B_XY;
	if (mode == ObjectHandlingModeToString(OHM_Mux_nA_to_mB))
		return OHM_Mux_nA_to_mB;
	if (mode == ObjectHandlingModeToString(OHM_Forward_only_valueChanges))
		return OHM_Forward_only_valueChanges;
	if (mode == ObjectHandlingModeToString(OHM_Forward_A_to_B_only))
		return OHM_Forward_A_to_B_only;
	if (mode == ObjectHandlingModeToString(OHM_Reverse_B_to_A_only))
		return OHM_Reverse_B_to_A_only;
	if (mode == ObjectHandlingModeToString(OHM_Mux_nA_to_mB_withValFilter))
		return OHM_Mux_nA_to_mB_withValFilter;
	if (mode == ObjectHandlingModeToString(OHM_A1active_withValFilter))
		return OHM_A1active_withValFilter;
	if (mode == ObjectHandlingModeToString(OHM_A2active_withValFilter))
		return OHM_A2active_withValFilter;

	return OHM_Invalid;
}
