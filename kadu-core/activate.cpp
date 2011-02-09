#include <QtGui/QWidget>

#include "activate.h"

#ifdef Q_WS_X11

	#include <QtGui/QX11Info>

	#include "configuration/configuration-file.h"
	#include "os/x11tools.h"

	bool _isActiveWindow( QWidget *window )
	{
		// we need to ensure we operate on widget's window, if not passed
		window = window->window();
		// desktop
		unsigned long desktopofwindow = X11_getDesktopOfWindow( QX11Info::display(), window->winId() );
		if( ( desktopofwindow != X11_ALLDESKTOPS ) && ( desktopofwindow != X11_NODESKTOP ) && ( desktopofwindow != X11_getCurrentDesktop( QX11Info::display() ) ) )
			return false;
		// standard isActiveWindow() method
		return window->isActiveWindow();
	}

	void _activateWindow( QWidget *window )
	{
		// we need to ensure we operate on widget's window, if not passed
		window = window->window();
		// unshade the window if needed (important!)
		if( X11_isWindowShaded( QX11Info::display(), window->winId() ) )
			X11_shadeWindow( QX11Info::display(), window->winId(), false );
		// read user settings
		int action = config_file.readNumEntry( "General", "WindowActivationMethod" );
		// window & desktop
		if( X11_getDesktopsCount( QX11Info::display() ) > 1 )
		{
			unsigned long desktopofwindow = X11_getDesktopOfWindow( QX11Info::display(), window->winId() );
			unsigned long currentdesktop = X11_getCurrentDesktop( QX11Info::display() );
			if( ( desktopofwindow != currentdesktop ) && ( desktopofwindow != X11_ALLDESKTOPS ) )
			{
				if( ( action == 1 ) && ( desktopofwindow != X11_NODESKTOP ) )
				{
					X11_setCurrentDesktop( QX11Info::display(), desktopofwindow );
				}
				else
				{
					if( X11_isWholeWindowOnOneDesktop( QX11Info::display(), window->winId() ) )
						X11_moveWindowToDesktop( QX11Info::display(), window->winId(), currentdesktop );
					else
						X11_centerWindow( QX11Info::display(), window->winId(), currentdesktop );
				}
			}
		}
		// activate
		if( window->isMinimized() )
			window->showNormal(); // unminimize
		X11_setActiveWindow( QX11Info::display(), window->winId() );
		window->activateWindow();
		window->raise();
	}

#elif defined(Q_OS_WIN)

	#include <windows.h>
	#include <stdio.h>

	bool _isActiveWindow( QWidget *window )
	{
		return window->isActiveWindow();
	}

	void _activateWindow( QWidget *window )
	{
		// we need to ensure we operate on widget's window, if not passed
		window = window->window();
		window->activateWindow();
		window->raise();
		SetForegroundWindow((HWND)(window->winId()));
	}

#else

	bool _isActiveWindow( QWidget *window )
	{
		return window->isActiveWindow();
	}

	void _activateWindow( QWidget *window )
	{
		if (window->isMinimized())
			window->showNormal();
		window->activateWindow();
		window->raise();
	}

#endif

bool _isWindowActiveOrFullyVisible( QWidget *window )
{
	// we need to ensure we operate on widget's window, if not passed
	window = window->window();
#ifdef Q_WS_X11
	if( _isActiveWindow( window ) )
		return true;

	Display *display = QX11Info::display();
	WId wId = window->winId();
	return ! window->isMinimized() &&
			X11_isWindowOnDesktop( display, wId, X11_getCurrentDesktop( display ) ) &&
			X11_isWholeWindowOnOneDesktop( display, wId ) &&
			! X11_isWindowCovered( display, wId );
#else
	return _isActiveWindow( window );
#endif
}
