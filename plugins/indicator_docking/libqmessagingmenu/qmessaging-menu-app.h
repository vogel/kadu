/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <glib.h>

class QMessagingMenuSource;
enum class QMessagingMenuStatus;
typedef struct _MessagingMenuApp MessagingMenuApp;

class QMessagingMenuApp : public QObject
{
	Q_OBJECT

public:
	explicit QMessagingMenuApp(const QString &desktopId, QObject *parent = nullptr);
	virtual ~QMessagingMenuApp();

	void registerMenu();
	void unregisterMenu();

	QMessagingMenuSource * addSource(const QString &id, const QString &label, const QString &icon, int count = 0);
	void removeSource(const QString &id);
	QMessagingMenuSource * source(const QString &id);

	void setStatus(QMessagingMenuStatus status);

public Q_SLOTS:
	void sourceActivatedSlot(const QString &id);
	void statusChangedSlot(QMessagingMenuStatus status);

Q_SIGNALS:
	void sourceActivated(const QString &id);
	void statusChanged(QMessagingMenuStatus status);

private:
	friend class QMessagingMenuSource;

	MessagingMenuApp *m_app;
	QMap<QString, QMessagingMenuSource *> m_sources;

	MessagingMenuApp * app() const;

};
