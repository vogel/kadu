#ifndef ACTIVATE_H
#define ACTIVATE_H

#include "QtGui/QWidget"

#include "exports.h"

#ifdef Q_WS_X11
	#include <QtGui/QX11Info>
#endif

bool KADUAPI _isActiveWindow( QWidget *window );
void KADUAPI _activateWindow( QWidget *window );

#endif // ACTIVATE_H
