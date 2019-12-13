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
			int protocolBIndex = absChNr / m_protoChCntB;
			int16 chForB = int16(absChNr % m_protoChCntB);

			msgData.addrVal.first = chForB;
			if (m_protocolBIds.size() >= protocolBIndex + 1)
				return m_parentNode->SendMessageTo(m_protocolBIds[protocolBIndex], Id, msgData);
		}
		else if (m_protocolBIds.contains(PId))
		{
			jassert(msgData.addrVal.first <= m_protoChCntB);
			int absChNr = m_protocolBIds.indexOf(PId) * m_protoChCntB + msgData.addrVal.first;
			int protocolAIndex = absChNr / m_protoChCntA;
			int16 chForA = int16(absChNr % m_protoChCntA);

			msgData.addrVal.first = chForA;
			if (m_protocolAIds.size() >= protocolAIndex + 1)
				return m_parentNode->SendMessageTo(m_protocolAIds[protocolAIndex], Id, msgData);
		}
	}

	return false;
}