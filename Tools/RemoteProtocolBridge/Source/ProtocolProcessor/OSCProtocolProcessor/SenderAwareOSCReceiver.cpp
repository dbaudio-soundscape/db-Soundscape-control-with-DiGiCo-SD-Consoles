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

#include "SenderAwareOSCReceiver.h"


namespace SenderAwareOSC
{

	namespace
	{
		//==============================================================================
		/** Allows a block of data to be accessed as a stream of OSC data.
	
			The memory is shared and will be neither copied nor owned by the OSCInputStream.
	
			This class is implementing the Open Sound Control 1.0 Specification for
			interpreting the data.
	
			Note: Some older implementations of OSC may omit the OSC Type Tag string
			in OSC messages. This class will treat such OSC messages as format errors.
		*/
		class SenderAwareOSCInputStream
		{
		public:
			/** Creates an SenderAwareOSCInputStream.
	
				@param sourceData               the block of data to use as the stream's source
				@param sourceDataSize           the number of bytes in the source data block
			*/
			SenderAwareOSCInputStream(const void* sourceData, size_t sourceDataSize)
				: input(sourceData, sourceDataSize, false)
			{}
	
			//==============================================================================
			/** Returns a pointer to the source data block from which this stream is reading. */
			const void* getData() const noexcept { return input.getData(); }
	
			/** Returns the number of bytes of source data in the block from which this stream is reading. */
			size_t getDataSize() const noexcept { return input.getDataSize(); }
	
			/** Returns the current position of the stream. */
			uint64 getPosition() { return (uint64)input.getPosition(); }
	
			/** Attempts to set the current position of the stream. Returns true if this was successful. */
			bool setPosition(int64 pos) { return input.setPosition(pos); }
	
			/** Returns the total amount of data in bytes accessible by this stream. */
			int64 getTotalLength() { return input.getTotalLength(); }
	
			/** Returns true if the stream has no more data to read. */
			bool isExhausted() { return input.isExhausted(); }
	
			//==============================================================================
			int32 readInt32()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading int32");
				return input.readIntBigEndian();
			}
	
			uint64 readUint64()
			{
				checkBytesAvailable(8, "OSC input stream exhausted while reading uint64");
				return (uint64)input.readInt64BigEndian();
			}
	
			float readFloat32()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading float");
				return input.readFloatBigEndian();
			}
	
			String readString()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading string");
	
				auto posBegin = (size_t)getPosition();
				auto s = input.readString();
				auto posEnd = (size_t)getPosition();
	
				if (static_cast<const char*> (getData())[posEnd - 1] != '\0')
					throw OSCFormatError("OSC input stream exhausted before finding null terminator of string");
	
				size_t bytesRead = posEnd - posBegin;
				readPaddingZeros(bytesRead);
	
				return s;
			}
	
			MemoryBlock readBlob()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading blob");
	
				auto blobDataSize = input.readIntBigEndian();
				checkBytesAvailable((blobDataSize + 3) % 4, "OSC input stream exhausted before reaching end of blob");
	
				MemoryBlock blob;
				auto bytesRead = input.readIntoMemoryBlock(blob, (ssize_t)blobDataSize);
				readPaddingZeros(bytesRead);
	
				return blob;
			}
	
			OSCColour readColour()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading colour");
				return OSCColour::fromInt32((uint32)input.readIntBigEndian());
			}
	
			OSCTimeTag readTimeTag()
			{
				checkBytesAvailable(8, "OSC input stream exhausted while reading time tag");
				return OSCTimeTag(uint64(input.readInt64BigEndian()));
			}
	
			OSCAddress readAddress()
			{
				return OSCAddress(readString());
			}
	
			OSCAddressPattern readAddressPattern()
			{
				return OSCAddressPattern(readString());
			}
	
			//==============================================================================
			OSCTypeList readTypeTagString()
			{
				OSCTypeList typeList;
	
				checkBytesAvailable(4, "OSC input stream exhausted while reading type tag string");
	
				if (input.readByte() != ',')
					throw OSCFormatError("OSC input stream format error: expected type tag string");
	
				for (;;)
				{
					if (isExhausted())
						throw OSCFormatError("OSC input stream exhausted while reading type tag string");
	
					const OSCType type = input.readByte();
	
					if (type == 0)
						break;  // encountered null terminator. list is complete.
	
					if (!OSCTypes::isSupportedType(type))
						throw OSCFormatError("OSC input stream format error: encountered unsupported type tag");
	
					typeList.add(type);
				}
	
				auto bytesRead = (size_t)typeList.size() + 2;
				readPaddingZeros(bytesRead);
	
				return typeList;
			}
	
			//==============================================================================
			OSCArgument readArgument(OSCType type)
			{
				switch (type)
				{
				case 'i':	return OSCArgument(readInt32());
				case 'f':	return OSCArgument(readFloat32());
				case 's':	return OSCArgument(readString());
				case 'b':	return OSCArgument(readBlob());
				case 'r':	return OSCArgument(readColour());
	
				default:
					// You supplied an invalid OSCType when calling readArgument! This should never happen.
					jassertfalse;
					throw OSCInternalError("OSC input stream: internal error while reading message argument");
				}
			}
	
			//==============================================================================
			OSCMessage readMessage()
			{
				auto ap = readAddressPattern();
				auto types = readTypeTagString();
	
				OSCMessage msg(ap);
	
				for (auto& type : types)
					msg.addArgument(readArgument(type));
	
				return msg;
			}
	
			//==============================================================================
			OSCBundle readBundle(size_t maxBytesToRead = std::numeric_limits<size_t>::max())
			{
				// maxBytesToRead is only passed in here in case this bundle is a nested
				// bundle, so we know when to consider the next element *not* part of this
				// bundle anymore (but part of the outer bundle) and return.
	
				checkBytesAvailable(16, "OSC input stream exhausted while reading bundle");
	
				if (readString() != "#bundle")
					throw OSCFormatError("OSC input stream format error: bundle does not start with string '#bundle'");
	
				OSCBundle bundle(readTimeTag());
	
				size_t bytesRead = 16; // already read "#bundle" and timeTag
				auto pos = getPosition();
	
				while (!isExhausted() && bytesRead < maxBytesToRead)
				{
					bundle.addElement(readElement());
	
					auto newPos = getPosition();
					bytesRead += (size_t)(newPos - pos);
					pos = newPos;
				}
	
				return bundle;
			}
	
			//==============================================================================
			OSCBundle::Element readElement()
			{
				checkBytesAvailable(4, "OSC input stream exhausted while reading bundle element size");
	
				auto elementSize = (size_t)readInt32();
	
				if (elementSize < 4)
					throw OSCFormatError("OSC input stream format error: invalid bundle element size");
	
				return readElementWithKnownSize(elementSize);
			}
	
			//==============================================================================
			OSCBundle::Element readElementWithKnownSize(size_t elementSize)
			{
				checkBytesAvailable((int64)elementSize, "OSC input stream exhausted while reading bundle element content");
	
				auto firstContentChar = static_cast<const char*> (getData())[getPosition()];
	
				if (firstContentChar == '/')  return OSCBundle::Element(readMessageWithCheckedSize(elementSize));
				if (firstContentChar == '#')  return OSCBundle::Element(readBundleWithCheckedSize(elementSize));
	
				throw OSCFormatError("OSC input stream: invalid bundle element content");
			}
	
		private:
			MemoryInputStream input;
	
			//==============================================================================
			void readPaddingZeros(size_t bytesRead)
			{
				size_t numZeros = ~(bytesRead - 1) & 0x03;
	
				while (numZeros > 0)
				{
					if (isExhausted() || input.readByte() != 0)
						throw OSCFormatError("OSC input stream format error: missing padding zeros");
	
					--numZeros;
				}
			}
	
			OSCBundle readBundleWithCheckedSize(size_t size)
			{
				auto begin = (size_t)getPosition();
				auto maxBytesToRead = size - 4; // we've already read 4 bytes (the bundle size)
	
				OSCBundle bundle(readBundle(maxBytesToRead));
	
				if (getPosition() - begin != size)
					throw OSCFormatError("OSC input stream format error: wrong element content size encountered while reading");
	
				return bundle;
			}
	
			OSCMessage readMessageWithCheckedSize(size_t size)
			{
				auto begin = (size_t)getPosition();
				auto message = readMessage();
	
				if (getPosition() - begin != size)
					throw OSCFormatError("OSC input stream format error: wrong element content size encountered while reading");
	
				return message;
			}
	
			void checkBytesAvailable(int64 requiredBytes, const char* message)
			{
				if (input.getNumBytesRemaining() < requiredBytes)
					throw OSCFormatError(message);
			}
		};
	
	} // namespace


	//==============================================================================
	/**
	 * Private implementation of osc receiver. This is an adapted implementation of
	 * JUCEs' OSCReceiver::pimpl implementation with additional support for sender
	 * awareness. JUCE OSC implementation cannot differentiate between senders of received
	 * data - since we need this, we have to provide a modified implementation for this.
	 */
	struct SenderAwareOSCReceiver::SAOPimpl : private Thread,
		private MessageListener
	{
		SAOPimpl() : Thread("SenderAware OSC server")
		{
			refCount = 0;
		}

		~SAOPimpl()
		{
			disconnect();
		}

		/**
		 * Method to connect to a port with given number
		 *
		 * @param portNumber	The port number to connect to.
		 * @return	True on success, false on failure.
		 */
		bool connectToPort(int portNumber)
		{
			if (!disconnect())
				return false;

			socket.setOwned(new DatagramSocket(false));

			if (!socket->bindToPort(portNumber))
				return false;

			startThread();
			return true;
		}

		/**
		 * Method to connect to a socket (UDP)
		 *
		 * @param newSocket		The udp socket object to use to connect.
		 * @return	True on success, false on failure.
		 */
		bool connectToSocket(DatagramSocket& newSocket)
		{
			if (!disconnect())
				return false;

			socket.setNonOwned(&newSocket);
			startThread();
			return true;
		}

		/**
		 * Method to disconnect established connections.
		 *
		 * @return	True on success, false on failure.
		 */
		bool disconnect()
		{
			if (socket != nullptr)
			{
				signalThreadShouldExit();

				if (socket.willDeleteObject())
					socket->shutdown();

				waitForThreadToExit(10000);
				socket.reset();
			}

			return true;
		}

		/**
		 * Method to add a Listener to internal list.
		 *
		 * @param listenerToAdd	The listener object to add.
		 */
		void addListener(SenderAwareOSCReceiver::SAOListener<OSCReceiver::MessageLoopCallback>* listenerToAdd)
		{
			listeners.add(listenerToAdd);
		}

		/**
		 * Method to add a Listener to internal list.
		 *
		 * @param listenerToAdd	The listener object to add.
		 */
		void addListener(SenderAwareOSCReceiver::SAOListener<OSCReceiver::RealtimeCallback>* listenerToAdd)
		{
			realtimeListeners.add(listenerToAdd);
		}

		/**
		 * Method to remove a Listener from internal list.
		 *
		 * @param listenerToRemove	The listener object to remove.
		 */
		void removeListener(SenderAwareOSCReceiver::SAOListener<OSCReceiver::MessageLoopCallback>* listenerToRemove)
		{
			listeners.remove(listenerToRemove);
		}

		/**
		 * Method to remove a Listener from internal list.
		 *
		 * @param listenerToRemove	The listener object to remove.
		 */
		void removeListener(SenderAwareOSCReceiver::SAOListener<OSCReceiver::RealtimeCallback>* listenerToRemove)
		{
			realtimeListeners.remove(listenerToRemove);
		}

		//==============================================================================
		/**
		 * Implementation of an osc message. This differs from JUCEs' OSCReceiver::pimpl::CallbackMessage
		 * to enable handling of sender IP and Port of the payload data.
		 */
		struct CallbackMessage : public Message
		{
			/**
			* Constructor with default initialization of sender ip and port.
			*
			* @param oscElement	The osc element data to use in this message.
			*/
			CallbackMessage(OSCBundle::Element oscElement) : content(oscElement), senderIPAddress(String()), senderPort(0) {}

			/**
			* Constructor with default initialization of sender ip and port.
			*
			* @param oscElement	The osc element data to use in this message.
			* @param sndIP	The sender ip of this message.
			* @param sndPort The port this message was received on.
			*/
			CallbackMessage(OSCBundle::Element oscElement, String sndIP, int sndPort) : content(oscElement), senderIPAddress(sndIP), senderPort(sndPort) {}

			OSCBundle::Element	content;			/**< The payload of the message. Can be either an OSCMessage or an OSCBundle. */
			String				senderIPAddress;	/**< The sender ip address from whom the message was received. */
			int					senderPort;			/**< The sender port from where the message was received. */
		};

		//==============================================================================
		/**
		 * Method to run to process received data buffer, incl. handling of ip and port the data originates from.
		 *
		 * @param data		The data buffer to handle.
		 * @param dataSize	The data buffer size.
		 * @param senderIPAddress	The ip the received data originates from.
		 * @param senderPort	The port the data was received on.
		 */
		void handleBuffer(const char* data, size_t dataSize, String& senderIPAddress, int& senderPort)
		{
			SenderAwareOSCInputStream inStream(data, dataSize);

			try
			{
				auto content = inStream.readElementWithKnownSize(dataSize);

				// realtime listeners should receive the OSC content first - and immediately
				// on this thread:
				callRealtimeListeners(content, senderIPAddress, senderPort);

				// now post the message that will trigger the handleMessage callback
				// dealing with the non-realtime listeners.
				if (listeners.size() > 0)
					postMessage(new CallbackMessage(content, senderIPAddress, senderPort));
			}
			catch (const OSCFormatError&)
			{
				if (formatErrorHandler != nullptr)
					formatErrorHandler(data, (int)dataSize);
			}
		}

		//==============================================================================
		/**
		 * Method to register a format error handling object.
		 *
		 * @param handler	The handler object to register.
		 */
		void registerFormatErrorHandler(OSCReceiver::FormatErrorHandler handler)
		{
			formatErrorHandler = handler;
		}

		//==============================================================================
		/**
		 * Getter for an instance corresponding to a given port number of this object (private impl. of sender aware osc, SAOPimpl).
		 * - If not existing, a new instance is being instanciated and added to internal hash.
		 * - If existing in hash, internal refcount is increased
		 *
		 * @param portNumber	The port number to return the corresponding instance of.
		 * @return	The instance of SAOPimpl for the given port number.
		 */
		static SAOPimpl* getInstance(int portNumber)
		{
			if (!m_pimples.count(portNumber))
			{
				m_pimples[portNumber] = std::make_unique<SAOPimpl>();
			}

			m_pimples[portNumber]->increaseRefCount();

			return m_pimples[portNumber].get();
		}
		/**
		 * Method to clear instances of SAOPimple for a given port number.
		 * - If refcount hints at this being the last reference to an instance, the instance is cleared.
		 * - If refcount hints at this not being the last reference to an instance, refcount is decreased.
		 *
		 * @param portNumber	The port number the corresponding instance of shall be cleared.
		 */
		static void cleanInstances(int portNumber)
		{
			if (m_pimples.count(portNumber) && m_pimples[portNumber])
			{
				if (m_pimples[portNumber]->isLastRef())
				{
					m_pimples[portNumber].reset();
					m_pimples.erase(portNumber);
				}
				else
				{
					m_pimples[portNumber]->decreaseRefCount();
				}
			}
		}

	private:
		//==============================================================================
		void run() override
		{
			int bufferSize = 65535;
			HeapBlock<char> oscBuffer(bufferSize);

			String senderIPAddress;
			int senderPortNumber;

			while (!threadShouldExit())
			{
				jassert(socket != nullptr);
				auto ready = socket->waitUntilReady(true, 100);

				if (ready < 0 || threadShouldExit())
					return;

				if (ready == 0)
					continue;

				auto bytesRead = (size_t)socket->read(oscBuffer.getData(), bufferSize, false, senderIPAddress, senderPortNumber);

				if (bytesRead >= 4)
					handleBuffer(oscBuffer.getData(), bytesRead, senderIPAddress, senderPortNumber);
			}
		}

		//==============================================================================
		template <typename ListenerType>
		void addListenerWithAddress(ListenerType* listenerToAdd,
			OSCAddress address,
			Array<std::pair<OSCAddress, ListenerType*>>& array)
		{
			for (auto& i : array)
				if (address == i.first && listenerToAdd == i.second)
					return;

			array.add(std::make_pair(address, listenerToAdd));
		}

		//==============================================================================
		template <typename ListenerType>
		void removeListenerWithAddress(ListenerType* listenerToRemove,
			Array<std::pair<OSCAddress, ListenerType*>>& array)
		{
			for (int i = 0; i < array.size(); ++i)
			{
				if (listenerToRemove == array.getReference(i).second)
				{
					// aarrgh... can't simply call array.remove (i) because this
					// requires a default c'tor to be present for OSCAddress...
					// luckily, we don't care about methods preserving element order:
					array.swap(i, array.size() - 1);
					array.removeLast();
					break;
				}
			}
		}

		//==============================================================================
		void handleMessage(const Message& msg) override
		{
			if (auto* callbackMessage = dynamic_cast<const CallbackMessage*> (&msg))
			{
				auto& content = callbackMessage->content;
				auto& senderIPAddress = callbackMessage->senderIPAddress;
				auto& senderPort = callbackMessage->senderPort;

				callListeners(content, senderIPAddress, senderPort);
			}
		}

		//==============================================================================
		void callListeners(const OSCBundle::Element& content, const String& senderIPAddress, const int& senderPort)
		{
			using Listener = SenderAwareOSCReceiver::SAOListener<OSCReceiver::MessageLoopCallback>;

			if (content.isMessage())
			{
				auto&& message = content.getMessage();
				listeners.call([&](Listener& l) { l.oscMessageReceived(message, senderIPAddress, senderPort); });
			}
			else if (content.isBundle())
			{
				auto&& bundle = content.getBundle();
				listeners.call([&](Listener& l) { l.oscBundleReceived(bundle, senderIPAddress, senderPort); });
			}
		}

		void callRealtimeListeners(const OSCBundle::Element& content, const String& senderIPAddress, const int& senderPort)
		{
			using Listener = SenderAwareOSCReceiver::SAOListener<OSCReceiver::RealtimeCallback>;

			if (content.isMessage())
			{
				auto&& message = content.getMessage();
				realtimeListeners.call([&](Listener& l) { l.oscMessageReceived(message, senderIPAddress, senderPort); });
			}
			else if (content.isBundle())
			{
				auto&& bundle = content.getBundle();
				realtimeListeners.call([&](Listener& l) { l.oscBundleReceived(bundle, senderIPAddress, senderPort); });
			}
		}

		//==============================================================================
		bool increaseRefCount()
		{
			if (refCount < INT_MAX)
				refCount++;
			else
				return false;
			
			return true;
		}
		bool decreaseRefCount()
		{
			if (refCount > 0)
				refCount--;
			else
				return false;

			return true;
		}
		bool isLastRef()
		{
			return refCount == 1;
		}

		//==============================================================================
		ListenerList<SenderAwareOSCReceiver::SAOListener<OSCReceiver::MessageLoopCallback>> listeners;
		ListenerList<SenderAwareOSCReceiver::SAOListener<OSCReceiver::RealtimeCallback>>    realtimeListeners;

		OptionalScopedPointer<DatagramSocket> socket;
		OSCReceiver::FormatErrorHandler formatErrorHandler{ nullptr };

		friend class SenderAwareOSCReceiver;
		friend struct pimples;

		int refCount;

		static std::map<int, std::unique_ptr<SAOPimpl>> m_pimples;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SAOPimpl)
	};

	//==============================================================================
	std::map<int, std::unique_ptr<SenderAwareOSCReceiver::SAOPimpl>> SenderAwareOSCReceiver::SAOPimpl::m_pimples;

	//==============================================================================
	SenderAwareOSCReceiver::SenderAwareOSCReceiver(int portNumber) : m_pimpl(SAOPimpl::getInstance(portNumber))
	{
		m_portNumber = portNumber;
	}

	SenderAwareOSCReceiver::~SenderAwareOSCReceiver()
	{
		SAOPimpl::cleanInstances(m_portNumber);
	}

	bool SenderAwareOSCReceiver::connect()
	{
		return m_pimpl->connectToPort(m_portNumber);
	}

	bool SenderAwareOSCReceiver::connectToSocket(DatagramSocket& socket)
	{
		return m_pimpl->connectToSocket(socket);
	}

	bool SenderAwareOSCReceiver::disconnect()
	{
		return m_pimpl->disconnect();
	}

	void SenderAwareOSCReceiver::addListener(SAOListener<OSCReceiver::MessageLoopCallback>* listenerToAdd)
	{
		m_pimpl->addListener(listenerToAdd);
	}

	void SenderAwareOSCReceiver::addListener(SAOListener<OSCReceiver::RealtimeCallback>* listenerToAdd)
	{
		m_pimpl->addListener(listenerToAdd);
	}

	void SenderAwareOSCReceiver::removeListener(SAOListener<OSCReceiver::MessageLoopCallback>* listenerToRemove)
	{
		m_pimpl->removeListener(listenerToRemove);
	}

	void SenderAwareOSCReceiver::removeListener(SAOListener<OSCReceiver::RealtimeCallback>* listenerToRemove)
	{
		m_pimpl->removeListener(listenerToRemove);
	}

	void SenderAwareOSCReceiver::registerFormatErrorHandler(OSCReceiver::FormatErrorHandler handler)
	{
		m_pimpl->registerFormatErrorHandler(handler);
	}

}