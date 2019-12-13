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

#include "../../Common.h"
#include "../ProtocolProcessor_Abstract.h"

#include "SenderAwareOSCReceiver.h"

#include "../../../JuceLibraryCode/JuceHeader.h"

using namespace SenderAwareOSC;

/**
 * Class OSCProtocolProcessor is a derived class for OSC protocol interaction.
 */
class OSCProtocolProcessor : public SenderAwareOSCReceiver::SAOListener<OSCReceiver::MessageLoopCallback>,
	public ProtocolProcessor_Abstract,
	private Timer
{
public:
	OSCProtocolProcessor(int listenerPortNumber);
	~OSCProtocolProcessor();

	bool Start() override;
	bool Stop() override;
	void SetRemoteObjectsActive(const Array<RemoteObject>& Objs) override;
	bool SendMessage(RemoteObjectIdentifier id, RemoteObjectMessageData& msgData) override;

	static String GetRemoteObjectString(RemoteObjectIdentifier id);

	virtual void oscBundleReceived(const OSCBundle &bundle, const String& senderIPAddress, const int& senderPort) override;
	virtual void oscMessageReceived(const OSCMessage &message, const String& senderIPAddress, const int& senderPort) override;

private:
	void timerCallback() override;

private:
	OSCSender				m_oscSender;			/**< An OSCSender object can connect to a network port. It then can send OSC
													   * messages and bundles to a specified host over an UDP socket. */
	SenderAwareOSCReceiver	m_oscReceiver;			/**< An OSCReceiver object can connect to a network port, receive incoming OSC packets from the network
													   * via UDP, parse them, and forward the included OSCMessage and OSCBundle objects to its listeners. */
	int						m_oscMsgRate;			/**< Interval at which OSC messages are sent to the host, in ms. */
	Array<RemoteObject>		m_activeRemoteObjects;	/**< List of remote objects to be activly handled. */
};
