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

#include "ObjectDataHandling.h"

#include "ProcessingEngineNode.h"
#include "ProcessingEngineConfig.h"


// **************************************************************************************
//    class ObjectDataHandling_Abstract
// **************************************************************************************

/**
 * @fn void ObjectDataHandling_Abstract::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
 * @param PId	The protocol id that a message was received on
 * @param Id	The id of the remote object that was received
 * @param msgData	The message that was received
 * Pure virtual function to be implemented by data handling objects to handle received protocol data.
 */

/**
 * Constructor of abstract class ObjectDataHandling_Abstract.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
ObjectDataHandling_Abstract::ObjectDataHandling_Abstract(ProcessingEngineNode* parentNode)
{
	m_parentNode = parentNode;
	m_mode = ObjectHandlingMode::OHM_Invalid;
}

/**
 * Destructor
 */
ObjectDataHandling_Abstract::~ObjectDataHandling_Abstract()
{
}

/**
 * Sets the configuration for the protocol processor object.
 * 
 * @param config	The overall configuration object that can be used to query protocol specific config data from
 * @param NId		The node id of the parent node this protocol processing object is child of (needed to access data from config)
 */
void ObjectDataHandling_Abstract::SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId)
{
	if (m_parentNode && m_parentNode->GetId() != NId)
		jassertfalse;

	ignoreUnused(config);

}

/**
 * Method to add a new id of a protocol of typeA to internal list of typeA protocolIds
 *
 * @param PAId	Protocol Id of typeA protocol to add.
 */
void ObjectDataHandling_Abstract::AddProtocolAId(ProtocolId PAId)
{
	m_protocolAIds.add(PAId);
}

/**
 * Method to add a new id of a protocol of typeB to internal list of typeB protocolIds
 *
 * @param PBId	Protocol Id of typeB protocol to add.
 */
void ObjectDataHandling_Abstract::AddProtocolBId(ProtocolId PBId)
{
	m_protocolBIds.add(PBId);
}

/**
 * Method to clear internal lists of both typeA and typeB protocolIds
 */
void ObjectDataHandling_Abstract::ClearProtocolIds()
{
	m_protocolAIds.clear();
}

/**
 * Getter for the parentNode member.
 * @return The parentNode pointer, can be nullptr.
 */
const ProcessingEngineNode* ObjectDataHandling_Abstract::GetParentNode()
{
	return m_parentNode;
}

/**
 * Getter for the mode member.
 * @return The object handling mode of this object.
 */
ObjectHandlingMode ObjectDataHandling_Abstract::GetMode()
{
	return m_mode;
}

/**
 * Setter for the mode member.
 * @param mode The object handling mode to set for this object.
 */
void  ObjectDataHandling_Abstract::SetMode(ObjectHandlingMode mode)
{
	m_mode = mode;
}

/**
 * Getter for the parent node id member.
 * @return The parentNode Id.
 */
NodeId ObjectDataHandling_Abstract::GetParentNodeId()
{
	return m_parentNodeId;
}

/**
 * Getter for the type a protocols array member.
 * @return The type a protocols.
 */
const Array<ProtocolId>& ObjectDataHandling_Abstract::GetProtocolAIds()
{
	return m_protocolAIds;
}

/**
 * Getter for the type b protocols array member.
 * @return The type b protocols.
 */
const Array<ProtocolId>& ObjectDataHandling_Abstract::GetProtocolBIds()
{
	return m_protocolBIds;
}


// **************************************************************************************
//    class BypassHandling
// **************************************************************************************
/**
 * Constructor of class BypassHandling.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
BypassHandling::BypassHandling(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Bypass);
}

/**
 * Destructor
 */
BypassHandling::~BypassHandling()
{
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool BypassHandling::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	bool sendSuccess = false;

	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode)
	{
		if (GetProtocolAIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol -> forward it to all typeB protocols
			int typeBProtocolCount = GetProtocolBIds().size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolBIds()[i], Id, msgData);
		}
		else if (GetProtocolBIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeB protocol
			int typeAProtocolCount = GetProtocolAIds().size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], Id, msgData);
		}
	}

	return sendSuccess;
}


// **************************************************************************************
//    class Remap_A_X_Y_to_B_XY_Handling
// **************************************************************************************
/**
 * Constructor of class Remap_A_X_Y_to_B_XY_Handling.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Remap_A_X_Y_to_B_XY_Handling::Remap_A_X_Y_to_B_XY_Handling(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Remap_A_X_Y_to_B_XY);
}

/**
 * Destructor
 */
Remap_A_X_Y_to_B_XY_Handling::~Remap_A_X_Y_to_B_XY_Handling()
{
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Remap_A_X_Y_to_B_XY_Handling::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode)
	{
		if (GetProtocolAIds().contains(PId))
		{
			// the message was received by a typeA protocol

			RemoteObjectIdentifier ObjIdToSend = Id;

			if (Id == ROI_SoundObject_Position_X)
			{
				// special handling of merging separate x message to a combined xy one
				jassert(msgData.valType == ROVT_FLOAT);
				jassert(msgData.valCount == 1);
				jassert(msgData.payloadSize == sizeof(float));

				uint32 addrId = msgData.addrVal.first + (msgData.addrVal.second << 16);

				xyzVals newVals = m_currentPosValue[addrId];
				newVals.x = ((float*)msgData.payload)[0];
				m_currentPosValue.set(addrId, newVals);

				float newXYVal[2];
				newXYVal[0] = m_currentPosValue[addrId].x;
				newXYVal[1] = m_currentPosValue[addrId].y;

				msgData.valCount = 2;
				msgData.payload = &newXYVal;
				msgData.payloadSize = 2 * sizeof(float);

				ObjIdToSend = ROI_SoundObject_Position_XY;
			}
			else if (Id == ROI_SoundObject_Position_Y)
			{
				// special handling of merging separate y message to a combined xy one
				jassert(msgData.valType == ROVT_FLOAT);
				jassert(msgData.valCount == 1);
				jassert(msgData.payloadSize == sizeof(float));

				int32 addrId = msgData.addrVal.first + (msgData.addrVal.second << 16);

				xyzVals newVals = m_currentPosValue[addrId];
				newVals.y = ((float*)msgData.payload)[0];
				m_currentPosValue.set(addrId, newVals);

				float newXYVal[2];
				newXYVal[0] = m_currentPosValue[addrId].x;
				newXYVal[1] = m_currentPosValue[addrId].y;

				msgData.valCount = 2;
				msgData.payload = &newXYVal;
				msgData.payloadSize = 2 * sizeof(float);

				ObjIdToSend = ROI_SoundObject_Position_XY;
			}

			// Send to all typeB protocols
			bool sendSuccess = true;
			int typeBProtocolCount = GetProtocolBIds().size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolBIds()[i], ObjIdToSend, msgData);

			return sendSuccess;
			
		}
		if (GetProtocolBIds().contains(PId))
		{
			if (Id == ROI_SoundObject_Position_XY)
			{
				// special handling of splitting a combined xy message to  separate x, y ones
				jassert(msgData.valType == ROVT_FLOAT);
				jassert(msgData.valCount == 2);
				jassert(msgData.payloadSize == 2 * sizeof(float));

				int32 addrId = msgData.addrVal.first + (msgData.addrVal.second << 16);

				xyzVals newVals = m_currentPosValue[addrId];
				newVals.x = ((float*)msgData.payload)[0];
				newVals.y = ((float*)msgData.payload)[1];
				m_currentPosValue.set(addrId, newVals);

				float newXVal = m_currentPosValue[addrId].x;
				float newYVal = m_currentPosValue[addrId].y;

				msgData.valCount = 1;
				msgData.payloadSize = sizeof(float);

				// Send to all typeA protocols
				bool sendSuccess = true;
				int typeAProtocolCount = GetProtocolAIds().size();
				for (int i = 0; i < typeAProtocolCount; ++i)
				{
					msgData.payload = &newXVal;
					sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], ROI_SoundObject_Position_X, msgData);

					msgData.payload = &newYVal;
					sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], ROI_SoundObject_Position_Y, msgData);
				}

				return sendSuccess;
			}
			else
			{
				// Send to all typeA protocols
				bool sendSuccess = true;
				int typeAProtocolCount = GetProtocolAIds().size();
				for (int i = 0; i < typeAProtocolCount; ++i)
					sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], Id, msgData);

				return sendSuccess;
			}
		}
	}

	return false;
}


// **************************************************************************************
//    class Mux_nA_to_mB
// **************************************************************************************
/**
 * Constructor of class Mux_nA_to_mB.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Mux_nA_to_mB::Mux_nA_to_mB(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Mux_nA_to_mB);
	m_protoChCntA	= 1;
	m_protoChCntB	= 1;
}

/**
 * Destructor
 */
Mux_nA_to_mB::~Mux_nA_to_mB()
{
}

/**
 * Reimplemented to set the custom parts from configuration for the datahandling object.
 *
 * @param config	The overall configuration object that can be used to query config data from
 * @param NId		The node id of the parent node this data handling object is child of (needed to access data from config)
 */
void Mux_nA_to_mB::SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId)
{
	ObjectDataHandling_Abstract::SetObjectHandlingConfiguration(config, NId);

	m_protoChCntA = config.GetObjectHandlingData(NId).ACnt;
	m_protoChCntB = config.GetObjectHandlingData(NId).BCnt;
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Mux_nA_to_mB::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode && m_protoChCntA>0 && m_protoChCntB>0)
	{
		if (GetProtocolAIds().contains(PId))
		{
			ProtocolId protocolBId = MapObjectAddressing(PId, msgData);
			if (protocolBId != INVALID_ADDRESS_VALUE)
				return parentNode->SendMessageTo(protocolBId, Id, msgData);
			else
				return false;
		}
		else if (GetProtocolBIds().contains(PId))
		{
			ProtocolId protocolAId = MapObjectAddressing(PId, msgData);
			if (protocolAId != INVALID_ADDRESS_VALUE)
				return parentNode->SendMessageTo(protocolAId, Id, msgData);
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

/**
 * Getter for the chCntA member.
 * @return The channel count value.
 */
int Mux_nA_to_mB::GetProtoChCntA()
{
	return m_protoChCntA;
}

/**
 * Getter for the chCntB member.
 * @return The channel count value.
 */
int Mux_nA_to_mB::GetProtoChCntB()
{
	return m_protoChCntB;
}

/**
 * Method to do the mapping of addressing value depending on configured multiplexing settings.
 *
 * @param PId		The id of the protocol that received the data
 * @param msgData	The actual message value/content data
 * @return	The protocol index the mapped value shall be sent to
 */
ProtocolId Mux_nA_to_mB::MapObjectAddressing(ProtocolId PId, RemoteObjectMessageData& msgData)
{
	if (GetProtocolAIds().contains(PId))
	{
		jassert(msgData.addrVal.first <= m_protoChCntA);
		int absChNr = GetProtocolAIds().indexOf(PId) * m_protoChCntA + msgData.addrVal.first;
		int protocolBIndex = absChNr / (m_protoChCntB + 1);
		int16 chForB = static_cast<int16>(absChNr % m_protoChCntB);
		if (chForB == 0)
			chForB = static_cast<int16>(m_protoChCntB);

		msgData.addrVal.first = chForB;

		if (GetProtocolBIds().size() >= protocolBIndex + 1)
			return GetProtocolBIds()[protocolBIndex];
		else
			return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
	}
	else if (GetProtocolBIds().contains(PId))
	{
		jassert(msgData.addrVal.first <= m_protoChCntB);
		int absChNr = GetProtocolBIds().indexOf(PId) * m_protoChCntB + msgData.addrVal.first;
		int protocolAIndex = absChNr / (m_protoChCntA + 1);
		int16 chForA = static_cast<int16>(absChNr % m_protoChCntA);
		if (chForA == 0)
			chForA = static_cast<int16>(m_protoChCntA);

		msgData.addrVal.first = chForA;

		if (GetProtocolAIds().size() >= protocolAIndex + 1)
			return GetProtocolAIds()[protocolAIndex];
		else
			return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
	}

	return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
}


// **************************************************************************************
//    class Forward_only_valueChanges
// **************************************************************************************
/**
 * Constructor of class Forward_only_valueChanges.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Forward_only_valueChanges::Forward_only_valueChanges(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Forward_only_valueChanges);
	m_precision = 0.001f;
}

/**
 * Destructor
 */
Forward_only_valueChanges::~Forward_only_valueChanges()
{
	for (std::pair<RemoteObjectIdentifier, std::map<RemoteObjectAddressing, RemoteObjectMessageData>> roi : m_currentValues)
	{
		for (std::pair<RemoteObjectAddressing, RemoteObjectMessageData> val : roi.second)
		{
			delete val.second.payload;
	
			val.second.payload = nullptr;
			val.second.payloadSize = 0;
			val.second.valCount = 0;
		}
	}
	
	m_currentValues.clear();
}

/**
 * Reimplemented to set the custom parts from configuration for the datahandling object.
 *
 * @param config	The overall configuration object that can be used to query config data from
 * @param NId		The node id of the parent node this data handling object is child of (needed to access data from config)
 */
void Forward_only_valueChanges::SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId)
{
	ObjectDataHandling_Abstract::SetObjectHandlingConfiguration(config, NId);

	m_precision = static_cast<float>(config.GetObjectHandlingData(NId).Prec);
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Forward_only_valueChanges::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode)
	{
		if (!IsChangedDataValue(Id, msgData))
			return false;

		if (GetProtocolAIds().contains(PId))
		{
			// Send to all typeB protocols
			bool sendSuccess = true;
			int typeBProtocolCount = GetProtocolBIds().size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolBIds()[i], Id, msgData);

			return sendSuccess;

		}
		if (GetProtocolBIds().contains(PId))
		{
			// Send to all typeA protocols
			bool sendSuccess = true;
			int typeAProtocolCount = GetProtocolAIds().size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], Id, msgData);

			return sendSuccess;
		}
	}

	return false;
}

/**
 * Helper method to detect if incoming value has changed in any way compared with the previously received one
 * (RemoteObjectIdentifier is taken in account as well as the channel/record addressing)
 *
 * @param Id	The ROI that was received and has to be checked
 * @param msgData	The received message data that has to be checked
 * @return True if a change has been detected, false if not
 */
bool Forward_only_valueChanges::IsChangedDataValue(const RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData)
{
	if (m_precision == 0)
		return true;

	bool isChangedDataValue = false;

	// if our hash does not yet contain our ROI, initialize it
	if ((m_currentValues.count(Id) == 0) || (m_currentValues.at(Id).count(msgData.addrVal) == 0))
	{
		isChangedDataValue = true;
	}
	else
	{
		const RemoteObjectMessageData& currentVal = m_currentValues.at(Id).at(msgData.addrVal);
		if ((currentVal.valType != msgData.valType) || (currentVal.valCount != msgData.valCount) || (currentVal.payloadSize != msgData.payloadSize))
		{
			isChangedDataValue = true;
		}
		else
		{
			uint16 valCount = currentVal.valCount;
			RemoteObjectValueType valType = currentVal.valType;
			void *refData = currentVal.payload;
			void *newData = msgData.payload;
	
			int referencePrecisionValue = 0;
			int newPrecisionValue = 0;
	
			bool changeFound = false;
			for (int i = 0; i < valCount; ++i)
			{
				switch (valType)
				{
				case ROVT_INT:
					{
					// convert payload to correct pointer type
					int *refVal = static_cast<int*>(refData);
					int *newVal = static_cast<int*>(newData);
					// grab actual value
					referencePrecisionValue = *refVal;
					newPrecisionValue = *newVal;
					// increase pointer to next value (to access it in next valCount loop iteration)
					refData = refVal+1;
					newData = newVal+1;
					}
					break;
				case ROVT_FLOAT:
					{
					// convert payload to correct pointer type
					float *refVal = static_cast<float*>(refData);
					float *newVal = static_cast<float *>(newData);
					// grab actual value and apply precision to get a comparable value
					referencePrecisionValue = static_cast<int>(std::roundf(*refVal / m_precision));
					newPrecisionValue		= static_cast<int>(std::roundf(*newVal / m_precision));
					// increase pointer to next value (to access it in next valCount loop iteration)
					refData = refVal+1;
					newData = newVal+1;
					}
					break;
				case ROVT_STRING:
					jassertfalse; // String not (yet?) supported
					changeFound = true;
					break;
				case ROVT_NONE:
				default:
					changeFound = true;
					break;
				}
	
	
				if (referencePrecisionValue != newPrecisionValue)
					changeFound = true;
			}
	
			isChangedDataValue = changeFound;
		}
	}

	if(isChangedDataValue)
		SetCurrentDataValue(Id, msgData);

	return isChangedDataValue;
}

/**
 * Helper method to set a new RemoteObjectMessageData obj. to internal map of current values.
 * Takes care of cleaning up previously stored data if required.
 *
 * @param Id	The ROI that shall be stored
 * @param msgData	The message data that shall be stored
 */
void Forward_only_valueChanges::SetCurrentDataValue(const RemoteObjectIdentifier Id, const RemoteObjectMessageData& msgData)
{
	RemoteObjectAddressing dataValAddr = msgData.addrVal;

	// Check if the new data value addressing is currently not present in internal hash
	// or if it differs in its value size and needs to be reinitialized
	if((m_currentValues.count(Id) == 0) || (m_currentValues.at(Id).count(dataValAddr) == 0) || 
		(m_currentValues.at(Id).at(dataValAddr).payloadSize != msgData.payloadSize))
	{
		// If the data value exists, but has wrong size, reinitialize it
		if((m_currentValues.count(Id) != 0) && (m_currentValues.at(Id).count(dataValAddr) != 0) && 
			(m_currentValues.at(Id).at(dataValAddr).payloadSize != msgData.payloadSize))
		{
			delete m_currentValues.at(Id).at(dataValAddr).payload;
			m_currentValues.at(Id).at(dataValAddr).payload = nullptr;
			m_currentValues.at(Id).at(dataValAddr).payloadSize = 0;
		}
	
		RemoteObjectMessageData dataCopy = msgData;
	
		dataCopy.payload = new unsigned char[msgData.payloadSize];
		memcpy(dataCopy.payload, msgData.payload, msgData.payloadSize);
	
		m_currentValues[Id][dataValAddr] = dataCopy;
	}
	else
	{
		// do not copy entire data struct, since we need to keep our payload ptr
		m_currentValues.at(Id).at(dataValAddr).addrVal = msgData.addrVal;
		m_currentValues.at(Id).at(dataValAddr).valCount = msgData.valCount;
		m_currentValues.at(Id).at(dataValAddr).valType = msgData.valType;
		memcpy(m_currentValues.at(Id).at(dataValAddr).payload, msgData.payload, msgData.payloadSize);
	}
}


// **************************************************************************************
//    class Forward_A_to_B_only
// **************************************************************************************
/**
 * Constructor of class Forward_A_to_B_only.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Forward_A_to_B_only::Forward_A_to_B_only(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Forward_A_to_B_only);
}

/**
 * Destructor
 */
Forward_A_to_B_only::~Forward_A_to_B_only()
{
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Forward_A_to_B_only::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	bool sendSuccess = false;

	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode)
	{
		if (GetProtocolAIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol -> forward it to all typeB protocols
			int typeBProtocolCount = GetProtocolBIds().size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolBIds()[i], Id, msgData);
		}
		else if (GetProtocolBIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeB protocol, which we do not want to forward in this OHM
		}
	}

	return sendSuccess;
}


// **************************************************************************************
//    class Reverse_B_to_A_only
// **************************************************************************************
/**
 * Constructor of class BypassHandling.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Reverse_B_to_A_only::Reverse_B_to_A_only(ProcessingEngineNode* parentNode)
	: ObjectDataHandling_Abstract(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Reverse_B_to_A_only);
}

/**
 * Destructor
 */
Reverse_B_to_A_only::~Reverse_B_to_A_only()
{
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Reverse_B_to_A_only::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	bool sendSuccess = false;

	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode)
	{
		if (GetProtocolAIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol, which we do not want to forward in this OHM
		}
		else if (GetProtocolBIds().contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeB protocol
			int typeAProtocolCount = GetProtocolAIds().size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && parentNode->SendMessageTo(GetProtocolAIds()[i], Id, msgData);
		}
	}

	return sendSuccess;
}


// **************************************************************************************
//    class Mux_nA_to_mB_withValFilter
// **************************************************************************************
/**
 * Constructor of class Mux_nA_to_mB_withValFilter.
 *
 * @param parentNode	The objects' parent node that is used by derived objects to forward received message contents to.
 */
Mux_nA_to_mB_withValFilter::Mux_nA_to_mB_withValFilter(ProcessingEngineNode* parentNode)
	: Forward_only_valueChanges(parentNode)
{
	SetMode(ObjectHandlingMode::OHM_Mux_nA_to_mB_withValFilter);
	m_protoChCntA = 1;
	m_protoChCntB = 1;
}

/**
 * Destructor
 */
Mux_nA_to_mB_withValFilter::~Mux_nA_to_mB_withValFilter()
{
}

/**
 * Reimplemented to set the custom parts from configuration for the datahandling object.
 *
 * @param config	The overall configuration object that can be used to query config data from
 * @param NId		The node id of the parent node this data handling object is child of (needed to access data from config)
 */
void Mux_nA_to_mB_withValFilter::SetObjectHandlingConfiguration(const ProcessingEngineConfig& config, NodeId NId)
{
	Forward_only_valueChanges::SetObjectHandlingConfiguration(config, NId);

	m_protoChCntA = config.GetObjectHandlingData(NId).ACnt;
	m_protoChCntB = config.GetObjectHandlingData(NId).BCnt;
}

/**
 * Method to be called by parent node on receiving data from node protocol with given id
 *
 * @param PId		The id of the protocol that received the data
 * @param Id		The object id to send a message for
 * @param msgData	The actual message value/content data
 * @return	True if successful sent/forwarded, false if not
 */
bool Mux_nA_to_mB_withValFilter::OnReceivedMessageFromProtocol(ProtocolId PId, RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	const ProcessingEngineNode* parentNode = ObjectDataHandling_Abstract::GetParentNode();
	if (parentNode && m_protoChCntA > 0 && m_protoChCntB > 0)
	{
		if (ObjectDataHandling_Abstract::GetProtocolAIds().contains(PId))
		{
			ProtocolId protocolBId = MapObjectAddressing(PId, msgData);

			if (protocolBId != INVALID_ADDRESS_VALUE && IsChangedDataValue(Id, msgData))
				return parentNode->SendMessageTo(protocolBId, Id, msgData);
			else
				return false;
		}
		else if (ObjectDataHandling_Abstract::GetProtocolBIds().contains(PId))
		{
			ProtocolId protocolAId = MapObjectAddressing(PId, msgData);
			if (protocolAId != INVALID_ADDRESS_VALUE && IsChangedDataValue(Id, msgData))
				return parentNode->SendMessageTo(protocolAId, Id, msgData);
			else
				return false;
		}
		else
			return false;
	}
	else
		return false;
}

/**
 * Method to do the mapping of addressing value depending on configured multiplexing settings.
 *
 * @param PId		The id of the protocol that received the data
 * @param msgData	The actual message value/content data
 * @return	The protocol index the mapped value shall be sent to
 */
ProtocolId Mux_nA_to_mB_withValFilter::MapObjectAddressing(ProtocolId PId, RemoteObjectMessageData &msgData)
{
	if(GetProtocolAIds().contains(PId))
	{
		jassert(msgData.addrVal.first <= m_protoChCntA);
		int absChNr		   = GetProtocolAIds().indexOf(PId) * m_protoChCntA + msgData.addrVal.first;
		int protocolBIndex = absChNr / (m_protoChCntB + 1);
		int16 chForB	   = static_cast<int16>(absChNr % m_protoChCntB);
		if(chForB == 0)
			chForB = static_cast<int16>(m_protoChCntB);

		msgData.addrVal.first = chForB;

		if(GetProtocolBIds().size() >= protocolBIndex + 1)
			return GetProtocolBIds()[protocolBIndex];
		else
			return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
	}
	else if(GetProtocolBIds().contains(PId))
	{
		jassert(msgData.addrVal.first <= m_protoChCntB);
		int absChNr		   = GetProtocolBIds().indexOf(PId) * m_protoChCntB + msgData.addrVal.first;
		int protocolAIndex = absChNr / (m_protoChCntA + 1);
		int16 chForA	   = static_cast<int16>(absChNr % m_protoChCntA);
		if(chForA == 0)
			chForA = static_cast<int16>(m_protoChCntA);

		msgData.addrVal.first = chForA;

		if(GetProtocolAIds().size() >= protocolAIndex + 1)
			return GetProtocolAIds()[protocolAIndex];
		else
			return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
	}

	return static_cast<ProtocolId>(INVALID_ADDRESS_VALUE);
}
