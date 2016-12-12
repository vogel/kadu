/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "widgets/configuration/notifier-configuration-widget.h"

#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class Configuration;

class QLineEdit;

class ExecConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

public:
	explicit ExecConfigurationWidget(QWidget *parent = nullptr);
	virtual ~ExecConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);

private:
	QPointer<Configuration> m_configuration;

	QString m_currentNotificationEvent;
	QMap<QString, QString> m_commands;

	QLineEdit *m_commandLineEdit;

private slots:
	INJEQT_SET void setConfiguration(Configuration *configuration);

};
