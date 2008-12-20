#ifndef ACTIVATE_H
#define ACTIVATE_H

#include "exports.h"



#ifdef Q_WS_X11

	#include <QtGui/QX11Info>

	void activateWindow(Qt::HANDLE id);

#elif defined(Q_OS_WIN)

	KADUAPI void activateWindow(void* id);

#else

	#define activateWindow(x)

#endif



#endif // ACTIVATE_H
