/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef WINDOW_NOTIFIER_H
#define WINDOW_NOTIFIER_H

#include "notify/notifier.h"

/**
 * @defgroup window_notify Window notify
 * @{
 */

class WindowNotifier : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();

public:
	explicit WindowNotifier(QObject *parent = 0);
	virtual ~WindowNotifier();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0) { return 0; };

};

/** @} */

#endif // WINDOW_NOTIFIER_H
