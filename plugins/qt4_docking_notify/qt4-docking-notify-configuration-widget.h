/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H
#define QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class ConfigurationWindow;

class Qt4NotifyConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QString currentNotifyEvent;

private slots:

	void showConfigurationWindow();

public:
	Qt4NotifyConfigurationWidget(QWidget *parent = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);
};

#endif // QT4_DOCKING_NOTIFY_CONFIGURATION_WIDGET_H
