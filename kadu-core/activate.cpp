#include <QtCore/QtGlobal>
#include "activate.h"


#ifdef Q_WS_X11

	#include <QtGui/QApplication>
	#include <QtGui/QDesktopWidget>
	#include <QtGui/QMdiSubWindow>
	#include <QtGui/QWidget>
	#include <QtGui/QX11Info>
	#include <math.h>
	#include "config_file.h"
	#include "x11tools.h"

	void activateWindow(Qt::HANDLE id)
	{
		// unshade the window
		X11_windowSendXEvent( QX11Info::display(), id, "_NET_WM_STATE", "_NET_WM_STATE_SHADED", false );
		XFlush( QX11Info::display() );
		// read user settings
		int action = config_file.readNumEntry("General", "WindowActivationMethod");
		// find widget
		QWidget *widget = QWidget::find(id);
		// desktops count
		if( X11_getDesktopsCount( QX11Info::display() ) > 1 )
		{
			long desktopofwindow = X11_getDesktopOfWindow( QX11Info::display(), id );
			long currentdesktop = X11_getCurrentDesktop( QX11Info::display() );
			if( desktopofwindow != -1 && currentdesktop != -1 )
			{
				if( desktopofwindow != currentdesktop )
				{
					if( desktopofwindow != X11_ALLDESKTOPS )
					{
						if( action==0 )
						{
							X11_moveWindowToDesktop( QX11Info::display(), id, currentdesktop );
						}
						if( action==1 )
						{
							X11_setCurrentDesktop( QX11Info::display(), desktopofwindow );
						}
					}
				}
			}
		}
		else
		{
			// Compiz compatibility (wide single desktop)
			QWidget *widget = QWidget::find(id);
			if( widget != NULL )
			{
				QRect screenrect = QApplication::desktop()->screenGeometry();
				QSize screensize = QSize( screenrect.width(), screenrect.height() );
				QPoint activeDesktop;
				activeDesktop.setX( QCursor::pos().x() / screensize.width()  );
				activeDesktop.setY( QCursor::pos().y() / screensize.height() );
				QPoint window_topleft;
				window_topleft.setX( floor( 1.0 * widget->pos().x() / screensize.width()  ) );
				window_topleft.setY( floor( 1.0 * widget->pos().y() / screensize.height() ) );
				QPoint window_topright;
				window_topright.setX( floor( 1.0 * ( widget->pos().x() + widget->size().width()  ) / screensize.width()  ) );
				window_topright.setY( floor( 1.0 * widget->pos().y() / screensize.height() ) );
				QPoint window_bottomleft;
				window_bottomleft.setX( floor( 1.0 * widget->pos().x() / screensize.width()  ) );
				window_bottomleft.setY( floor( 1.0 * ( widget->pos().y() + widget->size().height() ) / screensize.height() ) );
				QPoint window_bottomright;
				window_bottomright.setX( floor( 1.0 * ( widget->pos().x() + widget->size().width()  ) / screensize.width()  ) );
				window_bottomright.setY( floor( 1.0 * ( widget->pos().y() + widget->size().height() ) / screensize.height() ) );
				if( ( window_topleft != activeDesktop ) && ( window_topright != activeDesktop ) && ( window_bottomleft != activeDesktop ) && ( window_bottomright != activeDesktop ) )
				{
					// window is not visible on the current desktop
					widget->move( widget->pos().x() % screensize.width(), widget->pos().y() % screensize.height() );
				}
			}
		}
		if( widget != NULL )
		{
			if( widget->isMinimized() ) widget->showNormal(); // unminimize
			widget->raise();           // raise
			widget->activateWindow();  // activate
		}
		X11_setActiveWindow( QX11Info::display(), id );
	}

#elif defined(Q_OS_WIN)

	#include <windows.h>
	#include <stdio.h>
	
	void activateWindow(void* id)
	{
		SetForegroundWindow((HWND)id);
	}

#endif
