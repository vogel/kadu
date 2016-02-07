/*
 * %kadu copyright begin%
 * Copyright 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notification/notifier.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;
class InjectedFactory;
class NotificationManager;
class Parser;

class ExecNotifier : public QObject, public Notifier
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit ExecNotifier(QObject *parent = nullptr);
	virtual ~ExecNotifier();

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = nullptr);
	virtual void notify(const Notification &notification);

private:
	QPointer<Configuration> m_configuration;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotificationManager> m_notificationManager;
	QPointer<Parser> m_parser;

	void createDefaultConfiguration();
	void run(const QStringList &args);

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotificationManager(NotificationManager *notificationManager);
	INJEQT_SET void setParser(Parser *parser);
	INJEQT_INIT void init();

};
