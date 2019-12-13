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

#include "../JuceLibraryCode/JuceHeader.h"
#include <juce_osc/juce_osc.h>	


namespace SenderAwareOSC
{

/**
* This class implements a udp osc receiver, similar to JUCEs' own OCSReceiver implementation.
* The most important difference is the modification to be able to differentiate between udp data sources.
* This is realized by passing the sender ip adress and port along the received payload to recipients.
*/
class SenderAwareOSCReceiver
{
public:
	//==============================================================================
	/** Creates an OSCReceiver. */
	SenderAwareOSCReceiver(int portNumber);

	/** Destructor. */
	~SenderAwareOSCReceiver();

	//==============================================================================
	/** Connects to the specified UDP port using a datagram socket,
		and starts listening to OSC packets arriving on this port.

		@returns true if the connection was successful; false otherwise.
	*/
	bool connect();

	/** Connects to a UDP datagram socket that is already set up,
		and starts listening to OSC packets arriving on this port.
		Make sure that the object you give it doesn't get deleted while this
		object is still using it!
		@returns true if the connection was successful; false otherwise.
	*/
	bool connectToSocket(DatagramSocket& socketToUse);

	//==============================================================================
	/** Disconnects from the currently used UDP port.
		@returns true if the disconnection was successful; false otherwise.
	*/
	bool disconnect();

	//==============================================================================
	/** A class for receiving OSC data from an OSCReceiver.

		The template argument CallbackType determines how the callback will be called
		and has to be either MessageLoopCallback or RealtimeCallback. If not specified,
		MessageLoopCallback will be used by default.

		@see OSCReceiver::addListener, OSCReceiver::ListenerWithOSCAddress,
			 OSCReceiver::MessageLoopCallback, OSCReceiver::RealtimeCallback

	*/
	template <typename CallbackType = OSCReceiver::MessageLoopCallback>
	class SAOListener
	{
	public:
		/** Destructor. */
		virtual ~SAOListener() = default;

		/** Called when the OSCReceiver receives a new OSC message.
			You must implement this function.
		*/
		virtual void oscMessageReceived(const OSCMessage& message, const String& senderIPAddress, const int& senderPort) = 0;

		/** Called when the OSCReceiver receives a new OSC bundle.
			If you are not interested in OSC bundles, just ignore this method.
			The default implementation provided here will simply do nothing.
		*/
		virtual void oscBundleReceived(const OSCBundle& /*bundle*/, const String& /*senderIPAddress*/, const int& /*senderPort*/) {}
	};

	//==============================================================================
	/** A class for receiving only those OSC messages from an OSCReceiver that match a
		given OSC address.

		Use this class if your app receives OSC messages with different address patterns
		(for example "/juce/fader1", /juce/knob2" etc.) and you want to route those to
		different objects. This class contains pre-build functionality for that OSC
		address routing, including wildcard pattern matching (e.g. "/juce/fader[0-9]").

		This class implements the concept of an "OSC Method" from the OpenSoundControl 1.0
		specification.

		The template argument CallbackType determines how the callback will be called
		and has to be either MessageLoopCallback or RealtimeCallback. If not specified,
		MessageLoopCallback will be used by default.

		Note: This type of listener will ignore OSC bundles.

		@see OSCReceiver::addListener, OSCReceiver::Listener,
			 OSCReceiver::MessageLoopCallback, OSCReceiver::RealtimeCallback
	*/
	template <typename CallbackType = OSCReceiver::MessageLoopCallback>
	class SAOListenerWithOSCAddress
	{
	public:
		/** Destructor. */
		virtual ~SAOListenerWithOSCAddress() = default;

		/** Called when the OSCReceiver receives an OSC message with an OSC address
			pattern that matches the OSC address with which this listener was added.
		*/
		virtual void oscMessageReceived(const OSCMessage& message, const String& senderIPAddress, const int& senderPort) = 0;
	};

	//==============================================================================
	/** Adds a listener that listens to OSC messages and bundles.
		This listener will be called on the application's message loop.
	*/
	void addListener(SAOListener<OSCReceiver::MessageLoopCallback>* listenerToAdd);

	/** Adds a listener that listens to OSC messages and bundles.
		This listener will be called in real-time directly on the network thread
		that receives OSC data.
	*/
	void addListener(SAOListener<OSCReceiver::RealtimeCallback>* listenerToAdd);

	/** Removes a previously-registered listener. */
	void removeListener(SAOListener<OSCReceiver::MessageLoopCallback>* listenerToRemove);

	/** Removes a previously-registered listener. */
	void removeListener(SAOListener<OSCReceiver::RealtimeCallback>* listenerToRemove);

	/** Installs a custom error handler which is called in case the receiver
		encounters a stream it cannot parse as an OSC bundle or OSC message.

		By default (i.e. if you never use this method), in case of a parsing error
		nothing happens and the invalid packet is simply discarded.
	*/
	void registerFormatErrorHandler(OSCReceiver::FormatErrorHandler handler);

private:
	//==============================================================================
	int m_portNumber;	/**< Port number to listen on. */

	//==============================================================================
	struct SAOPimpl;
	SAOPimpl* m_pimpl; /**< Internal private implementation instance of SenderAwareOSCReceiver. */
	friend struct OSCReceiverCallbackMessage;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SenderAwareOSCReceiver)
};

}
