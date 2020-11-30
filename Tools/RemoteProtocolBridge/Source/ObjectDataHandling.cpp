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
 * Getter for the mode of this objectdata handling object
 *
 * @return The mode of this objectdata handling object
 */
ObjectHandlingMode ObjectDataHandling_Abstract::GetMode()
{
	return m_mode;
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
	m_mode = ObjectHandlingMode::OHM_Bypass;
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

	if (m_parentNode)
	{
		if (m_protocolAIds.contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol -> forward it to all typeB protocols
			int typeBProtocolCount = m_protocolBIds.size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolBIds[i], Id, msgData);
		}
		else if (m_protocolBIds.contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeB protocol
			int typeAProtocolCount = m_protocolAIds.size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], Id, msgData);
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
	m_mode = ObjectHandlingMode::OHM_Remap_A_X_Y_to_B_XY;
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
	if (m_parentNode)
	{
		if (m_protocolAIds.contains(PId))
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
			int typeBProtocolCount = m_protocolBIds.size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolBIds[i], ObjIdToSend, msgData);

			return sendSuccess;
			
		}
		if (m_protocolBIds.contains(PId))
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
				int typeAProtocolCount = m_protocolAIds.size();
				for (int i = 0; i < typeAProtocolCount; ++i)
				{
					msgData.payload = &newXVal;
					sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], ROI_SoundObject_Position_X, msgData);

					msgData.payload = &newYVal;
					sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], ROI_SoundObject_Position_Y, msgData);
				}

				return sendSuccess;
			}
			else
			{
				// Send to all typeA protocols
				bool sendSuccess = true;
				int typeAProtocolCount = m_protocolAIds.size();
				for (int i = 0; i < typeAProtocolCount; ++i)
					sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], Id, msgData);

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
	m_mode			= ObjectHandlingMode::OHM_Mux_nA_to_mB;
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
	if (m_parentNode && m_protoChCntA>0 && m_protoChCntB>0)
	{
		if (m_protocolAIds.contains(PId))
		{
			jassert(msgData.addrVal.first <= m_protoChCntA);
			int absChNr = m_protocolAIds.indexOf(PId) * m_protoChCntA + msgData.addrVal.first;
			int protocolBIndex = absChNr / (m_protoChCntB + 1);
			int16 chForB = int16(absChNr % m_protoChCntB);
			if (chForB == 0)
				chForB = m_protoChCntB;

			msgData.addrVal.first = chForB;
			if (m_protocolBIds.size() >= protocolBIndex + 1)
				return m_parentNode->SendMessageTo(m_protocolBIds[protocolBIndex], Id, msgData);
		}
		else if (m_protocolBIds.contains(PId))
		{
			jassert(msgData.addrVal.first <= m_protoChCntB);
			int absChNr = m_protocolBIds.indexOf(PId) * m_protoChCntB + msgData.addrVal.first;
			int protocolAIndex = absChNr / (m_protoChCntA + 1);
			int16 chForA = int16(absChNr % m_protoChCntA);
			if (chForA == 0)
				chForA = m_protoChCntA;

			msgData.addrVal.first = chForA;
			if (m_protocolAIds.size() >= protocolAIndex + 1)
				return m_parentNode->SendMessageTo(m_protocolAIds[protocolAIndex], Id, msgData);
		}
	}

	return false;
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
	m_mode = ObjectHandlingMode::OHM_Forward_only_valueChanges;
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

	m_precision = config.GetObjectHandlingData(NId).Prec;
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
	if (m_parentNode)
	{
		if (!IsChangedDataValue(Id, msgData))
			return false;

		if (m_protocolAIds.contains(PId))
		{
			// Send to all typeB protocols
			bool sendSuccess = true;
			int typeBProtocolCount = m_protocolBIds.size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolBIds[i], Id, msgData);

			return sendSuccess;

		}
		if (m_protocolBIds.contains(PId))
		{
			// Send to all typeA protocols
			bool sendSuccess = true;
			int typeAProtocolCount = m_protocolAIds.size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], Id, msgData);

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
	if ((m_currentValues.count(Id) == 0) || (m_currentValues.at(Id).count(msgData.addrVal)== 0))
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
					int *refVal = static_cast<int*>(refData);
					int *newVal = static_cast<int*>(newData);
					referencePrecisionValue = static_cast<int>(*refVal);
					newPrecisionValue = static_cast<int>(*newVal);
					refData = refVal+1;
					newData = newVal+1;
					}
					break;
				case ROVT_FLOAT:
					{
					float *refVal = static_cast<float*>(refData);
					float *newVal = static_cast<float*>(newData);
					referencePrecisionValue = static_cast<int>((*refVal) / m_precision);
					newPrecisionValue = static_cast<int>((*newVal) / m_precision);
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
	if ((m_currentValues.count(Id) == 0) || (m_currentValues.at(Id).count(msgData.addrVal) == 0) || (m_currentValues.at(Id).at(msgData.addrVal).payloadSize != msgData.payloadSize))
	{
		if ((m_currentValues.count(Id) != 0) && (m_currentValues.at(Id).count(msgData.addrVal) != 0) && (m_currentValues.at(Id).at(msgData.addrVal).payloadSize != msgData.payloadSize))
		{
			delete m_currentValues.at(Id).at(msgData.addrVal).payload;
			m_currentValues.at(Id).at(msgData.addrVal).payload = nullptr;
			m_currentValues.at(Id).at(msgData.addrVal).payloadSize = 0;
		}
	
		RemoteObjectMessageData dataCopy = msgData;
	
		dataCopy.payload = new unsigned char[msgData.payloadSize];
		memcpy(dataCopy.payload, msgData.payload, msgData.payloadSize);
	
		m_currentValues[Id][msgData.addrVal] = dataCopy;
	}
	else
	{
		// do not copy entire data struct, since we need to keep our payload ptr
		m_currentValues[Id][msgData.addrVal].addrVal = msgData.addrVal;
		m_currentValues[Id][msgData.addrVal].valCount = msgData.valCount;
		m_currentValues[Id][msgData.addrVal].valType = msgData.valType;
		memcpy(m_currentValues[Id][msgData.addrVal].payload, msgData.payload, msgData.payloadSize);
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
	m_mode = ObjectHandlingMode::OHM_Forward_A_to_B_only;
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

	if (m_parentNode)
	{
		if (m_protocolAIds.contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol -> forward it to all typeB protocols
			int typeBProtocolCount = m_protocolBIds.size();
			for (int i = 0; i < typeBProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolBIds[i], Id, msgData);
		}
		else if (m_protocolBIds.contains(PId))
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
	m_mode = ObjectHandlingMode::OHM_Reverse_B_to_A_only;
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

	if (m_parentNode)
	{
		if (m_protocolAIds.contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeA protocol, which we do not want to forward in this OHM
		}
		else if (m_protocolBIds.contains(PId))
		{
			sendSuccess = true;
			// the message was received by a typeB protocol
			int typeAProtocolCount = m_protocolAIds.size();
			for (int i = 0; i < typeAProtocolCount; ++i)
				sendSuccess = sendSuccess && m_parentNode->SendMessageTo(m_protocolAIds[i], Id, msgData);
		}
	}

	return sendSuccess;
}
