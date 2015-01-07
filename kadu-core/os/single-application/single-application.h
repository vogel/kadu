/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * Contains code from QtSingleApplication.
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore/QObject>
#include <functional>

class QLocalServer;

class SingleApplication : public QObject
{
	Q_OBJECT

public:
	explicit SingleApplication(
		const QString &applicationId,
		std::function<void(void)> executeAsFirst,
		std::function<void(SingleApplication &)> executeAsNext,
		std::function<void(const QString &)> onReceiveMessage,
		QObject *parent = nullptr);
	virtual ~SingleApplication();

	void sendMessage(const QString &message, int timeout) const;

private:
	static QString defaultApplicationId(const QString &applicationId);
	static QString normalizedPrefix(bool useOnlyLastSection, const QString &applicationId);
	static QString socketName(const QString &prefix, const QString &applicationId);
	static QString lockName(const QString &socketName);

	QString m_socketName;
	QLocalServer *m_localServer;
	std::function<void(const QString &)> m_onReceiveMessage;

	void startServer();

private slots:
	void receiveConnection();

};
