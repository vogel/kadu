/*
 * Copyright 2009 Jacek Jabłoński
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KDE_NOTIFY_H
#define KDE_NOTIFY_H

#include <QtCore/QQueue>

#include "gui/windows/main-configuration-window.h"
#include "notify/notifier.h"

class QDBusInterface;

class Notification;

class KdeNotify : public Notifier, public ConfigurationUiHandler
{
	Q_OBJECT

	QDBusInterface *KNotify;
	QRegExp StripHTML;
	QMap<unsigned int, Notification *> NotificationMap;
	QQueue<unsigned int> IdQueue;

	bool UseFreedesktopStandard;

	void createDefaultConfiguration();

private slots:
	void actionInvoked(unsigned int id, QString action);
	void deleteMapItem();

	void notificationClosed(Notification *notification);

public:
	explicit KdeNotify(QObject *parent = 0);
	virtual ~KdeNotify();

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0) { Q_UNUSED(parent); return 0; }
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);
	virtual void notify(Notification *notification);

};

extern KdeNotify *kde_notify;

#endif // KDE_NOTIFY_H
