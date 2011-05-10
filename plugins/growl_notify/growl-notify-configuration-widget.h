/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2008, 2010 Tomasz Rostański (rozteck@interia.pl)
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

#ifndef GROWL_NOTIFY_CONFIGURATION_WIDGET_H
#define GROWL_NOTIFY_CONFIGURATION_WIDGET_H

#include <QtCore/QMap>

#include "gui/widgets/configuration/notifier-configuration-widget.h"

class QLabel;
class QLineEdit;
class QSpinBox;
class QComboBox;
class QCheckBox;

struct GrowlNotifyProperties
{
	QString eventName;
	QString syntax;
	QString title;
	bool    showAvatar;
};

class GrowlNotifyConfigurationWidget : public NotifierConfigurationWidget
{
	Q_OBJECT

	QMap<QString, GrowlNotifyProperties> properties;
	GrowlNotifyProperties currentProperties;
	QString currentNotifyEvent;

	QLineEdit *syntax;
	QLineEdit *title;
//	QCheckBox *showAvatar;

private slots:
	void syntaxChanged(const QString &syntax);
	void titleChanged(const QString &title);
//	void avatarChanged(int state);

public:
	GrowlNotifyConfigurationWidget(QWidget *parent = 0);

	virtual void loadNotifyConfigurations() {}
	virtual void saveNotifyConfigurations();

	virtual void switchToEvent(const QString &event);
};

#endif
