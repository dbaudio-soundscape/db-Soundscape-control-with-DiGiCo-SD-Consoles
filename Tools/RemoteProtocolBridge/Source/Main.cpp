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

#include <JuceHeader.h>

#include "MainRemoteProtocolBridgeComponent.h"

/**
 * Class definition/declaration of RemoteProtocolBridgeApplication is mostly the
 * default JUCEApplication implementation for a desktop application. A minor differenc
 * is setting application window to fullscreen mode for mobile platforms.
 */
class RemoteProtocolBridgeApplication  : public JUCEApplication
{
public:
    RemoteProtocolBridgeApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }	/**< JUCE default implementation to get project name from projucer configuration. */
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }	/**< JUCE default implementation to get version string from projucer configuration. */
    bool moreThanOneInstanceAllowed() override       { return true; }						/**< JUCE default implementation to get multi instance support info from projucer configuration. */

	/**
	 * JUCE default implementation for initialization
	 */
    void initialise (const String& commandLine) override
    {
		ignoreUnused(commandLine);
        m_mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

	/**
	 * JUCE default implementation for shutdown
	 */
    void shutdown() override
    {
        m_mainWindow.reset();
    }

	/**
	 * JUCE default implementation for handling of system quit request
	 */
    void systemRequestedQuit() override
    {
        quit();
    }

	/**
	 * JUCE default implementation for handling of another instance being started
	 */
    void anotherInstanceStarted (const String& commandLine) override
    {
		ignoreUnused(commandLine);
    }

	/**
     *   This class implements the desktop window that contains an instance of
     *   our MainComponent class.
     */
	class MainWindow : public DocumentWindow
	{
	public:
		/**
		 * JUCE default implementation of MainWindow constructor, extended with
		 * special sizing intialization for mobile platforms vs. desktop (fullscreen vs. fix size, centered)
		 */
		MainWindow(String name)
			: DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(new MainRemoteProtocolBridgeComponent(), true);

#if JUCE_IOS || JUCE_ANDROID
			setFullScreen(true);
#else
			Rectangle<int> r = Desktop::getInstance().getDisplays().getMainDisplay().userArea;
			setResizable(true, true);
			setResizeLimits(UIS_MainComponentWidth, int(0.5 * UIS_MainComponentWidth), r.getWidth(), r.getHeight());

			centreWithSize(getWidth(), getHeight());
#endif

			setVisible(true);
		}

		void closeButtonPressed() override { JUCEApplication::getInstance()->systemRequestedQuit(); }	/**< JUCE default implementation to handle close button click. */

	private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
	};

private:
    std::unique_ptr<MainWindow> m_mainWindow;	/**< This applications' main window. */
};

START_JUCE_APPLICATION (RemoteProtocolBridgeApplication)
