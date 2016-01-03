/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class FreedesktopNotifier;
class NotificationManager;
class PathsProvider;

class FreedesktopNotifyPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit FreedesktopNotifyPluginObject(QObject *parent = nullptr);
	virtual ~FreedesktopNotifyPluginObject();

	virtual void done() override;

private:
	QPointer<FreedesktopNotifier> m_freedesktopNotifier;
	QPointer<NotificationManager> m_notificationManager;
	QPointer<PathsProvider> m_pathsProvider;

private slots:
	INJEQT_INIT void init();
	INJEQT_SETTER void setFreedesktopNotifier(FreedesktopNotifier *freedesktopNotifier);
	INJEQT_SETTER void setNotificationManager(NotificationManager *notificationManager);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);

};
