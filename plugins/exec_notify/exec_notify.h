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

#ifndef EXEC_NOTIFY_H
#define EXEC_NOTIFY_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QStringList>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

#include "notification/notifier.h"


class QLineEdit;

class ExecConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotificationEvent;
	QMap<QString, QString> Commands;

	QLineEdit *commandLineEdit;

public:
	ExecConfigurationWidget(QWidget *parent = 0);
	virtual ~ExecConfigurationWidget();

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();
	virtual void switchToEvent(const QString &event);
};

class ExecNotify : public Notifier
{
	Q_OBJECT

	void createDefaultConfiguration();

	void run(const QStringList &args);

public:
	ExecNotify(QObject *parent = 0);
	~ExecNotify();

	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = 0);
	virtual void notify(Notification *notification);

};

#endif // EXEC_NOTIFY_H
