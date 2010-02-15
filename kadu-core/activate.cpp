#include <QtCore/QtGlobal>
#include "activate.h"


#ifdef Q_WS_X11

	#include <QtGui/QApplication>
	#include <QtGui/QDesktopWidget>
	#include <QtGui/QMdiSubWindow>
	#include <QtGui/QWidget>
	#include <QtGui/QX11Info>
	#include <math.h>

	#include "configuration/configuration-file.h"
	#include "x11tools.h"

	bool _isActiveWindow( QWidget *window )
	{
		// desktop
		long desktopofwindow = X11_getDesktopOfWindow( QX11Info::display(), window->winId() );
		if( ( desktopofwindow != X11_getCurrentDesktop( QX11Info::display() ) ) && ( desktopofwindow != (long)X11_ALLDESKTOPS ) )
			return false;
		// standard isActiveWindow() method
		return window->isActiveWindow();
	}

	void _activateWindow( QWidget *window )
	{
		// unshade the window (important)
		X11_windowSendXEvent( QX11Info::display(), window->winId(), "_NET_WM_STATE", "_NET_WM_STATE_SHADED", false );
		XFlush( QX11Info::display() );
		// read user settings
		int action = config_file.readNumEntry( "General", "WindowActivationMethod" );
		// window & desktop
		if( X11_getDesktopsCount( QX11Info::display() ) > 1 )
		{
			long desktopofwindow = X11_getDesktopOfWindow( QX11Info::display(), window->winId() );
			long currentdesktop = X11_getCurrentDesktop( QX11Info::display() );
			if( ( desktopofwindow != currentdesktop ) && ( desktopofwindow != (long)X11_ALLDESKTOPS ) )
			{
				if( action==0 )
				{
					if( X11_isWholeWindowOnOneDesktop( QX11Info::display(), window->winId() ) )
					{
						X11_moveWindowToDesktop( QX11Info::display(), window->winId(), currentdesktop );
					}
					else
					{
						X11_centerWindow( QX11Info::display(), window->winId(), currentdesktop );
					}
				}
				if( action==1 )
				{
					X11_setCurrentDesktop( QX11Info::display(), desktopofwindow );
				}
			}
		}
		// activate
		if( window->isMinimized() ) window->showNormal(); // unminimize
		window->raise();                                  // raise
		window->activateWindow();                         // activate
		X11_setActiveWindow( QX11Info::display(), window->winId() );
		XFlush( QX11Info::display() );
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
		SetForegroundWindow((HWND)(window->winId()));
	}

#else

	bool _isActiveWindow( QWidget *window )
	{
		return window->isActiveWindow();
	}

#endif
