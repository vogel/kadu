/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

	void import_0_6_5_configuration();
	void createDefaultConfiguration();

private slots:
	void notificationClosed(Notification *notification);

public:
	explicit WindowNotifier(QObject *parent = 0);
	virtual ~WindowNotifier();

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0) { Q_UNUSED(parent) return 0; }

};

/** @} */

#endif // WINDOW_NOTIFIER_H
