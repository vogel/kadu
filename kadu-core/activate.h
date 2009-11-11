#ifndef ACTIVATE_H
#define ACTIVATE_H

#include "QtGui/QWidget"

#include "exports.h"



bool KADUAPI _isActiveWindow( QWidget *window );

#ifdef Q_WS_X11

	#include <QtGui/QX11Info>
	void _activateWindow( QWidget *window );

#elif defined(Q_OS_WIN)

	KADUAPI void _activateWindow( QWidget *window );

#else

	#define _activateWindow(x)

#endif



#endif // ACTIVATE_H
