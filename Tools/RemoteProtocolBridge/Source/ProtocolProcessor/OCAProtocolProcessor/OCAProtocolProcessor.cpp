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

#include "OCAProtocolProcessor.h"

#include "../../ProcessingEngineConfig.h"


// **************************************************************************************
//    class OCAProtocolProcessor
// **************************************************************************************
/**
 * Derived OCA remote protocol processing class
 */
OCAProtocolProcessor::OCAProtocolProcessor()
	: ProtocolProcessor_Abstract()
{
	m_type = ProtocolType::PT_OCAProtocol;
}

/**
 * Destructor
 */
OCAProtocolProcessor::~OCAProtocolProcessor()
{
	
}

/**
 * Overloaded method to start the protocol processing object.
 * Usually called after configuration has been set.
 */
bool OCAProtocolProcessor::Start()
{
	return true;
}

/**
 * Overloaded method to stop to protocol processing object.
 */
bool OCAProtocolProcessor::Stop()
{
	return true;
}

/**
 * Setter for remote object to specifically activate.
 * For OCA processing this is used to initialize OCA Object Subscriptions
 * NOT YET IMPLEMENTED
 *
 * @param Objs	The list of RemoteObjects that shall be activated
 */
void OCAProtocolProcessor::SetRemoteObjectsActive(const Array<RemoteObject>& Objs)
{
	ignoreUnused(Objs);
}

/**
 * Method to trigger sending of a message
 * NOT YET IMPLEMENTED
 *
 * @param Id		The id of the object to send a message for
 * @param msgData	The message payload and metadata
 */
bool OCAProtocolProcessor::SendMessage(RemoteObjectIdentifier Id, RemoteObjectMessageData& msgData)
{
	ignoreUnused(Id);
	ignoreUnused(msgData);

	return false;
}

/**
 * Private method to get OCA object specific ObjectName string
 * 
 * @param id	The object id to get the OCA specific object name
 * @return		The OCA specific object name
 */
String OCAProtocolProcessor::GetRemoteObjectString(RemoteObjectIdentifier id)
{
	switch (id)
	{
	case ROI_SoundObject_Position_X:
		return "Positioning_Source_Position_X";
	case ROI_SoundObject_Position_Y:
		return "Positioning_Source_Position_Y";
	case ROI_SoundObject_Spread:
		return "Positioning_Source_Spread";
	case ROI_SoundObject_DelayMode:
		return "Positioning_Source_DelayMode";
	case ROI_ReverbSendGain:
		return "MatrixInput_ReverbSendGain";
	default:
		return "";
	}
}